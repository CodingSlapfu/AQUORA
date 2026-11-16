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

static void check_effects(void)
{
    arrangement bank;
    sr_init(&engine,123);
    engine.filter_cutoff=7;
    engine.filter_resonance=15;
    engine.slicer_mask=7;
    arrangement_snapshot(&bank);
    assert(arrangement_valid(&bank));
    sr_init(&engine,123);
    arrangement_apply(&bank);
    assert(engine.filter_cutoff==7&&engine.filter_resonance==15&&engine.slicer_mask==7);
    engine.running=1;
    unsigned pattern=slicer_pattern(&engine);
    unsigned closed=0;
    while((pattern>>closed)&1u)closed++;
    engine.sequence_step=closed+1u;
    static float dry[3*64];
    for(unsigned block=0;block<5;block++)render_samples(&engine,dry,64);
    assert(engine.slicer_gain[1]<.20f&&engine.slicer_gain[2]<.20f);
    engine.slicer_mask=0;
    for(unsigned block=0;block<5;block++)render_samples(&engine,dry,64);
    assert(engine.slicer_gain[1]==1.f&&engine.slicer_gain[2]==1.f);
    bank.reserved=0;
    bank.crc8=arrangement_crc(&bank);
    arrangement_apply(&bank);
    assert(!engine.filter_cutoff&&!engine.filter_resonance&&!engine.slicer_mask);
}

static void check_patch_registers(void)
{
    static int16_t pcm[512];
    for(unsigned patch=0;patch<SR_PATCHES;patch++){
        for(unsigned note=45;note<=81;note+=18){
            sr_init(&engine,123);
            engine.tracks[SR_LEAD].patch=patch;
            engine.filter_cutoff=15;
            engine.filter_resonance=15;
            engine.preview_frames=64u*256u;
            engine.preview_role=SR_LEAD;
            assert(note_on(&engine,SR_LEAD,note,110,.5f));
            unsigned peak=0;
            for(unsigned block=0;block<64;block++){
                sr_render(&engine,pcm,256);
                for(unsigned i=0;i<512;i++){
                    unsigned sample=pcm[i]<0?-pcm[i]:pcm[i];
                    if(sample>peak)peak=sample;
                }
            }
            assert(peak>10&&peak<32767);
        }
    }
}

int main(void)
{
    check_drum_mutes();
    check_speaker();
    check_effects();
    check_patch_registers();
    for(unsigned patch=0;patch<SR_PATCHES;patch++){
        assert(fm_operators[patch].ratio>0&&fm_operators[patch].depth>0);
        sr_track track={.patch=patch};
        for(unsigned phase=0;phase<32;phase++)
            assert(isfinite(sr_patch_preview(&track,phase/32.f)));
    }
    assert(SR_SCENES==22&&sr_frequencies[sr_scenes[21].frequency].hz==726.f);
    puts("PASS drum bank migration and silence, speaker mono/EQ and binaural recovery, filter/slicer recall and bounds, 63 FM patches in three registers, Fortune reference");
}
