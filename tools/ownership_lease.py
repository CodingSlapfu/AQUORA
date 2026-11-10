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
    p=argparse.ArgumentParser();p.add_argument('action',choices=['acquire','renew','check','release'])
    p.add_argument('--resource',required=True);p.add_argument('--owner',required=True)
    p.add_argument('--token');p.add_argument('--ttl',type=int,default=900);a=p.parse_args()
    if not 60<=a.ttl<=3600:p.error('TTL must be 60..3600 seconds')
    ref='refs/heads/leases/'+a.resource;sha,state=read(ref);now=int(time.time())
    if a.action=='acquire':
        if state.get('expires',0)>now:raise SystemExit('Resource has an unexpired lease: '+state['owner'])
        state={'owner':a.owner,'expires':now+a.ttl,'epoch':state.get('epoch',0)+1,'released':False}
        token=update(ref,sha,state);print(json.dumps({'token':token,**state}));return
    if sha!=a.token or state.get('owner')!=a.owner or state.get('expires',0)<=now or state.get('released'):
        raise SystemExit('Lease fence rejected: expired, replaced, released or wrong owner')
    if a.action=='check':print(json.dumps({'token':sha,**state}));return
    state['expires']=now+a.ttl if a.action=='renew' else now
    state['released']=a.action=='release'
    print(json.dumps({'token':update(ref,sha,state),**state}))
if __name__=='__main__':main()
