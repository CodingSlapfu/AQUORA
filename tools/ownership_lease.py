#!/usr/bin/env python3
"""Durable Git lease: compare-and-swap refs fence expired owners.
Uses a dedicated remote ref, never the checkout index or shared workspace locks.
Every publisher must run check immediately before its mutation; this protocol
cannot fence legacy publishers, so migrating publication requires quiescence.
"""
import argparse, json, os, subprocess, time

def git(*args, data=None, check=True):
    return subprocess.run(['git',*args],input=data,text=True,capture_output=True,check=check).stdout.strip()

def read(ref):
    remote=git('ls-remote','origin',ref)
    if not remote:return None,{}
    sha=remote.split()[0]
    git('fetch','--quiet','origin',ref)
    return sha,json.loads(git('show',sha+':lease.json'))

def update(ref,prior,state):
    blob=git('hash-object','-w','--stdin',data=json.dumps(state,sort_keys=True)+'\n')
    tree=git('mktree',data='100644 blob '+blob+'\tlease.json\n')
    args=['commit-tree',tree,'-m','Renewable AQUORA ownership lease']
    if prior:args+=['-p',prior]
    sha=git(*args)
    git('push','--porcelain','origin',sha+':'+ref,'--force-with-lease='+ref+':'+(prior or ''))
    return sha

def main():
    p=argparse.ArgumentParser();p.add_argument('action',choices=['acquire','renew','check','release','run'])
    p.add_argument('--resource',required=True);p.add_argument('--owner',required=True)
    p.add_argument('--command',nargs=argparse.REMAINDER);p.add_argument('--checkpoint');
    p.add_argument('--token');p.add_argument('--ttl',type=int,default=900);a=p.parse_args()
    if not 60<=a.ttl<=3600:p.error('TTL must be 60..3600 seconds')
    ref='refs/heads/leases/'+a.resource;sha,state=read(ref);now=int(time.time())
    if a.action=='acquire':
        if state.get('expires',0)>now:raise SystemExit('Resource has an unexpired lease: '+state['owner'])
        state={'owner':a.owner,'expires':now+a.ttl,'epoch':state.get('epoch',0)+1,'released':False}
        token=update(ref,sha,state);print(json.dumps({'token':token,**state}));return
    if sha!=a.token or state.get('owner')!=a.owner or state.get('expires',0)<=now or state.get('released'):
        raise SystemExit('Lease fence rejected: expired, replaced, released or wrong owner')
    if a.action=='run':
        if not a.command:p.error('run requires --command PROGRAM ARG...')
        from pathlib import Path
        child=None;token=sha;deadline=time.monotonic()+a.ttl/3
        def checkpoint():
            if a.checkpoint:
                target=Path(a.checkpoint);tmp=target.with_suffix('.tmp')
                tmp.write_text(json.dumps({'token':token,'resource':a.resource,**state})+'\n');os.replace(tmp,target)
        try:
            checkpoint();child=subprocess.Popen(a.command)
            while child.poll() is None:
                time.sleep(1)
                if time.monotonic()>=deadline:
                    current,active=read(ref)
                    if current!=token or active.get('owner')!=a.owner or active.get('expires',0)<=int(time.time()):
                        raise RuntimeError('Ownership fence lost; stopping this owned child')
                    state['expires']=int(time.time())+a.ttl
                    token=update(ref,token,state);checkpoint();deadline=time.monotonic()+a.ttl/3
            code=child.returncode
        finally:
            if child is not None and child.poll() is None:
                child.terminate()
                try:child.wait(timeout=10)
                except subprocess.TimeoutExpired:child.kill();child.wait()
            current,active=read(ref)
            if current==token and active.get('owner')==a.owner:
                state.update(expires=int(time.time()),released=True)
                token=update(ref,token,state);checkpoint()
        raise SystemExit(code)
    if a.action=='check':print(json.dumps({'token':sha,**state}));return
    state['expires']=now+a.ttl if a.action=='renew' else now
    state['released']=a.action=='release'
    print(json.dumps({'token':update(ref,sha,state),**state}))
if __name__=='__main__':main()
