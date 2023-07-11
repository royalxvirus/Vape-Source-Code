#pragma once

#ifndef DLLMAIN_H_
#define DLLMAIN_H_

#include <Windows.h>
#include <jni.h>
#include <jvmti.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "java/a_class.h"
#include "java/b_class.h"

#include "java/util/j_util.h"

typedef struct JVM_CTX {
	JNIEnv* jni;
	JavaVM* jvm;
	jvmtiEnv* jvmti;
} *PJVM_CTX;

extern JNIEnv* env;

// todo push to class?
typedef struct APP_CTX {
	jint loadedClassesCount;
	jclass* loadedClasses;
	jobject objClassLoader;
	jmethodID midFindClass;

	jclass lastLookup; // really?

	jint forgeVersion;
	int lookupIndex;

	JVM_CTX* jvm;

	bool isVanilla;

	int setTo1onError;

	bool running;

	jclass clsMain;
	jclass clsMappings;
	jclass clsEventRegistry;

	std::vector<jobject> classes;
	std::map<int, void*> map; // mapped by forge version and something else.
} *PAPP_CTX;

extern PAPP_CTX app_ctx;

void InitializeMinecraftClassLoaderContext(PAPP_CTX ctx);

jclass LookupMinecraftClass(PAPP_CTX ctx, JNIEnv* env, const char* name);

jclass GetClassByName(PAPP_CTX ctx, JNIEnv* env, const char* name);

jfieldID GetFieldByName(PAPP_CTX ctx, JNIEnv* env, jclass cls, const char* name, const char* signature);

jmethodID GetMethodByName(PAPP_CTX ctx, JNIEnv* env, jclass cls, const char* name, const char* signature);

typedef struct FORGE_CTX {
	jclass clsEvent;
	jclass clsListenerList;
	jclass clsEventPriority;
	jclass clsListenerListInstance;

	jmethodID midGetListenerList;
	jmethodID midRegister;
	jmethodID midUnregister;
	//jmethodID midUnregisterAll;
	jmethodID midSetCancelled; // investigate usage.
	jmethodID midBuildCache; // investiage usage.

	jfieldID fidEventPriority;
	jfieldID fidLists; // investigate usage.
	jfieldID fidAllLists; // investigate usage.

	jclass clsVapeListener;
	jmethodID midVapeCall;
	jobject objVapeListener;

	jclass* listenerInstances;
} *PFORGE_CTX;

extern PFORGE_CTX forge_ctx;

void SetupForgeEventListener(PFORGE_CTX forge_ctx, PAPP_CTX app_ctx);

#endif // !DLLMAIN_H_
