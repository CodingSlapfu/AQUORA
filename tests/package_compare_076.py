"""Compare FM-1 package structure without using or writing to a device."""
from dataclasses import dataclass
from hashlib import sha256
from pathlib import Path
from struct import unpack_from
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "vendor/fm1-base/tools"))
from fm1pkg_make import crc16, enc


@dataclass(frozen=True)
class Entry:
    name: str
    kind: int
    offset: int
    length: int
    padded_length: int
    payload_hash: str


def inspect(path):
    raw = path.read_bytes()
    if len(raw) < 20 * 48:
        raise ValueError(f"{path}: incomplete FWSC marker area")
    markers = raw[47:20 * 48:48]
    identity = "".join(chr((b - i - 1) & 255) for i, b in enumerate(markers) if b != 0x7D)
    if not identity.startswith("FM-1_") or not identity[5:].isascii() or not identity[5:].isdigit():
        raise ValueError(f"{path}: invalid FM-1 identity {identity!r}")
    logical = b"".join(raw[i:i + 47] for i in range(0, 20 * 48, 48)) + raw[20 * 48:]
    header = bytearray(logical[:64])
    enc(header, 0, len(header))
    hdr_crc, table_crc, size, count, flags, version, reserved = unpack_from("<HHIHHHH", header)
    if size != len(logical) or crc16(header[2:]) != hdr_crc:
        raise ValueError(f"{path}: invalid UFW size or header CRC")
    table = logical[64:64 + count * 80]
    if len(table) != count * 80 or crc16(table) != table_crc:
        raise ValueError(f"{path}: invalid UFW entry table")
    entries = []
    cursor = 64 + len(table)
    for idx in range(count):
        record = bytearray(table[idx * 80:(idx + 1) * 80])
        enc(record, 0, len(record))
        kind, index, expected_crc, _, offset, length, padded_length = unpack_from("<HHHHIII", record)
        name = bytes(record[64:]).split(b"\0", 1)[0].decode("ascii")
        if index != idx or offset < cursor or offset + length > size:
            raise ValueError(f"{path}: invalid entry bounds or index for {name}")
        if padded_length != (length + 31) & ~31 or any(b != 0xFF for b in logical[cursor:offset]):
            raise ValueError(f"{path}: invalid padding for {name}")
        payload = logical[offset:offset + length]
        if crc16(payload) != expected_crc:
            raise ValueError(f"{path}: invalid payload CRC for {name}")
        entries.append(Entry(name, kind, offset, length, padded_length, sha256(payload).hexdigest()))
        cursor = offset + length
    if cursor != size:
        raise ValueError(f"{path}: unexpected trailing data")
    chip = bytes(header[16:]).split(b"\0", 1)[0].decode("ascii")
    marker_layout = tuple(i for i, marker in enumerate(markers) if marker != 0x7D)
    print(f"{path.name}: {identity} {len(raw)} bytes SHA256 {sha256(raw).hexdigest()}")
    print(f"  UFW: {chip}, flags {flags:#06x}, version {version:#06x}, reserved {reserved}, {count} entries")
    for entry in entries:
        print(f"  {entry.name}: type {entry.kind}, offset {entry.offset:#x}, size {entry.length}, "
              f"padding length {entry.padded_length}, SHA256 {entry.payload_hash}")
    layout = tuple((e.name, e.kind, e.offset, e.length, e.padded_length) for e in entries)
    return (chip, flags, version, reserved, marker_layout, layout), entries


if __name__ == "__main__":
    if len(sys.argv) != 3:
        raise SystemExit("usage: package_compare_076.py BASELINE.fwsc CANDIDATE.fwsc")
    baseline_layout, baseline_files = inspect(Path(sys.argv[1]))
    candidate_layout, candidate_files = inspect(Path(sys.argv[2]))
    print("UFW and FWSC layout:", "identical" if baseline_layout == candidate_layout else "different")
    for first, second in zip(baseline_files, candidate_files):
        if first.name == second.name:
            print(f"{first.name} payload:", "identical" if first.payload_hash == second.payload_hash else "different")
