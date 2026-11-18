import assert from "node:assert/strict";
import { readFileSync } from "node:fs";

let memory;
let clock = 0n;
const wasi = {
    clock_time_get(_clock, _precision, pointer) {
        new DataView(memory.buffer).setBigUint64(pointer, clock, true);
        return 0;
    },
    fd_write(_fd, _iov, _len, pointer) {
        new DataView(memory.buffer).setUint32(pointer, 0, true);
        return 0;
    },
    proc_exit(code) { throw Error(`WASM exited (${code})`); },
};
const stub = new Proxy(wasi, { get: (target, key) => target[key] ?? (() => 0) });
const { instance } = await WebAssembly.instantiate(readFileSync(process.argv[2]), {
    wasi_snapshot_preview1: stub,
    env: new Proxy({}, { get: () => () => 0 }),
});
const app = instance.exports;
memory = app.memory;
app._initialize?.();
app.web_master(2048);
app.web_boot();
assert.equal(app.web_nobj(), 20);
let peak = 0;
const captures = new Set();
for (let i = 0; i < 500; i++) {
    clock += 23219955n;
    app.web_render(1024);
    for (const sample of new Float32Array(memory.buffer, app.web_out_l(), 1024)) {
        assert(Number.isFinite(sample));
        peak = Math.max(peak, Math.abs(sample));
    }
    if (i % 50 === 0) captures.add(app.web_blits());
}
const screen = new Uint16Array(memory.buffer, app.web_fb(), 240 * 240);
assert(screen.some(pixel => pixel !== screen[0]));
assert(peak > 0.001 && peak <= 1 && captures.size >= 3);
console.log(`PASS 512000 WASM audio frames, peak ${peak.toFixed(4)}, moving 240x240 framebuffer, 20 flash objects`);
