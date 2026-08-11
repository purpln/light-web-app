#pragma once

#if defined(__wasm__)

#include <stdint.h>
#include <float.h>

#define wasm_import(m,n) __attribute__((import_module(m))) __attribute__((import_name(n)))
#define wasm_export(n) __attribute__((export_name(n)))

wasm_import("js", "emptyObject") __externref_t emptyObjectJS(void);

wasm_import("js", "emptyArray") __externref_t emptyArrayJS(void);

wasm_import("js", "arrayPush") void arrayPushJS(__externref_t self, __externref_t element);
wasm_import("js", "arrayPush") void arrayPushIntJS(__externref_t self, long element);
wasm_import("js", "arrayPush") void arrayPushFloatJS(__externref_t self, float element);

__externref_t arrayGetJS(__externref_t self, long index) wasm_import("js", "getProperty");
void arraySetJS(__externref_t self, long index, __externref_t element) wasm_import("js", "setProperty");

long arrayGetIntJS(__externref_t self, long index) wasm_import("js", "getProperty");
void arraySetIntJS(__externref_t self, long index, long element) wasm_import("js", "setProperty");

float arrayGetFloatJS(__externref_t self, long index) wasm_import("js", "getProperty");
void arraySetFloatJS(__externref_t self, long index, float element) wasm_import("js", "setProperty");

__externref_t bridgeStringJS(const uint8_t *str, uint32_t bytes) wasm_import("js", "bridgeString");
void* stringMemoryJS(__externref_t self) wasm_import("js", "stringMemory");

__externref_t getPropertyJS(__externref_t self, __externref_t name) wasm_import("js", "getProperty");
void setPropertyJS(__externref_t self, __externref_t name, __externref_t val) wasm_import("js", "setProperty");

long getIntPropertyJS(__externref_t self, __externref_t name) wasm_import("js", "getProperty");
void setIntPropertyJS(__externref_t self, __externref_t name, long val) wasm_import("js", "setProperty");

float getFloatPropertyJS(__externref_t self, __externref_t name) wasm_import("js", "getProperty");
void setFloatPropertyJS(__externref_t self, __externref_t name, long val) wasm_import("js", "setProperty");

__externref_t stringifyJS(__externref_t self) wasm_import("js", "stringify");
__externref_t callbackJS(void (*function)(void *), void *data) wasm_import("js", "callback");
void retainJS(__externref_t value) wasm_import("js", "retain");
void releaseJS(__externref_t value) wasm_import("js", "release");

__externref_t getDocumentJS(void) wasm_import("document", "getDocument");
__externref_t getElementByIdJS(__externref_t id) wasm_import("document", "getElementById");
__externref_t createElementJS(__externref_t name) wasm_import("document", "createElement");
__externref_t getContextJS(__externref_t self, __externref_t name) wasm_import("document", "getContext");
void appendChildJS(__externref_t self, __externref_t child) wasm_import("document", "appendChild");
void addEventListenerJS(__externref_t self, __externref_t name, __externref_t callback) wasm_import("document", "addEventListener");
void fillRectJS(__externref_t self, long x, long y, long width, long height) wasm_import("document", "fillRect");
void beginPathJS(__externref_t self) wasm_import("document", "beginPath");
void closePathJS(__externref_t self) wasm_import("document", "closePath");
void moveToJS(__externref_t self, long x, long y) wasm_import("document", "moveTo");
void lineToJS(__externref_t self, long x, long y) wasm_import("document", "lineTo");
void strokeJS(__externref_t self) wasm_import("document", "stroke");
#endif
