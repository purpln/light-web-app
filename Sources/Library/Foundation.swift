#if hasFeature(Extern)
@_extern(wasm, module: "env", name: "puts")
@_extern(c)
func puts(_ str: UnsafePointer<CChar>?) -> CInt
#endif

let BUFFER_SIZE = 1024
nonisolated(unsafe)
var letters = [CChar](repeating: 0, count: BUFFER_SIZE)
nonisolated(unsafe)
var length: Int = 0

@_cdecl("putchar")
public func putchar(_ c: CInt) -> CInt {
    // Handle newline, null terminator, or buffer full
    if unsafe c == 10 || c == 0 || length >= BUFFER_SIZE - 1 {
        // Ensure null termination
        if unsafe length < BUFFER_SIZE {
            unsafe letters[length] = 0
        } else {
            unsafe letters[BUFFER_SIZE - 1] = 0
        }
        
        _ = unsafe puts(letters)
        unsafe length = 0
        
        // If it was a regular character that filled the buffer,
        // we still need to handle the current character
        if unsafe c != 10 && c != 0 && length < BUFFER_SIZE - 1 {
            unsafe letters[0] = CChar(truncatingIfNeeded: c)
            unsafe length = 1
        }
    } else {
        unsafe letters[length] = CChar(truncatingIfNeeded: c)
        unsafe length += 1
    }
    return c
}

@_cdecl("strlen")
func strlen(_ str: UnsafePointer<CChar>?) -> Int {
    guard let str = unsafe str else { return 0 }
    var count = 0
    var pointer = unsafe str
    while unsafe pointer.pointee != 0 {
        count += 1
        unsafe pointer = unsafe pointer.advanced(by: 1)
    }
    return count
}
