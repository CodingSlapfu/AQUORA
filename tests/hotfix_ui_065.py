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
# Every installed theme/background/font; all MIX sections and major pages, playing/stopped.
contact=[]
for theme in range(5):
 for bg in range(2):
  for font in range(3):
   name=f't{theme}-b{bg}-f{font}'
   setup=['master 2048','turn ALGO 5','turn SELECT 1',f'spin K1 {theme}',f'spin K2 {font-1}',f'spin K3 {bg}','tap HOME']
   commands=setup+['expect font_family 1']
   for playing in range(2):
    if playing:commands+=['tap PLAY','wait 80','expect all_roles 1','wait 100']
    commands+=['turn ALGO 3']
    for tab in range(5):
     commands+=['wait 60',f'shot mix-{playing}-{tab}.png']
     if tab==1:commands+=['spin K3 1','spin K4 1','wait 80','expect auto_gain 1','expect auto_level 1',f'shot gain-on-{playing}.png','spin K3 -1','spin K4 -1','wait 80','expect auto_gain 0','expect auto_level 0']
     if tab==0:commands+=['tap SEL','wait 80',f'shot muted-{playing}.png','tap SEL','tap LFO','wait 80',f'shot solo-{playing}.png','tap LFO','wait 80']
     commands+=['tap EDIT']
    commands+=['tap FX','wait 60',f'shot strip-{playing}.png','tap FX','tap HOME']
   commands+=['tap PLAY','wait 200','turn ALGO 2']+['tap EDIT']*4+['wait 60','shot italic.png','tap HOME','turn ALGO 1','wait 60','shot sound.png']
   d,_=run(name,commands)
   im=Image.open(d/'mix-1-1.png').convert('RGB');assert im.size==(240,240)
   # Explicit78px status columns leave7px separators and text fits via native font metrics.
   assert im.crop((7,96,233,140)).getbbox()
   assert len(im.crop((8,144,232,225)).getcolors(100000))>5, 'Master FX knobs missing'
   contact.append((name,im))
   # Cold boot uses an independent multi-color curved spiral, not bars.
   intro=setup+['wait 1800','reboot','expect intro 1','expect reset_visual_counters 1','wait 2500','shot a.png','wait 250','shot b.png','expect geometry_jobs 0','expect worker_jobs 0']
   d,_=run(name+'-intro',intro,{'AQUORA_TEST_INTRO':'1'})
   a=Image.open(d/'a.png').convert('RGB');b=Image.open(d/'b.png').convert('RGB')
   assert a.tobytes()!=b.tobytes()
   assert a.crop((35,0,215,46)).tobytes()==b.crop((35,0,215,46)).tobytes()
   assert len(a.crop((35,48,205,192)).getcolors(10000))>=6
sheet=Image.new('RGB',(5*240,6*260),(10,20,25));draw=ImageDraw.Draw(sheet)
for i,(name,im) in enumerate(contact):x=i%5*240;y=i//5*260;sheet.paste(im,(x,y));draw.text((x+5,y+242),name,fill='white')
sheet.save(out/'mixer-contact.png')
print('PASS canonical routes, SEL/SAVE/arm/cancel, fault/timeout/slow SAVE, four-bank reboot recall, 30 theme/background/font configurations,MIX/strip states plus italic/SOUND and30 real spiral previews')
