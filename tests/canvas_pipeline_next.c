/* Exercise the actual canvas against a delayed DMA consumer. */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#define APP_CANVAS_PX (240u*48u)
#define APP_CANVAS_DOUBLE_BUFFER 1
static const uint16_t *pending;
static uint16_t expected[APP_CANVAS_PX];
static unsigned pending_n,completed;
static void lcd_sync(void)
{
    if(!pending)return;
    for(unsigned i=0;i<pending_n;i++)assert(pending[i]==expected[i]);
    completed++;pending=0;
}
static void lcd_blit(uint32_t x,uint32_t y,uint32_t w,uint32_t h,const uint16_t *px)
{
    (void)x;(void)y;lcd_sync();
    pending=px;pending_n=w*h;
    for(unsigned i=0;i<pending_n;i++)expected[i]=px[i];
}
#include "../vendor/fm1-base/device/gfx.c"
int main(void)
{
    for(unsigned frame=0;frame<1000;frame++){
        cv_begin(240,48,(uint16_t)frame);
        /* A cancelled begin must not cycle back into the in-flight buffer. */
        cv_begin(240,48,(uint16_t)(frame+1));
        cv_rect(0,0,240,48,RGB(30,120,180));
        cv_line(0,0,239,47,RGB(255,255,255));
        if(frame%3)cv_blit(0,0);else cv_blit_from(0,0,7);
    }
    lcd_sync();assert(completed==1000);
    draw_text_box(0,0,240,&FONT_S,"AQUORA",RGB(255,255,255),1);
    assert(!pending);
    puts("PASS delayed DMA ownership, repeated begin, partial blits and synchronous one-shots (1000 frames)");
}
