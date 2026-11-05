/* SPDX-License-Identifier: MPL-2.0 */
/* A directory entry identifies the actual knob on the editable destination. */
static const struct { uint8_t id, page, tab, knob; } parameter_routes[]={

#define AQ_PARAMETER(id,page,tab,knob) {id,page,tab,knob},
#include "parameters.def"
#undef AQ_PARAMETER
};
#define QUICK_CATEGORIES (sizeof quick_names/sizeof quick_names[0])
#define QUICK_COUNT (QUICK_CATEGORIES+sizeof parameter_routes/sizeof parameter_routes[0])

static void open_parameter(unsigned index)
{
    unsigned tab=parameter_routes[index].tab;
    page=parameter_routes[index].page;
    debug=0;
    switch(page){
    case PAGE_SOUND:
        group=tab;
        sound_picker=0;
        if((!tab&&selected==SR_DRUMS)||(tab==6&&selected>SR_BASS))selected=SR_LEAD;
        break;
    case PAGE_AIR:air_tab=tab;break;
    case PAGE_EVOLVE:song_tab=tab;break;
    case PAGE_MIX:mix_tab=tab;if(tab==5)mix_focus=selected;break;
    case PAGE_DRUMS:drum_tab=tab;break;
    case PAGE_SETUP:
        if(tab==6){setup_tab=0;display_subtab=1;}
        else if(tab>=4)debug=tab==4?1:3;
        else {setup_tab=tab;if(!tab)display_subtab=0;}
        break;
    default:break;
    }
    knob_focus=parameter_routes[index].knob;
    knob_until=plat_ms()+1500u;
}
