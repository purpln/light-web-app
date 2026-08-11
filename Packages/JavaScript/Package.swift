// swift-tools-version: 6.1

import PackageDescription

let package = Package(name: "JavaScript", products: [
    .library(name: "JavaScript", targets: ["JavaScript"]),
], targets: [
    .target(name: "externref"),
    .target(name: "JavaScript", dependencies: [
        "externref",
    ]),
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
