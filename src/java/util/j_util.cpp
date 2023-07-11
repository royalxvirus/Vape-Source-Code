#include "j_util.h"

std::string GetInternalClassName(JNIEnv* env, jclass cls) {
	jclass clsClass = env->FindClass("java/lang/Class");
	jmethodID midGetName = env->GetMethodID(clsClass, "getName", "()Ljava/lang/String;");
	jstring name = (jstring)env->CallObjectMethod(cls, midGetName);
	const char* charName = env->GetStringUTFChars(name, NULL);
	std::string strName(charName);
	env->ReleaseStringUTFChars(name, charName);
	std::replace(strName.begin(), strName.end(), '.', '/');
	return strName;
}

std::string GetInternalName(std::string name) {
	name = "L" + name + ";";
	std::replace(name.begin(), name.end(), '.', '/');
	return name;
}

std::string StripClassName(std::string name) {
	if (name.find(";") != std::string::npos) {
		return name.substr(1, name.length() - 1);
	}
	return name;
}

std::string GetString(JNIEnv* env, jstring string) {
	const char* charString = env->GetStringUTFChars(string, NULL);
	std::string strString(charString);
	env->ReleaseStringUTFChars(string, charString);
	return strString;
}
