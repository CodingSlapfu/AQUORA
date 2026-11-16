"""Compare all Cymatics drawing inputs under identical prerecorded drive."""
from pathlib import Path
import subprocess
import tempfile


def render(source, directory, name):
    start = source.index("static float nodal_level;")
    end = source.index("/* Original vibrating-plate mode")
    harness = r"""
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#define SR_TRACKS 8
#define SR_BASS 1
#define fm_sqrtf sqrtf
static float fm_clampf(float x,float lo,float hi){return x<lo?lo:x>hi?hi:x;}
static float vis_sine[128];
static struct {
    unsigned phase,frequency,scene,note_class;
    float meter[8],onset[8],pitch;
} vis_input;
static struct {
    int16_t sand[512][2],nodes[512][4];
    uint16_t node_count;
    uint8_t water_light[30][30];
} vis_output;
"""
    runner = r"""
int main(void){
    for(unsigned i=0;i<128;i++)vis_sine[i]=sinf(i*.0490873852f);
    for(unsigned frame=0;frame<1200;frame++){
        vis_input.phase=frame*50u/35u;
        vis_input.scene=(frame/53)%22;
        vis_input.note_class=(frame/7)%12;
        vis_input.pitch=110.f+vis_input.note_class*103.f;
        vis_input.frequency=174u+vis_input.scene*39u;
        for(unsigned t=0;t<8;t++){
            vis_input.meter[t]=frame%300<50?0.f:((frame+t*7)%31)*.004f;
            vis_input.onset[t]=frame%300>=50&&(frame+t)%13==0;
        }
        resonance_job();
        fwrite(vis_output.sand,sizeof vis_output.sand,1,stdout);
        fwrite(vis_output.water_light,sizeof vis_output.water_light,1,stdout);
        fwrite(vis_output.nodes,sizeof vis_output.nodes[0],vis_output.node_count,stdout);
    }
}
"""
    c_file = directory / f"{name}.c"
    executable = directory / name
    c_file.write_text(harness + source[start:end] + runner)
    subprocess.run(["cc", "-O2", "-ffp-contract=off", str(c_file), "-lm",
                    "-o", str(executable)], check=True)
    return subprocess.check_output([str(executable)])


ref = subprocess.check_output(
    ["git", "show", "e41d1b08a9f87f95b01ca3db9822e5feca5fd07b:device/visual_worker.c"],
    text=True,
)
candidate = Path("device/visual_worker.c").read_text()
with tempfile.TemporaryDirectory() as path:
    assert render(ref, Path(path), "reference") == render(candidate, Path(path), "candidate")
print("PASS 1,200 Cymatics frames with identical 512 particle coordinates, glints and illumination")
