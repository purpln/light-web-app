import externref

public protocol JavaScriptRepresentable: ~Copyable {
    init(ref: ExternRefIndex)
    
    var ref: ExternRefIndex { get }
}

public func stringify<Object: JavaScriptRepresentable & ~Copyable>(_ object: borrowing Object) -> JSString {
    JSString(ref: stringify(object.ref))
}

extension JSObject: JavaScriptRepresentable {}

extension JSArray: JavaScriptRepresentable {}

extension JSString: JavaScriptRepresentable {}
