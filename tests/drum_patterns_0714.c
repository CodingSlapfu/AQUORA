#define main legacy_bank_main
#include "bank_070.c"
#undef main

int main(void)
{
    legacy_bank_main();
    for(unsigned slot=0;slot<AQ_BANKS;slot++){
        sr_init(&engine,714);
        engine.drum_manual_mask=0xffffu;
        engine.drum_hit=0;
        for(unsigned lane=0;lane<16;lane++)engine.drum_pattern[lane]=1u<<lane;
        arrangement record,loaded;
        arrangement_snapshot(&record);
        assert(sizeof record==512 && arrangement_valid(&record));
        assert(!plat_store_save(slot,&record,sizeof record) && arrangement_read(slot,&loaded));
        sr_init(&engine,714);
        arrangement_apply(&loaded);
        assert(engine.drum_manual_mask==0xffffu);
        for(unsigned step=0;step<64;step++){
            drum_trigger(&engine,step,2);
            assert((engine.drum_steps[step]&0xffffu)==(1u<<(step&15u)));
        }
        flash[slot][offsetof(arrangement,drum_pattern)]^=1u;
        assert(!arrangement_read(slot,&loaded));

        record.version=AQ_BANK_V9;
        record.crc9=arrangement_crc(&record);
        assert(!plat_store_save(slot,&record,480) && arrangement_read(slot,&loaded));
        arrangement_apply(&loaded);
        assert(!engine.drum_manual_mask);
        for(unsigned lane=0;lane<16;lane++)assert(!engine.drum_pattern[lane]);
    }
    puts("PASS all 16 manual lanes, 64-step playback, four V10 bank round trips, V9 migration and corrupt payload rejection");
}
