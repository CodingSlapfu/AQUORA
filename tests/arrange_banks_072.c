#define main previous_bank_main
#include "bank_070.c"
#undef main
int main(void){
 previous_bank_main();
 for(unsigned slot=0;slot<4;slot++){
  sr_init(&engine,72);sr_generate_song(&engine,44,0);sr_start(&engine);sr_arrange_queue(&engine,17,3);
  arrangement r,loaded;arrangement_snapshot(&r);assert(arrangement_valid(&r));
  assert(!plat_store_save(slot,&r,sizeof r)&&arrangement_read(slot,&loaded));sr_init(&engine,1);arrangement_apply(&loaded);
  assert(engine.arrange_next==18&&engine.arrange_length==3&&sr_arrange_section(&engine,1)==17);
  sr_start(&engine);for(unsigned n=0;n<256&&!engine.arrange_active;n++){trigger(&engine);memset(engine.scheduled,0,sizeof engine.scheduled);}
  assert(engine.arrange_active==3);arrangement_snapshot(&r);sr_init(&engine,2);arrangement_apply(&r);
  assert(engine.arrange_active==3&&sr_arrange_section(&engine,0)==17);
  r.version=AQ_BANK_V6;r.crc6=arrangement_crc(&r);
  assert(!plat_store_save(slot,&r,offsetof(arrangement,arrange_next))&&arrangement_read(slot,&loaded));
  arrangement_apply(&loaded);assert(!engine.arrange_next&&!engine.arrange_active);
 }
 puts("PASS four bank V7 queue and active-section roundtrips; V6 byte-size migration resets new fields");
}
