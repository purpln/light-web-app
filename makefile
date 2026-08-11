.PHONY: build

build:
	rm -rf .build
	swift build -c release --triple wasm32-unknown-none-wasm \
		-Xbuild-tools-swiftc -DWASM \
		-Xswiftc -enable-experimental-feature -Xswiftc Embedded \
		-Xswiftc -enable-experimental-feature -Xswiftc Extern \
		-Xcc -pthread \
		-Xcc -Wall \
		-Xcc -Wextra \
		-Xcc -mmultivalue \
		-Xcc -matomics \
		-Xcc -mbulk-memory \
		-Xlinker --shared-memory \
		-Xlinker --import-memory \
		-Xlinker --export-if-defined=__indirect_function_table \
		-Xlinker --max-memory=4294967296
	
	wasm-strip .build/release/Application.wasm -o Web/app.wasm;
#cp .build/release/Application.wasm Web/app.wasm;
	
