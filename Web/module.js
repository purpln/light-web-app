const decoder = new TextDecoder('utf-8');
const encoder = new TextEncoder();

export async function instantiateWasm(
    module,
    memory,
    { log, spawn, imports: additionalImports = {} }
) {
    let instance = null;

    function memoryAsString(address, length) {
        const bytes = new Uint8Array(memory.buffer, address, length);
        return decoder.decode(Uint8Array.from(bytes));
    }

    function pointerAsString(address) {
        const bytes = new Uint8Array(memory.buffer);
        let terminator = address;

        while (terminator < bytes.length && bytes[terminator] !== 0) {
            terminator++;
        }

        if (terminator === bytes.length) {
            throw new RangeError(`unterminated string at address ${address}`);
        }

        return decoder.decode(Uint8Array.from(bytes.subarray(address, terminator)));
    }

    function stringAsMemory(string) {
        if (instance === null) {
            throw new Error('WebAssembly instance is not initialized');
        }

        const bytes = encoder.encode(`${string}\0`);
        const address = instance.exports.malloc(bytes.length);
        new Uint8Array(memory.buffer).set(bytes, address);
        return address;
    }

    function puts(address) {
        const string = pointerAsString(address);
        log(`${string}\n`);
        return string.length;
    }

    function indirect(index, address) {
        instance.exports.__indirect_function_table.get(index)(address);
    }

    const imports = {
        ...additionalImports,
        js: {
            emptyObject: () => ({}),
            emptyArray: () => [],
            arrayPush: (self, element) => self.push(element),
            bridgeString: memoryAsString,
            stringMemory: stringAsMemory,
            getProperty: (self, name) => self[name],
            setProperty: (self, name, value) => { self[name] = value; },
            stringify: JSON.stringify,
            callback: (index, address) => () => indirect(index, address),
            ...additionalImports.js,
        },
        env: {
            memory,
            puts,
            ...additionalImports.env,
        },
        wasi: {
            'thread-spawn': spawn,
            ...additionalImports.wasi,
        },
    };

    instance = await WebAssembly.instantiate(module, imports);
    return instance;
}
