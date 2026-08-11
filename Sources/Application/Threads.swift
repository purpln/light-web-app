#if os(none)
import threads
#elseif canImport(Darwin)
import Darwin.C
#endif

private class Context<Result> {
    let closure: () -> Result
    var result: Result?
    
    init(_ closure: @escaping () -> Result) {
        self.closure = closure
    }
}

#if os(none)
private func body(_ pointer: UnsafeMutableRawPointer?) -> UnsafeMutableRawPointer? {
    guard let pointer = pointer else { return nil }
    let context = Unmanaged<Context<Double>>.fromOpaque(pointer).takeUnretainedValue()
    context.result = context.closure()
    return pointer
}
#else
private func body(_ pointer: UnsafeMutableRawPointer) -> UnsafeMutableRawPointer? {
    let context = Unmanaged<Context<Double>>.fromOpaque(pointer).takeUnretainedValue()
    context.result = context.closure()
    return pointer
}
#endif

private let iterations: UInt = 2_000_000_000
private let count = 10

class Threads {
    private var threads: [pthread_t] = []
    
    func run() {
        for _ in 0..<count {
            let context = Context(action)
            let pointer = Unmanaged.passRetained(context).toOpaque()
            
            var thread: pthread_t?
            guard pthread_create(&thread, nil, body, pointer) == 0 else {
                Unmanaged<Context<Double>>.fromOpaque(pointer).release()
                continue
            }
            
            threads.append(thread!)
        }
        
        for thread in threads {
            guard pthread_equal(thread, nil) == 0 else { continue }
            
            var result: UnsafeMutableRawPointer?
            pthread_join(thread, &result)
            guard let pointer = result else { continue }
            let context = Unmanaged<Context<Double>>.fromOpaque(pointer).takeRetainedValue()
            let value = context.result.map({ "\($0)" }) ?? "nil"
            print("\(value)")
        }
    }
}

private func action() -> Double {
    print(pthread_self().id)
    
    return pi(iterations: iterations)
}
/*
func memory() {
    let capacity = 256 * 1024 * 1024
    guard let pointer = malloc(capacity) else { return }
    
    let bytes = pointer.assumingMemoryBound(to: UInt8.self)
    let buffer = UnsafeMutableBufferPointer(start: bytes, count: capacity)
    
    let index = capacity - 1
    buffer[index] = 1
    
    free(pointer)
}
*/
private extension pthread_t {
    var id: Int {
#if os(none)
        Int(pointee.tid)
#else
        Int(pthread_mach_thread_np(self))
#endif
    }
}
