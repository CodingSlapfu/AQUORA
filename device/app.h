#pragma once
#include <stdint.h>
#include "plat.h"
#define APP_NAME "AQUORA"
#define APP_SPLASH_RGB RGB(140,220,190)
#define APP_BLE_AT_BOOT 0
#define APP_BLOCK 256u
#define APP_CANVAS_PX (240u*48u)
#define APP_CANVAS_DOUBLE_BUFFER 1
enum {OBJ_SETTINGS,OBJ_PANEL,OBJ_BANK1,OBJ_BANK2,OBJ_BANK3,OBJ_BANK4,OBJ_BLE,OBJ_MIDI,
      OBJ_FAVORITE5,OBJ_FAVORITE16=OBJ_FAVORITE5+11,OBJ_NOBJ};
#define AQ_LEGACY_BANKS 4u
#define AQ_BANKS 16u
#define AQ_BANK_RECORD_MAX 576u
#define AQ_BANK_BASE 0x97000u
#define AQ_BANK_STRIDE 0x2000u
/* AQUORA samples live in the application image. These sector pairs use
 * the app-data area only, leaving the four old pairs and all object IDs intact. */
#define AQ_FAVORITES_BASE 0xA0000u
#define AQ_FAVORITE_PAIR(i) (AQ_FAVORITES_BASE+(i)*AQ_BANK_STRIDE)
#define APP_STORE_MAP {0xFC000u,0xDC000u,AQ_BANK_BASE,AQ_BANK_BASE+AQ_BANK_STRIDE, \
    AQ_BANK_BASE+2*AQ_BANK_STRIDE,AQ_BANK_BASE+3*AQ_BANK_STRIDE,0xDE000u,0xDA000u, \
    AQ_FAVORITE_PAIR(0),AQ_FAVORITE_PAIR(1),AQ_FAVORITE_PAIR(2),AQ_FAVORITE_PAIR(3), \
    AQ_FAVORITE_PAIR(4),AQ_FAVORITE_PAIR(5),AQ_FAVORITE_PAIR(6),AQ_FAVORITE_PAIR(7), \
    AQ_FAVORITE_PAIR(8),AQ_FAVORITE_PAIR(9),AQ_FAVORITE_PAIR(10),AQ_FAVORITE_PAIR(11)}
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
