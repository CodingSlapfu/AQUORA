/* Original visual_job from commit 128662b; MPL-2.0; exact equations retained. */
static void reference_original(uint32_t unused)
{
    (void)unused;
    unsigned begin=plat_ticks24();
    if(vis_input.water)resonance_job();
    else for(unsigned t=0;t<SR_TRACKS;t++){
      vis_output.count[t]=33;
      unsigned phase=vis_input.phase+t*16+vis_input.event[t]*5;
      float pulse=vis_input.meter[t]*55.f;
      float radius=(vis_input.full?52.f+t*9.f:45.f+t*6.f)+(pulse>13.f?13.f:pulse);
      for(unsigned j=0;j<=32;j++){
        unsigned a=j*4;
        float petal=radius*(.82f+.18f*vis_sine[(3*a+phase)&127]);
        vis_output.xy[t][j][0]=120+(int)(petal*vis_sine[(a+phase)&127]);
        vis_output.xy[t][j][1]= (vis_input.full?120:96)+(int)(petal*(vis_input.full?1.04f:.44f)*vis_sine[(2*a+phase)&127]);
      }
    }
    vis_output.busy_us=(plat_ticks24()-begin)/24;
}

