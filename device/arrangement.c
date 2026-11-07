/* SPDX-License-Identifier: MPL-2.0
 * Arrangement records contain musical state only. The platform store provides
 * two alternating CRC-protected sectors per object. The app CRC also guards the
 * versioned payload independently of the platform envelope.
 */
#define AQ_BANKS 4u
#define AQ_BANK_LEGACY 0x41514201u
#define AQ_BANK_V2 0x41514202u
#define AQ_BANK_V3 0x41514203u
#define AQ_BANK_VERSION 0x41514204u
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
    uint8_t frequency_auto, lock_textures; /* Legacy zero in the alignment padding. */
    struct {
        float volume, pan, send;
        uint8_t patch, mute;
        int8_t tone;
        uint8_t contour; /* Signed attack and release nibbles, zero for legacy banks. */
    } track[SR_TRACKS];
    sr_motion motion[2];
    float reverb_amount, delay_amount, bed_level, drum_level;
    float binaural_carrier, binaural_rate, binaural_level;
    float tone_level, energy, variation, sacred_level, sacred_harmonics;
    uint32_t legacy_crc; /* v2 packs sound locks and encoded Drum Presence here. */
    uint8_t loop_length, keyboard_mode, tonic_override, pending_length, pending_key, frequency_lock, auto_gain, auto_level;
    uint32_t v2_crc;
    int8_t motif_home[16];
    uint32_t crc; /* original v3 boundary */
    uint32_t song_seed;
    uint8_t music_mode, motion_manual;
    uint16_t reserved;
    uint32_t crc4;
} arrangement;
_Static_assert(sizeof(arrangement)<512,"Arrangement record budget");

static unsigned visual_mode, bank_slot, bank_action, bank_confirm;
static unsigned bank_status[AQ_BANKS], bank_message;
enum { BANK_IDLE, BANK_STOP, BANK_EMPTY, BANK_CONFIRM, BANK_SAVED,
       BANK_LOADED, BANK_ERROR, BANK_SAVING };

