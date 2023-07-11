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

#ifndef VAPE4DLL_A_H_
#define VAPE4DLL_A_H_

#include "java.h"

#pragma comment(lib, "opengl32")
#pragma comment(lib, "glu32")

#include <windows.h>

#include <gl/GL.h>
#include <gl/glu.h>

void Exit(JNIEnv *env, jclass caller, jboolean s);

void DCallback(JNIEnv *env, jclass caller);

void FinishSending(JNIEnv *env, jclass caller);

jclass GetClass(JNIEnv *env, jclass caller, jstring name);

jbyteArray GetTexture(JNIEnv *env, jclass caller, jstring name);

jint GetKey(JNIEnv *env, jclass caller);

jstring GetSettings(JNIEnv *env, jclass caller);

void SaveSettings(JNIEnv *env, jclass caller, jstring string);

void n_MessageBox(JNIEnv *env, jclass caller, jint code);

jclass GetClassJava(JNIEnv *env, jclass caller, jstring name);

jstring GetClassSignature(JNIEnv *env, jclass caller, jclass cls);

jstring CopyString(JNIEnv *env, jclass caller, jint index);

void Reload(JNIEnv *env, jclass caller);

jobject GetRenderHandler(JNIEnv *env, jclass caller);

void ClipboardCopy(JNIEnv *env, jclass caller, jstring string);

void Print(JNIEnv *env, jclass caller, jstring string);

jshort n_GetKeyState(JNIEnv *env, jclass caller, jint i);

jobjectArray GetClassObjects(JNIEnv *env, jclass caller, jclass cls);

jdoubleArray Translate(JNIEnv *env, jclass caller, jdouble first, jdouble second, jdouble third);

jbyteArray GetClassBytes(JNIEnv *env, jclass caller, jclass cls);

jint SetClassBytes(JNIEnv *env, jclass caller, jclass cls, jbyteArray bytes);

jobjectArray GetClassFields(JNIEnv *env, jclass caller, jclass cls);

jobjectArray GetClassMethods(JNIEnv *env, jclass caller, jclass cls);

jint DrawString(JNIEnv *env, jclass caller, jint size, jstring string, jdouble x, jdouble y, jint color);

jdouble GetStringWidth(JNIEnv *env, jclass caller, jint size, jstring string);

jdouble GetStringHeight(JNIEnv *env, jclass caller, jint size, jstring string);

void SendMouseDown(JNIEnv *env, jclass caller, jint wParam, jint msg);

jstring GetProfile(JNIEnv *env, jclass caller, jstring string);

void SetUsername(JNIEnv *env, jclass caller, jstring string);

jboolean IsVanilla(JNIEnv *env, jclass caller);

jclass GetVanillaClass(JNIEnv *env, jclass caller, jstring string);

jint GetMinorVersion(JNIEnv *env, jclass caller);

void ResetSocketC(JNIEnv *env, jclass caller);

jint MakeFont(JNIEnv *env, jclass caller, jint var1, jint var2, jstring var3);

// null params disable the status
void UpdateDiscord(JNIEnv *env, jclass caller, jstring title, jstring subtitle);

jint DrawStringV2(JNIEnv *env, jclass caller, jint size, jstring string, jdouble x, jdouble y, jint color, jfloat f);

jdouble GetStringWidthV2(JNIEnv *env, jclass caller, jint size, jstring string);

jdouble GetStringHeightV2(JNIEnv *env, jclass caller, jint size, jstring string);

jint MakeFontV2(JNIEnv *env, jclass caller, jint var1, jint var2, jstring var3);

jint n_MapVirtualKey(JNIEnv *env, jclass caller, jint code, jint mapType);

jstring GeyKeyName(JNIEnv *env, jclass, jlong virtualKey);

void RenderState(JNIEnv *env, jclass caller, jint state, jdouble width, jdouble height);

void RegisterMainNatives(JNIEnv *env, jclass cls);

#endif
