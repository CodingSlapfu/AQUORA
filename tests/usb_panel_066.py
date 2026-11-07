"""Real app/engine USB routing fixtures. No BLE work in this release."""
from pathlib import Path
import os,re,subprocess,sys
host,out=sys.argv[1],Path(sys.argv[2]);out.mkdir(parents=True,exist_ok=True)
def run(name,commands):
 commands=sum((['wait 12',c] if (c.startswith('expect midi_external_voices') or c.startswith('expect midi_steps')) else [c] for c in commands),[])
 d=out/name;d.mkdir(exist_ok=True);p=d/'scenario.txt';p.write_text('\n'.join(commands)+'\n')
 r=subprocess.run([host,str(p),str(d)],env=dict(os.environ,AQUORA_TEST_SEED='1'),text=True,capture_output=True)
 (d/'panel.log').write_text(r.stdout+r.stderr);assert not r.returncode,(name,r.stdout+r.stderr)
 return r.stdout
setup=['master 2048','turn ALGO 5']+['tap EDIT']*4
run('routes',['expect route_unique 1','expect directory_ok 1','expect directory_edits 1'])
run('receive-no-echo',['master 2048','out','midi USB 90 3C 64','wait 100','expect midi_external_voices 1','expect_no_out USB 90 3C 64','midi USB B0 40 7F','midi USB 80 3C 00','wait 40','expect midi_external_voices 1','midi USB B0 40 00','wait 40','expect midi_external_voices 0','midi USB 90 40 64','midi USB 91 40 64','expect midi_external_voices 2','midi USB 90 40 00','expect midi_external_voices 1','midi USB E1 00 60','midi USB 81 40 00','expect midi_external_voices 0','midi USB 90 43 64','midi USB 91 43 64','midi USB B0 7B 00','expect midi_external_voices 1','midi USB B1 7B 00','expect midi_external_voices 0'])
run('filter-persistence',setup+['spin K4 3','tap EDIT','spin K1 2','tap EDIT','spin K1 -2','wait 1800','reboot','expect midi_tx_channel 4','expect midi_rx_channel 2','expect startup_chime 0','midi USB 90 3C 64','expect midi_external_voices 0','midi USB 91 3C 64','expect midi_external_voices 1','midi USB B1 7B 00','expect midi_external_voices 0'])
run('disconnect',['master 2048','midi USB 90 3C 64','expect midi_external_voices 1','expect usb_unplug 1','wait 50','expect midi_external_voices 0','midi USB 90 3D 64','expect midi_external_voices 0','expect usb_replug 1','wait 50','midi USB 90 3D 64','expect midi_external_voices 1','midi USB 80 3D 00','expect midi_external_voices 0'])
run('active-sensing',['master 2048','midi USB FE 00 00','midi USB 90 3C 64','wait 200','expect midi_external_voices 1','wait 300','expect midi_external_voices 0'])
run('master-clock',['master 2048','turn ALGO 2','spin K3 100','tap HOME','tap PLAY','wait 1000','expect midi_clock_exact 1','expect_out USB FA 00 00','expect_out USB F8 00 00','tap SAVE','wait 500','expect bank_message 4','expect running 1','tap PLAY','wait 60','expect midi_tx_active 0','expect_out USB FC 00 00'])
slave=setup+['spin K1 1','tap HOME','out','midi USB FA 00 00','midi USB F8 00 00','wait 30','expect midi_steps 1']
for i in range(24):slave+=['wait 18','midi USB F8 00 00']
slave+=['expect midi_steps 5','expect_no_out USB F8 00 00','midi USB FC 00 00','wait 50','expect running 0','midi USB F2 08 00','midi USB FB 00 00','midi USB F8 00 00','wait 40','expect midi_steps 9','wait 2100','expect running 0','expect midi_external_voices 0']
run('external-phase-transport',slave)
log=run('master-note-lifecycle',['master 2048','turn ALGO 2','tap EDIT','tap EDIT','tap EDIT','spin K2 -100','tap HOME','tap PLAY','wait 8000','tap PLAY','wait 100','expect midi_tx_active 0','out'])
ledger={};ons=0
for s,a,b in re.findall(r'out ([0-9A-F]{2}) ([0-9A-F]{2}) ([0-9A-F]{2}) -> USB',log):
 s,a,b=int(s,16),int(a,16),int(b,16)
 if s>>4==9 and b:ledger[s&15,a]=1;ons+=1
 elif s>>4==8 or (s>>4==9 and not b):ledger.pop((s&15,a),None)
 elif s>>4==11 and a in (120,123):ledger={k:v for k,v in ledger.items() if k[0]!=(s&15)}
assert ons>0 and not ledger,(ons,ledger)
run('ui',setup+['shot sync.png','tap EDIT','shot io.png','tap EDIT','shot startup.png'])
print('PASS USB real-app routing, channel/velocity/sustain/bend, no echo, config reboot, disconnect/reconnect, sensing/clock loss, 24PPQN/SPP/Continue, SAVE while synced, note lifecycle and UI routes')