static uint32_t arrangement_crc(const arrangement *record)
{
    const uint8_t *bytes=(const uint8_t *)record;
    uint32_t crc=0xffffffffu;
    size_t end=record->version==AQ_BANK_LEGACY?offsetof(arrangement,legacy_crc):
        record->version==AQ_BANK_V2?offsetof(arrangement,v2_crc):
        record->version==AQ_BANK_V3?offsetof(arrangement,crc):offsetof(arrangement,crc4);
    for(size_t i=0;i<end;i++){
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
    int legacy=r->version==AQ_BANK_LEGACY;
    if((!legacy&&r->version!=AQ_BANK_V2&&r->version!=AQ_BANK_V3&&r->version!=AQ_BANK_VERSION)||
       (legacy?r->legacy_crc:r->version==AQ_BANK_V2?r->v2_crc:r->version==AQ_BANK_V3?r->crc:r->crc4)!=arrangement_crc(r)||
       (!legacy&&((r->loop_length!=16&&r->loop_length!=32&&r->loop_length!=64)||
        (r->pending_length&&r->pending_length!=16&&r->pending_length!=32&&r->pending_length!=64)||
        r->keyboard_mode>1||r->pending_key>12||r->frequency_lock>1||r->auto_gain>1||r->auto_level>1||
        (r->tonic_override&&(r->tonic_override<36||r->tonic_override>47))))||
       (r->version==AQ_BANK_VERSION&&(r->music_mode>=SR_MUSIC_MODES||r->motion_manual>3))||
       r->scene>=SR_SCENES||r->frequency>=SR_FREQUENCIES||r->bed_type>=SR_BEDS||
       r->drum_kit>=SR_DRUM_KITS||r->drum_density>SR_DENSITY_MAX||r->solo>SR_TRACKS||
       r->frequency_auto>1||r->lock_textures>1||r->sacred_mode>2||r->visual>1||r->tick_length<5512||r->tick_length>22050||
       r->tuning_432>1||r->drums_enabled>1||r->binaural_enabled>1||
       r->freeze>1||r->perfect_key>1)return 0;
    if(!range(r->reverb_amount,0,.8f)||!range(r->delay_amount,0,.7f)||
       !range(r->bed_level,0,1)||!range(r->drum_level,0,1)||
       !range(r->binaural_carrier,80,1000)||!range(r->binaural_rate,.5f,40)||
       !range(r->binaural_level,0,.10f)||!range(r->tone_level,0,.10f)||
       !range(r->energy,0,1)||!range(r->variation,0,1)||
       !range(r->sacred_level,0,.20f)||!range(r->sacred_harmonics,0,1))return 0;
    for(unsigned i=0;i<16;i++){
        if(r->melody[i]<-1||r->melody[i]>6)return 0;
        if(r->version>=AQ_BANK_V3&&(r->motif_home[i]<-1||r->motif_home[i]>6))return 0;
    }
    for(unsigned i=0;i<SR_TRACKS;i++){
        if((r->version>=AQ_BANK_V3&&(r->track[i].tone<-7||r->track[i].tone>7||
            (r->track[i].contour&15)==8||(r->track[i].contour>>4)==8))||r->track[i].patch>=SR_PATCHES||r->track[i].mute>1||
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
    memset(record,0,sizeof(*record));
    int size=plat_store_size(OBJ_BANK1+slot);
    if(size!=(int)sizeof(*record)&&size!=(int)offsetof(arrangement,song_seed)&&size!=(int)offsetof(arrangement,motif_home)
        &&size!=(int)offsetof(arrangement,loop_length))return 0;
    if(plat_store_load(OBJ_BANK1+slot,record,sizeof(*record))!=size)return 0;
    unsigned version=size==(int)sizeof(*record)?AQ_BANK_VERSION:
        size==(int)offsetof(arrangement,song_seed)?AQ_BANK_V3:size==(int)offsetof(arrangement,motif_home)?AQ_BANK_V2:AQ_BANK_LEGACY;
    if(record->version!=version)return 0;
    return arrangement_valid(record);
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
    record->v2_crc=(engine.play_strength+1u)|((uint32_t)engine.play_feel<<7)|((uint32_t)engine.nature_style<<8)|((uint32_t)engine.evolve_pace<<10)|((uint32_t)engine.nature_level_manual<<12)|((uint32_t)(engine.nature_activity+1u)<<13)|((uint32_t)(engine.nature_distance+1u)<<16);
    record->legacy_crc=engine.sound_locks|((uint32_t)(engine.drum_presence+1u)<<10)|((uint32_t)(engine.play_grid&3u)<<17)|((uint32_t)(engine.play_grid>>2)<<28)|((uint32_t)engine.groove_mode<<19)|((uint32_t)(engine.groove_strength+1u)<<21);
#define COPY_FIELD(field) record->field=engine.field;
    AQ_BANK_FIELDS(COPY_FIELD)
#undef COPY_FIELD
    record->visual=visual_mode;
    memcpy(record->melody,engine.melody,sizeof(record->melody));
    memcpy(record->motif_home,engine.motif_home,sizeof(record->motif_home));
    memcpy(record->motion,engine.motion,sizeof(record->motion));
    for(unsigned i=0;i<SR_TRACKS;i++){
        record->track[i].volume=engine.tracks[i].volume;
        record->track[i].pan=engine.tracks[i].pan;
        record->track[i].send=engine.tracks[i].send;
        record->track[i].patch=engine.tracks[i].patch;
        record->track[i].mute=engine.tracks[i].mute;
        record->track[i].tone=engine.tracks[i].tone;
        record->track[i].contour=(engine.tracks[i].attack&15)|((engine.tracks[i].release&15)<<4);
    }
    record->frequency_auto=engine.frequency_auto;
    record->lock_textures=engine.lock_textures;
    record->loop_length=engine.loop_length;
    record->pending_length=engine.pending_length;
    record->keyboard_mode=engine.keyboard_mode;
    record->tonic_override=engine.tonic_override;
    record->pending_key=engine.pending_key;
    record->frequency_lock=engine.frequency_lock;
    record->auto_gain=engine.auto_gain;
    record->auto_level=engine.auto_level;
    plat_unlock(irq);
    record->song_seed=engine.song_seed;record->music_mode=engine.music_mode;record->motion_manual=engine.motion_manual;
    record->version=AQ_BANK_V3;record->crc=arrangement_crc(record);
    record->version=AQ_BANK_VERSION;record->crc4=arrangement_crc(record);
}

static void arrangement_apply(const arrangement *record)
{
    unsigned irq=plat_lock();
    unsigned off=!engine.bed_type&&(engine.sound_locks&SR_SOUND_NATURE);
#define COPY_FIELD(field) engine.field=record->field;
    AQ_BANK_FIELDS(COPY_FIELD)
#undef COPY_FIELD
    engine.sound_locks=record->version==AQ_BANK_LEGACY?0:record->legacy_crc&SR_SOUND_LOCK_MASK;
    unsigned presence=record->version==AQ_BANK_LEGACY?0:(record->legacy_crc>>10)&127u;
    engine.drum_presence=presence&&presence<=101?presence-1:50;
    unsigned grid=record->version==AQ_BANK_LEGACY?3:((record->legacy_crc>>17)&3u)|((record->legacy_crc>>26)&4u);
    engine.play_grid=grid<=4?grid:3;
    unsigned key_timing=record->version>=AQ_BANK_V3?record->v2_crc:0;
    unsigned snap=key_timing&127u;
    engine.play_strength=snap&&snap<=101?snap-1u:100;
    engine.play_feel=(key_timing>>7)&1u;
    engine.nature_style=(key_timing>>8)&3u;
    engine.evolve_pace=(key_timing>>10)&3u;
    engine.nature_level_manual=(key_timing>>12)&1u;
    engine.evolve_next=0;
    unsigned activity=(key_timing>>13)&7u, distance=(key_timing>>16)&7u;
    engine.nature_activity=activity&&activity<=5?activity-1u:2;
    engine.nature_distance=distance&&distance<=5?distance-1u:2;
    if(off)engine.bed_type=0;
    if(!engine.bed_type)engine.sound_locks|=SR_SOUND_NATURE;
    unsigned groove=record->version==AQ_BANK_LEGACY?0:(record->legacy_crc>>19)&3u;
    unsigned strength=record->version==AQ_BANK_LEGACY?0:(record->legacy_crc>>21)&127u;
    engine.groove_mode=groove<=2?groove:0;
    engine.groove_strength=strength&&strength<=101?strength-1u:100;
    engine.play_queued=0;
    engine.frequency_auto=record->frequency_auto;
    engine.lock_textures=record->lock_textures;
    engine.loop_length=record->version==AQ_BANK_LEGACY?16:record->loop_length;
    engine.pending_length=record->pending_length;
    engine.loop_pos=0;
    engine.keyboard_mode=record->keyboard_mode;
    engine.tonic_override=record->tonic_override;
    engine.pending_key=record->pending_key;
    engine.pending_physical_key=0;
    engine.frequency_lock=engine.pending_lock=record->frequency_lock;
    engine.auto_gain=record->auto_gain;
    engine.auto_level=record->auto_level;
    if(engine.frequency_auto)engine.frequency=sr_scenes[engine.scene].frequency;
    engine.drum_hit=engine.drum_hit>SR_HIT_MAX?SR_HIT_MAX:engine.drum_hit;
    visual_mode=record->visual;
    memcpy(engine.melody,record->melody,sizeof(engine.melody));
    memcpy(engine.motif_home,record->version>=AQ_BANK_V3?record->motif_home:record->melody,
           sizeof(engine.motif_home));
    memset(engine.ornaments,0,sizeof engine.ornaments);
    memcpy(engine.motion,record->motion,sizeof(engine.motion));
    for(unsigned i=0;i<SR_TRACKS;i++){
        engine.tracks[i].volume=record->track[i].volume;
        engine.tracks[i].pan=record->track[i].pan;
        engine.tracks[i].send=record->track[i].send;
        engine.tracks[i].patch=record->track[i].patch;
        engine.tracks[i].mute=record->track[i].mute;
        unsigned contour=record->version>=AQ_BANK_V3?record->track[i].contour:0;
        int attack=contour&15, release=contour>>4;
        engine.tracks[i].tone=record->version>=AQ_BANK_V3?record->track[i].tone:0;
        engine.tracks[i].attack=attack<8?attack:attack-16;
        engine.tracks[i].release=release<8?release:release-16;
    }
    engine.song_seed=record->version==AQ_BANK_VERSION?record->song_seed:record->rng;
    engine.music_mode=record->version==AQ_BANK_VERSION?record->music_mode:SR_STANDARD;
    engine.motion_manual=record->version==AQ_BANK_VERSION?record->motion_manual:sr_legacy_motion_manual(record->motion,record->scene);
    engine.restart=0;
    qread=qwrite;
    state_revision++;
    plat_unlock(irq);
}

static void arrangement_execute(void)
{
    if(requested_play||engine.running||engine.stop_frames||engine.external_audio||qread!=qwrite){
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

