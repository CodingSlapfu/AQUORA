/* SPDX-License-Identifier: MPL-2.0 */
static arrangement quick_record;
static unsigned quick_requests,quick_retries;
static int quick_last_result;
static unsigned quick_next_ms,quick_request_ms,quick_snapshot_ms,quick_flash_ms,quick_total_ms;
static unsigned quick_fence,quick_request_slot,quick_polls,quick_timeout;
static uint32_t quick_progress[9];
static unsigned quick_pending, quick_slot, quick_started, quick_finished_ms;
static unsigned quick_saved_revision, quick_has_save, quick_snapshot_revision;
#ifdef PLAT_HOST
static int quick_begin(unsigned obj,const void *data,unsigned len)
{(void)obj;(void)data;(void)len;return 0;}
static int quick_poll(void){
 const char *fault=getenv("AQUORA_SAVE_TEST");quick_progress[0]=2;
 if(fault&&!strcmp(fault,"stall"))return 1;
 if(fault&&!strcmp(fault,"fail"))return -1;
 if(fault&&!strcmp(fault,"slow")&&quick_polls<80)return 1;
 return plat_store_save(OBJ_BANK1+quick_slot,&quick_record,sizeof(quick_record));
}
#else
extern int plat_aquora_save_begin(uint32_t,const void *,uint32_t);
extern int plat_aquora_save_poll(void);
#define quick_begin plat_aquora_save_begin
static int quick_poll(void){
 int r=plat_aquora_save_poll();
 extern void plat_aquora_save_progress(uint32_t *);
 plat_aquora_save_progress(quick_progress);return r;
}
#endif
/* Input only requests a snapshot. Main-loop service waits for the audio-owned
 * command queue to drain; snapshot copies one consistent state under IRQ lock.
 * A second press during a transaction coalesces into one latest-state request.
 */
static void arrangement_quick_save(void)
{
    quick_requests++;if(quick_last_result<0)quick_retries++;
    quick_next_ms=plat_ms();
    if(!quick_started&&!quick_pending){quick_request_ms=quick_next_ms;quick_timeout=0;quick_polls=0;}
    quick_pending=1;quick_request_slot=bank_slot;quick_fence=command_enqueued;
    bank_message=BANK_SAVING;
    dirty=1;
}
static void arrangement_service(void)
{
    if(!quick_timeout&&(quick_started||quick_pending)&&plat_ms()-quick_request_ms>=5000u){
      quick_timeout=1;quick_last_result=-8;quick_last_result=-1;bank_message=BANK_ERROR;quick_finished_ms=plat_ms();dirty=1;if(!quick_started)quick_pending=0;
    }
    if(quick_started){quick_polls++;
        int result=quick_poll();
        if(result==1)return;
        quick_last_result=quick_timeout?-8:result;quick_started=0;
        quick_total_ms=plat_ms()-quick_request_ms;
        quick_flash_ms=plat_ms()-quick_snapshot_ms;
        bank_message=result==0&&!quick_timeout?BANK_SAVED:BANK_ERROR;
        if(!result){
            bank_status[quick_slot]=1;
            quick_has_save=1;
            quick_saved_revision=quick_snapshot_revision;
        }
        quick_finished_ms=plat_ms();dirty=1;
    }
    if(quick_pending&&!quick_started&&(int32_t)(command_applied-quick_fence)>=0){
        quick_slot=quick_request_slot;quick_request_ms=quick_next_ms;quick_timeout=0;quick_polls=0;quick_snapshot_ms=plat_ms();
        quick_snapshot_revision=state_revision;
        arrangement_snapshot(&quick_record);
        quick_pending=0;
        if(quick_begin(OBJ_BANK1+quick_slot,&quick_record,sizeof quick_record)){
            quick_last_result=-1;bank_message=BANK_ERROR;quick_finished_ms=plat_ms();
        }else{quick_started=1;bank_message=BANK_SAVING;}
        dirty=1;
    }
}
