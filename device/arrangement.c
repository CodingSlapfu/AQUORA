/* SPDX-License-Identifier: MPL-2.0
 * Arrangement records contain musical state only. The platform store provides
 * two alternating CRC-protected sectors per object. The app CRC also guards the
 * versioned payload independently of the platform envelope.
 */
#define AQ_BANKS 4u
#define AQ_BANK_VERSION 0x41514201u
_Static_assert(AQ_BANK_BASE >= 0x97000u && AQ_BANK_BASE + AQ_BANKS * AQ_BANK_STRIDE <= 0xA0000u,
               "Bank sector pairs must stay in the documented data region");

#define AQ_BANK_FIELDS(X) \
 X(scene) X(frequency) X(tuning_432) X(bed_type) X(drums_enabled) \
 X(drum_density) X(drum_kit) X(drum_hit) X(binaural_enabled) \
 X(freeze) X(perfect_key) X(solo) X(sacred_mode) X(tick_length) \
 X(rng) X(drum_rng) X(phrase) X(reverb_amount) X(delay_amount) \
 X(bed_level) X(drum_level) X(binaural_carrier) X(binaural_rate) \
 X(binaural_level) X(tone_level) X(energy) X(variation) \
 X(sacred_level) X(sacred_harmonics)

typedef struct {
    uint32_t version;
    uint32_t rng, drum_rng, phrase, tick_length;
    uint8_t scene, frequency, tuning_432, bed_type, drums_enabled;
    uint8_t drum_density, drum_kit, drum_hit, binaural_enabled;
    uint8_t freeze, perfect_key, solo, sacred_mode, visual;
    int8_t melody[16];
    uint8_t frequency_auto; /* Legacy zero in the existing alignment padding. */
    struct {
        float volume, pan, send;
        uint8_t patch, mute;
    } track[SR_TRACKS];
    sr_motion motion[2];
    float reverb_amount, delay_amount, bed_level, drum_level;
    float binaural_carrier, binaural_rate, binaural_level;
    float tone_level, energy, variation, sacred_level, sacred_harmonics;
    uint32_t crc;
} arrangement;
_Static_assert(sizeof(arrangement)<512,"Arrangement record budget");

static unsigned visual_mode, bank_slot, bank_action, bank_confirm;
static unsigned bank_status[AQ_BANKS], bank_message;
enum { BANK_IDLE, BANK_STOP, BANK_EMPTY, BANK_CONFIRM, BANK_SAVED,
       BANK_LOADED, BANK_ERROR };

static uint32_t arrangement_crc(const arrangement *record)
{
    const uint8_t *bytes=(const uint8_t *)record;
    uint32_t crc=0xffffffffu;
    for(size_t i=0;i<offsetof(arrangement,crc);i++){
        crc^=bytes[i];
        for(unsigned j=0;j<8;j++)crc=(crc>>1)^((0u-(crc&1u))&0xedb88320u);
    }
    return ~crc;
}

static int range(float value,float lo,float hi)
{
    return value>=lo&&value<=hi;
}

static int arrangement_valid(const arrangement *r)
{
    if(r->version!=AQ_BANK_VERSION||r->crc!=arrangement_crc(r)||
       r->scene>=SR_SCENES||r->frequency>=SR_FREQUENCIES||r->bed_type>7||
       r->drum_kit>=SR_DRUM_KITS||r->drum_density>3||r->solo>SR_TRACKS||
       r->frequency_auto>1||r->sacred_mode>2||r->visual>1||r->tick_length<5512||r->tick_length>22050||
       r->tuning_432>1||r->drums_enabled>1||r->binaural_enabled>1||
       r->freeze>1||r->perfect_key>1)return 0;
    if(!range(r->reverb_amount,0,.8f)||!range(r->delay_amount,0,.7f)||
       !range(r->bed_level,0,1)||!range(r->drum_level,0,1)||
       !range(r->binaural_carrier,80,1000)||!range(r->binaural_rate,.5f,40)||
       !range(r->binaural_level,0,.10f)||!range(r->tone_level,0,.10f)||
       !range(r->energy,0,1)||!range(r->variation,0,1)||
       !range(r->sacred_level,0,.20f)||!range(r->sacred_harmonics,0,1))return 0;
    for(unsigned i=0;i<16;i++)if(r->melody[i]<-1||r->melody[i]>6)return 0;
    for(unsigned i=0;i<SR_TRACKS;i++){
        if(r->track[i].patch>=SR_PATCHES||r->track[i].mute>1||
           !range(r->track[i].volume,0,1)||!range(r->track[i].pan,-1,1)||
           !range(r->track[i].send,0,1))return 0;
    }
    for(unsigned i=0;i<2;i++){
        sr_motion m=r->motion[i];
        if(m.glide_mode>2||m.glide_ms<10||m.glide_ms>800||
           m.vibrato_rate<5||m.vibrato_rate>80||m.vibrato_depth>50)return 0;
    }
    return 1;
}

