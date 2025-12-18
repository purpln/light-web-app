import Library

class Callback {
    private let body: () -> Void
    
    init(_ body: @escaping () -> Void) {
        self.body = body
    }
    
    func callback() {
        body()
    }
}

func callback(_ pointer: UnsafeMutableRawPointer?) {
    guard let pointer = pointer else { return }
    let context = Unmanaged<Callback>.fromOpaque(pointer).takeUnretainedValue()
    context.callback()
}

@main
struct Application {
    func run() throws {
        html()
        log()
        json()
    }
    
    func html() {
        let body = Document.global.body
        let h1 = Document.global.createElement(name: JSString("h1"))
        body.append(child: h1)
        h1.innerHTML = JSString("Hello, World!")
        
        let button = Document.global.createElement(name: JSString("button"))
        body.append(child: button)
        button.innerHTML = JSString("click")
        let closure = Callback {
            print("click")
            let h1 = Document.global.createElement(name: JSString("h4"))
            body.append(child: h1)
            h1.innerHTML = JSString("Hello, World!")
        }
        let pointer = Unmanaged.passRetained(closure).toOpaque()
        button.addEventListener(name: JSString("click"), callback: JSClosure(callback, pointer))
    }
    
    func log() {
        print("Hello, World! from wasm directly")
        
        let jsString = JSString("Hello, world! from wasm through js")
        print(jsString.string)
    }
    
    func json() {
        let object = JSObject()
        object["string"] = JSString("Hello, world!")
        object["value"] = 1
        let array = JSArray()
        array[0] = JSString("0")
        array.append(1.0)
        array.append(2)
        object["array"] = array
        let description = stringify(object)
        print(description.string)
    }
}

extension Application {
    static func main() throws {
        try Application().run()
    }
}
