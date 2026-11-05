/* Targeted oscillator regression; includes implementation to test the exact
 * bounded binaural kernel for three hours without rendering unrelated DSP. */
#include "../device/dsp/engine.c"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static sr_engine e;
static int16_t pcm[512];
static void setup(float carrier,float beat){
  sr_init(&e,456);sr_start(&e);
  e.isolate=SR_ISOLATE_BINAURAL;e.bed_level=0;
  e.reverb_amount=.8f;e.delay_amount=.7f;
  sr_set_binaural_carrier(&e,carrier);sr_set_binaural_rate(&e,beat);
  sr_set_binaural(&e,1);
  for(unsigned b=0;b<400;b++)sr_render(&e,pcm,256);
  assert(e.carrier_now==carrier&&e.beat_now==beat&&e.binaural_env==1.f);
}
static void frequencies(float carrier,float beat){
  setup(carrier,beat);
  unsigned crossings[2]={0};int armed[2]={0};
  for(unsigned b=0;b<1400;b++){
    sr_render(&e,pcm,256);
    for(unsigned i=0;i<256;i++)for(unsigned c=0;c<2;c++){
      int v=pcm[2*i+c];if(v < -8)armed[c]=1;
      if(armed[c]&&v>8){crossings[c]++;armed[c]=0;}
    }
  }
  float duration=1400.f*256.f/SR_RATE;
  float l=crossings[0]/duration,r=crossings[1]/duration;

  assert(fabsf(l-(carrier-beat*.5f))<.14f);
  assert(fabsf(r-(carrier+beat*.5f))<.14f);
  assert(fabsf((r-l)-beat)<.25f);
  printf("carrier %.1f beat %.1f: measured L %.4f R %.4f Hz\n",carrier,beat,l,r);
  sr_set_binaural(&e,0);
  for(unsigned b=0;b<200;b++)sr_render(&e,pcm,256);
  assert(e.binaural_env==0);
  for(unsigned i=0;i<512;i++)assert(pcm[i]==0);
}
int main(int argc,char **argv){
  for(unsigned c=0;c<3;c++)for(unsigned b=0;b<3;b++){
    const float carrier[]={80.f,432.f,1000.f},beat[]={.5f,6.f,40.f};
    frequencies(carrier[c],beat[b]);
  }
  setup(220.f,6.f);
  for(unsigned n=0;n<100;n++){
    sr_set_binaural(&e,n&1);sr_set_binaural_carrier(&e,n&1?1000.f:80.f);
    sr_set_binaural_rate(&e,n&1?.5f:40.f);sr_render(&e,pcm,256);
    for(unsigned j=2;j<512;j++)assert(abs(pcm[j]-pcm[j-2])<1200);
  }
  e.solo=SR_LEAD;
  for(unsigned b=0;b<200;b++)sr_render(&e,pcm,256);
  assert(e.binaural_env==0);
  if(argc>1&&argv[1][0]=='l'){
    setup(1000.f,.5f);
    const uint32_t frames=3u*3600u*SR_RATE;
    unsigned wraps_l=0,wraps_r=0;
    float start_l=e.binaural_phase_l,start_r=e.binaural_phase_r;
    binaural_block plan=prepare_binaural(&e);assert(plan.steady);
    for(uint32_t i=0;i<frames;i++){
      float old_l=e.binaural_phase_l,old_r=e.binaural_phase_r,l=0,r=0;
      render_binaural(&e,&plan,&l,&r);
      wraps_l+=e.binaural_phase_l<old_l;wraps_r+=e.binaural_phase_r<old_r;
      assert(isfinite(l)&&isfinite(r));
    }
    double l=(wraps_l+e.binaural_phase_l-start_l)/10800.;
    double r=(wraps_r+e.binaural_phase_r-start_r)/10800.;
    assert(fabsf((float)(l-999.75))<.01&&fabsf((float)(r-1000.25))<.01);
    assert(fabsf((float)((r-l)-.5))<.003);
    printf("PASS actual 3h binaural kernel: L %.6f R %.6f difference %.6f Hz\n",l,r,r-l);
  }
  puts("PASS carrier/beat extremes, exact settling, FX isolation, off silence, solo fade and rapid sweeps");
}
