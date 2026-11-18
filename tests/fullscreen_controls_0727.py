from pathlib import Path
import hashlib, os, subprocess, sys
host, output = Path(sys.argv[1]).resolve(), Path(sys.argv[2]).resolve()
for visual in range(3):
    out = output / str(visual)
    out.mkdir(parents=True, exist_ok=True)
    script = ['master 2048','wait 9000','tap HOME','turn SELECT 7',
              'turn K1 1','turn K2 4','turn K4 30']
    for tab in range(1, 7):
        script += ['turn ALGO 1']+[f'spin K{k} 100' for k in range(1, 5)]
    script += ['tap HOME','turn SELECT 5','tap SEL',f'turn K4 {visual}',f'expect visual {visual}',
               'tap HOME','spin K2 1','wait 150','expect fullscreen 1','expect running 1']
    for sample in range(8):
        for edge in range(50):
            script += [f'spin K{sample%4+1} {1 if edge%2 else -1}']
        script += ['expect fullscreen 1', f'fbdump frame{sample}.raw']
    script += ['expect vocal_heard 1','expect drops 0']
    script_path=out/'commands.txt'
    script_path.write_text('\n'.join(script)+'\n')
    result=subprocess.run([str(host), str(script_path), str(out)],capture_output=True,text=True,
                          env=dict(os.environ,AQUORA_TEST_SEED='1',AQUORA_TEST_CALLBACK_US='5900'))
    (out/'run.log').write_text(result.stdout+result.stderr)
    assert not result.returncode, result.stdout+result.stderr
    raw=[(out/f'frame{i}.raw').read_bytes() for i in range(8)]
    assert all(len(frame)==240*240*2 for frame in raw)
    changed=len({hashlib.sha256(frame[48*480:192*480]).digest() for frame in raw})
    assert changed>=3, (visual,changed)
    print(f'PASS visualizer {visual}: 400 K1–K4 edges, {changed}/8 distinct middle LCD captures, vocal PCM and zero input drops',flush=True)
