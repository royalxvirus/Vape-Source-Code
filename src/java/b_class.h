#pragma once

#include <jni.h>
#include <jvmti.h>
#include <Windows.h>

void GetMethod(JNIEnv* env, jclass caller, jint id, jclass cls, jstring name, jstring desc, jboolean isStatic);

void GetMethodMapped(JNIEnv* env, jclass caller, jint id, jclass cls, jstring name, jstring desc, jboolean isStatic);

void InvokeMethodVoid(JNIEnv* env, jclass caller, jint id, jobject instance, jobjectArray params);

jboolean InvokeBooleanMethod(JNIEnv* env, jclass caller, jint id, jobject instance, jobjectArray params);

jchar InvokeCharMethod(JNIEnv* env, jclass caller, jint id, jobject instance, jobjectArray params);

jshort InvokeShortMethod(JNIEnv* env, jclass caller, jint id, jobject instance, jobjectArray params);

jint InvokeIntMethod(JNIEnv* env, jclass caller, jint id, jobject instance, jobjectArray params);

jlong InvokeLongMethod(JNIEnv* env, jclass caller, jint id, jobject instance, jobjectArray params);

jfloat InvokeFloatMethod(JNIEnv* env, jclass caller, jint id, jobject instance, jobjectArray params);

jdouble InvokeDoubleMethod(JNIEnv* env, jclass caller, jint id, jobject instance, jobjectArray params);

jobject InvokeObjectMethod(JNIEnv* env, jclass caller, jint id, jobject instance, jobjectArray params);

jbooleanArray InvokeBooleanArrayMethod(JNIEnv* env, jclass caller, jint id, jobject instance, jobjectArray params);

jcharArray InvokeCharArrayMethod(JNIEnv* env, jclass caller, jint id, jobject instance, jobjectArray params);

jshortArray InvokeShortArrayMethod(JNIEnv* env, jclass caller, jint id, jobject instance, jobjectArray params);

jintArray InvokeIntArrayMethod(JNIEnv* env, jclass caller, jint id, jobject instance, jobjectArray params);

jlongArray InvokeLongArrayMethod(JNIEnv* env, jclass caller, jint id, jobject instance, jobjectArray params);

jfloatArray InvokeFloatArrayMethod(JNIEnv* env, jclass caller, jint id, jobject instance, jobjectArray params);

jdoubleArray InvokeDoubleArrayMethod(JNIEnv* env, jclass caller, jint id, jobject instance, jobjectArray params);

jobjectArray InvokeObjectArrayMethod(JNIEnv* env, jclass caller, jint id, jobject instance, jobjectArray params);

void GetField(JNIEnv* env, jclass caller, jint id, jclass cls, jstring name, jstring desc, jboolean isStatic);

void GetFieldMapped(JNIEnv* env, jclass caller, jint id, jclass cls, jstring name, jstring desc, jboolean isStatic);

jboolean GetBooleanField(JNIEnv* env, jclass caller, jint id, jobject instance);

jchar GetCharField(JNIEnv* env, jclass caller, jint id, jobject instance);

jshort GetShortField(JNIEnv* env, jclass caller, jint id, jobject instance);

jint GetIntField(JNIEnv* env, jclass caller, jint id, jobject instance);

jlong GetLongField(JNIEnv* env, jclass caller, jint id, jobject instance);

jfloat GetFloatField(JNIEnv* env, jclass caller, jint id, jobject instance);

jdouble GetDoubleField(JNIEnv* env, jclass caller, jint id, jobject instance);

jobject GetObjectField(JNIEnv* env, jclass caller, jint id, jobject instance);

jbooleanArray GetBooleanArrayField(JNIEnv* env, jclass caller, jint id, jobject instance);

jcharArray GetCharArrayField(JNIEnv* env, jclass caller, jint id, jobject instance);

jshortArray GetShortArrayField(JNIEnv* env, jclass caller, jint id, jobject instance);

jintArray GetIntArrayField(JNIEnv* env, jclass caller, jint id, jobject instance);

jlongArray GetLongArrayField(JNIEnv* env, jclass caller, jint id, jobject instance);

jfloatArray GetFloatArrayField(JNIEnv* env, jclass caller, jint id, jobject instance);

jdoubleArray GetDoubleArrayField(JNIEnv* env, jclass caller, jint id, jobject instance);

jobjectArray GetObjectArrayField(JNIEnv* env, jclass caller, jint id, jobject instance);

void SetBooleanField(JNIEnv* env, jclass caller, jint id, jobject instance, jboolean value);

void SetCharField(JNIEnv* env, jclass caller, jint id, jobject instance, jchar value);

void SetShortField(JNIEnv* env, jclass caller, jint id, jobject instance, jshort value);

void SetIntField(JNIEnv* env, jclass caller, jint id, jobject instance, jint value);

void SetLongField(JNIEnv* env, jclass caller, jint id, jobject instance, jlong value);

void SetFloatField(JNIEnv* env, jclass caller, jint id, jobject instance, jfloat value);

void SetDoubleField(JNIEnv* env, jclass caller, jint id, jobject instance, jdouble value);

void SetObjectField(JNIEnv* env, jclass caller, jint id, jobject instance, jobject value);

void SetBooleanArrayField(JNIEnv* env, jclass caller, jint id, jobject instance, jbooleanArray value);

void SetCharArrayField(JNIEnv* env, jclass caller, jint id, jobject instance, jcharArray value);

void SetShortArrayField(JNIEnv* env, jclass caller, jint id, jobject instance, jshortArray value);

void SetIntArrayField(JNIEnv* env, jclass caller, jint id, jobject instance, jintArray value);

void SetLongArrayField(JNIEnv* env, jclass caller, jint id, jobject instance, jlongArray value);

void SetFloatArrayField(JNIEnv* env, jclass caller, jint id, jobject instance, jfloatArray value);

void SetDoubleArrayField(JNIEnv* env, jclass caller, jint id, jobject instance, jdoubleArray value);

void SetObjectArrayField(JNIEnv* env, jclass caller, jint id, jobject instance, jobjectArray value);

void InvokeNonVirtualVoidMethod(JNIEnv* env, jclass caller, jint id, jobject instance, jobjectArray value);

jobject InvokeConstructor(JNIEnv* env, jclass caller, jint id, jclass cls, jobjectArray params);

jbyte InvokeByteMethod(JNIEnv* env, jclass caller, jint id, jobject instance, jobjectArray params);

jbyteArray InvokeByteArrayMethod(JNIEnv* env, jclass caller, jint id, jobject instance, jobjectArray params);

jbyte GetByteField(JNIEnv* env, jclass caller, jint id, jobject instance);

void SetByteField(JNIEnv* env, jclass caller, jint id, jobject instance, jbyte value);

jbyteArray GetByteArrayField(JNIEnv* env, jclass caller, jint id, jobject instance);

void SetByteArrayField(JNIEnv* env, jclass caller, jint id, jobject instance, jbyteArray value);

jstring GetFieldName(JNIEnv* env, jclass caller, jint id);

jstring GetMethodName(JNIEnv* env, jclass caller, jint id);