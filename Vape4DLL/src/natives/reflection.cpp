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
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "reflection.h"

#include <algorithm>
#include <iostream>
#include <map>

#include "rpc/rpcsocket.h"

jclass clsBoolean;
jclass clsCharacter;
jclass clsByte;
jclass clsShort;
jclass clsInteger;
jclass clsLong;
jclass clsFloat;
jclass clsDouble;

jfieldID fidBooleanValue;
jfieldID fidCharacterValue;
jfieldID fidByteValue;
jfieldID fidShortValue;
jfieldID fidIntegerValue;
jfieldID fidLongValue;
jfieldID fidFloatValue;
jfieldID fidDoubleValue;

std::map<int, field_t> fields;
std::map<int, method_t> methods;
std::map<int, jclass> loadedClasses;
std::map<int, constructor_t> constructors;

// used in GetMethodInternal
std::map<std::string, std::string> unk0;

std::map<char const *, int> unk2;

void GetPrimitiveValue(JNIEnv *env, jobject obj_instance, jvalue *ptr_value)
{
    if (obj_instance)
    {
        if (env->IsInstanceOf(obj_instance, clsBoolean))
            ptr_value->z = env->GetBooleanField(obj_instance, fidBooleanValue);
        else if (env->IsInstanceOf(obj_instance, clsCharacter))
            ptr_value->c = env->GetCharField(obj_instance, fidCharacterValue);
        else if (env->IsInstanceOf(obj_instance, clsByte))
            ptr_value->b = env->GetByteField(obj_instance, fidByteValue);
        else if (env->IsInstanceOf(obj_instance, clsShort))
            ptr_value->s = env->GetShortField(obj_instance, fidShortValue);
        else if (env->IsInstanceOf(obj_instance, clsInteger))
            ptr_value->i = env->GetIntField(obj_instance, fidIntegerValue);
        else if (env->IsInstanceOf(obj_instance, clsLong))
            ptr_value->j = env->GetLongField(obj_instance, fidLongValue);
        else if (env->IsInstanceOf(obj_instance, clsFloat))
            ptr_value->f = env->GetFloatField(obj_instance, fidFloatValue);
        else if (env->IsInstanceOf(obj_instance, clsDouble))
            ptr_value->d = env->GetDoubleField(obj_instance, fidDoubleValue);
        else
            ptr_value->l = obj_instance;
    }
    else
        ptr_value->l = NULL;
}

#define GET_VARARGS(jni, params)                                                                                       \
    jsize length = env->GetArrayLength(params);                                                                        \
    jvalue *args = new jvalue[length];                                                                                 \
    if (length > 0)                                                                                                    \
    {                                                                                                                  \
        jvalue value;                                                                                                  \
        jobject arg;                                                                                                   \
        for (jsize i = 0; i < length; i++)                                                                             \
        {                                                                                                              \
            arg = env->GetObjectArrayElement(params, i);                                                               \
            GetPrimitiveValue(jni, arg, &value);                                                                       \
            *(args + i) = value;                                                                                       \
        }                                                                                                              \
    }

#define SET_TYPE_FIELD(type, capitalized)                                                                              \
    void Set##capitalized##Field(JNIEnv *env, jclass caller, jint id, jobject instance, type value)                    \
    {                                                                                                                  \
        if (fields.count(id))                                                                                          \
        {                                                                                                              \
            field_t fd = fields[id];                                                                                   \
            if (fd.isStatic)                                                                                           \
                env->SetStatic##capitalized##Field(fd.cls, fd.fid, value);                                             \
            else                                                                                                       \
                env->Set##capitalized##Field(instance, fd.fid, value);                                                 \
        }                                                                                                              \
    }

#define GET_TYPE_FIELD(type, capitalized)                                                                              \
    type Get##capitalized##Field(JNIEnv *env, jclass caller, jint id, jobject instance)                                \
    {                                                                                                                  \
        if (fields.count(id))                                                                                          \
        {                                                                                                              \
            field_t fd = fields[id];                                                                                   \
            if (fd.isStatic)                                                                                           \
                return env->GetStatic##capitalized##Field(fd.cls, fd.fid);                                             \
            else                                                                                                       \
                return env->Get##capitalized##Field(instance, fd.fid);                                                 \
        }                                                                                                              \
    }

