// swift-tools-version: 6.1

import PackageDescription

let package = Package(name: "Foundation", products: [
    .library(name: "Foundation", targets: ["Foundation", "runtime", "threads", "wasm"]),
], targets: [
    .target(name: "Foundation", dependencies: [
        "runtime",
        "threads",
        "wasm",
    ]),
    .target(name: "runtime"),
    .target(name: "threads", dependencies: [
        "wasm",
    ]),
    .target(name: "wasm"),
])

for target in package.targets {
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
