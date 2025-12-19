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
    if c == 10 || c == 0 || length >= BUFFER_SIZE - 1 {
        // Ensure null termination
        if length < BUFFER_SIZE {
            letters[length] = 0
        } else {
            letters[BUFFER_SIZE - 1] = 0
        }
        
        _ = puts(letters)
        length = 0
        
        // If it was a regular character that filled the buffer,
        // we still need to handle the current character
        if c != 10 && c != 0 && length < BUFFER_SIZE - 1 {
            letters[0] = CChar(truncatingIfNeeded: c)
            length = 1
        }
    } else {
        letters[length] = CChar(truncatingIfNeeded: c)
        length += 1
    }
    return c
}

@_cdecl("strlen")
func strlen(_ str: UnsafePointer<CChar>?) -> Int {
    guard let str = str else { return 0 }
    var count = 0
    var pointer = str
    while pointer.pointee != 0 {
        count += 1
        pointer = pointer.advanced(by: 1)
    }
    return count
}
