const encoder = new TextEncoder();
const remoteReference = Symbol('remoteReference');
const callbackReference = Symbol('callbackReference');

export function createMainBridge(threads, memory, log) {
    const objects = new Map([[1, { value: document, references: Infinity }]]);
    const references = new Map([[document, 1]]);
    let nextReference = 2;

    const finalizer = typeof FinalizationRegistry === 'undefined' ? null
        : new FinalizationRegistry(({ thread, callback }) => {
            threads.get(thread)?.postMessage({ method: 'release', callback });
        });

    function retainValue(value) {
        const existing = references.get(value);
        if (existing !== undefined) {
            objects.get(existing).references++;
            return existing;
        }

        const reference = nextReference++;
        references.set(value, reference);
        objects.set(reference, { value, references: 1 });
        return reference;
    }

    function retain(reference) {
        const entry = objects.get(reference);
        if (entry !== undefined && entry.references !== Infinity) {
            entry.references++;
        }
    }

    function release(reference) {
        const entry = objects.get(reference);
        if (entry === undefined || entry.references === Infinity) return;
        if (--entry.references !== 0) return;
        references.delete(entry.value);
        objects.delete(reference);
    }

    function decode(value) {
        if (value?.remote !== undefined) {
            const entry = objects.get(value.remote);
            if (entry === undefined) {
                throw new ReferenceError(`unknown remote reference ${value.remote}`);
            }
            return entry.value;
        }
        return value;
    }

    function encode(result, reply) {
        const state = new Int32Array(reply);
        const number = new Float64Array(reply, 16, 1);

        if (result === undefined) {
            state[1] = 0;
        } else if (result === null) {
            state[1] = 1;
        } else if (typeof result === 'boolean') {
            state[1] = 2;
            state[2] = result;
        } else if (typeof result === 'number') {
            state[1] = 3;
            number[0] = result;
        } else {
            state[1] = 4;
            state[2] = retainValue(result);
        }

        Atomics.store(state, 0, 1);
        Atomics.notify(state, 0);
    }

    function listener(thread, callback) {
        const value = () => {
            threads.get(thread)?.postMessage({ method: 'callback', callback });
        };
        finalizer?.register(value, { thread, callback });
        return value;
    }

    function handle(thread, message) {
        try {
            const args = message.arguments.map(decode);
            let result;

            switch (message.operation) {
            case 'get':
                result = args[0][args[1]];
                break;
            case 'set':
                args[0][args[1]] = args[2];
                break;
            case 'call':
                result = args[0][args[1]].apply(args[0], args.slice(2));
                break;
            case 'listen':
                args[0].addEventListener(
                    args[1],
                    listener(thread, message.callback)
                );
                break;
            case 'stringLength':
                result = encoder.encode(String(args[0])).length;
                break;
            case 'copyString': {
                const bytes = encoder.encode(String(args[0]));
                new Uint8Array(memory.buffer).set(bytes, args[1]);
                new Uint8Array(memory.buffer)[args[1] + bytes.length] = 0;
                break;
            }
            default:
                throw new Error(`unknown remote operation ${message.operation}`);
            }

            encode(result, message.reply);
        } catch (error) {
            log(`${error.toString()}\n`);
            const state = new Int32Array(message.reply);
            Atomics.store(state, 0, -1);
            Atomics.notify(state, 0);
        }
    }

    return { handle, release, retain };
}

export function createWorkerBridge(memory) {
    const callbacks = new Map();
    let nextCallback = 1;
    let instance = null;

    function remote(reference) {
        return { [remoteReference]: reference };
    }

    function encode(value) {
        return value?.[remoteReference] === undefined
            ? value : { remote: value[remoteReference] };
    }

    function call(operation, arguments$1, callback) {
        const reply = new SharedArrayBuffer(24);
        const state = new Int32Array(reply);
        postMessage({
            method: 'remote',
            operation,
            arguments: arguments$1.map(encode),
            callback,
            reply,
        });
        Atomics.wait(state, 0, 0);
        if (state[0] < 0) throw new Error(`remote ${operation} failed`);

        switch (state[1]) {
        case 0: return undefined;
        case 1: return null;
        case 2: return state[2] !== 0;
        case 3: return new Float64Array(reply, 16, 1)[0];
        case 4: return remote(state[2]);
        default: throw new Error(`unknown remote result ${state[1]}`);
        }
    }

    function indirect(index, address) {
        instance.exports.__indirect_function_table.get(index)(address);
    }

    function createCallback(index, address) {
        const reference = nextCallback++;
        const value = () => indirect(index, address);
        value[callbackReference] = reference;
        callbacks.set(reference, { index, address, references: 1 });
        return value;
    }

    function retainCallback(reference) {
        const callback = callbacks.get(reference);
        if (callback !== undefined) callback.references++;
    }

    function releaseCallback(reference) {
        const callback = callbacks.get(reference);
        if (callback === undefined) return;
        if (--callback.references === 0) callbacks.delete(reference);
    }

    function retain(value) {
        if (value?.[remoteReference] !== undefined) {
            postMessage({ method: 'retain', reference: value[remoteReference] });
        } else if (value?.[callbackReference] !== undefined) {
            retainCallback(value[callbackReference]);
        }
    }

    function release(value) {
        if (value?.[remoteReference] !== undefined) {
            postMessage({ method: 'release', reference: value[remoteReference] });
        } else if (value?.[callbackReference] !== undefined) {
            releaseCallback(value[callbackReference]);
        }
    }

    function stringMemory(value) {
        if (value?.[remoteReference] !== undefined) {
            const length = call('stringLength', [value]);
            const address = instance.exports.malloc(length + 1);
            call('copyString', [value, address]);
            return address;
        }

        const bytes = encoder.encode(`${value}\0`);
        const address = instance.exports.malloc(bytes.length);
        new Uint8Array(memory.buffer).set(bytes, address);
        return address;
    }

    function getProperty(self, name) {
        return self?.[remoteReference] === undefined
            ? self[name] : call('get', [self, name]);
    }

    function setProperty(self, name, value) {
        if (self?.[remoteReference] === undefined) {
            self[name] = value;
        } else {
            call('set', [self, name, value]);
        }
    }

    const root = remote(1);
    const imports = {
        js: {
            callback: createCallback,
            getProperty,
            retain,
            release,
            setProperty,
            stringMemory,
        },
        document: {
            getDocument: () => root,
            getElementById: id => call('call', [root, 'getElementById', id]),
            createElement: name => call('call', [root, 'createElement', name]),
            getContext: (element, name) =>
                call('call', [element, 'getContext', name]),
            appendChild: (element, child) =>
                call('call', [element, 'appendChild', child]),
            addEventListener: (element, name, callback) =>
                call('listen', [element, name], callback[callbackReference]),
            fillRect: (context, ...args) =>
                call('call', [context, 'fillRect', ...args]),
            beginPath: context => call('call', [context, 'beginPath']),
            closePath: context => call('call', [context, 'closePath']),
            moveTo: (context, ...args) =>
                call('call', [context, 'moveTo', ...args]),
            lineTo: (context, ...args) =>
                call('call', [context, 'lineTo', ...args]),
            stroke: context => call('call', [context, 'stroke']),
        },
    };

    return {
        hasCallbacks: () => callbacks.size !== 0,
        imports,
        invoke: reference => {
            const callback = callbacks.get(reference);
            if (callback !== undefined) indirect(callback.index, callback.address);
        },
        releaseCallback,
        setInstance: value => { instance = value; },
    };
}
