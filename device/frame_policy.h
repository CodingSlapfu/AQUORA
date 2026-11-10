/* SPDX-License-Identifier: MPL-2.0 */
#ifndef AQUORA_FRAME_POLICY_H
#define AQUORA_FRAME_POLICY_H
/* Fixed mode honours the requested cadence. A measured underrun overrides
 * either mode temporarily; P95 estimates alone only govern AUTO. */
static unsigned aq_frame_interval(unsigned automatic,unsigned running,
                                  unsigned intro,unsigned p95,unsigned xruns)
{
    if(intro||!running)return 50u;
    if(xruns)return 150u;
    if(!automatic)return 50u;
    return p95>=5400u?150u:p95>=4900u?100u:p95>=4400u?75u:50u;
}
#endif
