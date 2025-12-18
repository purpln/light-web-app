import externref

public struct JSObject: Sendable, ~Copyable {
    public let ref: ExternRefIndex
    
    public init(ref: ExternRefIndex) {
        self.ref = ref
    }
    
    public init() {
        self.ref = emptyObject()
    }
    
    deinit {
        freeExternRef(ref)
    }
}

public extension JSObject {
    subscript<Object: JavaScriptRepresentable & ~Copyable>(_ key: StaticString) -> Object {
        get {
            let key = JSString(key)
            return Object(ref: getProperty(ref, key.ref))
        }
        nonmutating set {
            let key = JSString(key)
            setProperty(ref, key.ref, newValue.ref)
        }
    }
    
    subscript(_ key: StaticString) -> Int {
        get {
            let key = JSString(key)
            return getIntProperty(ref, key.ref)
        }
        nonmutating set {
            let key = JSString(key)
            setIntProperty(ref, key.ref, newValue)
        }
    }
    
    subscript(_ key: StaticString) -> Float {
        get {
            let key = JSString(key)
            return getFloatProperty(ref, key.ref)
        }
        nonmutating set {
            let key = JSString(key)
            setFloatProperty(ref, key.ref, newValue)
        }
    }
}

public struct JSArray: Sendable, ~Copyable {
    public let ref: ExternRefIndex
    
    public init(ref: ExternRefIndex) {
        self.ref = ref
    }
    
    public init() {
        self.ref = emptyArray()
    }
    
    deinit {
        freeExternRef(ref)
    }
}

public extension JSArray {
    func append<Object: JavaScriptRepresentable & ~Copyable>(_ object: borrowing Object) {
        arrayPush(ref, object.ref)
    }
    
    func append(_ value: Int) {
        arrayPushInt(ref, value)
    }
    
    func append(_ value: Float) {
        arrayPushFloat(ref, value)
    }
    
    subscript<Object: JavaScriptRepresentable & ~Copyable>(_ index: Int) -> Object {
        get {
            return Object(ref: arrayGet(ref, index))
        }
        nonmutating set {
            arraySet(ref, index, newValue.ref)
        }
    }
    
    subscript(_ index: Int) -> Int {
        get {
            return arrayGetInt(ref, index)
        }
        nonmutating set {
            arraySetInt(ref, index, newValue)
        }
    }
    
    subscript(_ index: Int) -> Float {
        get {
            return arrayGetFloat(ref, index)
        }
        nonmutating set {
            arraySetFloat(ref, index, newValue)
        }
    }
}

public struct JSString: Sendable, ~Copyable {
    public let ref: ExternRefIndex
    
    public init(ref: ExternRefIndex) {
        self.ref = ref
    }
    
    deinit {
        freeExternRef(ref)
    }
}

public extension JSString {
    init(_ string: StaticString) {
        self.ref = unsafe bridgeString(string.utf8Start, string.utf8CodeUnitCount)
    }
    
    var string: StaticString {
        unsafe StaticString(pointer: stringMemory(ref))
    }
}

public struct JSClosure: Sendable, ~Copyable {
    let ref: ExternRefIndex
    
    public init(_ callback: @convention(c) (UnsafeMutableRawPointer?) -> Void, _ data: UnsafeMutableRawPointer?) {
        self.ref = externref.callback(callback, data)
    }
    
    deinit {
        freeExternRef(ref)
    }
}

public struct HTMLElement: ~Copyable {
    let ref: ExternRefIndex
    
    public func append(child: borrowing HTMLElement) {
        appendChild(self.ref, child.ref)
    }
    
    public func addEventListener(name: borrowing JSString, callback: borrowing JSClosure) {
        externref.addEventListener(self.ref, name.ref, callback.ref)
    }
    
    public static let innerHTMLName = JSString("innerHTML")
    
    public var innerHTML: JSString {
        get {
            JSString(ref: getProperty(self.ref, Self.innerHTMLName.ref))
        }
        nonmutating set {
            setProperty(self.ref, Self.innerHTMLName.ref, newValue.ref)
        }
    }
}

public struct Document: Sendable, ~Copyable {
    let object: JSObject
    
    public static let global = Document(object: JSObject(ref: getDocument()))
    
    public static let bodyName = JSString("body")
    
    public func getElementById(id: borrowing JSString) -> HTMLElement {
        HTMLElement(ref: externref.getElementById(id.ref))
    }
    
    public func createElement(name: borrowing JSString) -> HTMLElement {
        HTMLElement(ref: externref.createElement(name.ref))
    }
    
    public var body: HTMLElement {
        HTMLElement(ref: getProperty(self.object.ref, Self.bodyName.ref))
    }
}
