/* SPDX-License-Identifier: MPL-2.0
 * P25Q80H suspend/resume RAM driver; include after fm1_flash.h. */
static RAMFN int aq_flash_slice(uint32_t command, uint32_t off,
                               const uint8_t *data, uint32_t n, uint32_t resume)
{
    fl_saved_t saved;
    if(fl_enter(&saved))return -2;
    if(resume){fl_cs(0);fl_tx(0x7a);fl_cs(1);}
    else{
        fl_wren();
        if(!(fl_rdsr(0x05)&2u)){fl_exit(&saved);return -3;}
        fl_cs(0);fl_tx((uint8_t)command);fl_addr(off);
        for(uint32_t i=0;i<n;i++)fl_tx(data[i]);
        fl_cs(1);
    }
    uint32_t start=T4_CNT;
    while((uint32_t)(T4_CNT-start)<2400u){
        if(!(fl_rdsr(0x05)&1u)){
            fl_exit(&saved);return 0;
        }
    }
    fl_cs(0);fl_tx(0x75);fl_cs(1);
    /* Never restore XIP on a busy part. A faulty suspend remains in RAM
     * until completion (bounded existing flash timeout), returns an error.
     * This hardware-fault path cannot promise uninterrupted playback. */
    start=T4_CNT;
    while(fl_rdsr(0x05)&1u){
        if((uint32_t)(T4_CNT-start)>12000u){
            uint32_t took;
            int rc=fl_wait_ready(500000u,&took);
            if(rc){ /* watchdog reset rather than execute unreadable flash */
                for(;;){}
            }
            fl_exit(&saved);return -10;
        }
    }
    uint32_t status=fl_rdsr(0x35);
    fl_exit(&saved);
    return (status&0x84u)?1:0;
}