#define SET_TYPE_ARRAY_FIELD(type, capitalized)                                                                        \
    void Set##capitalized##ArrayField(JNIEnv *env, jclass caller, jint id, jobject instance, type##Array value)        \
    {                                                                                                                  \
        if (fields.count(id))                                                                                          \
        {                                                                                                              \
            field_t fd = fields[id];                                                                                   \
            if (fd.isStatic)                                                                                           \
                env->SetStaticObjectField(fd.cls, fd.fid, value);                                                      \
            else                                                                                                       \
                env->SetObjectField(instance, fd.fid, value);                                                          \
        }                                                                                                              \
    }

#define GET_TYPE_ARRAY_FIELD(type, capitalized)                                                                        \
    type##Array Get##capitalized##ArrayField(JNIEnv *env, jclass caller, jint id, jobject instance)                    \
    {                                                                                                                  \
        if (fields.count(id))                                                                                          \
        {                                                                                                              \
            field_t fd = fields[id];                                                                                   \
            if (fd.isStatic)                                                                                           \
                return (type##Array)env->GetStaticObjectField(fd.cls, fd.fid);                                         \
            else                                                                                                       \
                return (type##Array)env->GetObjectField(instance, fd.fid);                                             \
        }                                                                                                              \
    }

#define INVOKE_TYPE_METHOD(type, capitalized)                                                                          \
    type Invoke##capitalized##Method(JNIEnv *env, jclass caller, jint id, jobject instance, jobjectArray params)       \
    {                                                                                                                  \
        if (methods.count(id))                                                                                         \
        {                                                                                                              \
            GET_VARARGS(env, params)                                                                                   \
            method_t method = methods[id];                                                                             \
            type ret = NULL;                                                                                           \
            if (method.isStatic)                                                                                       \
                ret = env->CallStatic##capitalized##MethodA(method.cls, method.mid, args);                             \
            else                                                                                                       \
                type ret = env->Call##capitalized##MethodA(instance, method.mid, args);                                \
            delete[] args;                                                                                             \
            return ret;                                                                                                \
        }                                                                                                              \
    }

#define INVOKE_TYPE_ARRAY_METHOD(type, capitalized)                                                                    \
    type##Array Invoke##capitalized##ArrayMethod(JNIEnv *env, jclass caller, jint id, jobject instance,                \
                                                 jobjectArray params)                                                  \
    {                                                                                                                  \
        if (methods.count(id))                                                                                         \
        {                                                                                                              \
            GET_VARARGS(env, params)                                                                                   \
            method_t method = methods[id];                                                                             \
            type##Array ret = NULL;                                                                                    \
            if (method.isStatic)                                                                                       \
                ret = (type##Array)env->CallStaticObjectMethodA(method.cls, method.mid, args);                         \
            else                                                                                                       \
                ret = (type##Array)env->CallObjectMethodA(instance, method.mid, args);                                 \
            delete[] args;                                                                                             \
            return ret;                                                                                                \
        }                                                                                                              \
    }

// generic
jobject CallObjectMethod(JNIEnv *env, jclass, jint id, jobject instance, jobjectArray params)
{
    if (methods.count(id))
    {
        GET_VARARGS(env, params)
        method_t method = methods[id];
        jobject ret = NULL;
        if (method.isStatic)
            ret = env->CallStaticObjectMethodA(method.cls, method.mid, args);
        else
            ret = env->CallObjectMethodA(instance, method.mid, args);
        delete[] args;
        return ret;
    }
}

#define DEFINE_TYPE_METHODS(type, capitalized)                                                                         \
    GET_TYPE_FIELD(type, capitalized)                                                                                  \
    SET_TYPE_FIELD(type, capitalized)                                                                                  \
    GET_TYPE_ARRAY_FIELD(type, capitalized)                                                                            \
    SET_TYPE_ARRAY_FIELD(type, capitalized)                                                                            \
    INVOKE_TYPE_METHOD(type, capitalized)                                                                              \
    INVOKE_TYPE_ARRAY_METHOD(type, capitalized)

DEFINE_TYPE_METHODS(jboolean, Boolean)
DEFINE_TYPE_METHODS(jchar, Char)
DEFINE_TYPE_METHODS(jshort, Short)
DEFINE_TYPE_METHODS(jint, Int)
DEFINE_TYPE_METHODS(jlong, Long)
DEFINE_TYPE_METHODS(jfloat, Float)
DEFINE_TYPE_METHODS(jdouble, Double)
DEFINE_TYPE_METHODS(jobject, Object)
DEFINE_TYPE_METHODS(jbyte, Byte)

char *GetFieldMapping(void *ctx, char *cls, char *name)
{
    // todo
    RPCSocket::RPCSocketContext *socket;

    bool isVanilla = &ctx + 0x88 == 0;
    int packetId = isVanilla ? RPCSocket::PacketId::GET_FIELD : RPCSocket::PacketId::GET_FIELD_FORGE;

    RPCSocket::SocketSendPacketId(socket, packetId);
    RPCSocket::SocketSendXORContents(socket, cls);
    RPCSocket::SocketSendXORContents(socket, name);
}

char *GetMethodMapping(void *ctx, char *cls, char *name)
{
    // todo
    RPCSocket::RPCSocketContext *socket;

    bool isVanilla = &ctx + 0x88 == 0;
    int packetId = isVanilla ? RPCSocket::PacketId::GET_METHOD : RPCSocket::PacketId::GET_METHOD_FORGE;

    RPCSocket::SocketSendPacketId(socket, packetId);
    RPCSocket::SocketSendXORContents(socket, cls);
    RPCSocket::SocketSendXORContents(socket, name);
}

jfieldID GetFieldSpecial(pfield_t ctx, JNIEnv *env, jclass cls, char *name, char *desc)
{
    JavaVM *vm;
    jvmtiEnv *jvmti_env;
    env->GetJavaVM(&vm);
    vm->GetEnv(reinterpret_cast<void **>(&jvmti_env), JVMTI_VERSION_1_1);

    jfieldID fid;
    jint fieldsCount;
    jfieldID *fields;

    if (!jvmti_env || !ctx->isSpecial)
    {
        fid = env->GetFieldID(cls, name, desc);
        if (fid == NULL)
            fid = env->GetStaticFieldID(cls, name, desc);
        return fid;
    }

    jvmti_env->GetClassFields(cls, &fieldsCount, &fields);

    char *classSignature;
    jvmti_env->GetClassSignature(cls, &classSignature, NULL);

    for (jint fieldIndex = 0; fieldIndex < fieldsCount; fieldIndex++)
    {
        fid = fields[fieldIndex];

        char *fieldName;
        char *fieldSignature;

        jvmti_env->GetFieldName(cls, fid, &fieldName, &fieldSignature, NULL);

        if (strstr(fieldName, name) && strstr(fieldSignature, desc))
        {
            // todo
            if (!ctx->isSpecial || strstr(classSignature, "lwjgl"))
            {
                break;
            }

            return fid;
        }
    }

    jvmti_env->Deallocate((unsigned char *)fields);

    // check flow.
    if (ctx->isSpecial)
    {
        printf("Failed to get field special %s -> %s %s\n", classSignature, name, desc);
        jvmti_env->Deallocate((unsigned char *)classSignature);
        return NULL;
    }
    else
    {
        fid = env->GetFieldID(cls, name, desc);
        if (fid == NULL)
            fid = env->GetStaticFieldID(cls, name, desc);
        return fid;
    }
}

// TODO
JNIEnv *env;

jmethodID GetMethodWithModifiers(void *ctx, pmethod_t method, jclass cls, bool isStatic)
{
    JavaVM *vm;
    jvmtiEnv *jvmti_env;
    env->GetJavaVM(&vm);
    vm->GetEnv(reinterpret_cast<void **>(&jvmti_env), JVMTI_VERSION_1_1);

    jint methodsCount;
    jmethodID *methods;

    jint matchingMethods = 0;
    jmethodID returnMethod = NULL;

    jvmti_env->GetClassMethods(cls, &methodsCount, &methods);

    for (jint index = 0; index < methodsCount; index++)
    {
        jmethodID mid = methods[index];

        char *methodName;
        char *methodDesc;

        jvmti_env->GetMethodName(mid, &methodName, &methodDesc, NULL);

        if (methodName && methodDesc)
        {
            jint modifiers;
            jvmti_env->GetMethodModifiers(mid, &modifiers);

            if (!isStatic || (modifiers & JVM_ACC_STATIC) != 0)
            {
                returnMethod = mid;
                matchingMethods++;
            }

            jvmti_env->Deallocate((unsigned char *)methodName);
            jvmti_env->Deallocate((unsigned char *)methodDesc);
        }
    }

    jvmti_env->Deallocate((unsigned char *)methods);

    if (matchingMethods == 1) // Only return if there was one matching result.
        return returnMethod;

    return NULL;
}

jmethodID GetMethodSpecial(void *ctx, pmethod_t method, JNIEnv *env, jclass cls, char *name, char *desc,
                           jboolean isStatic)
{
    JavaVM *vm;
    jvmtiEnv *jvmti_env;
    env->GetJavaVM(&vm);
    vm->GetEnv(reinterpret_cast<void **>(&jvmti_env), JVMTI_VERSION_1_1);

    jmethodID returnMethod = GetMethodWithModifiers(0, method, cls, isStatic);

    if (!returnMethod)
    {
        if (jvmti_env) // todo check from global JVM ctx instead.
        {
            jint methodsCount;
            jmethodID *methods;
            jvmti_env->GetClassMethods(cls, &methodsCount, &methods);

            for (jint index = 0; index < methodsCount; index++)
            {
                jmethodID mid = methods[index];

                char *methodName;
                char *methodDesc;

                jvmti_env->GetMethodName(mid, &methodName, &methodDesc, NULL);

                if (!strcmp(methodName, name) && !strcmp(methodDesc, desc))
                    returnMethod = mid;

                jvmti_env->Deallocate((unsigned char *)methodName);
                jvmti_env->Deallocate((unsigned char *)methodDesc);

                if (returnMethod)
                    break; // FIX: do not continue searching for the method.
            }

            jvmti_env->Deallocate((unsigned char *)methods);

            if (!returnMethod)
            {
                returnMethod = env->GetMethodID(cls, name, desc);
                if (returnMethod == NULL)
                    returnMethod = env->GetStaticMethodID(cls, name, desc);
            }
        }
        else
        {
            returnMethod = env->GetMethodID(cls, name, desc);
            if (returnMethod == NULL)
                returnMethod = env->GetStaticMethodID(cls, name, desc);
        }
    }

    return returnMethod;
}

// Ported as is from the DLL
// - a(ILjava/lang/Class;Ljava/lang/String;Ljava/lang/String;Z)V sets map to true,
// - b(ILjava/lang/Class;Ljava/lang/String;Ljava/lang/String;Z)V sets map to false
pmethod_t GetMethod0(pmethod_t method, JNIEnv *env, jint id, jclass cls, jstring name, jstring desc, jboolean map,
                     jboolean isStatic)
{
    method->isStatic = isStatic;
    method->cls = (jclass)env->NewGlobalRef(cls);

    char *charName = new char[1024];
    char *charDesc = new char[1024];

    env->GetStringUTFRegion(name, 0, env->GetStringUTFLength(name), charName);
    env->GetStringUTFRegion(desc, 0, env->GetStringUTFLength(desc), charName);

    if (!map) // The method does not need to be mapped, so we just get the method
              // with the supplied name & desc.
    {
        if (!isStatic)
            method->mid = env->GetMethodID(cls, charName, charDesc);
        else
            method->mid = env->GetStaticMethodID(cls, charName, charDesc);

        return method;
    }

    JavaVM *vm;
    jvmtiEnv *jvmti_env;
    env->GetJavaVM(&vm);
    vm->GetEnv(reinterpret_cast<void **>(&jvmti_env), JVMTI_VERSION_1_1);

    char *signature;
    jvmti_env->GetClassSignature(cls, &signature, NULL);

    char *dup = strdup(signature);

    jvmti_env->Deallocate((unsigned char *)signature);

    // todo

    char *mapping = GetMethodMapping(0, dup, charName);

    jmethodID mid = GetMethodWithModifiers(0, method, cls, isStatic);
    method->mid = mid;
    if (!mid)
    {
        // method->mid = GetMethodSpecial(0, method, env, cls, mapping, isStatic);
    }

    return method;
}

pfield_t GetField0(pfield_t field, JNIEnv *env, jint id, jclass cls, jstring name, jstring desc, jboolean map,
                   jboolean isStatic)
{
    field->isStatic = isStatic;

    field->cls = (jclass)env->NewGlobalRef(cls);

    char *charName = new char[1024];
    char *charDesc = new char[1024];

    env->GetStringUTFRegion(name, 0, env->GetStringUTFLength(name), charName);
    env->GetStringUTFRegion(desc, 0, env->GetStringUTFLength(desc), charName);

    if (!map) // The field does not need to be mapped, so we just get the field
              // with the supplied name & desc.
    {
        if (!isStatic)
            field->fid = env->GetFieldID(cls, charName, charDesc);
        else
            field->fid = env->GetStaticFieldID(cls, charName, charDesc);

        return field;
    }

    JavaVM *vm;
    jvmtiEnv *jvmti_env;
    env->GetJavaVM(&vm);
    vm->GetEnv(reinterpret_cast<void **>(&jvmti_env), JVMTI_VERSION_1_1);

    char *signature;
    jvmti_env->GetClassSignature(cls, &signature, NULL);

    char *dup = strdup(signature);

    jvmti_env->Deallocate((unsigned char *)signature);

    // todo

    char *mapping = GetFieldMapping(0, dup, charName);

    return field;
}

void GetMethodMapped(JNIEnv *env, jclass caller, jint id, jclass cls, jstring name, jstring desc, jboolean isStatic)
{
    method_t method{};

    GetMethod0(&method, env, id, cls, name, desc, true, isStatic);

    methods[id] = method;

#ifdef _DEBUG
    if (env->ExceptionOccurred())
        env->ExceptionDescribe();
#else
    if (env->ExceptionOccurred())
        env->ExceptionClear();
#endif

    if (!method.mid)
    {
        env->ThrowNew(env->FindClass("java/lang/Exception"), "nig");
    }
}

void GetMethod(JNIEnv *env, jclass caller, jint id, jclass cls, jstring name, jstring desc, jboolean isStatic)
{
    method_t method{};

    GetMethod0(&method, env, id, cls, name, desc, false, isStatic);

    methods[id] = method;

#ifdef _DEBUG
    if (env->ExceptionOccurred())
        env->ExceptionDescribe();
#else
    if (env->ExceptionOccurred())
        env->ExceptionClear();
#endif

    if (!method.mid)
    {
        env->ThrowNew(env->FindClass("java/lang/Exception"), "nig");
    }
}

void InvokeMethodVoid(JNIEnv *env, jclass caller, jint id, jobject instance, jobjectArray params)
{
    if (methods.count(id))
    {
        GET_VARARGS(env, params)
        method_t method = methods[id];
        if (method.isStatic)
            env->CallStaticVoidMethodA(method.cls, method.mid, args);
        else
            env->CallVoidMethodA(instance, method.mid, args);
        delete[] args;
    }
}

void GetFieldMapped(JNIEnv *env, jclass caller, jint id, jclass cls, jstring name, jstring desc, jboolean isStatic)
{
    field_t field{};

    GetField0(&field, env, id, cls, name, desc, false, isStatic);

    fields[id] = field;

#ifdef _DEBUG
    if (env->ExceptionOccurred())
        env->ExceptionDescribe();
#else
    if (env->ExceptionOccurred())
        env->ExceptionClear();
#endif

    if (!field.fid)
    {
        env->ThrowNew(env->FindClass("java/lang/Exception"), "nig");
    }
}

void GetField(JNIEnv *env, jclass caller, jint id, jclass cls, jstring name, jstring desc, jboolean isStatic)
{
    field_t field{};

    GetField0(&field, env, id, cls, name, desc, true, isStatic);

    fields[id] = field;

#ifdef _DEBUG
    if (env->ExceptionOccurred())
        env->ExceptionDescribe();
#else
    if (env->ExceptionOccurred())
        env->ExceptionClear();
#endif

    if (!field.fid)
    {
        env->ThrowNew(env->FindClass("java/lang/Exception"), "nig");
    }
}

void InvokeNonVirtualVoidMethod(JNIEnv *env, jclass caller, jint id, jobject instance, jobjectArray params)
{
    if (methods.count(id))
    {
        GET_VARARGS(env, params)
        method_t method = methods[id];
        jclass super_cls = env->GetSuperclass(env->GetObjectClass(instance));
        env->CallNonvirtualVoidMethodA(instance, super_cls, method.mid, args);
        delete[] args;
    }
}

jobject InvokeConstructor(JNIEnv *env, jclass caller, jint id, jclass cls, jobjectArray params)
{
    if (constructors.count(id))
    {
        GET_VARARGS(env, params)
        constructor_t constructor = constructors[id];
        jobject ret = env->NewObjectA(cls, constructor.mid, args);
        delete[] args;
        return ret;
    }
    return NULL;
}

jstring GetFieldName(JNIEnv *env, jclass caller, jint id)
{
    if (fields.count(id))
    {
        field_t fd = fields[id];

        JavaVM *vm;
        jvmtiEnv *jvmti_env;
        env->GetJavaVM(&vm);
        vm->GetEnv(reinterpret_cast<void **>(&jvmti_env), JVMTI_VERSION_1_1);

        char *fieldName;
        jvmti_env->GetFieldName(fd.cls, fd.fid, &fieldName, NULL, NULL);

        jstring strName = env->NewStringUTF(fieldName);

        jvmti_env->Deallocate((unsigned char *)fieldName);

        return strName;
    }
}

jstring GetMethodName(JNIEnv *env, jclass caller, jint id)
{
    if (methods.count(id))
    {
        method_t md = methods[id];

        JavaVM *vm;
        jvmtiEnv *jvmti_env;
        env->GetJavaVM(&vm);
        vm->GetEnv(reinterpret_cast<void **>(&jvmti_env), JVMTI_VERSION_1_1);

        char *methodName;
        jvmti_env->GetMethodName(md.mid, &methodName, NULL, NULL);

        jstring strName = env->NewStringUTF(methodName);

        jvmti_env->Deallocate((unsigned char *)methodName);

        return strName;
    }
}

void RegisterReflectionNatives(JNIEnv *env, jclass cls)
{
    clsBoolean = env->FindClass("java/lang/Boolean");
    clsCharacter = env->FindClass("java/lang/Character");
    clsByte = env->FindClass("java/lang/Byte");
    clsShort = env->FindClass("java/lang/Short");
    clsInteger = env->FindClass("java/lang/Integer");
    clsLong = env->FindClass("java/lang/Long");
    clsFloat = env->FindClass("java/lang/Float");
    clsDouble = env->FindClass("java/lang/Double");
    fidBooleanValue = env->GetFieldID(clsBoolean, "value", "Z");
    fidCharacterValue = env->GetFieldID(clsCharacter, "value", "C");
    fidByteValue = env->GetFieldID(clsByte, "value", "B");
    fidShortValue = env->GetFieldID(clsShort, "value", "S");
    fidIntegerValue = env->GetFieldID(clsInteger, "value", "I");
    fidLongValue = env->GetFieldID(clsLong, "value", "J");
    fidFloatValue = env->GetFieldID(clsFloat, "value", "F");
    fidDoubleValue = env->GetFieldID(clsDouble, "value", "D");

    JNINativeMethod natives[] = {
        DEFINE_NATIVE_METHOD("a", "(ILjava/lang/Class;Ljava/lang/String;Ljava/lang/String;Z)V", GetMethod),
        DEFINE_NATIVE_METHOD("b", "(ILjava/lang/Class;Ljava/lang/String;Ljava/lang/String;Z)V", GetMethodMapped),
        DEFINE_NATIVE_METHOD("c", "(ILjava/lang/Object;[Ljava/lang/Object;)V", InvokeMethodVoid),
        DEFINE_NATIVE_METHOD("d", "(ILjava/lang/Object;[Ljava/lang/Object;)Z", InvokeBooleanMethod),
        DEFINE_NATIVE_METHOD("e", "(ILjava/lang/Object;[Ljava/lang/Object;)C", InvokeCharMethod),
        DEFINE_NATIVE_METHOD("f", "(ILjava/lang/Object;[Ljava/lang/Object;)S", InvokeShortMethod),
        DEFINE_NATIVE_METHOD("g", "(ILjava/lang/Object;[Ljava/lang/Object;)I", InvokeIntMethod),
        DEFINE_NATIVE_METHOD("h", "(ILjava/lang/Object;[Ljava/lang/Object;)J", InvokeLongMethod),
        DEFINE_NATIVE_METHOD("i", "(ILjava/lang/Object;[Ljava/lang/Object;)F", InvokeFloatMethod),
        DEFINE_NATIVE_METHOD("j", "(ILjava/lang/Object;[Ljava/lang/Object;)D", InvokeDoubleMethod),
        DEFINE_NATIVE_METHOD("k", "(ILjava/lang/Object;[Ljava/lang/Object;)Ljava/lang/Object;", InvokeObjectMethod),
        DEFINE_NATIVE_METHOD("l", "(ILjava/lang/Object;[Ljava/lang/Object;)[Z", InvokeBooleanArrayMethod),
        DEFINE_NATIVE_METHOD("m", "(ILjava/lang/Object;[Ljava/lang/Object;)[C", InvokeCharArrayMethod),
        DEFINE_NATIVE_METHOD("n", "(ILjava/lang/Object;[Ljava/lang/Object;)[S", InvokeShortArrayMethod),
        DEFINE_NATIVE_METHOD("o", "(ILjava/lang/Object;[Ljava/lang/Object;)[I", InvokeIntArrayMethod),
        DEFINE_NATIVE_METHOD("p", "(ILjava/lang/Object;[Ljava/lang/Object;)[J", InvokeLongArrayMethod),
        DEFINE_NATIVE_METHOD("q", "(ILjava/lang/Object;[Ljava/lang/Object;)[F", InvokeFloatArrayMethod),
        DEFINE_NATIVE_METHOD("r", "(ILjava/lang/Object;[Ljava/lang/Object;)[D", InvokeDoubleArrayMethod),
        DEFINE_NATIVE_METHOD("s", "(ILjava/lang/Object;[Ljava/lang/Object;)[Ljava/lang/Object;",
                             InvokeObjectArrayMethod),
        DEFINE_NATIVE_METHOD("t", "(ILjava/lang/Class;Ljava/lang/String;Ljava/lang/String;Z)V", GetField),
        DEFINE_NATIVE_METHOD("u", "(ILjava/lang/Class;Ljava/lang/String;Ljava/lang/String;Z)V", GetFieldMapped),
        DEFINE_NATIVE_METHOD("v", "(ILjava/lang/Object;)Z", GetBooleanField),
        DEFINE_NATIVE_METHOD("w", "(ILjava/lang/Object;)C", GetCharField),
        DEFINE_NATIVE_METHOD("x", "(ILjava/lang/Object;)S", GetShortField),
        DEFINE_NATIVE_METHOD("y", "(ILjava/lang/Object;)I", GetIntField),
        DEFINE_NATIVE_METHOD("z", "(ILjava/lang/Object;)J", GetLongField),
        DEFINE_NATIVE_METHOD("aa", "(ILjava/lang/Object;)F", GetFloatField),
        DEFINE_NATIVE_METHOD("bb", "(ILjava/lang/Object;)D", GetDoubleField),
        DEFINE_NATIVE_METHOD("cc", "(ILjava/lang/Object;)Ljava/lang/Object;", GetObjectField),
        DEFINE_NATIVE_METHOD("dd", "(ILjava/lang/Object;)[Z", GetBooleanArrayField),
        DEFINE_NATIVE_METHOD("ee", "(ILjava/lang/Object;)[C", GetCharArrayField),
        DEFINE_NATIVE_METHOD("ff", "(ILjava/lang/Object;)[S", GetShortArrayField),
        DEFINE_NATIVE_METHOD("gg", "(ILjava/lang/Object;)[I", GetIntArrayField),
        DEFINE_NATIVE_METHOD("hh", "(ILjava/lang/Object;)[J", GetLongArrayField),
        DEFINE_NATIVE_METHOD("ii", "(ILjava/lang/Object;)[F", GetFloatArrayField),
        DEFINE_NATIVE_METHOD("jj", "(ILjava/lang/Object;)[D", GetDoubleArrayField),
        DEFINE_NATIVE_METHOD("kk", "(ILjava/lang/Object;)[Ljava/lang/Object;", GetObjectArrayField),
        DEFINE_NATIVE_METHOD("ll", "(ILjava/lang/Object;Z)V", SetBooleanField),
        DEFINE_NATIVE_METHOD("mm", "(ILjava/lang/Object;C)V", SetCharField),
        DEFINE_NATIVE_METHOD("nn", "(ILjava/lang/Object;S)V", SetShortField),
        DEFINE_NATIVE_METHOD("oo", "(ILjava/lang/Object;I)V", SetIntField),
        DEFINE_NATIVE_METHOD("pp", "(ILjava/lang/Object;J)V", SetLongField),
        DEFINE_NATIVE_METHOD("qq", "(ILjava/lang/Object;F)V", SetFloatField),
        DEFINE_NATIVE_METHOD("rr", "(ILjava/lang/Object;D)V", SetDoubleField),
        DEFINE_NATIVE_METHOD("ss", "(ILjava/lang/Object;Ljava/lang/Object;)V", SetObjectField),
        DEFINE_NATIVE_METHOD("tt", "(ILjava/lang/Object;[Z)V", SetBooleanArrayField),
        DEFINE_NATIVE_METHOD("uu", "(ILjava/lang/Object;[C)V", SetCharArrayField),
        DEFINE_NATIVE_METHOD("vv", "(ILjava/lang/Object;[S)V", SetShortArrayField),
        DEFINE_NATIVE_METHOD("ww", "(ILjava/lang/Object;[I)V", SetIntArrayField),
        DEFINE_NATIVE_METHOD("xx", "(ILjava/lang/Object;[J)V", SetLongArrayField),
        DEFINE_NATIVE_METHOD("yy", "(ILjava/lang/Object;[F)V", SetFloatArrayField),
        DEFINE_NATIVE_METHOD("zz", "(ILjava/lang/Object;[D)V", SetDoubleArrayField),
        DEFINE_NATIVE_METHOD("aaa", "(ILjava/lang/Object;[Ljava/lang/Object;)V", SetObjectArrayField),
        DEFINE_NATIVE_METHOD("bbb", "(ILjava/lang/Object;[Ljava/lang/Object;)V", InvokeNonVirtualVoidMethod),
        DEFINE_NATIVE_METHOD("ccc", "(ILjava/lang/Class;[Ljava/lang/Object;)Ljava/lang/Object;", InvokeConstructor),
        DEFINE_NATIVE_METHOD("ddd", "(ILjava/lang/Object;[Ljava/lang/Object;)B", InvokeByteMethod),
        DEFINE_NATIVE_METHOD("eee", "(ILjava/lang/Object;[Ljava/lang/Object;)[B", InvokeByteArrayMethod),
        DEFINE_NATIVE_METHOD("fff", "(ILjava/lang/Object;)B", GetByteField),
        DEFINE_NATIVE_METHOD("ggg", "(ILjava/lang/Object;B)V", SetByteField),
        DEFINE_NATIVE_METHOD("hhh", "(ILjava/lang/Object;)[B", GetByteArrayField),
        DEFINE_NATIVE_METHOD("iii", "(ILjava/lang/Object;[B)V", SetByteArrayField),
        DEFINE_NATIVE_METHOD("gfn", "(I)Ljava/lang/String;", GetFieldName),
        DEFINE_NATIVE_METHOD("gmn", "(I)Ljava/lang/String;", GetMethodName),
    };

    env->RegisterNatives(cls, natives, sizeof(natives) / sizeof(JNINativeMethod));
}