import JavaScript

@main
struct Application {
    func run() {
#if os(none)
        JavaScript().run()
#endif
        Threads().run()
        Lock().run()
    }
}

extension Application {
    static func main() {
        Application().run()
    }
}
