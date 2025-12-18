let instance = null;

const decoder = new TextDecoder();
const encoder = new TextEncoder();

function wasmMemoryAsString(address, length) {
    return decoder.decode(instance.exports.memory.buffer.slice(address, address + length));
}

function stringAsWasmMemory(string) {
    const bytes = encoder.encode(string + '\0');
    const address = instance.exports.malloc(bytes.length);
    
    const memory = new Uint8Array(instance.exports.memory.buffer);
    memory.set(bytes, address);
    
    return address;
}

function puts(address) {
    const buffer = new Uint8Array(instance.exports.memory.buffer);
    
    let terminator = address;
    while (terminator < buffer.length && buffer[terminator] !== 0) {
        terminator++;
    }
    
    const slice = buffer.subarray(address, terminator);
    const string = decoder.decode(buffer.subarray(address, terminator));
    
    console.log(string);
    
    return string ? string.length : 0;
}

function indirect(index, address, parameters) {
    console.log(parameters);
    instance.exports.__indirect_function_table.get(index)(address);
}

const imports = {
    js: {
        emptyObject: () => { return {} },
        emptyArray: () => [],
        arrayPush: (self, element) => self.push(element),
        bridgeString: (address, length) => wasmMemoryAsString(address, length),
        stringMemory: (string) => stringAsWasmMemory(string),
        getProperty: (self, name) => self[name],
        setProperty: (self, name, value) => { self[name] = value; },
        stringify: (self) => JSON.stringify(self),
        callback: (index, address) => (...parameters) => indirect(index, address, parameters),
        getDocument: () => document,
    },
    document: {
        getElementById: (id) => document.getElementById(id),
        createElement: (name) => document.createElement(name),
        appendChild: (element, child) => element.appendChild(child),
        addEventListener: (element, name, callback) => element.addEventListener(name, callback),
    },
    env: {
        puts: (address) => puts(address),
    },
}

fetch("app.wasm")
.then(response => {
    if (!response.ok) {
        throw new Error(`status: ${response.status}`);
    }
    return response.arrayBuffer();
})
.then(result => {
    const bytes = new Uint8Array(result);
    
    WebAssembly.compile(bytes)
    .then(module => {
        WebAssembly.instantiate(module, imports)
        .then(result => {
            instance = result
            
            instance.exports._start();
        })
        .catch(error => {
            console.error(`instantiate: ${error.toString()}`);
        });
    })
    .catch(error => {
        console.error(`compile: ${error.toString()}`);
    });
})
.catch(error => {
    console.error(`fetch: ${error.toString()}`);
});
