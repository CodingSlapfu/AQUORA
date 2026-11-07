"""Expanded065 actual button/LCD acceptance; images are the firmware renderer."""
from pathlib import Path
from PIL import Image,ImageDraw
import os,subprocess,sys,json,re
host,out=sys.argv[1],Path(sys.argv[2]);out.mkdir(parents=True,exist_ok=True)
def run(name,commands,env=None):
 d=out/name;d.mkdir(parents=True,exist_ok=True);p=d/'test.txt';p.write_text('\n'.join(commands)+'\n')
 r=subprocess.run([host,str(p),str(d)],text=True,capture_output=True,env=dict(os.environ,AQUORA_TEST_SEED='1',**(env or {})))
 (d/'panel.log').write_text(r.stdout+r.stderr);assert r.returncode==0,(name,r.stdout+r.stderr)
 return d,r.stdout
# Actual routes, no editable duplicate canonical IDs within any page's sections.
d,log=run('routes',['expect route_unique 1','expect directory_ok 1','expect directory_edits 1','expect font_family 1'])
rows=[dict(zip(('page','section','knob','id'),map(int,l.split()[1:]))) for l in log.splitlines() if l.startswith('ROUTE ')]
(out/'routes.json').write_text(json.dumps(rows,indent=2)+'\n')
# Exact physical semantics: arm only; SAVE bank only; SEL action only; cancel.
run('new-song',['master 2048','turn ALGO 2','tap EDIT','expect remember_song 1','spin K1 1','expect song_armed 58','expect song_unchanged 1','tap SAVE','wait 150','expect bank_message 4','expect song_unchanged 1','tap SEL','wait 80','expect song_armed 0','expect song_unchanged 0','expect bank_mask 1','spin K2 1','tap HOME','expect song_armed 0'])
run('reroll',['master 2048','tap ENV','wait 80','expect remember_motif 1','turn ALGO 2','tap EDIT','spin K2 1','expect song_armed 59','tap SEL','wait 80','expect motif_unchanged 1','expect bank_mask 0','tap PLAY','wait 80','spin K1 1','expect song_armed 0','tap PLAY'])
run('reset',['master 2048','turn ALGO 3','tap EDIT','tap EDIT','spin K1 1','expect mix_reset_armed 1','expect remember_song 1','tap SAVE','wait 150','expect song_unchanged 1','expect mix_reset_armed 1','tap HOME','expect mix_reset_armed 0'])
for fault in ('fail','stall','slow'):
 cmds=['master 2048','tap PLAY','wait 300','tap SAVE','wait 6000' if fault=='stall' else 'wait 1800']
 cmds+=['expect bank_message 6','expect save_timeout 1'] if fault=='stall' else ['expect bank_message 6','expect bank_mask 0'] if fault=='fail' else ['expect bank_message 4','expect saving 0','expect bank_mask 1']
 cmds+=['expect running 1','shot result.png']
 run('save-'+fault,cmds,{'AQUORA_SAVE_TEST':fault})
run('save-command-stream',['master 2048','tap PLAY','wait 100','tap SAVE']+sum((['spin K2 '+str(1 if i%2 else -1),'wait 10'] for i in range(60)),[])+['wait 1200','expect saving 0','expect bank_message 4','expect bank_mask 1','expect drops 0'],{'AQUORA_SAVE_TEST':'slow'})
for slot in range(4):
 run(f'bank-{slot}',['master 2048','turn ALGO 2']+['tap EDIT']*4+['spin K1 2','wait 80','tap HOME','turn ALGO 5','tap EDIT',f'spin K1 {slot}','tap PLAY','wait 80','tap SAVE','wait 150','expect bank_message 4','tap PLAY','wait 200','reboot','turn ALGO 5','tap EDIT',f'spin K1 {slot}','tap SEL','wait 80','expect bank_message 5','expect music_mode 2','expect bank_exact 1'])

print("PASS current-source New Song/Reroll/motif locks/SEL/SAVE/fault recovery/four-bank recall")
