/* SPDX-License-Identifier: MPL-2.0
 * AQUORA live-save transaction. Includes the existing storage envelope verbatim.
 * The inactive sector is erased/programmed in suspended 100us work slices.
 * P25Q80H v1.7 sections 5.3/10.36/10.38: 75h/7Ah, >=100us progress.
 * No XIP fetch is allowed until WIP=0. CPU1 must be idle in its SRAM loop.
 */
static struct {
    st_hdr_t header;
    uint8_t payload[512];
    uint32_t base, pos, phase, resumed, slices, busy, wait_tick, phase_tick, phase_us[4], suspends;
} aq_store;

#include "../bsp/chip/fm1_flash_aquora.h"

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
    aq_store.phase=1;aq_store.busy=0;aq_store.wait_tick=T4_CNT;aq_store.phase_tick=aq_store.wait_tick;aq_store.suspends=0;
    memset(aq_store.phase_us,0,sizeof aq_store.phase_us);
    return 0;
}
/* 1 pending, 0 durable readback verified, <0 failed. Main/UI only. */
int plat_aquora_save_poll(void)
{
    if(!aq_store.phase)return -1;
#if FM1B_CPU2
    if(plat_cpu2_busy()){
      aq_store.busy++;
      if((uint32_t)(T4_CNT-aq_store.wait_tick)>480000u)plat_cpu2_wait();
      return 1;
    }
#endif
    aq_store.slices++;
    uint32_t phase=aq_store.phase;
    if(phase==4){
        aq_flash_busy_base=0;
        fl_inval(aq_store.base,4096u);
        st_hdr_t chk;
        int ok=!st_head(aq_store.header.type,aq_store.header.slot,&chk)&&
            chk.seq==aq_store.header.seq&&!st_body(chk.type,chk.slot,&chk);
        aq_store.phase_us[3]=(T4_CNT-aq_store.phase_tick)/24u;aq_store.phase=0;
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
    aq_store.phase_us[phase-1u]=(T4_CNT-aq_store.phase_tick)/24u;
    if(rc<0){aq_store.phase=0;aq_flash_busy_base=0;return rc;}
    aq_store.resumed=rc!=0;
    if(rc){aq_store.suspends++;return 1;}
    fl_inval(off,phase==1?4096:n);
    if(phase==2){aq_store.pos+=n;if(aq_store.pos>=aq_store.header.len)aq_store.phase=3;}
    else aq_store.phase++;
    if(aq_store.phase!=phase)aq_store.phase_tick=T4_CNT;
    return 1;
}

void plat_aquora_save_progress(uint32_t *p){p[0]=aq_store.phase;p[1]=aq_store.slices;p[2]=aq_store.busy;p[3]=aq_store.pos;for(unsigned i=0;i<4;i++)p[4+i]=aq_store.phase_us[i];p[8]=aq_store.suspends;}
