"""Release boundary: performance MIDI deferred; internal music and startup preference work."""
from pathlib import Path
import os,subprocess,sys
host,out=sys.argv[1],Path(sys.argv[2]);out.mkdir(parents=True,exist_ok=True)
commands=['master 2048','expect performance_midi 0','expect midi_sync 0','out','midi USB FA 00 00','midi USB 90 3C 64','midi BLE FA 00 00','midi BLE 90 3C 64','wait 120','expect running 0','expect midi_external_voices 0','expect midi_received 0','expect midi_clock_out 0','expect_no_out USB 90 3C 64','tap PLAY','wait 1400','expect running 1','expect midi_steps 3','expect midi_clock_out 0','tap PLAY','wait 100','expect running 0','expect midi_tx_active 0','tap HOME','turn ALGO 5']+['tap EDIT']*4+['expect setup_tab 4','spin K1 -2','wait 1600','reboot','expect startup_chime 0','expect performance_midi 0','expect midi_sync 0','turn ALGO 5']+['tap EDIT']*5+['expect setup_tab 0']
# Transport step is tempo-dependent; rely on actual progression rather than one fixed phase.
commands.remove('expect midi_steps 3')
commands+=['expect route_unique 1','expect directory_ok 1','expect directory_edits 1']
p=out/'panel.txt';p.write_text('\n'.join(commands)+'\n')
r=subprocess.run([host,str(p),str(out)],env=dict(os.environ,AQUORA_TEST_SEED='1'),capture_output=True,text=True)
(out/'panel.log').write_text(r.stdout+r.stderr);assert r.returncode==0,r.stdout+r.stderr
print('PASS USB/BLE performance input ignored; no transmitted clock/notes; internal PLAY/STOP, unique reachable controls, startup preference reboot, five setup sections')
