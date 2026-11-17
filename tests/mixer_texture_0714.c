#include <assert.h>
#include <stdio.h>
#include "../device/dsp/engine.c"

static sr_engine engine;
static float baseline[32768][2];

static void check_texture(unsigned effect)
{
    sr_init(&engine,714);
    engine.isolate=SR_ISOLATE_MIX;
    engine.fx_mask=3;
    engine.reverb_amount=.8f;
    engine.delay_amount=.8f;
    if(effect==1)engine.fx_chorus=100;
    if(effect==2)engine.fx_autopan=100;
    if(effect==3)engine.fx_filter=100;
    if(effect==4)engine.fx_crush=100;
    if(effect==5)engine.fx_flanger=100;
    if(effect==6)engine.fx_tremolo=100;
    if(effect==7)engine.fx_drive=100;
    if(effect==8)engine.fx_stutter=100;
    if(effect==9)engine.fx_shimmer=100;
    if(effect==10)engine.fx_granular=100;
    float difference=0.f;
    for(unsigned frame=0;frame<32768;frame+=64){
        float mix[64*4]={0};
        for(unsigned i=0;i<64;i++)
            mix[i*4+3]=mix[i*4+2]=(frame+i<4096u)?((i&16u)?.7f:-.7f):0.f;
        apply_fx(&engine,mix,64);
        for(unsigned i=0;i<64;i++)for(unsigned ch=0;ch<2;ch++){
            float sample=mix[i*4+ch];
            assert(isfinite(sample)&&fabsf(sample)<2.f);
            if(!effect)baseline[frame+i][ch]=sample;
            else difference+=fabsf(sample-baseline[frame+i][ch]);
        }
        engine.frame_count+=64;
    }
    if(effect)assert(difference>1.f);
}

static void check_granular_hold(void)
{
    sr_init(&engine,714);
    engine.fx_granular=100;
    float mix[64*4];
    for(unsigned block=0;block<80;block++){
        memset(mix,0,sizeof mix);
        if(block<16)
            for(unsigned i=0;i<64;i++)mix[4*i+3]=mix[4*i+2]=i<32?.26f:-.26f;
        apply_fx(&engine,mix,64);
    }
    assert(engine.fx_grain_written==1024);
    float held=0;
    for(unsigned i=0;i<64;i++)held+=fabsf(mix[i*4]);
    assert(held>.5f);
    engine.fx_granular=0;
    for(unsigned block=0;block<128;block++){
        memset(mix,0,sizeof mix);
        apply_fx(&engine,mix,64);
    }
    assert(engine.fx_grain_written==0&&fabsf(mix[0])<.0001f);
}

static void check_ducking(void)
{
    sr_init(&engine,714);
    engine.fx_duck=100;
    float mix[64*4];
    for(unsigned block=0;block<64;block++){
        memset(mix,0,sizeof mix);
        for(unsigned i=0;i<64;i++)mix[i*4]=mix[i*4+1]=.6f;
        apply_fx(&engine,mix,64);
        assert(mix[0]==.6f&&mix[1]==.6f);
    }
    assert(engine.fx_duck_gain<.2f);
    for(unsigned block=0;block<800;block++){
        memset(mix,0,sizeof mix);
        apply_fx(&engine,mix,64);
    }
    assert(engine.fx_duck_gain>.98f);
}

int main(void)
{
    for(unsigned effect=0;effect<=10;effect++)check_texture(effect);
    check_granular_hold();
    check_ducking();
    puts("PASS independent bounded Mixer modulation and experimental effects");
}
