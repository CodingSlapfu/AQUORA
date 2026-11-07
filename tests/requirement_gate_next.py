"""Fail closed on missing scope or unsupported release claims. No network needed."""
import json
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]

def validate(register, snapshot, release=False):
    errors = []
    rows = register.get('requirements', [])
    ids = [r.get('id') for r in rows]
    if len(set(ids)) != len(ids):
        errors.append('duplicate requirement IDs')
    for issue in snapshot['issues']:
        if f"ISSUE-{issue['number']:02}" not in ids:
            errors.append(f"unclassified issue {issue['number']}")
    for row in rows:
        for key in ('id','requirement','source','target','priority','status',
                    'physical_owner','carry_forward','audit'):
            if not row.get(key):
                errors.append(f"{row.get('id')}: missing {key}")
        if row.get('status') not in {'OPEN','PARTIAL','BLOCKED','COMPLETED SOFTWARE','OWNER-HARDWARE-UNVERIFIED'}:
            errors.append(f"{row.get('id')}: unknown status")
        if row.get('status') in {'COMPLETED SOFTWARE','OWNER-HARDWARE-UNVERIFIED'}:
            for field in ('code','tests','artifacts'):
                if not row.get(field):
                    errors.append(f"{row['id']}: missing {field} proof")
        if release and row.get('status') not in {'COMPLETED SOFTWARE','OWNER-HARDWARE-UNVERIFIED'}:
            errors.append(f"{row.get('id')}: unresolved software requirement")
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
    assert any('missing code proof' in e for e in validate(broken, snapshot))
    assert validate(register, snapshot, release=True), 'open work must block release'
    print('PASS missing deferred issue, unsupported completion and open-release negative tests')

if __name__ == '__main__':
    register = json.loads((ROOT/'validation/next-requirements.json').read_text())
    snapshot = json.loads((ROOT/'validation/next-owner-scope.json').read_text())
    if '--self-test' in sys.argv:
        self_test(register, snapshot)
    errors = validate(register, snapshot, '--release' in sys.argv)
    if errors:
        print('\n'.join(errors))
        raise SystemExit(1)
    print('PASS scope coverage structure' + (' and release evidence' if '--release' in sys.argv else ' (audit unfinished; release remains blocked)'))
