/**
 * The MIT License (MIT)
 *
 * Copyright (C) 2022 Decencies
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _DLLCORE_JAVA_H
#define _DLLCORE_JAVA_H

#include <string>

#include <classfile_constants.h>
#include <jni.h>
#include <jvmti.h>

#define DEFINE_NATIVE_METHOD(name, signature, fn)                                                                      \
    {                                                                                                                  \
        const_cast<char *>(name), const_cast<char *>(signature), (void *)fn                                            \
    }

#define GET_CONTSTRUCTOR(cls) env->GetMethodID(cls, "<init>", "()V")

typedef struct JVM_CTX
{
    JavaVM *vm;
    JNIEnv *jni;
    jvmtiEnv *jvmti;
} * PJVM_CTX;

typedef struct CLIENT_CTX
{
    PJVM_CTX jvm;
} * PCLIENT_CTX;

PJVM_CTX GetJVMContext();

// Different implementations in Lite & V4 (more pain)

// void
// LookupClass(char* name);

// void
// LookupMinecraftClass();

jmethodID GetMethodByName(PJVM_CTX ctx, jclass cls, const char *name, const char *signature);

jfieldID GetFieldByName(PJVM_CTX ctx, jclass cls, const char *name, const char *signature);

jvmtiError EnableClassFileLoadHook();

jvmtiError DisableClassFileLoadHook();

#endif