"""Exercise the production RGB565 strip cache against a simulated LCD."""
from pathlib import Path
import subprocess
import tempfile

ui = Path("device/ui.c").read_text()
cache = ui[ui.index("static uint16_t panel_pixels"):ui.index("static unsigned graph_future_count;")]
harness = r"""
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define PLAT_POOL
#define APP_CANVAS_DOUBLE_BUFFER 1
static uint16_t canvas[240*48],actual[240*240];
static uint16_t *cv_px=canvas,*cv_last_sent;
static unsigned transmissions,height;
static void lcd_blit(unsigned x,unsigned y,unsigned w,unsigned h,const uint16_t *pixels){
    assert(x==0&&w==240&&y+h<=240);
    memcpy(actual+y*240,pixels,h*240*sizeof *pixels);
    height=h;
    transmissions++;
}
"""
runner = r"""
int main(void){
    static uint16_t expected[240*240];
    for(unsigned frame=0;frame<40;frame++){
        for(unsigned band=0;band<5;band++){
            uint16_t *rows=expected+band*240*48;
            memcpy(canvas,rows,sizeof canvas);
            if(frame==0){
                for(unsigned i=0;i<240*48;i++)canvas[i]=(uint16_t)(i+band);
            }else if((band+frame)%3==0){
                unsigned stripe=(frame+band)%6;
                canvas[stripe*240*8+frame]^=(uint16_t)(1u<<(frame%16));
            }
            unsigned motion=frame==14 || frame==15;
            unsigned full=motion || frame==16 || frame==0;
            unsigned changed=full || memcmp(rows,canvas,sizeof canvas)!=0;
            unsigned before=transmissions;
            assert(submit_panel_band(band,motion)==changed);
            assert(transmissions-before==changed);
            if(changed)assert(height==(full?48u:8u)&&cv_last_sent==canvas);
            memcpy(rows,canvas,sizeof canvas);
        }
        assert(memcmp(expected,actual,sizeof actual)==0);
    }
    puts("PASS exact 240x240 RGB565 parity, idle DMA suppression and eight-row transfer windows");
}
"""
with tempfile.TemporaryDirectory() as path:
    c_file = Path(path) / "panel.c"
    executable = Path(path) / "panel"
    c_file.write_text(harness + cache + runner)
    subprocess.run(["cc", "-O2", str(c_file), "-o", str(executable)], check=True)
    subprocess.run([str(executable)], check=True)
