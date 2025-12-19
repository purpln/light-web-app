private struct StaticStringLayout {
    let pointer: Int
    let count: Int
    let flags: Int8
    
    init(pointer: UnsafePointer<Int8>) {
        self.pointer = Int(bitPattern: pointer)
        self.count = strlen(pointer)
        self.flags = 0
    }
}

extension StaticString {
    init(pointer: UnsafePointer<Int8>) {
        let layout = StaticStringLayout(pointer: pointer)
        let pointer = UnsafeMutablePointer<StaticStringLayout>.allocate(capacity: 1)
        pointer.initialize(to: layout)
        self = UnsafeRawPointer(pointer).load(as: StaticString.self)
    }
}
