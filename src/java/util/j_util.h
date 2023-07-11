#pragma once

#include <jni.h>
#include <string>
#include <algorithm>

#define CHECK_AND_RAISE_JNI_EXCEPTION(env) \
if (env->ExceptionCheck()) {			   \
	env->ExceptionDescribe();			   \
	env->ExceptionClear();				   \
}										   \


std::string GetInternalClassName(JNIEnv* env, jclass cls);

std::string GetInternalName(std::string name);

std::string StripClassName(std::string name);

std::string GetString(JNIEnv* env, jstring string);