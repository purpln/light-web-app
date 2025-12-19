#pragma once

#if defined(__wasm__)

#include <stdint.h>
#include <float.h>

#define WASM_IMPORT(m,n) __attribute__((import_module(m))) __attribute__((import_name(n)));
#define WASM_EXPORT(n) __attribute__((visibility("default")))

__externref_t emptyObjectJS(void) WASM_IMPORT("js", "emptyObject");

__externref_t emptyArrayJS(void) WASM_IMPORT("js", "emptyArray");

void arrayPushJS(__externref_t self, __externref_t element) WASM_IMPORT("js", "arrayPush");
void arrayPushIntJS(__externref_t self, long element) WASM_IMPORT("js", "arrayPush");
void arrayPushFloatJS(__externref_t self, float element) WASM_IMPORT("js", "arrayPush");

__externref_t arrayGetJS(__externref_t self, long index) WASM_IMPORT("js", "getProperty");
void arraySetJS(__externref_t self, long index, __externref_t element) WASM_IMPORT("js", "setProperty");

long arrayGetIntJS(__externref_t self, long index) WASM_IMPORT("js", "getProperty");
void arraySetIntJS(__externref_t self, long index, long element) WASM_IMPORT("js", "setProperty");

float arrayGetFloatJS(__externref_t self, long index) WASM_IMPORT("js", "getProperty");
void arraySetFloatJS(__externref_t self, long index, float element) WASM_IMPORT("js", "setProperty");

__externref_t bridgeStringJS(const uint8_t *str, uint32_t bytes) WASM_IMPORT("js", "bridgeString");
void* stringMemoryJS(__externref_t self) WASM_IMPORT("js", "stringMemory");

__externref_t getPropertyJS(__externref_t self, __externref_t name) WASM_IMPORT("js", "getProperty");
void setPropertyJS(__externref_t self, __externref_t name, __externref_t val) WASM_IMPORT("js", "setProperty");

long getIntPropertyJS(__externref_t self, __externref_t name) WASM_IMPORT("js", "getProperty");
void setIntPropertyJS(__externref_t self, __externref_t name, long val) WASM_IMPORT("js", "setProperty");

float getFloatPropertyJS(__externref_t self, __externref_t name) WASM_IMPORT("js", "getProperty");
void setFloatPropertyJS(__externref_t self, __externref_t name, long val) WASM_IMPORT("js", "setProperty");

__externref_t callbackJS(void (callback)(void *), void *data) WASM_IMPORT("js", "callback");

__externref_t stringifyJS(__externref_t self) WASM_IMPORT("js", "stringify");

__externref_t getDocumentJS(void) WASM_IMPORT("document", "getDocument");

__externref_t getElementByIdJS(__externref_t id) WASM_IMPORT("document", "getElementById");
__externref_t createElementJS(__externref_t name) WASM_IMPORT("document", "createElement");
__externref_t getContextJS(__externref_t self, __externref_t name) WASM_IMPORT("document", "getContext");
void appendChildJS(__externref_t self, __externref_t child) WASM_IMPORT("document", "appendChild");
void addEventListenerJS(__externref_t self, __externref_t name, __externref_t callback) WASM_IMPORT("document", "addEventListener");
void fillRectJS(__externref_t self, long x, long y, long width, long height) WASM_IMPORT("document", "fillRect");

void beginPathJS(__externref_t self) WASM_IMPORT("document", "beginPath");
void closePathJS(__externref_t self) WASM_IMPORT("document", "closePath");
void moveToJS(__externref_t self, long x, long y) WASM_IMPORT("document", "moveTo");
void lineToJS(__externref_t self, long x, long y) WASM_IMPORT("document", "lineTo");
void strokeJS(__externref_t self) WASM_IMPORT("document", "stroke");
#endif
