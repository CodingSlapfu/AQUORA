/* SPDX-License-Identifier: MPL-2.0 */
/* A directory entry identifies the actual knob on the editable destination. */
static const struct { uint8_t id, page, tab, knob; } parameter_routes[]={
 {1,PAGE_PLAY,0,0},{18,PAGE_PLAY,0,1},{22,PAGE_PLAY,0,2},{5,PAGE_PLAY,0,3},
 {2,PAGE_SOUND,0,0},{75,PAGE_SOUND,0,1},{76,PAGE_SOUND,0,2},{77,PAGE_SOUND,0,3},
 {21,PAGE_SOUND,1,0},{54,PAGE_SOUND,1,2},{78,PAGE_SOUND,1,3},
 {4,PAGE_SOUND,2,0},{48,PAGE_SOUND,2,2},{45,PAGE_SOUND,2,3},
 {8,PAGE_SOUND,3,0},{7,PAGE_SOUND,3,1},{23,PAGE_SOUND,3,2},{6,PAGE_SOUND,3,3},
 {30,PAGE_SOUND,5,0},{31,PAGE_SOUND,5,1},{36,PAGE_SOUND,5,2},
 {32,PAGE_SOUND,6,0},{33,PAGE_SOUND,6,1},{34,PAGE_SOUND,6,2},{35,PAGE_SOUND,6,3},
 {13,PAGE_SOUND,7,0},{24,PAGE_SOUND,7,1},{29,PAGE_SOUND,7,2},{28,PAGE_SOUND,7,3},
 {43,PAGE_SOUND,11,0},
 {47,PAGE_SOUND,12,0},{63,PAGE_SOUND,12,1},{73,PAGE_SOUND,12,2},{74,PAGE_SOUND,12,3},
 {55,PAGE_SOUND,13,0},{14,PAGE_SOUND,13,1},
 {79,PAGE_SOUND,14,0},{85,PAGE_SOUND,14,1},{86,PAGE_SOUND,14,2},
 {19,PAGE_EVOLVE,0,0},{20,PAGE_EVOLVE,0,1},{25,PAGE_EVOLVE,0,2},{46,PAGE_EVOLVE,0,3},
 {58,PAGE_EVOLVE,1,0},{59,PAGE_EVOLVE,1,1},{60,PAGE_EVOLVE,1,2},{61,PAGE_EVOLVE,1,3},
 {64,PAGE_EVOLVE,2,0},{65,PAGE_EVOLVE,2,1},
 {82,PAGE_EVOLVE,3,0},{83,PAGE_EVOLVE,3,2},{84,PAGE_EVOLVE,3,3},
 {11,PAGE_MIX,1,0},{12,PAGE_MIX,1,1},{50,PAGE_MIX,1,2},{51,PAGE_MIX,1,3},
 {52,PAGE_MIX,2,0},
 {9,PAGE_DRUMS,0,0},{10,PAGE_DRUMS,0,1},{37,PAGE_DRUMS,0,2},{17,PAGE_DRUMS,0,3},
 {16,PAGE_DRUMS,1,0},
 {49,PAGE_DRUMS,2,0},{62,PAGE_DRUMS,2,2},
 {38,PAGE_SETUP,0,0},{56,PAGE_SETUP,0,1},{80,PAGE_SETUP,0,2},{81,PAGE_SETUP,0,3},
 {90,PAGE_SETUP,6,0},{91,PAGE_SETUP,6,1},{92,PAGE_SETUP,6,2},
 {39,PAGE_SETUP,1,0},{40,PAGE_SETUP,1,1},{41,PAGE_SETUP,1,2},
 {66,PAGE_AIR,0,0},{67,PAGE_AIR,0,1},{68,PAGE_AIR,0,2},{69,PAGE_AIR,0,3},
 {57,PAGE_SETUP,4,0},
 {42,PAGE_SETUP,5,0}
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
    case PAGE_EVOLVE:song_tab=tab;break;
    case PAGE_MIX:mix_tab=tab;break;
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
