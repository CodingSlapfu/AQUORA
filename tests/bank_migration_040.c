#include "serenity.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define AQ_BANK_BASE 0x97000u
#define AQ_BANK_STRIDE 0x2000u
#define OBJ_BANK1 0u
static sr_engine engine;
static unsigned requested_play,qread,qwrite,state_revision;
static unsigned char flash[4][512];
static unsigned sizes[4];
static unsigned plat_lock(void){return 0;}
static void plat_unlock(unsigned token){(void)token;}
static int plat_store_size(unsigned slot){return sizes[slot];}
static int plat_store_load(unsigned slot,void *out,unsigned capacity)
{
    unsigned n=sizes[slot];
    if(n>capacity)n=capacity;
    memcpy(out,flash[slot],n);
    return n;
}
static int plat_store_save(unsigned slot,const void *data,unsigned n)
{
    assert(n<=sizeof(flash[slot]));
    memcpy(flash[slot],data,n);
    sizes[slot]=n;
    return 0;
}
#include AQ_ARRANGEMENT

int main(int argc,char **argv)
{
    assert(argc==2);
    FILE *file;
    sr_init(&engine,403);
#ifdef LEGACY_FIXTURE
    arrangement record;
    sr_set_scene(&engine,6);
    engine.frequency=7;
#ifdef AQ_BANK_LEGACY
    sr_set_frequency_auto(&engine,0);
#endif
    engine.tone_level=.035f;
    engine.tracks[SR_BASS].pan=-.75f;
    engine.motion[0]=(sr_motion){240,2,35,61};
    visual_mode=1;
    arrangement_snapshot(&record);
    file=fopen(argv[1],"wb");assert(file);
    assert(fwrite(&record,1,sizeof(record),file)==sizeof(record));
    printf("Archived firmware serializer wrote %zu-byte fixture\n",sizeof(record));
#else
    file=fopen(argv[1],"rb");assert(file);
    sizes[0]=fread(flash[0],1,sizeof(flash[0]),file);
    assert(sizes[0]==offsetof(arrangement,loop_length)||sizes[0]==sizeof(arrangement)||sizes[0]==offsetof(arrangement,motif_home));
    arrangement_init();
    assert(bank_status[0]==1);
    arrangement_execute();
    assert(bank_message==BANK_LOADED);
    assert(engine.scene==6&&engine.frequency==7&&!engine.frequency_auto);
    assert(engine.tone_level==.035f&&engine.tracks[SR_BASS].pan==-.75f);
    assert(engine.motion[0].glide_ms==240&&engine.motion[0].vibrato_depth==35);
    assert(visual_mode==1&&engine.loop_length==16&&!engine.keyboard_mode&&!engine.frequency_lock);
    assert(!engine.auto_gain&&!engine.auto_level&&!engine.sound_locks);
    assert(engine.drum_presence==50);

    uint32_t crc[4];
    static const unsigned lengths[]={16,32,64,32};
    for(unsigned slot=0;slot<4;slot++){
        sr_set_scene(&engine,9+slot);
        sr_set_frequency_auto(&engine,slot&1);
        if(!engine.frequency_auto)sr_set_frequency(&engine,6+slot);
        engine.keyboard_mode=slot&1;
        engine.play_grid=slot+1;
        engine.sound_locks=(1u<<slot)|SR_SOUND_NATURE;
        engine.drum_presence=slot*30u;
        engine.tonic_override=37+slot;
        engine.pending_key=1+slot;
        sr_set_loop_length(&engine,lengths[slot]);
        sr_set_frequency_lock(&engine,(slot>>1)&1);
        engine.lock_textures=slot==3;
        engine.auto_gain=slot&1;
        engine.auto_level=(slot>>1)&1;
        engine.reverb_amount=.12f*slot;
        engine.delay_amount=.07f*slot;
        engine.solo=slot;
        for(unsigned t=0;t<SR_TRACKS;t++){
            engine.tracks[t].volume=(t+slot)*.08f;
            engine.tracks[t].pan=(int)t*.2f-.7f;
            engine.tracks[t].send=(t+slot)*.07f;
            engine.tracks[t].mute=(t+slot)&1;
        }
        arrangement expected;
        arrangement_snapshot(&expected);
        crc[slot]=expected.crc;
        bank_slot=slot;bank_action=1;bank_confirm=1;
        arrangement_execute();
        assert(bank_message==BANK_SAVED);
    }
    sr_init(&engine,9);
    arrangement_init();
    for(unsigned slot=0;slot<4;slot++){
        bank_slot=slot;
        arrangement_execute();
        assert(bank_message==BANK_LOADED);
        arrangement actual;
        arrangement_snapshot(&actual);
        assert(actual.crc==crc[slot]);
        assert(engine.play_grid==slot+1);
    }
    puts("PASS archived record migration and four distinct 0.5.0 lock payloads across reboot");
#endif
    return fclose(file);
}
