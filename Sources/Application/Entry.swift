#if os(none)
@_extern(c, "__main_argc_argv")
func main(_ argc: Int32, _ argv: UnsafeMutablePointer<UnsafeMutablePointer<Int8>?>) -> Int32

@_cdecl("_start")
public func entry() {
    let pointer = UnsafeMutablePointer<UnsafeMutablePointer<Int8>?>.allocate(capacity: 1)
    unsafe pointer.initialize(to: nil)
    _ = main(0, unsafe pointer)
}
#endif
