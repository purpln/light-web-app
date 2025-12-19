#if defined(__wasm__)

#include "js.h"
#include "refs.h"

static __externref_t table[0];

typedef void (*__funcref funcref_t)(void*);
static funcref_t ftable[0];

static int nextAvailableTableIndex = 0;
static const int delta = 256;

void freeExternRef(ExternRefIndex ref) {
    __builtin_wasm_table_set(table, ref.index, __builtin_wasm_ref_null_extern());
}

ExternRefIndex tableAppend(__externref_t ref) {
    ExternRefIndex idx = { .index = nextAvailableTableIndex++ };
    
    if (idx.index >= __builtin_wasm_table_size(table)) {
        __builtin_wasm_table_grow(table, __builtin_wasm_ref_null_extern(), delta);
    }
    
    __builtin_wasm_table_set(table, idx.index, ref);
    
    return idx;
}

ExternRefIndex bridgeString(const uint8_t *str, size_t bytes) {
    return tableAppend(bridgeStringJS(str, bytes));
}

char* stringMemory(ExternRefIndex self) {
    return (char *)stringMemoryJS(__builtin_wasm_table_get(table, self.index));
}

ExternRefIndex emptyObject(void) {
    return tableAppend(emptyObjectJS());
}

ExternRefIndex getProperty(ExternRefIndex self, ExternRefIndex name) {
    return tableAppend(getPropertyJS(__builtin_wasm_table_get(table, self.index), __builtin_wasm_table_get(table, name.index)));
}

void setProperty(ExternRefIndex self, ExternRefIndex name, ExternRefIndex value) {
    setPropertyJS(__builtin_wasm_table_get(table, self.index), __builtin_wasm_table_get(table, name.index), __builtin_wasm_table_get(table, value.index));
}

long getIntProperty(ExternRefIndex self, ExternRefIndex name) {
    return getIntPropertyJS(__builtin_wasm_table_get(table, self.index), __builtin_wasm_table_get(table, name.index));
}

void setIntProperty(ExternRefIndex self, ExternRefIndex name, long value) {
    setIntPropertyJS(__builtin_wasm_table_get(table, self.index), __builtin_wasm_table_get(table, name.index), value);
}

float getFloatProperty(ExternRefIndex self, ExternRefIndex name) {
    return getFloatPropertyJS(__builtin_wasm_table_get(table, self.index), __builtin_wasm_table_get(table, name.index));
}

void setFloatProperty(ExternRefIndex self, ExternRefIndex name, float value) {
    setFloatPropertyJS(__builtin_wasm_table_get(table, self.index), __builtin_wasm_table_get(table, name.index), value);
}

ExternRefIndex emptyArray(void) {
    return tableAppend(emptyArrayJS());
}

void arrayPush(ExternRefIndex self, ExternRefIndex element) {
    arrayPushJS(__builtin_wasm_table_get(table, self.index), __builtin_wasm_table_get(table, element.index));
}

void arrayPushInt(ExternRefIndex self, long element) {
    arrayPushIntJS(__builtin_wasm_table_get(table, self.index), element);
}

void arrayPushFloat(ExternRefIndex self, float element) {
    arrayPushFloatJS(__builtin_wasm_table_get(table, self.index), element);
}

ExternRefIndex arrayGet(ExternRefIndex self, long index) {
    return tableAppend(arrayGetJS(__builtin_wasm_table_get(table, self.index), index));
}

void arraySet(ExternRefIndex self, long index, ExternRefIndex element) {
    arraySetJS(__builtin_wasm_table_get(table, self.index), index, __builtin_wasm_table_get(table, element.index));
}

long arrayGetInt(ExternRefIndex self, long index) {
    return arrayGetIntJS(__builtin_wasm_table_get(table, self.index), index);
}

void arraySetInt(ExternRefIndex self, long index, long element) {
    arraySetIntJS(__builtin_wasm_table_get(table, self.index), index, element);
}

float arrayGetFloat(ExternRefIndex self, long index) {
    return arrayGetFloatJS(__builtin_wasm_table_get(table, self.index), index);
}

void arraySetFloat(ExternRefIndex self, long index, float element) {
    arraySetFloatJS(__builtin_wasm_table_get(table, self.index), index, element);
}

ExternRefIndex callback(void (callback)(void *), void *data) {
    return tableAppend(callbackJS(callback, data));
}

ExternRefIndex stringify(ExternRefIndex self) {
    return tableAppend(stringifyJS(__builtin_wasm_table_get(table, self.index)));
}

ExternRefIndex floatString(float self) {
    return tableAppend(floatStringJS(self));
}

ExternRefIndex getDocument() {
    return tableAppend(getDocumentJS());
}

ExternRefIndex getElementById(ExternRefIndex id) {
    return tableAppend(getElementByIdJS(__builtin_wasm_table_get(table, id.index)));
}

ExternRefIndex createElement(ExternRefIndex name) {
    return tableAppend(createElementJS(__builtin_wasm_table_get(table, name.index)));
}

ExternRefIndex getContext(ExternRefIndex self, ExternRefIndex name) {
    return tableAppend(getContextJS(__builtin_wasm_table_get(table, self.index), __builtin_wasm_table_get(table, name.index)));
}

void appendChild(ExternRefIndex self, ExternRefIndex child) {
    appendChildJS(__builtin_wasm_table_get(table, self.index), __builtin_wasm_table_get(table, child.index));
}

void addEventListener(ExternRefIndex self, ExternRefIndex name, ExternRefIndex callback) {
    addEventListenerJS(__builtin_wasm_table_get(table, self.index), __builtin_wasm_table_get(table, name.index), __builtin_wasm_table_get(table, callback.index));
}

void fillRect(ExternRefIndex self, long x, long y, long width, long height) {
    fillRectJS(__builtin_wasm_table_get(table, self.index), x, y, width, height);
}

void beginPath(ExternRefIndex self) {
    beginPathJS(__builtin_wasm_table_get(table, self.index));
}

void closePath(ExternRefIndex self) {
    closePathJS(__builtin_wasm_table_get(table, self.index));
}

void moveTo(ExternRefIndex self, long x, long y) {
    moveToJS(__builtin_wasm_table_get(table, self.index), x, y);
}

void lineTo(ExternRefIndex self, long x, long y) {
    lineToJS(__builtin_wasm_table_get(table, self.index), x, y);
}

void stroke(ExternRefIndex self) {
    strokeJS(__builtin_wasm_table_get(table, self.index));
}

#endif
