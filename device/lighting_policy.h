/* SPDX-License-Identifier: MPL-2.0 */
#ifndef AQUORA_LIGHTING_POLICY_H
#define AQUORA_LIGHTING_POLICY_H
/* Physical keyboard starts at F3, independently of Perfect Key quantization. */
static unsigned aq_black_key(unsigned key)
{
    unsigned note=(5u+key)%12u;
    return note==1u||note==3u||note==6u||note==8u||note==10u;
}
/* Engine advances in sixteenth notes. Initial tick_length means the very
 * first beat is pending; no independent wall clock can drift from audio. */
static unsigned aq_play_pulse(unsigned running,unsigned step,unsigned phase,
                              unsigned length)
{
    if(!running||!length)return 0;
    if(!step)return 1;
    return ((step-1u)&3u)==0u&&phase<length/2u;
}
#endif
