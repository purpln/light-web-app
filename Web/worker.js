import { createWorkerBridge } from './bridge.js';
import { instantiateWasm } from './module.js';

let bridge = null;
let instance = null;
let threadId = null;

function log(string) {
    postMessage({ method: 'log', string });
}

function spawnThread(argument) {
    const threadId = new Int32Array(new SharedArrayBuffer(4));
    Atomics.store(threadId, 0, -1);

    postMessage({ method: 'spawn', argument, threadId });
    Atomics.wait(threadId, 0, -1);
    return Atomics.load(threadId, 0);
}

async function instantiate(message) {
    threadId = message.threadId;
    try {
        bridge = createWorkerBridge(message.memory);
        instance = await instantiateWasm(message.module, message.memory, {
            log,
            spawn: spawnThread,
            imports: bridge.imports,
        });
        bridge.setInstance(instance);

        if (threadId === 0) {
            instance.exports._start();
        } else {
            instance.exports.__wasi_thread_start(threadId, message.argument);
        }
    } catch (error) {
        log(`${error.toString()}\n`);
    } finally {
        if (threadId !== 0 || !bridge.hasCallbacks()) {
            postMessage({ method: 'terminate' });
        }
    }
}

onmessage = event => {
    if (event.data.method === 'callback') {
        bridge.invoke(event.data.callback);
    } else if (event.data.method === 'release') {
        bridge.releaseCallback(event.data.callback);
        if (threadId === 0 && !bridge.hasCallbacks()) {
            postMessage({ method: 'terminate' });
        }
    } else if (event.data.method === 'instantiate') {
        instantiate(event.data);
    } else {
        log(`unknown callback ${JSON.stringify(event.data)}\n`);
    }
};

onerror = error => {
    log(`internal onerror: ${error.toString()}\n`);
};

onmessageerror = error => {
    log(`internal onmessageerror: ${error.toString()}\n`);
};
