#pragma once
#include <stdint.h>
#include "plat.h"
#define APP_NAME "AQUORA"
#define APP_SPLASH_RGB RGB(140,220,190)
#define APP_BLOCK 256u
#define APP_CANVAS_PX (240u*48u)
enum {OBJ_SETTINGS,OBJ_PANEL,OBJ_BANK1,OBJ_BANK2,OBJ_BANK3,OBJ_BANK4,OBJ_NOBJ};
#define AQ_BANK_BASE 0x97000u
#define AQ_BANK_STRIDE 0x2000u
#define APP_STORE_MAP {0xFC000u,0xDC000u,AQ_BANK_BASE,AQ_BANK_BASE+AQ_BANK_STRIDE,                        AQ_BANK_BASE+2*AQ_BANK_STRIDE,AQ_BANK_BASE+3*AQ_BANK_STRIDE}
void app_init(void);
void app_render(int32_t*,uint32_t,uint32_t);
void app_panic(void);
void app_ui_init(void);
void app_frame(void);
void app_input_only(void);
void app_say(const char*,const char*);
#define APP_HOST_EXPECT
int app_expect(const char*,int*);

#define APP_HOST_CPU2
#define FM1_CPU1_TIMEOUT_US 2000u
