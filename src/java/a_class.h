#pragma once


#include <jni.h>
#include <Windows.h>
#include <iostream>

#include "../dllmain.h"
#include "util/j_util.h"
#include "java_class.h"

void Exit(JNIEnv* env, jclass caller, jboolean s);

void DiscordCallback(JNIEnv* env, jclass caller);

void FS(JNIEnv* env, jclass caller);

jclass GetClass(JNIEnv* env, jclass caller, jstring name);

jbyteArray GetTexture(JNIEnv* env, jclass caller, jstring name);

jint GetKey(JNIEnv* env, jclass caller);

jstring GetSettings(JNIEnv* env, jclass caller);

void SaveSettings(JNIEnv* env, jclass caller, jstring string);

void n_MessageBox(JNIEnv* env, jclass caller, jint code);

jclass GetClassJava(JNIEnv* env, jclass caller, jstring name);

jstring GetClassSignature(JNIEnv* env, jclass caller, jclass cls);

jstring CopyString(JNIEnv* env, jclass caller, jint index);

void Reload(JNIEnv* env, jclass caller);

jobject GetRenderHandler(JNIEnv* env, jclass caller);

void ClipboardCopy(JNIEnv* env, jclass caller, jstring string);

void Print(JNIEnv* env, jclass caller, jstring string);

jshort n_GetKeyState(JNIEnv* env, jclass caller, jint i);

jobjectArray GetClassObjects(JNIEnv* env, jclass caller, jclass cls);

jdoubleArray Translate(JNIEnv* env, jclass caller, jdouble first, jdouble second, jdouble third);

jbyteArray GetClassBytes(JNIEnv* env, jclass caller, jclass cls);

jint SetClassBytes(JNIEnv* env, jclass caller, jclass cls, jbyteArray bytes);

jobjectArray GetClassFields(JNIEnv* env, jclass caller, jclass cls);

jobjectArray GetClassMethods(JNIEnv* env, jclass caller, jclass cls);

jint DrawString(JNIEnv* env, jclass caller, jint size, jstring string, jdouble x, jdouble y, jint color);

jdouble GetStringWidth(JNIEnv* env, jclass caller, jint size, jstring string);

jdouble GetStringHeight(JNIEnv* env, jclass caller, jint size, jstring string);

void SendMouseDown(JNIEnv* env, jclass caller, jint var0, jint var1);

jstring GetProfile(JNIEnv* env, jclass caller, jstring string);

void SetUsername(JNIEnv* env, jclass caller, jstring string);

jboolean IsVanilla(JNIEnv* env, jclass caller);

jclass GetVanillaClass(JNIEnv* env, jclass caller, jstring string);

jint GetMinorVersion(JNIEnv* env, jclass caller);

void RSC(JNIEnv* env, jclass caller);

jint MakeFont(JNIEnv* env, jclass caller, jint var1, jint var2, jstring var3);

void UpdateDiscord(JNIEnv* env, jclass caller, jstring title, jstring subtitle);

jint DrawStringV2(JNIEnv* env, jclass caller, jint size, jstring string, jdouble x, jdouble y, jint color, jfloat f);

jdouble GetStringWidthV2(JNIEnv* env, jclass caller, jint size, jstring string);

jdouble GetStringHeightV2(JNIEnv* env, jclass caller, jint size, jstring string);

jint MakeFontV2(JNIEnv* env, jclass caller, jint var1, jint var2, jstring var3);

// v4.07 ADDITIONS

jint n_MapVirtualKey(JNIEnv* env, jclass caller, jint code, jint mapType);

jstring GeyKeyName(JNIEnv* env, jclass, jlong virtualKey);

void RenderState(JNIEnv* env, jclass caller, jint state, jdouble width, jdouble height);