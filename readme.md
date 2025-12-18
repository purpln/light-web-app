```shell
swift build -c release --triple wasm32-unknown-none-wasm \
    -Xswiftc -enable-experimental-feature -Xswiftc Embedded \
    -Xswiftc -enable-experimental-feature -Xswiftc Extern \
    -Xswiftc -Xfrontend -Xswiftc -gnone
wasm-opt -Os --strip-debug .build/release/Application.wasm -o Web/app.wasm
cd Web && python3 -m http.server 8080
```
