#define main previous_bank_main
#include "bank_070.c"
#undef main

static void check_drum_mutes(void)
{
    arrangement bank;
    sr_init(&engine,67);
    engine.drum_mutes=7;
    arrangement_snapshot(&bank);
    assert(arrangement_valid(&bank));
    engine.drum_mutes=0;
    arrangement_apply(&bank);
    assert(engine.drum_mutes==7);
    engine.kick_env=engine.hand_env=engine.shaker_env=1.f;
    for(unsigned i=0;i<512;i++)assert(drum_sample(&engine)==0.f);

    bank.future=0;
    bank.crc8=arrangement_crc(&bank);
    arrangement_apply(&bank);
    assert(engine.drum_mutes==0);
}

static void check_speaker(void)
{
    sr_init(&engine,67);
    engine.running=engine.binaural_enabled=1;
    engine.speaker_enabled=1;
    static float dry[3*1024];
    static int16_t out[2*1024];
    for(unsigned i=0;i<1024;i++){
        dry[3*i]=.25f;
        dry[3*i+1]=.125f;
    }
    for(unsigned j=0;j<8;j++)output_samples(&engine,dry,out,1024);
    assert(engine.speaker_mix==1.f);
    assert(engine.binaural_env==0.f);
    assert(out[2046]==out[2047]&&out[2046]>0&&out[2046]<1000);
    engine.speaker_enabled=0;
    for(unsigned j=0;j<8;j++)output_samples(&engine,dry,out,1024);
    assert(engine.speaker_mix==0.f);
    assert(engine.binaural_env>0.9f);
    assert(out[2046]!=out[2047]);
}

int main(void)
{
    check_drum_mutes();
    check_speaker();
    for(unsigned patch=0;patch<SR_PATCHES;patch++){
        assert(fm_operators[patch].ratio>0&&fm_operators[patch].depth>0);
        sr_track track={.patch=patch};
        for(unsigned phase=0;phase<32;phase++)
            assert(isfinite(sr_patch_preview(&track,phase/32.f)));
    }
    assert(SR_SCENES==22&&sr_frequencies[sr_scenes[21].frequency].hz==726.f);
    puts("PASS drum bank migration and silence, speaker mono/EQ and binaural recovery, 63 FM paths, Fortune reference");
}
