/* SPDX-License-Identifier: MPL-2.0 */
static arrangement quick_record;
static unsigned quick_pending, quick_slot, quick_started, quick_finished_ms;
static unsigned quick_saved_revision, quick_has_save, quick_snapshot_revision;
#ifdef PLAT_HOST
static int quick_begin(unsigned obj,const void *data,unsigned len)
{(void)obj;(void)data;(void)len;return 0;}
static int quick_poll(void)
{return plat_store_save(OBJ_BANK1+quick_slot,&quick_record,sizeof(quick_record));}
#else
extern int plat_aquora_save_begin(uint32_t,const void *,uint32_t);
extern int plat_aquora_save_poll(void);
#define quick_begin plat_aquora_save_begin
#define quick_poll plat_aquora_save_poll
#endif
/* Input only requests a snapshot. Main-loop service waits for the audio-owned
 * command queue to drain; snapshot copies one consistent state under IRQ lock.
 * A second press during a transaction coalesces into one latest-state request.
 */
static void arrangement_quick_save(void)
{
    quick_pending=1;
    bank_message=BANK_SAVING;
    dirty=1;
}
static void arrangement_service(void)
{
    if(quick_started){
        int result=quick_poll();
        if(result==1)return;
        quick_started=0;
        bank_message=result==0?BANK_SAVED:BANK_ERROR;
        if(!result){
            bank_status[quick_slot]=1;
            quick_has_save=1;
            quick_saved_revision=quick_snapshot_revision;
        }
        quick_finished_ms=plat_ms();dirty=1;
    }
    if(quick_pending&&!quick_started&&qread==qwrite){
        quick_slot=bank_slot;
        quick_snapshot_revision=state_revision;
        arrangement_snapshot(&quick_record);
        quick_pending=0;
        if(quick_begin(OBJ_BANK1+quick_slot,&quick_record,sizeof quick_record)){
            bank_message=BANK_ERROR;quick_finished_ms=plat_ms();
        }else{quick_started=1;bank_message=BANK_SAVING;}
        dirty=1;
    }
}