static int arrangement_read(unsigned slot,arrangement *record)
{
    return plat_store_load(OBJ_BANK1+slot,record,sizeof(*record))==(int)sizeof(*record)
           && arrangement_valid(record);
}

static void arrangement_init(void)
{
    bank_slot=bank_action=bank_confirm=bank_message=visual_mode=0;
    for(unsigned i=0;i<AQ_BANKS;i++){
        arrangement record;
        bank_status[i]=arrangement_read(i,&record)?1:
            plat_store_size(OBJ_BANK1+i)>0?2:0;
    }
}

static void arrangement_snapshot(arrangement *record)
{
    memset(record,0,sizeof(*record));
    unsigned irq=plat_lock();
    record->version=AQ_BANK_VERSION;
#define COPY_FIELD(field) record->field=engine.field;
    AQ_BANK_FIELDS(COPY_FIELD)
#undef COPY_FIELD
    record->visual=visual_mode;
    memcpy(record->melody,engine.melody,sizeof(record->melody));
    memcpy(record->motion,engine.motion,sizeof(record->motion));
    for(unsigned i=0;i<SR_TRACKS;i++){
        record->track[i].volume=engine.tracks[i].volume;
        record->track[i].pan=engine.tracks[i].pan;
        record->track[i].send=engine.tracks[i].send;
        record->track[i].patch=engine.tracks[i].patch;
        record->track[i].mute=engine.tracks[i].mute;
    }
    record->frequency_auto=engine.frequency_auto;
    plat_unlock(irq);
    record->crc=arrangement_crc(record);
}

static void arrangement_apply(const arrangement *record)
{
    unsigned irq=plat_lock();
#define COPY_FIELD(field) engine.field=record->field;
    AQ_BANK_FIELDS(COPY_FIELD)
#undef COPY_FIELD
    engine.frequency_auto=record->frequency_auto;
    if(engine.frequency_auto)engine.frequency=sr_scenes[engine.scene].frequency;
    engine.drum_hit=engine.drum_hit>SR_HIT_MAX?SR_HIT_MAX:engine.drum_hit;
    visual_mode=record->visual;
    memcpy(engine.melody,record->melody,sizeof(engine.melody));
    memcpy(engine.motion,record->motion,sizeof(engine.motion));
    for(unsigned i=0;i<SR_TRACKS;i++){
        engine.tracks[i].volume=record->track[i].volume;
        engine.tracks[i].pan=record->track[i].pan;
        engine.tracks[i].send=record->track[i].send;
        engine.tracks[i].patch=record->track[i].patch;
        engine.tracks[i].mute=record->track[i].mute;
    }
    engine.restart=0;
    qread=qwrite;
    state_revision++;
    plat_unlock(irq);
}

static void arrangement_execute(void)
{
    if(requested_play||engine.running||engine.stop_frames||qread!=qwrite){
        bank_message=BANK_STOP;
        return;
    }
    arrangement record;
    if(!bank_action){
        if(!bank_status[bank_slot])bank_message=BANK_EMPTY;
        else if(!arrangement_read(bank_slot,&record))bank_message=BANK_ERROR;
        else{
            arrangement_apply(&record);
            bank_message=BANK_LOADED;
        }
    }else if(bank_status[bank_slot]&&!bank_confirm)bank_message=BANK_CONFIRM;
    else{
        arrangement_snapshot(&record);
        if(plat_store_save(OBJ_BANK1+bank_slot,&record,sizeof(record))==0){
            bank_status[bank_slot]=1;
            bank_message=BANK_SAVED;
        }else bank_message=BANK_ERROR;
    }
    bank_confirm=0;
}
