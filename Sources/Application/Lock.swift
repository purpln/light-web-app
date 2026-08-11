#if os(none)
import threads
#elseif canImport(Darwin)
import Darwin.C
#endif

private class Counter {
    var mutex = pthread_mutex_t()
    var value = 0

    init() {
        pthread_mutex_init(&mutex, nil)
    }

    func increment() {
        pthread_mutex_lock(&mutex)
        value += 1
        pthread_mutex_unlock(&mutex)
    }

    deinit {
        pthread_mutex_destroy(&mutex)
    }
}

private class Context {
    let iterations: Int
    let counter: Counter

    init(iterations: Int, counter: Counter) {
        self.iterations = iterations
        self.counter = counter
    }
}

#if os(none)
private func body(_ pointer: UnsafeMutableRawPointer?) -> UnsafeMutableRawPointer? {
    guard let pointer = pointer else { return nil }
    let context = Unmanaged<Context>.fromOpaque(pointer).takeUnretainedValue()
    for _ in 0..<context.iterations {
        context.counter.increment()
    }
    return pointer
}
#else
private func body(_ pointer: UnsafeMutableRawPointer) -> UnsafeMutableRawPointer? {
    let context = Unmanaged<Context>.fromOpaque(pointer).takeUnretainedValue()
    for _ in 0..<context.iterations {
        context.counter.increment()
    }
    return pointer
}
#endif

private let iterations = 1_000_000
private let count = 10

class Lock {
    private var threads: [pthread_t] = []
    private let counter = Counter()
    
    func run() {
        for _ in 0..<count {
            let context = Context(iterations: iterations, counter: counter)
            let pointer = Unmanaged.passRetained(context).toOpaque()
            
            var thread: pthread_t?
            guard pthread_create(&thread, nil, body, pointer) == 0 else {
                Unmanaged<Context>.fromOpaque(pointer).release()
                continue
            }
            threads.append(thread!)
        }
        
        for thread in threads {
            guard pthread_equal(thread, nil) == 0 else { continue }
            var result: UnsafeMutableRawPointer?
            pthread_join(thread, &result)
            if let pointer = result {
                _ = Unmanaged<Context>.fromOpaque(pointer).takeRetainedValue()
            }
        }
        
        print(counter.value)
    }
}
