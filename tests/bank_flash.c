#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../device/app.h"

static uint8_t nor[0x100000];
static int fault=-1;

static int st_read(uint32_t offset,void *dst,uint32_t bytes)
{
    memcpy(dst,nor+offset,bytes);
    return 0;
}
static int st_erase(uint32_t offset)
{
    memset(nor+offset,0xff,4096);
    return 0;
}
static int st_prog(uint32_t offset,const void *src,uint32_t bytes)
{
    if(fault==0)return -9;
    if(fault>0)fault--;
    assert((offset&255u)+bytes<=256);
    const uint8_t *data=src;
    for(uint32_t i=0;i<bytes;i++)nor[offset+i]&=data[i];
    return 0;
}

#include "../vendor/fm1-base/device/storage.c"

int main(void)
{
    char first[300], replacement[300], actual[300];
    memset(nor,0xff,sizeof(nor));
    memset(replacement,'Z',sizeof(replacement));
    for(unsigned slot=0;slot<4;slot++){
        unsigned obj=OBJ_BANK1+slot;
        assert(st_sector(obj,0)==AQ_BANK_BASE+slot*AQ_BANK_STRIDE);
        assert(st_sector(obj,1)+4096<=0xA0000u);
        memset(first,'A'+slot,sizeof(first));
        assert(st_load(obj,actual,sizeof(actual))<0);
        assert(st_save(obj,first,sizeof(first))==0);
    }
    for(unsigned slot=0;slot<4;slot++){
        unsigned obj=OBJ_BANK1+slot;
        for(int cut=0;cut<3;cut++){
            fault=cut;
            assert(st_save(obj,replacement,sizeof(replacement))<0);
            fault=-1;
            for(unsigned other=0;other<4;other++){
                memset(first,'A'+other,sizeof(first));
                assert(st_load(OBJ_BANK1+other,actual,sizeof(actual))==(int)sizeof(actual));
                assert(!memcmp(actual,first,sizeof(actual)));
            }
        }
    }
    assert(st_load(OBJ_SETTINGS,actual,sizeof(actual))<0);
    assert(st_load(OBJ_PANEL,actual,sizeof(actual))<0);
    puts("PASS production storage, four separate sector pairs, torn-write rollback at each program boundary");
    return 0;
}
