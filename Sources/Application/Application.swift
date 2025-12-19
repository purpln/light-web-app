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

nonisolated(unsafe) var value: Float = 0 {
    didSet {
        let element = Document.global.getElementById(id: JSString("counter"))
        element.innerHTML = JSString(value)
    }
}

@main
struct Application {
    func run() throws {
        html()
        log()
        json()
    }
    
    func html() {
        text()
        canvas()
        counter()
    }
    
    func text() {
        let body = Document.global.body
        
        let h1 = Document.global.createElement(name: JSString("h1"))
        body.append(child: h1)
        h1.innerHTML = JSString("Hello, World!")
    }
    
    func canvas() {
        let body = Document.global.body
        
        let canvas = Document.global.createElement(name: JSString("canvas"))
        canvas.width = 500
        canvas.height = 400
        body.append(child: canvas)
        let context = canvas.getContext(name: JSString("2d"))
        context.fillStyle = JSString("#00ff00")
        context.fillRect(x: 20, y: 20, width: 200, height: 100)
        
        context.strokeStyle = JSString("#ff0000")
        context.lineWidth = 4
        
        context.beginPath()
        context.moveTo(x: 80, y: 200)
        context.lineTo(x: 160, y: 60)
        context.lineTo(x: 240, y: 200)
        context.closePath()
        context.stroke()
    }
    
    func counter() {
        let body = Document.global.body
        
        button(label: "-0.1", {
            value -= 0.1
        })
        
        let h1 = Document.global.createElement(name: JSString("h4"))
        h1.id = JSString("counter")
        body.append(child: h1)
        h1.innerHTMLInt = 0
        
        button(label: "+0.1", {
            value += 0.1
        })
    }
    
    func button(label: StaticString, _ closure: @escaping () -> Void) {
        let body = Document.global.body
        
        let button = Document.global.createElement(name: JSString("button"))
        body.append(child: button)
        button.innerHTML = JSString(label)
        let closure = Callback(closure)
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
        array.append(0.1)
        array.append(1)
        print(JSString(array[1] as Float).string)
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
