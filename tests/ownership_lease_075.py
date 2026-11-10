"""Exercise actual Git CAS acquisition/recovery/fencing without network."""
import importlib.util,os,subprocess,tempfile,time
from pathlib import Path
spec=importlib.util.spec_from_file_location('lease',Path(__file__).resolve().parents[1]/'tools/ownership_lease.py')
m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
with tempfile.TemporaryDirectory() as tmp:
 root=Path(tmp);subprocess.run(['git','init','--bare','-q',str(root/'remote')],check=True)
 subprocess.run(['git','init','-q',str(root/'work')],check=True);os.chdir(root/'work')
 for args in [('config','user.name','lease-test'),('config','user.email','lease@test.invalid'),('remote','add','origin',str(root/'remote'))]:m.git(*args)
 ref='refs/heads/leases/test';now=int(time.time());prior,state=m.read(ref);assert prior is None
 token=m.update(ref,None,{'owner':'one','epoch':1,'expires':now+900,'released':False})
 assert m.read(ref)[0]==token
 try:m.update(ref,None,{'owner':'racer','expires':now+900});raise AssertionError('CAS accepted stale acquisition')
 except subprocess.CalledProcessError:pass
 renewed=m.update(ref,token,{'owner':'one','epoch':1,'expires':now+1000,'released':False})
 try:m.update(ref,token,{'owner':'stale','expires':now+900});raise AssertionError('CAS accepted stale heartbeat')
 except subprocess.CalledProcessError:pass
 expired=m.update(ref,renewed,{'owner':'one','epoch':1,'expires':now-1,'released':False})
 recovered=m.update(ref,expired,{'owner':'two','epoch':2,'expires':now+900,'released':False})
 try:m.update(ref,expired,{'owner':'one','epoch':1,'expires':now+900});raise AssertionError('Old owner recovered after fence')
 except subprocess.CalledProcessError:pass
 assert m.read(ref)[1]['epoch']==2
 released=m.update(ref,recovered,{'owner':'two','epoch':2,'expires':now,'released':True})
 assert m.read(ref)[1]['released']
print('PASS actual Git CAS: concurrent acquisition rejection, heartbeat fencing, expired recovery, old-owner rejection, explicit release')
