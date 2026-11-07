from pathlib import Path
import subprocess,sys
root=Path(__file__).resolve().parents[1];out=Path(sys.argv[1]);out.mkdir(exist_ok=True,parents=True)
s=(root/'vendor/fm1-base/bsp/boards/fm1/fm1_input.h').read_text();a=s.index('#define FM1__LIT(c)');b=s.index('#if FM1_LED_DIM_DIV',a)
c='#include <assert.h>\n#include <stdio.h>\nstatic unsigned fm1__aq_lit70,fm1__aq_lit50,fm1__br_lit=1,fm1__mid_ph;\nstatic unsigned fm1_led_aquora70[1]={2},fm1_led_aquora50[1]={4},fm1_led[1]={30},fm1_led_breath[1]={30},fm1_led_dim[1],fm1_led_mid[1]={30};\n'+s[a:b]+"""
int main(void){unsigned a=0,b=0,black=0,white=0;
for(unsigned f=0;f<10000;f++){a+=7;b+=5;fm1__aq_lit70=a>=10;if(fm1__aq_lit70)a-=10;fm1__aq_lit50=b>=10;if(fm1__aq_lit50)b-=10;unsigned lit=FM1__LIT(0);black+=(lit&2)!=0;white+=(lit&4)!=0;assert(lit&8);}
assert(black==7000&&white==5000);puts("PASS production scan macro: 70/50 masks override inherited full/mid/breath bits; buttons preserved; physical brightness owner-unverified");}
""";p=out/'led.c';p.write_text(c);subprocess.run(['gcc','-O2',str(p),'-o',str(out/'led')],check=True);subprocess.run([str(out/'led')],check=True)
