#if os(none)
public import externref

public struct JSClosure: Sendable, ~Copyable {
    let ref: ExternRefIndex

    public init(
        _ callback: @convention(c) (UnsafeMutableRawPointer?) -> Void,
        _ data: UnsafeMutableRawPointer?
    ) {
        self.ref = externref.callback(callback, data)
    }

    deinit {
        freeExternRef(ref)
    }
}

public struct HTMLElement: Sendable, ~Copyable {
    let ref: ExternRefIndex

    init(ref: ExternRefIndex) {
        self.ref = ref
    }

    deinit {
        freeExternRef(ref)
    }

    public func append(child: borrowing HTMLElement) {
        appendChild(ref, child.ref)
    }

    public func addEventListener(
        name: borrowing JSString,
        callback: borrowing JSClosure
    ) {
        externref.addEventListener(ref, name.ref, callback.ref)
    }

    public func getContext(name: borrowing JSString) -> Context {
        Context(ref: externref.getContext(ref, name.ref))
    }

    private static let idName = JSString("id")

    public var id: JSString {
        get {
            JSString(ref: getProperty(ref, Self.idName.ref))
        }
        nonmutating set {
            setProperty(ref, Self.idName.ref, newValue.ref)
        }
    }

    private static let innerHTMLName = JSString("innerHTML")

    public var innerHTML: JSString {
        get {
            JSString(ref: getProperty(ref, Self.innerHTMLName.ref))
        }
        nonmutating set {
            setProperty(ref, Self.innerHTMLName.ref, newValue.ref)
        }
    }

    public var innerHTMLInt: Int {
        get {
            getIntProperty(ref, Self.innerHTMLName.ref)
        }
        nonmutating set {
            setIntProperty(ref, Self.innerHTMLName.ref, newValue)
        }
    }

    private static let widthName = JSString("width")

    public var width: Int {
        get {
            getIntProperty(ref, Self.widthName.ref)
        }
        nonmutating set {
            setIntProperty(ref, Self.widthName.ref, newValue)
        }
    }

    private static let heightName = JSString("height")

    public var height: Int {
        get {
            getIntProperty(ref, Self.heightName.ref)
        }
        nonmutating set {
            setIntProperty(ref, Self.heightName.ref, newValue)
        }
    }
}

public struct Document: Sendable, ~Copyable {
    let object: JSObject

    public static let global = Document(object: JSObject(ref: getDocument()))

    public func getElementById(id: borrowing JSString) -> HTMLElement {
        HTMLElement(ref: externref.getElementById(id.ref))
    }

    public func createElement(name: borrowing JSString) -> HTMLElement {
        HTMLElement(ref: externref.createElement(name.ref))
    }

    private static let bodyName = JSString("body")

    public var body: HTMLElement {
        HTMLElement(ref: getProperty(object.ref, Self.bodyName.ref))
    }
}

public struct Context: Sendable, ~Copyable {
    let ref: ExternRefIndex

    init(ref: ExternRefIndex) {
        self.ref = ref
    }

    deinit {
        freeExternRef(ref)
    }

    private static let fillStyleName = JSString("fillStyle")

    public var fillStyle: JSString {
        get {
            JSString(ref: getProperty(ref, Self.fillStyleName.ref))
        }
        nonmutating set {
            setProperty(ref, Self.fillStyleName.ref, newValue.ref)
        }
    }

    private static let strokeStyleName = JSString("strokeStyle")

    public var strokeStyle: JSString {
        get {
            JSString(ref: getProperty(ref, Self.strokeStyleName.ref))
        }
        nonmutating set {
            setProperty(ref, Self.strokeStyleName.ref, newValue.ref)
        }
    }

    private static let lineWidthName = JSString("lineWidth")

    public var lineWidth: Int {
        get {
            getIntProperty(ref, Self.lineWidthName.ref)
        }
        nonmutating set {
            setIntProperty(ref, Self.lineWidthName.ref, newValue)
        }
    }

    public func fillRect(x: Int, y: Int, width: Int, height: Int) {
        externref.fillRect(ref, x, y, width, height)
    }

    public func beginPath() {
        externref.beginPath(ref)
    }

    public func closePath() {
        externref.closePath(ref)
    }

    public func moveTo(x: Int, y: Int) {
        externref.moveTo(ref, x, y)
    }

    public func lineTo(x: Int, y: Int) {
        externref.lineTo(ref, x, y)
    }

    public func stroke() {
        externref.stroke(ref)
    }
}
#endif
