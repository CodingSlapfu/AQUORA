/* SPDX-License-Identifier: MPL-2.0
 * AQUORA live-save transaction. Includes the existing storage envelope verbatim.
 * The inactive sector is erased/programmed in suspended 100us work slices.
 * P25Q80H v1.7 sections 5.3/10.36/10.38: 75h/7Ah, >=100us progress.
 * No XIP fetch is allowed until WIP=0. CPU1 must be idle in its SRAM loop.
 */
static struct {
    st_hdr_t header;
    uint8_t payload[512];
    uint32_t base, pos, phase, resumed, slices;
} aq_store;

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

int plat_aquora_save_begin(uint32_t obj,const void *src,uint32_t len)
{
    if(aq_store.phase||!flash_ok||obj<OBJ_BANK1||obj>OBJ_BANK4||!len||len>512)return -1;
    st_hdr_t old;
    int cur=st_current(obj,&old);
    st_hdr_t *h=&aq_store.header;
    memset(h,0xff,sizeof(*h));
    h->magic=ST_MAGIC;h->type=obj;h->slot=cur==0?1:0;
    h->seq=cur<0?1:old.seq+1u;h->len=len;
    memcpy(aq_store.payload,src,len);
    h->crc=st_crc32(src,len);h->hcrc=st_crc32(h,sizeof(*h)-4u);
    aq_store.base=st_sector(obj,h->slot);
    aq_flash_busy_base=aq_store.base;
    aq_store.pos=aq_store.resumed=aq_store.slices=0;
    aq_store.phase=1;
    return 0;
}
/* 1 pending, 0 durable readback verified, <0 failed. Main/UI only. */
int plat_aquora_save_poll(void)
{
    if(!aq_store.phase)return -1;
#if FM1B_CPU2
    if(plat_cpu2_busy())return 1;
#endif
    uint32_t phase=aq_store.phase;
    if(phase==4){
        aq_flash_busy_base=0;
        fl_inval(aq_store.base,4096u);
        st_hdr_t chk;
        int ok=!st_head(aq_store.header.type,aq_store.header.slot,&chk)&&
            chk.seq==aq_store.header.seq&&!st_body(chk.type,chk.slot,&chk);
        aq_store.phase=0;
        return ok?0:-7;
    }
    uint32_t off=aq_store.base, n=0, cmd=0x20;
    const uint8_t *data=aq_store.payload;
    if(phase==2){
        off+=ST_PAYLOAD_OFF+aq_store.pos;
        n=aq_store.header.len-aq_store.pos;
        if(n>32)n=32;
        cmd=0x02;data+=aq_store.pos;
    }else if(phase==3){
        n=sizeof(st_hdr_t);cmd=0x02;data=(const uint8_t *)&aq_store.header;
    }
    uint32_t flags=irq_save();
    int rc=FL_FAR(aq_flash_slice)(cmd,off,data,n,aq_store.resumed);
    irq_restore(flags);
    if(rc<0){aq_store.phase=0;aq_flash_busy_base=0;return rc;}
    aq_store.resumed=rc!=0;
    if(rc)return 1;
    fl_inval(off,phase==1?4096:n);
    if(phase==2){aq_store.pos+=n;if(aq_store.pos>=aq_store.header.len)aq_store.phase=3;}
    else aq_store.phase++;
    return 1;
}
