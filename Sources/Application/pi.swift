@_expose(wasm, "pi")
@inline(never)
func pi(iterations: UInt) -> Double {
    var quarter = 0.0
    var sign = 1.0
    
    for index in 0..<iterations {
        let denominator = Double(2 * index + 1)
        quarter += sign / denominator
        sign.negate()
    }
    
    return 4.0 * quarter
}
