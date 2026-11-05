/* Production asynchronous transaction + RAM slice, mocked P25Q80H SPI. */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../device/app.h"
static uint8_t nor[0x100000], bytes[256];
static uint32_t aq_flash_busy_base, tick, opoff, oplen, remain, opcmd, suspended;
static uint32_t cmd, address, spi_index, selected, count, max_irq, irq_start;
static int flash_ok=1;
static int st_read(uint32_t off,void *dst,uint32_t n){
 if(aq_flash_busy_base&&off<aq_flash_busy_base+4096&&off+n>aq_flash_busy_base)return -1;
 memcpy(dst,nor+off,n);return 0;
}
static int st_erase(uint32_t off){memset(nor+off,255,4096);return 0;}
static int st_prog(uint32_t off,const void *src,uint32_t n){for(uint32_t i=0;i<n;i++)nor[off+i]&=((const uint8_t*)src)[i];return 0;}
#include "../vendor/fm1-base/device/storage.c"
static uint32_t timer(void){
 tick+=24;
 if(remain&&!suspended&&!--remain){
  if(opcmd==0x20)memset(nor+opoff,255,4096);
  else for(unsigned i=0;i<oplen;i++)nor[opoff+i]&=bytes[i];
 }
 return tick;
}
#define T4_CNT timer()
uint32_t plat_ticks24(void){return timer();}
#define RAMFN
#define FL_FAR(fn) fn
#define fl_inval(a,b) ((void)(a),(void)(b))
typedef int fl_saved_t;
static int fl_enter(fl_saved_t *s){*s=0;assert(!selected);return 0;}
static void fl_exit(const fl_saved_t *s){(void)s;assert(!remain||suspended);}
static void fl_wren(void){}
static void fl_cs(int high){
 if(!high){selected=1;cmd=address=spi_index=count=0;}
 else{
  assert(selected);selected=0;
  if(cmd==0x75&&remain)suspended=1;
  if(cmd==0x7a)suspended=0;
  if(cmd==0x20||cmd==0x02){assert(!remain);opcmd=cmd;opoff=address;oplen=count;remain=cmd==0x20?360:240;}
 }
}
static void fl_tx(uint8_t b){
 assert(selected);
 if(!spi_index++){cmd=b;return;}
 if(spi_index<=4)address=(address<<8)|b;
 else{assert(count<256);bytes[count++]=b;}
}
static void fl_addr(uint32_t a){fl_tx(a>>16);fl_tx(a>>8);fl_tx(a);}
static uint8_t fl_rdsr(uint8_t c){
 timer();
 if(c==0x35)return suspended?(opcmd==0x20?0x80:4):0;
 return (remain&&!suspended?1:0)|2;
}
static int fl_wait_ready(uint32_t max,uint32_t *took){(void)max;*took=0;while(remain)timer();return 0;}
static uint32_t irq_save(void){irq_start=tick;return 0;}
static void irq_restore(uint32_t f){(void)f;uint32_t dt=(tick-irq_start)/24;if(dt>max_irq)max_irq=dt;}
static unsigned worker_busy,worker_retires;
int plat_cpu2_busy(void){return worker_busy;}
int plat_cpu2_wait(void){worker_busy=0;worker_retires++;return -1;}
#include "../vendor/fm1-base/device/aquora_async_store.h"
static void reset_job(void){memset(&aq_store,0,sizeof aq_store);aq_flash_busy_base=0;remain=suspended=0;}
int main(void){
 uint8_t previous[400],next[400],got[400],baseline[0x100000];
 memset(nor,255,sizeof nor);memset(previous,42,sizeof previous);memset(next,73,sizeof next);
 for(unsigned bank=0;bank<4;bank++)assert(!st_save(OBJ_BANK1+bank,previous,400));
 memcpy(baseline,nor,sizeof nor);
 for(unsigned bank=0;bank<4;bank++){
  unsigned total=0;
  for(int cut=0;cut<100;cut++){
   memcpy(nor,baseline,sizeof nor);reset_job();
   assert(!plat_aquora_save_begin(OBJ_BANK1+bank,next,400));
   assert(plat_aquora_save_begin(OBJ_BANK1+bank,next,400)<0);
   int rc=1;for(int i=0;i<cut&&rc==1;i++)rc=plat_aquora_save_poll();
   if(!rc)total=cut;
   reset_job(); /* power loss at every erase/program/suspend/commit boundary */
   assert(st_load(OBJ_BANK1+bank,got,400)==400);
   assert(!memcmp(got,previous,400)||!memcmp(got,next,400));
   for(unsigned other=0;other<4;other++)if(other!=bank){assert(st_load(OBJ_BANK1+other,got,400)==400);assert(!memcmp(got,previous,400));}
  }
  assert(total);printf("BANK %u interrupted at 100 boundaries; complete by poll %u\n",bank+1,total);
  memcpy(nor,baseline,sizeof nor);reset_job();assert(!plat_aquora_save_begin(OBJ_BANK1+bank,next,400));
  int rc;do{rc=plat_aquora_save_poll();}while(rc==1);assert(!rc);
  uint32_t base=aq_store.base;nor[base+256]^=1; /* bad payload -> previous CRC-valid copy */
  assert(st_load(OBJ_BANK1+bank,got,400)==400);assert(!memcmp(got,previous,400));
 }
 memcpy(nor,baseline,sizeof nor);reset_job();worker_busy=1;
 assert(!plat_aquora_save_begin(OBJ_BANK1,next,400));
 int rc=1;unsigned polls=0;
 while(rc==1&&polls++<200){tick+=24000;rc=plat_aquora_save_poll();}
 assert(!rc&&worker_retires==1&&aq_store.busy>0);
 assert(st_load(OBJ_BANK1,got,400)==400&&!memcmp(got,next,400));
 assert(max_irq<150);printf("PASS async live save: CRC fallback, prior-good preservation; modeled IRQ slice max %u us (not device timing)\n",max_irq);
}
