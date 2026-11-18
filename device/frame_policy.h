/* SPDX-License-Identifier: MPL-2.0 */
#ifndef AQUORA_FRAME_POLICY_H
#define AQUORA_FRAME_POLICY_H
/* Aim for 20 FPS. Back off optional rendering as the audio callback
 * approaches the DMA deadline or reports a measured underrun. */
static unsigned aq_frame_interval(unsigned running,
                                  unsigned intro,unsigned p95,unsigned xruns)
{
    if(intro||!running)return 50u;
    if(xruns)return 150u;
    return p95>=5400u?150u:p95>=4900u?100u:p95>=4400u?75u:50u;
}
/* Geometry is fixed for a frame; the fullscreen label reads the latest knob
 * value when its own band is submitted. Rotating a knob cannot cancel the
 * rest of the frame, but a change of display mode still can. */
static inline unsigned aq_frame_invalid(unsigned was_fullscreen,unsigned fullscreen,
                         unsigned was_visual,unsigned visual,unsigned was_debug,unsigned debug,
                         unsigned input_changes,unsigned selection_changes)
{
    if(was_fullscreen!=fullscreen||was_debug!=debug)return 1;
    if(fullscreen)return was_visual!=visual;
    return input_changes!=selection_changes;
}
#endif
