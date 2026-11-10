#define main prior_main
#include "bank_070.c"
#undef main
int main(void){
 prior_main();
 for(unsigned slot=0;slot<4;slot++){
   sr_init(&engine,75);sr_generate_song(&engine,777,0);engine.energy=.81f;engine.current_energy=.49f;engine.response_enabled=1;engine.chord_mode=2;engine.chord_gap=220;
   for(unsigned n=0;n<16;n++)engine.melody[n]=n%4;remember_motif(&engine);
   arrangement r,loaded;arrangement_snapshot(&r);assert(sizeof r<512&&arrangement_valid(&r));
   assert(!plat_store_save(slot,&r,sizeof r)&&arrangement_read(slot,&loaded));
   sr_init(&engine,99);arrangement_apply(&loaded);
   assert(engine.motif_count==1&&engine.energy==.81f&&engine.current_energy==.49f&&engine.response_enabled&&engine.chord_gap==220);
   assert(!memcmp(engine.motif_memory,r.motif_memory,sizeof r.motif_memory));
   /* Actual previous 376-byte V7 object migrates, keeping old fields. */
   r.chord_gap=100;r.reserved6=0;r.version=AQ_BANK_V7;r.crc7=arrangement_crc(&r);
   assert(!plat_store_save(slot,&r,offsetof(arrangement,motif_memory))&&arrangement_read(slot,&loaded));
   arrangement_apply(&loaded);assert(!engine.motif_count&&!engine.response_enabled&&engine.chord_gap==100);
   r.version=AQ_BANK_VERSION;r.motif_count=5;r.crc8=arrangement_crc(&r);assert(!arrangement_valid(&r));
 }
 puts("PASS all four V8 banks: complete motif memory, intensity ramp and response; exact V7-size migration and corrupt-memory rejection");
}
