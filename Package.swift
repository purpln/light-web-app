// swift-tools-version: 6.2

import PackageDescription

#if WASM
let linkerSettings: [LinkerSetting] = [
    .unsafeFlags([
        "-Xclang-linker", "-nostdlib",
        "-Xlinker", "--allow-undefined-file=symbols", //symbols that linker lacks
    ]),
]
#else
let linkerSettings: [LinkerSetting] = []
#endif

let package = Package(name: "Application", products: [
    .executable(name: "Application", targets: ["Application"]),
], dependencies: [
    .package(path: "Packages/Foundation"),
    .package(path: "Packages/JavaScript"),
], targets: [
    .executableTarget(name: "Application", dependencies: [
        .product(name: "Foundation", package: "Foundation"),
        .product(name: "JavaScript", package: "JavaScript"),
    ], linkerSettings: linkerSettings),
])

for target in package.targets {
    guard target.type != .plugin else { continue }
    target.swiftSettings = target.swiftSettings ?? []
    target.swiftSettings? += [
        //swift 6
        .enableUpcomingFeature("StrictConcurrency"),
        
        //swift 7
        .enableUpcomingFeature("ExistentialAny"),
        .enableUpcomingFeature("InternalImportsByDefault"),
        .enableUpcomingFeature("MemberImportVisibility"),
        .enableUpcomingFeature("InferIsolatedConformances"),
        .enableUpcomingFeature("NonisolatedNonsendingByDefault"),
        .enableUpcomingFeature("ImmutableWeakCaptures"),
    ]
}
