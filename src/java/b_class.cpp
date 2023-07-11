#include "b_class.h"

#include <map>
#include <iostream>
#include <algorithm>

#include "./util/j_util.h"

#include "java_class.h"

#pragma warning(disable: 4715) // disable macro warnings.

using namespace std;

#define DEFINE_TYPE_COMPARISON(jni_env, obj_instance, ptr_value, type_name, java_type_class_name, type_bytecode_name, type_bytecode_name_lowercase, jni_type) \
	static jclass cls##java_type_class_name; \
	if (cls##java_type_class_name == NULL) { \
		cls##java_type_class_name = (jclass)jni_env->NewGlobalRef(jni_env->FindClass("java/lang/" #java_type_class_name)); \
	} \
	static jclass cls##type_bytecode_name; \
	if (cls##type_bytecode_name == NULL) { \
		cls##type_bytecode_name = (jclass)jni_env->NewGlobalRef(jni_env->GetStaticObjectField(cls##java_type_class_name, jni_env->GetStaticFieldID(cls##java_type_class_name, "TYPE", "Ljava/lang/Class;")));\
	}\
	static jfieldID fid##type_bytecode_name##Value; \
	if (fid##type_bytecode_name##Value == NULL) { \
		fid##type_bytecode_name##Value = jni_env->GetFieldID(cls##java_type_class_name, "value", #type_bytecode_name); \
	} \
	if (jni_env->IsSameObject(obj_instance, cls##type_bytecode_name)) {\
		(*ptr_value).type_bytecode_name_lowercase = jni_env->Get##type_name##Field(obj_instance, fid##type_bytecode_name##Value);\
		return;\
	}\

#define GET_VARARGS(jni, params) \
	jsize length = env->GetArrayLength(params); \
	jvalue* args = new jvalue[length]; \
	if (length > 0) { \
		jvalue element;\
		jobject arg; \
		for (jsize i = 0; i < length; i++) { \
			arg = env->GetObjectArrayElement(params, i);\
			GetTypeValue(jni, arg, &element);\
			*(args + i) = element; \
		} \
	}

void GetTypeValue(JNIEnv* jni_env, jobject obj_instance, jvalue* ptr_value)
{

	if (obj_instance == nullptr) {
		(*ptr_value).l = NULL;
		return;
	}

	DEFINE_TYPE_COMPARISON(jni_env, obj_instance, ptr_value, Boolean, Boolean, Z, z, jboolean)
	DEFINE_TYPE_COMPARISON(jni_env, obj_instance, ptr_value, Char, Character, C, c, jchar)
	DEFINE_TYPE_COMPARISON(jni_env, obj_instance, ptr_value, Byte, Byte, B, b, jbyte)
	DEFINE_TYPE_COMPARISON(jni_env, obj_instance, ptr_value, Short, Short, S, s, jshort)
	DEFINE_TYPE_COMPARISON(jni_env, obj_instance, ptr_value, Int, Integer, I, i, jint)
	DEFINE_TYPE_COMPARISON(jni_env, obj_instance, ptr_value, Long, Long, J, j, jlong)
	DEFINE_TYPE_COMPARISON(jni_env, obj_instance, ptr_value, Float, Float, F, f, jfloat)
	DEFINE_TYPE_COMPARISON(jni_env, obj_instance, ptr_value, Double, Double, D, d, jdouble)

	(*ptr_value).l = obj_instance;

}

typedef struct field_t {
	jclass cls;
	jfieldID fid;
	bool isStatic;
	bool isSpecial;
} * pfield_t;

typedef struct method_t {
	jclass cls;
	jmethodID mid;
	bool isStatic;
	bool isSpecial;
} * pmethod_t;

typedef struct constructor_t {
	jclass cls;
	jmethodID mid;
} * pconstructor_t;

map<int, field_t> fields;
map<int, method_t> methods;
map<int, jclass> loadedClasses;
map<int, constructor_t> constructors;

// TODO: Merge from Vape4DLL (v4.07)

void GetFieldSpecial(pfield_t ctx, JNIEnv* env, jclass cls, char* name, char* desc) {


	JavaVM* vm;
	jvmtiEnv* jvmti_env;
	env->GetJavaVM(&vm);
	vm->GetEnv(reinterpret_cast<void**>(&jvmti_env), JVMTI_VERSION_1_1);

	jfieldID fid;

	// if JavaVM null????????
	if (!ctx->isSpecial) {
		fid = env->GetFieldID(cls, name, desc);
		if (fid == NULL) {
			fid = env->GetStaticFieldID(cls, name, desc);
		}
		// presumably set fieldId in ctx lmao.
		// surely?
		ctx->fid = fid;
		return;
	}

	bool flag = false;
	jint fieldsCount;
	jfieldID* fields;
	bool iVar11 = false;
	int iVar12 = 0;

	while (true) {
		jvmti_env->GetClassFields(cls, &fieldsCount, &fields);
		if ((!ctx->isSpecial || flag) || fieldsCount != 0) break;
		env->GetFieldID(cls, "a", "a"); // force crash?
		flag = true;
	}
	int index = 0;
	do {

		char* fieldName;
		char* fieldSignature;
		char* fieldGeneric;

		// LAB_340744e6:
		if (fieldsCount <= index) {
			if (!ctx->isSpecial) {

			}
			else {

				char* classSignature;
				char* classGeneric;

				jvmti_env->GetClassSignature(cls, &classSignature, &classGeneric);
				printf("Failed to get field special %s -> %s %s\n", classSignature, name, desc);
				jvmti_env->Deallocate((unsigned char*)classSignature);
				jvmti_env->Deallocate((unsigned char*)classGeneric);
			}

			jvmti_env->GetFieldName(cls, fields[index], &fieldName, &fieldSignature, &fieldGeneric);

			if (fieldName == NULL || fieldSignature == NULL) {
			INCREMENT:
				index = index + 1;
				//fields++; //lVar7 = lVar7 + 8;
				continue; //goto LAB_340744e6;
			}

			if (!ctx->isSpecial || iVar11) {

				if (!strcmp(fieldName, name)) {
					if (desc != NULL) {
						if (!strcmp(fieldSignature, desc)) {
							fid = fields[index];
						}
						else {
							goto DEALLOCATE;
						}
					}
				}
			}

		DEALLOCATE:
			jvmti_env->Deallocate((unsigned char*)fieldName);
			jvmti_env->Deallocate((unsigned char*)fieldSignature);
			jvmti_env->Deallocate((unsigned char*)fieldGeneric);

			goto INCREMENT;
		}

		if (!strstr(fieldSignature, desc)) break;

		char* classSignature;
		char* classGeneric;

		jvmti_env->GetClassSignature(cls, &classSignature, &classGeneric);
		if (strstr(classSignature, "lwjgl")) break;
		iVar11 = true;
	} while (true);

	// surely?
	ctx->fid = fid;
}

JNIEnv* env;

#define ACC_STATIC 8

// Lite & v4
jmethodID GetMethodWithModifiers(pmethod_t param_1, jclass cls, bool isStatic) {

	JavaVM* vm;
	jvmtiEnv* jvmti_env;
	env->GetJavaVM(&vm);
	vm->GetEnv(reinterpret_cast<void**>(&jvmti_env), JVMTI_VERSION_1_1);

	boolean flag = false;

	jint methodsCount;
	jmethodID* methods;

	jmethodID returnMethod = NULL;

	while (true) {
		jvmti_env->GetClassMethods(cls, &methodsCount, &methods);
		if ((!param_1->isSpecial || flag) || methodsCount != 0) break;
		env->GetMethodID(cls, "a", "a"); // force crash?
		flag = true;
	}

	for (jint index = 0; index < methodsCount; index++) {
		jmethodID mid = methods[index];

		char* methodName;
		char* methodDesc;

		jvmti_env->GetMethodName(mid, &methodName, &methodDesc, NULL);

		if (methodName && methodDesc) {

			jint modifiers;
			jvmti_env->GetMethodModifiers(mid, &modifiers);

			if (!isStatic || (modifiers & ACC_STATIC) != 0) {
				returnMethod = mid;
			}

			jvmti_env->Deallocate((unsigned char*)methodName);
			jvmti_env->Deallocate((unsigned char*)methodDesc);
		}
	}

	return returnMethod;
}

char* g_methodName;
char* g_methodDesc;

jmethodID GetMethodSpecial(pmethod_t method, JNIEnv* env, jclass cls, char* name, char* desc, jboolean bruh) {

	JavaVM* vm;
	jvmtiEnv* jvmti_env;
	env->GetJavaVM(&vm);
	vm->GetEnv(reinterpret_cast<void**>(&jvmti_env), JVMTI_VERSION_1_1);

	char* className;


	jvmti_env->GetClassSignature(cls, &className, NULL);
	jmethodID mid = GetMethodWithModifiers(method, cls, false);

	if (!mid) {

		if (jvmti_env == NULL) {
			mid = env->GetMethodID(cls, name, desc);
			if (mid == NULL) {
				env->GetStaticMethodID(cls, name, desc);
			}
		}
		else {

			boolean flag = false;

			jint methodsCount;
			jmethodID* methods;

			while (true) {
				jvmti_env->GetClassMethods(cls, &methodsCount, &methods);
				if ((!method->isSpecial || flag) || methodsCount != 0) break;
				env->GetMethodID(cls, "a", "a"); // force crash?
				flag = true;
			}

			for (jint index = 0; index < methodsCount; index++) {
				jmethodID mid = methods[index];

				char* methodName;
				char* methodDesc;

				jvmti_env->GetMethodName(mid, &methodName, &methodDesc, NULL);

				if (methodName && methodDesc) {

					jvmti_env->Deallocate((unsigned char*)methodName);
					jvmti_env->Deallocate((unsigned char*)methodDesc);
				}

				if ((mid == NULL) && (mid = env->GetMethodID(cls, name, desc), mid == NULL)) {
					mid = env->GetStaticMethodID(cls, name, desc);
				}
			}
		}
	}

	return mid;
}



/*-------------------------------------------------------
	Defines Getters, Setters & Invokers for the method type
	(and array type).
-------------------------------------------------------*/
#define DEFINE_TYPE_METHODS(type, capitalized) \
type \
Invoke##capitalized##Method(JNIEnv* env, jclass caller, jint id, jobject instance, jobjectArray params)\
{\
	if (methods.count(id))\
	{\
		GET_VARARGS(env, params) \
		method_t method = methods[id]; \
		if (method.isStatic)\
		{\
			type ret = env->CallStatic##capitalized##MethodA(method.cls, method.mid, args); \
			CHECK_AND_RAISE_JNI_EXCEPTION(env) \
			delete[] args; \
			return ret; \
		}\
		else\
		{\
			type ret = env->Call##capitalized##MethodA(instance, method.mid, args); \
			CHECK_AND_RAISE_JNI_EXCEPTION(env) \
			delete[] args; \
			return ret; \
		}\
	}\
}; \
type##Array \
Invoke##capitalized##ArrayMethod(JNIEnv* env, jclass caller, jint id, jobject instance, jobjectArray params)\
{\
	if (methods.count(id))\
	{\
		GET_VARARGS(env, params) \
		method_t method = methods[id]; \
		if (method.isStatic) \
		{ \
			type##Array ret = (type##Array)env->CallStaticObjectMethodA(method.cls, method.mid, args); \
			CHECK_AND_RAISE_JNI_EXCEPTION(env) \
			delete[] args; \
			return ret;  \
		} \
		else \
		{ \
			type##Array ret = (type##Array)env->CallObjectMethodA(instance, method.mid, args); \
			CHECK_AND_RAISE_JNI_EXCEPTION(env) \
			delete[] args; \
			return ret;  \
		} \
	}\
}; \
type \
Get##capitalized##Field(JNIEnv* env, jclass caller, jint id, jobject instance) \
{ \
    if (fields.count(id)) \
	{ \
		field_t fd = fields[id]; \
		if (fd.isStatic) \
		{ \
			type ret = env->GetStatic##capitalized##Field(fd.cls, fd.fid); \
			CHECK_AND_RAISE_JNI_EXCEPTION(env) \
			return ret; \
		}\
		else \
		{ \
			return env->Get##capitalized##Field(instance, fd.fid); \
		} \
	} \
}; \
type##Array \
Get##capitalized##ArrayField(JNIEnv* env, jclass caller, jint id, jobject instance) \
{ \
    if (fields.count(id)) \
	{ \
		field_t fd = fields[id]; \
		if (fd.isStatic) \
		{ \
			type##Array ret = (type##Array)env->GetStaticObjectField(fd.cls, fd.fid); \
			CHECK_AND_RAISE_JNI_EXCEPTION(env) \
			return ret; \
		}\
		else \
		{ \
			type##Array ret = (type##Array)env->GetObjectField(instance, fd.fid); \
			CHECK_AND_RAISE_JNI_EXCEPTION(env) \
			return ret; \
		} \
	} \
}; \
void \
Set##capitalized##Field(JNIEnv* env, jclass caller, jint id, jobject instance, type value) \
{\
    if (fields.count(id)) \
	{ \
		field_t fd = fields[id]; \
		if (fd.isStatic) \
		{ \
			env->SetStatic##capitalized##Field(fd.cls, fd.fid, value); \
			CHECK_AND_RAISE_JNI_EXCEPTION(env) \
			return; \
		}\
		else \
		{ \
			env->Set##capitalized##Field(instance, fd.fid, value); \
			CHECK_AND_RAISE_JNI_EXCEPTION(env) \
			return; \
		} \
	} \
}; \
void \
Set##capitalized##ArrayField(JNIEnv* env, jclass caller, jint id, jobject instance, type##Array value) \
{\
    if (fields.count(id)) \
	{ \
		field_t fd = fields[id]; \
		if (fd.isStatic) \
		{ \
			env->SetStaticObjectField(fd.cls, fd.fid, value); \
			CHECK_AND_RAISE_JNI_EXCEPTION(env) \
			return; \
		}\
		else \
		{ \
			env->SetObjectField(instance, fd.fid, value); \
			CHECK_AND_RAISE_JNI_EXCEPTION(env) \
			return; \
		} \
	} \
}

DEFINE_TYPE_METHODS(jboolean, Boolean)
DEFINE_TYPE_METHODS(jchar, Char)
DEFINE_TYPE_METHODS(jshort, Short)
DEFINE_TYPE_METHODS(jint, Int)
DEFINE_TYPE_METHODS(jlong, Long)
DEFINE_TYPE_METHODS(jfloat, Float)
DEFINE_TYPE_METHODS(jdouble, Double)
DEFINE_TYPE_METHODS(jobject, Object)
DEFINE_TYPE_METHODS(jbyte, Byte)

char* MapFieldName(char* packetStruct, void* unknown, jclass cls) {

}

char* MapMethodName(char* packetStruct, void* unknown, jclass cls) {

}

char* MapClassName(char* packetStruct, void* unknown, jclass cls) {

	if (*packetStruct == '\0') {

	}
	else {
		JNIEnv* env; // pull from DllMain

		JavaVM* vm;
		jvmtiEnv* jvmti_env;
		env->GetJavaVM(&vm);
		vm->GetEnv(reinterpret_cast<void**>(&jvmti_env), JVMTI_VERSION_1_1);

		char* signature;
		char* generic;
		jvmti_env->GetClassSignature(cls, &signature, &generic);

		char* duplicatedSignature = _strdup(signature);

		if (signature != NULL) {
			jvmti_env->Deallocate((unsigned char*)signature);
		}

		if (generic != NULL) {
			jvmti_env->Deallocate((unsigned char*)generic);
		}
	}

	// request the class name

	return NULL;
}

void SendPacket() {
}

void GetField(pfield_t field, JNIEnv* env, jint id, jclass cls, jstring name, jstring desc, jboolean map, jboolean isStatic) {

	field->isStatic = isStatic;
	field->cls = (jclass)env->NewGlobalRef(cls);

	const char* charName = env->GetStringUTFChars(name, NULL);
	const char* charDesc = env->GetStringUTFChars(desc, NULL);

	if (!map) {
		if (!isStatic) {
			field->fid = env->GetFieldID(cls, charName, charDesc);
		}
		else {
			field->fid = env->GetStaticFieldID(cls, charName, charDesc);
		}
	}
	else {
		//jclass clsMappings = env->FindClass("fake/Mappings");
		//jmethodID midGetMethodMapping = env->GetStaticMethodID(clsMappings, "getMethod", "(ILjava/lang/Class;Ljava/lang/String;Ljava/lang/String;Z)V");
		//env->CallStaticVoidMethod(clsMappings, midGetMethodMapping, id, cls, name, desc, isStatic);
	}

}

// why
#include <mbstring.h>

// Ported as is from the DLL
// - a(ILjava/lang/Class;Ljava/lang/String;Ljava/lang/String;Z)V sets map to true, 
// - b(ILjava/lang/Class;Ljava/lang/String;Ljava/lang/String;Z)V sets map to false
void GetMethod(pmethod_t method, JNIEnv* env, jint id, jclass cls, jstring name, jstring desc, jboolean map, jboolean isStatic) {

	method->isStatic = isStatic;
	method->cls = (jclass) env->NewGlobalRef(cls);

	//char charName[128]; // initialize lmao
	//char charDesc[122]; // initialize lmao

	//jint length;

	const char* charName = env->GetStringUTFChars(name, NULL);
	const char* charDesc = env->GetStringUTFChars(desc, NULL);

	//length = env->GetStringUTFLength(name);
	//env->GetStringUTFRegion(name, 0, length, charName);
	//length = env->GetStringUTFLength(desc);
	//env->GetStringUTFRegion(desc, 0, length, charDesc);
	if (!map) {
		if (!isStatic) {
			method->mid = env->GetMethodID(cls, charName, charDesc);
		}
		else {
			method->mid = env->GetStaticMethodID(cls, charName, charDesc);
		}
	}
	else {

		JavaVM* vm;
		jvmtiEnv* jvmti_env;
		env->GetJavaVM(&vm);
		vm->GetEnv(reinterpret_cast<void**>(&jvmti_env), JVMTI_VERSION_1_1);

		char* signature;
		jvmti_env->GetClassSignature(cls, &signature, NULL);

		//unsigned char* dup = _mbsdup(signature);

		if (signature != NULL) {
			jvmti_env->Deallocate((unsigned char*)signature);
		}

		//jclass clsMappings = env->FindClass("fake/Mappings");
		//jmethodID midGetMethodMapping = env->GetStaticMethodID(clsMappings, "getMethod", "(ILjava/lang/Class;Ljava/lang/String;Ljava/lang/String;Z)V");
		//env->CallStaticVoidMethod(clsMappings, midGetMethodMapping, id, cls, name, desc, isStatic);
	}
	/*else {

		JavaVM* vm;
		jvmtiEnv* jvmti_env;
		env->GetJavaVM(&vm);
		vm->GetEnv(reinterpret_cast<void**>(&jvmti_env), JVMTI_VERSION_1_1);

		char* signature;
		char* generic;
		jvmti_env->GetClassSignature(cls, &signature, &generic);

		if (signature != NULL) {
			jvmti_env->Deallocate((unsigned char*)signature);
		}

		if (generic != NULL) {
			jvmti_env->Deallocate((unsigned char*)generic);
		}


		char* socketName;

		// get name mapping from socket.
		// desc is mapped from class map.

		if (true) { // todo, comparing against APP_CTX global?
			if (!isStatic) {
				method->mid = env->GetMethodID(cls, socketName, charDesc);
			}
			else {
				method->mid = env->GetStaticMethodID(cls, socketName, charDesc);
			}
		}

		if (socketName == NULL) {
			env->ThrowNew(env->FindClass("java/lang/Exception"), "nice"); // what is your addiction with the n-word Manthe?
		}
	}*/
}

// b(ILjava/lang/Class;Ljava/lang/String;Ljava/lang/String;Z)V
void GetMethodMapped(JNIEnv* env, jclass caller, jint id, jclass cls, jstring name, jstring desc, jboolean isStatic) {

	method_t method{};

	GetMethod(&method, env, id, cls, name, desc, true, isStatic);

	methods[id] = method;

	//jclass clsMappings = env->FindClass("fake/Mappings");
	//jmethodID midGetMethodMapping = env->GetStaticMethodID(clsMappings, "getMethod", "(ILjava/lang/Class;Ljava/lang/String;Ljava/lang/String;Z)V");
	//env->CallStaticVoidMethod(clsMappings, midGetMethodMapping, id, cls, name, desc, isStatic);
}

void GetMethod(JNIEnv* env, jclass caller, jint id, jclass cls, jstring name, jstring desc, jboolean isStatic) {

	method_t method{};
	
	GetMethod(&method, env, id, cls, name, desc, false, isStatic);
	
	methods[id] = method;

	/*if (!methods.count(id)) {

		//method_t method{};

		//VAPE_GetMethodSocket(&method, env, caller, id, cls, name, desc, true, isStatic);

		//methods[id] = method;

		string mdName = GetString(env, name);
		string mdDesc = GetString(env, desc);
		string clsName = GetInternalClassName(env, cls);

		if (mdName.find("<init>") != string::npos) {

			std::cout << "[METHOD] Getting constructor method " << mdName << " desc: " << mdDesc << " from class: " << clsName << " at id: " << id << std::endl;

			constructors[id] = {
			   (jclass)env->NewGlobalRef(cls),
				env->GetMethodID(cls, mdName.c_str(), mdDesc.c_str())
			};
			return;
		}

		if (isStatic) {
			std::cout << "[METHOD] Getting method " << mdName << " desc: " << mdDesc << " from class: " << clsName << " at id: " << id << std::endl;

			jmethodID mid = env->GetStaticMethodID(cls, mdName.c_str(), mdDesc.c_str());

			if (env->ExceptionCheck()) {
				env->ExceptionClear();
				return;
			}

			methods[id] = {
				(jclass)env->NewGlobalRef(cls),
				mid,
				true
			};
		}
		else {
			std::cout << "[METHOD] Getting method " << mdName << " desc: " << mdDesc << " from class: " << clsName << " at id: " << id << std::endl;

			jmethodID mid = env->GetMethodID(cls, mdName.c_str(), mdDesc.c_str());

			if (env->ExceptionCheck()) {
				env->ExceptionClear();
				return;
			}

			methods[id] = {
				(jclass)env->NewGlobalRef(cls),
				mid,
				false
			};
		}
	}
	else {
		std::cout << "Attempted to get method that already exists (" << id << ")" << std::endl;
	}*/
}

void InvokeMethodVoid(JNIEnv* env, jclass caller, jint id, jobject instance, jobjectArray params) {
	if (methods.count(id)) {
		GET_VARARGS(env, params)
		method_t method = methods[id];
		if (method.isStatic) {
			env->CallStaticVoidMethodA(method.cls, method.mid, args);
			CHECK_AND_RAISE_JNI_EXCEPTION(env)
			delete[] args;
		}
		else {
			env->CallVoidMethodA(instance, method.mid, args);
			CHECK_AND_RAISE_JNI_EXCEPTION(env)
			delete[] args;
		}
	}
	else {
		std::cout << "Attempted to invoke method that does not exist (" << id << ")" << std::endl;
	}
}

// u(ILjava/lang/Class;Ljava/lang/String;Ljava/lang/String;Z)V
void GetFieldMapped(JNIEnv* env, jclass caller, jint id, jclass cls, jstring name, jstring desc, jboolean isStatic) {

	field_t field{};

	GetField(&field, env, id, cls, name, desc, false, isStatic);

	fields[id] = field;
}

// t(ILjava/lang/Class;Ljava/lang/String;Ljava/lang/String;Z)V
void GetField(JNIEnv* env, jclass caller, jint id, jclass cls, jstring name, jstring desc, jboolean isStatic) {
	if (!fields.count(id)) {
		string fieldName = GetString(env, name);
		string fieldDesc = GetString(env, desc);
		string className = GetInternalClassName(env, cls);

		if (isStatic) {

			std::cout << "[FIELD] Getting static field " << fieldName << " desc: " << fieldDesc << " from class: " << className << " at id: " << id << std::endl;

			jfieldID fid = env->GetStaticFieldID(cls, fieldName.c_str(), fieldDesc.c_str());

			if (env->ExceptionCheck()) {
				env->ExceptionClear();
				return;
			}

			fields[id] = {
				(jclass)env->NewGlobalRef(cls),
				fid,
				true
			};
		}
		else {
			std::cout << "[FIELD] Getting field " << fieldName << " desc: " << fieldDesc << " from class: " << className << " at id: " << id << std::endl;

			jfieldID fid = env->GetFieldID(cls, fieldName.c_str(), fieldDesc.c_str());

			if (env->ExceptionCheck()) {
				env->ExceptionClear();
				return;
			}

			fields[id] = {
				(jclass)env->NewGlobalRef(cls),
				fid,
				false
			};
		}
	}
	else {
		std::cout << "Attempted to get field that already exists (" << id << ")" << std::endl;
	}
}

void InvokeNonVirtualVoidMethod(JNIEnv* env, jclass caller, jint id, jobject instance, jobjectArray params) {
	if (methods.count(id)) {
		GET_VARARGS(env, params)
		method_t method = methods[id];
		jclass instance_cls = env->GetObjectClass(instance);
		jclass instance_super_cls = env->GetSuperclass(instance_cls);
		env->CallNonvirtualVoidMethodA(instance, instance_super_cls, method.mid, args);
		CHECK_AND_RAISE_JNI_EXCEPTION(env)
		delete[] args;
	}
	else {
		std::cout << "Attempted to call NonVirtualVoidMethod that does not exist (" << id << ")" << std::endl;
	}
}

jobject InvokeConstructor(JNIEnv* env, jclass caller, jint id, jclass cls, jobjectArray params) {
	if (constructors.count(id)) {
		GET_VARARGS(env, params)
		constructor_t constructor = constructors[id];
		jobject ret = env->NewObjectA(cls, constructor.mid, args);
		CHECK_AND_RAISE_JNI_EXCEPTION(env)
		delete[] args;
		return ret;
	}
	else {
		std::cout << "Attempted to call constructor that does not exist (" << id << ")" << std::endl;
	}
	return NULL;
}

jstring GetFieldName(JNIEnv* env, jclass caller, jint id) {
	if (fields.count(id)) {
		field_t fd = fields[id];

		JavaVM* vm;
		jvmtiEnv* jvmti_env;
		env->GetJavaVM(&vm);
		vm->GetEnv(reinterpret_cast<void**>(&jvmti_env), JVMTI_VERSION_1_1);

		char* fieldName;
		jvmti_env->GetFieldName(fd.cls, fd.fid, &fieldName, NULL, NULL);

		jstring strName = env->NewStringUTF(fieldName);

		jvmti_env->Deallocate((unsigned char*)fieldName);
		
		return strName;
	}
	else {
		std::cout << "Attempted to get name of field that does not exist (" << id << ")" << std::endl;
	}
}

jstring GetMethodName(JNIEnv* env, jclass caller, jint id) {
	if (methods.count(id)) {
		method_t md = methods[id];

		JavaVM* vm;
		jvmtiEnv* jvmti_env;
		env->GetJavaVM(&vm);
		vm->GetEnv(reinterpret_cast<void**>(&jvmti_env), JVMTI_VERSION_1_1);

		char* methodName;
		jvmti_env->GetMethodName(md.mid, &methodName, NULL, NULL);

		jstring strName = env->NewStringUTF(methodName);

		jvmti_env->Deallocate((unsigned char*)methodName);

		return strName;
	}
	else {
		std::cout << "Attempted to get name of method that does not exist (" << id << ")" << std::endl;
	}
}