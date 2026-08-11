import { createMainBridge } from './bridge.js';

const threads = new Map();
let nextThreadId = 1;
let bridge = null;
let memory = null;
let wasmModule = null;

function log(string) {
    console.log(string.endsWith('\n') ? string.slice(0, -1) : string);
}

function terminateThread(id) {
    const thread = threads.get(id);
    if (thread === undefined) {
        log(`[thread ${id}] cannot terminate an unknown thread\n`);
        return;
    }

    thread.terminate();
    threads.delete(id);
}

function handleWorkerMessage(id, event) {
    switch (event.data.method) {
    case 'log':
        log(id === 0 ? event.data.string : `[thread ${id}] ${event.data.string}`);
        break;
    case 'spawn': {
        const childId = spawnThread(event.data.argument);
        Atomics.store(event.data.threadId, 0, childId);
        Atomics.notify(event.data.threadId, 0);
        break;
    }
    case 'remote':
        bridge.handle(id, event.data);
        break;
    case 'retain':
        bridge.retain(event.data.reference);
        break;
    case 'release':
        bridge.release(event.data.reference);
        break;
    case 'terminate':
        terminateThread(id);
        break;
    default:
        log(`[thread ${id}] unknown callback ${JSON.stringify(event.data)}\n`);
    }
}

function startThread(id, argument) {
    const thread = new Worker('worker.js', { name: `${id}`, type: 'module' });

    thread.onerror = error => {
        log(`[thread ${id}] onerror: ${error.toString()}\n`);
    };
    thread.onmessage = event => handleWorkerMessage(id, event);
    thread.onmessageerror = error => {
        log(`[thread ${id}] onmessageerror: ${error.toString()}\n`);
    };

    threads.set(id, thread);
    thread.postMessage({
        method: 'instantiate',
        threadId: id,
        argument,
        module: wasmModule,
        memory,
    });
    return id;
}

function spawnThread(argument) {
    return startThread(nextThreadId++, argument);
}

async function instantiate(bytes) {
    try {
        memory = new WebAssembly.Memory({
            initial: 2,
            maximum: 65536,
            shared: true,
        });
        wasmModule = await WebAssembly.compile(bytes);
        bridge = createMainBridge(threads, memory, log);
        startThread(0);
    } catch (error) {
        log(`${error.toString()}\n`);
    }
}

function load(path) {
    fetch(path)
        .then(response => {
            if (!response.ok) {
                throw new Error(`request failed with status ${response.status}`);
            }
            return response.arrayBuffer();
        })
        .then(instantiate)
        .catch(error => log(`${error.toString()}\n`));
}

load('app.wasm');
