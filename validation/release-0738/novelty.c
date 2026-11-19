#include <stdio.h>
#include "engine.c"

static sr_engine engine;
typedef struct {
    uint16_t melody, rhythm, harmony;
    uint8_t form[32];
} song_signature;

static song_signature signature(void)
{
    song_signature score={0};
    score.melody=motif_signature(engine.motif_home);
    for(unsigned i=0;i<16u;i++)
        if(engine.motif_home[i]>=0)score.rhythm|=1u<<i;
    for(unsigned bar=0;bar<8u;bar++)
        score.harmony=(uint16_t)(31u*score.harmony+phrase_chord(&engine,bar));
    memcpy(score.form,engine.form_route,sizeof score.form);
    return score;
}

int main(void)
{
    unsigned total=0,same_melody=0,same_rhythm=0,same_harmony=0,same_form=0,all_same=0;
    for(unsigned mode=0;mode<SR_MUSIC_MODES;mode++){
        for(unsigned scene=0;scene<SR_SCENES;scene++){
            sr_init(&engine,7338u);
            sr_set_music_mode(&engine,mode);
            sr_set_scene(&engine,scene);
            song_signature prior=signature();
            for(unsigned i=1;i<=12u;i++){
                sr_generate_song(&engine,7338u+i*701u,0);
                song_signature now=signature();
                unsigned melody=now.melody==prior.melody;
                unsigned rhythm=now.rhythm==prior.rhythm;
                unsigned harmony=now.harmony==prior.harmony;
                unsigned form=!memcmp(now.form,prior.form,sizeof now.form);
                total++;same_melody+=melody;same_rhythm+=rhythm;same_harmony+=harmony;same_form+=form;
                all_same+=melody&&rhythm&&harmony&&form;
                prior=now;
            }
        }
    }
    printf("%u adjacent New Song pairs across %u intentions and %u modes\n",total,SR_SCENES,SR_MUSIC_MODES);
    printf("Unchanged component fingerprints: contour/onset %u, onset mask %u, first eight chord degrees %u, 32-section form %u\n",
           same_melody,same_rhythm,same_harmony,same_form);
    printf("Unchanged across all four fingerprints: %u\n",all_same);
    return all_same!=0;
}
