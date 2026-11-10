#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include "../vendor/fm1-base/device/aquora_air_source.h"
int main(void){aq_air_source s={0};uint8_t a[8]={0,6,1,2,3,4,5,6},b[8]={0,6,9,8,7,6,5,4};assert(aq_air_source_take(&s,a,8,10));unsigned hash=s.hash;for(unsigned n=20;n<5000;n+=40){assert(!aq_air_source_take(&s,b,8,n));assert(aq_air_source_take(&s,a,8,n+1));assert(s.hash==hash);}assert(!aq_air_source_take(&s,b,7,9000));assert(aq_air_source_take(&s,b,8,11000));assert(s.hash!=hash);puts("PASS advertiser identity held across interference; malformed reports rejected; source changes only after loss and changes calibration identity");}
