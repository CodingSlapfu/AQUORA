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
    float difference=0.f;
    for(unsigned frame=0;frame<32768;frame+=64){
        float mix[64*3]={0};
        for(unsigned i=0;i<64;i++)
            mix[i*3+2]=(frame+i<4096u)?((i&16u)?.7f:-.7f):0.f;
        apply_fx(&engine,mix,64);
        for(unsigned i=0;i<64;i++)for(unsigned ch=0;ch<2;ch++){
            float sample=mix[i*3+ch];
            assert(isfinite(sample)&&fabsf(sample)<2.f);
            if(!effect)baseline[frame+i][ch]=sample;
            else difference+=fabsf(sample-baseline[frame+i][ch]);
        }
        engine.frame_count+=64;
    }
    if(effect)assert(difference>1.f);
}

int main(void)
{
    for(unsigned effect=0;effect<=4;effect++)check_texture(effect);
    puts("PASS independent bounded Mixer chorus, autopan, filtering and bitcrush");
}
