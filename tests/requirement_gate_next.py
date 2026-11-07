"""Fail closed on missing scope or unsupported release claims. No network needed."""
import json
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]

def validate(register, snapshot, release=False, queue_text=''):
    errors = []
    rows = register.get('requirements', [])
    ids = [r.get('id') for r in rows]
    if len(set(ids)) != len(ids):
        errors.append('duplicate requirement IDs')
    for issue in snapshot['issues']:
        if f"ISSUE-{issue['number']:02}" not in ids:
            errors.append(f"unclassified issue {issue['number']}")
        if issue['number']==31:
            for required in ('MIDI-USB','MIDI-BLE','MIDI-AIR'):
                if required not in ids:
                    errors.append(f'missing current MIDI scope classification {required}')
    if release:
        reviewed=set(register.get('reviewed_owner_comments',[]))
        for thread in snapshot['comments']:
            for comment in thread['comments']:
                if comment['author']=='CodingSlapfu' and comment['url'] not in reviewed:
                    errors.append(f"unclassified owner comment {comment['url']}")
    for row in rows:
        for key in ('id','requirement','source','target','priority','status',
                    'physical_owner','carry_forward','audit'):
            if not row.get(key):
                errors.append(f"{row.get('id')}: missing {key}")
        if row.get('status') not in {'OPEN','PARTIAL','BLOCKED','COMPLETED SOFTWARE','OWNER-HARDWARE-UNVERIFIED','OWNER-AUTHORIZED DEFERRED'}:
            errors.append(f"{row.get('id')}: unknown status")
        if row.get('status') in {'COMPLETED SOFTWARE','OWNER-HARDWARE-UNVERIFIED'}:
            for field in ('code','tests','artifacts'):
                if not row.get(field):
                    errors.append(f"{row['id']}: missing {field} proof")
        if row.get('status')=='OWNER-AUTHORIZED DEFERRED' and not row.get('owner_scope_approval'):
            errors.append(f"{row.get('id')}: missing owner deferral approval")
        if release and row.get('status') not in {'COMPLETED SOFTWARE','OWNER-HARDWARE-UNVERIFIED','OWNER-AUTHORIZED DEFERRED'}:
            errors.append(f"{row.get('id')}: unresolved software requirement")
        if str(row.get('carry_forward','')).startswith('DEFERRED'):
            if row.get('id','') not in queue_text:
                errors.append(f"{row.get('id')}: deferred requirement missing from next queue")
            if not row.get('owner_scope_approval'):
                errors.append(f"{row.get('id')}: deferral has no owner scope approval")
    if release and (not register.get('audit_complete') or register.get('target') == 'next'):
        errors.append('reconciliation incomplete or stale release version')
    return errors

def self_test(register, snapshot):
    import copy
    broken = copy.deepcopy(register)
    broken['requirements'] = [r for r in broken['requirements'] if r['id'] != 'ISSUE-17']
    assert 'unclassified issue 17' in validate(broken, snapshot)
    broken = copy.deepcopy(register)
    broken['requirements'][0]['status'] = 'COMPLETED SOFTWARE'
    broken['requirements'][0]['code'] = []
    assert any('missing code proof' in e for e in validate(broken, snapshot))
    assert validate(register, snapshot, release=True), 'open USB and other work must block release'
    broken=copy.deepcopy(register)
    row=next(r for r in broken['requirements'] if r['id']=='ISSUE-17')
    row['carry_forward']='DEFERRED to explicit future version'
    assert 'ISSUE-17: deferred requirement missing from next queue' in validate(broken,snapshot)
    approved=copy.deepcopy(next(r for r in register['requirements'] if r['id']=='MIDI-BLE'))
    fixture=dict(target='0.6.6',audit_complete=True,requirements=[approved])
    empty=dict(issues=[],comments=[])
    assert not validate(fixture,empty,True,'MIDI-BLE')
    assert validate(fixture,empty,True,'')
    approved.pop('owner_scope_approval')
    assert validate(fixture,empty,True,'MIDI-BLE')
    missing_usb=copy.deepcopy(register)
    missing_usb['requirements']=[r for r in missing_usb['requirements'] if r['id']!='MIDI-USB']
    assert 'missing current MIDI scope classification MIDI-USB' in validate(missing_usb,snapshot)
    print('PASS missing deferred issue, unsupported completion and open-release negative tests')

if __name__ == '__main__':
    register = json.loads((ROOT/'validation/next-requirements.json').read_text())
    snapshot = json.loads((ROOT/'validation/next-owner-scope.json').read_text())
    if '--self-test' in sys.argv:
        self_test(register, snapshot)
    errors = validate(register, snapshot, '--release' in sys.argv,
                      (ROOT/'NEXT_BUILD_ACTIVE.md').read_text())
    if errors:
        print('\n'.join(errors))
        raise SystemExit(1)
    print('PASS scope coverage structure' + (' and release evidence' if '--release' in sys.argv else ' (audit unfinished; release remains blocked)'))
