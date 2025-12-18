private struct StaticStringLayout {
    let pointer: Int
    let count: Int
    let flags: Int8
    
    init(pointer: UnsafePointer<Int8>) {
        self.pointer = Int(bitPattern: pointer)
        self.count = unsafe strlen(pointer)
        self.flags = 0
    }
}

extension StaticString {
    init(pointer: UnsafePointer<Int8>) {
        let layout = unsafe StaticStringLayout(pointer: pointer)
        let pointer = UnsafeMutablePointer<StaticStringLayout>.allocate(capacity: 1)
        unsafe pointer.initialize(to: layout)
        self = unsafe UnsafeRawPointer(pointer).load(as: StaticString.self)
    }
}
