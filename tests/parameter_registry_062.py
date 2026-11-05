"""Audit the registry consumed by actual parameter-specific Quick Find routing."""
from pathlib import Path
import json,re
root=Path(__file__).resolve().parents[1]
entries=re.findall(r"AQ_PARAMETER\((\d+),(PAGE_\w+),(\d+),(\d+)\)",(root/"device/parameters.def").read_text())
assert len(entries)>75
assert len({x[0] for x in entries})==len(entries)
app=(root/"device/app.c").read_text()
ui=(root/"device/ui.c").read_text()
consume=app[app.index("static void consume"):app.index("void app_init")]
result=[]
for ident,page,tab,knob in entries:
    ident=int(ident)
    assert f"case {ident}:" in ui,ident
    assert f"case {ident}:" in consume or f"control=={ident}" in app or ident in (71,72),ident
    match=re.search(rf"case {ident}:.*?(?=\n case |\n default:)",ui,re.S)
    text=match.group(0) if match else ""
    labels=re.findall(r'\*name="([^"]+)"',text)
    result.append(dict(id=ident,owner=page.removeprefix("PAGE_"),tab=int(tab),knob=int(knob)+1,labels=labels))
assert next(x for x in result if x["id"]==13)["owner"]=="MIX"
assert all(next(x for x in result if x["id"]==i)["owner"]=="SETUP" for i in (90,91,92))
assert all(next(x for x in result if x["id"]==i)["owner"]=="AIR" for i in (93,94,95))
print(f"PASS {len(result)} unique canonical IDs; UI labels, setters, ownership and physical knob destinations")
if __name__=="__main__" and len(__import__("sys").argv)>1:
    Path(__import__("sys").argv[1]).write_text(json.dumps(result,indent=2)+"\n")
