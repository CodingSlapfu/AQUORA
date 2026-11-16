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
#endif
