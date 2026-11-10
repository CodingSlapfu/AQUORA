from pathlib import Path
import os,subprocess,sys
host,out=sys.argv[1],Path(sys.argv[2]);out.mkdir(parents=True,exist_ok=True)
for name,commands in [
 ('timeout',['master 0','wait 7900','expect running 0','wait 400','expect running 1','expect boot_starts 1','expect peak_db_max -90']),
 ('skip-button',['master 0','tap HOME','wait 80','expect running 1','expect boot_starts 1']),
 ('skip-knob',['master 0','spin K1 1','wait 80','expect running 1','expect boot_starts 1'])]:
 commands+=['tap PLAY','wait 80','expect running 0','wait 2000','expect running 0','expect boot_starts 1','tap PLAY','wait 80','expect running 1','expect boot_starts 1']
 p=out/(name+'.txt');p.write_text('\n'.join(commands)+'\n')
 q=subprocess.run([host,str(p),str(out)],env=dict(os.environ,AQUORA_TEST_INTRO='1',AQUORA_TEST_BOOT_SEED='12345'),text=True,capture_output=True)
 (out/(name+'.log')).write_text(q.stdout+q.stderr);assert q.returncode==0,q.stdout+q.stderr
 print('PASS',name,'single start, master mute and explicit STOP held')
