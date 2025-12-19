#pragma once

#if defined(__wasm__)

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct ExternRefIndex {
    int index;
} ExternRefIndex;

void freeExternRef(ExternRefIndex ref);

ExternRefIndex bridgeString(const uint8_t *str, size_t bytes);
char* stringMemory(ExternRefIndex self);

ExternRefIndex emptyObject(void);
ExternRefIndex getProperty(ExternRefIndex self, ExternRefIndex name);
void setProperty(ExternRefIndex self, ExternRefIndex name, ExternRefIndex value);
long getIntProperty(ExternRefIndex self, ExternRefIndex name);
void setIntProperty(ExternRefIndex self, ExternRefIndex name, long value);
float getFloatProperty(ExternRefIndex self, ExternRefIndex name);
void setFloatProperty(ExternRefIndex self, ExternRefIndex name, float value);

ExternRefIndex emptyArray(void);
void arrayPush(ExternRefIndex self, ExternRefIndex element);
void arrayPushInt(ExternRefIndex self, long element);
void arrayPushFloat(ExternRefIndex self, float element);

ExternRefIndex arrayGet(ExternRefIndex self, long index);
void arraySet(ExternRefIndex self, long index, ExternRefIndex element);

long arrayGetInt(ExternRefIndex self, long index);
void arraySetInt(ExternRefIndex self, long index, long element);

float arrayGetFloat(ExternRefIndex self, long index);
void arraySetFloat(ExternRefIndex self, long index, float element);

ExternRefIndex callback(void (callback)(void *), void *data);
ExternRefIndex stringify(ExternRefIndex self);

ExternRefIndex getDocument(void);
ExternRefIndex getElementById(ExternRefIndex id);
ExternRefIndex createElement(ExternRefIndex name);
ExternRefIndex getContext(ExternRefIndex self, ExternRefIndex name);
void appendChild(ExternRefIndex self, ExternRefIndex child);
void addEventListener(ExternRefIndex self, ExternRefIndex name, ExternRefIndex callback);
void fillRect(ExternRefIndex self, long x, long y, long width, long height);

void beginPath(ExternRefIndex self);
void closePath(ExternRefIndex self);
void moveTo(ExternRefIndex self, long x, long y);
void lineTo(ExternRefIndex self, long x, long y);
void stroke(ExternRefIndex self);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
