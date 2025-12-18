// swift-tools-version: 6.2

import PackageDescription

let package = Package(name: "Application", products: [
    .executable(name: "Application", targets: ["Application"]),
], dependencies: [
    .package(url: "https://github.com/swiftwasm/swift-dlmalloc.git", branch: "main"),
], targets: [
    .executableTarget(name: "Application", dependencies: [
        .product(name: "dlmalloc", package: "swift-dlmalloc"),
        "Library",
    ], swiftSettings: [
        .unsafeFlags([
            "-Xfrontend", "-disable-stack-protector", //minimal app misses some symbols
        ]),
    ], linkerSettings: [
        .unsafeFlags([
            "-Xclang-linker", "-mexec-model=command",
            //"-Xclang-linker", "-mexec-model=reactor", //no _start entry
            "-Xclang-linker", "-nostdlib",
            //"-Xlinker", "--no-entry",
            "-Xlinker", "--allow-undefined-file=symbols", //symbols that linker lacks
            "-Xlinker", "--export-if-defined=__indirect_function_table",
            "-Xlinker", "--export=malloc",
            "-Xlinker", "--export=free",
        ]),
    ]),
    .target(name: "externref"),
    .target(name: "Foundation", dependencies: [
        "externref",
    ]),
    .target(name: "Library", dependencies: [
        "externref",
    ]),
])
