#if os(none)
public import externref

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
    subscript<Object: JavaScriptRepresentable & ~Copyable>(
        _ key: StaticString
    ) -> Object {
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
    func append<Object: JavaScriptRepresentable & ~Copyable>(
        _ object: borrowing Object
    ) {
        arrayPush(ref, object.ref)
    }
    
    func append(_ value: Int) {
        arrayPushInt(ref, value)
    }
    
    func append(_ value: Float) {
        arrayPushFloat(ref, value)
    }
    
    subscript<Object: JavaScriptRepresentable & ~Copyable>(
        _ index: Int
    ) -> Object {
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
    init(_ string: String) {
        self.ref = string.withCString { pointer in
            bridgeString(
                UnsafeRawPointer(pointer).assumingMemoryBound(to: UInt8.self),
                string.utf8.count
            )
        }
    }

    init(_ string: StaticString) {
        self.ref = bridgeString(string.utf8Start, string.utf8CodeUnitCount)
    }
    
    var staticString: StaticString {
        StaticString(pointer: stringMemory(ref))
    }
}
#endif
