#include <jni.h>
#include <jvmti.h>
#include <Windows.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <mutex>
#include <MinHook.h>
#include <Shlobj.h>

jint key;
std::mutex dumpMutex;

std::string GetString(JNIEnv* env, jstring string) {
	const char* charString = env->GetStringUTFChars(string, NULL);
	std::string strString(charString);
	env->ReleaseStringUTFChars(string, charString);
	return strString;
}

void XOR(std::string& nString)
{
	const int KEY = key;
	int strLen = (nString.length());
	char* cString = (char*)(nString.c_str());

	for (int i = 0; i < strLen; i++)
	{
		*(cString + i) = (*(cString + i) ^ KEY);
	}
}

bool fileExists(const std::wstring& name) {
	if (FILE* file = _wfopen(name.c_str(), L"r")) {
		fclose(file);
		return true;
	}
	else {
		return false;
	}
}

bool getNextFreeFilePath(std::wstring& path, const char* name, std::wstring extension) {
	path.resize(MAX_PATH);
	if (!SHGetSpecialFolderPathW(HWND_DESKTOP, (wchar_t*)path.data(), CSIDL_DESKTOP, FALSE)) {
		return false;
	}
	path.resize(wcslen(path.data()));

	std::wstring fixedName(MAX_PATH, '\x00');
	mbstowcs((wchar_t*)fixedName.data(), name, strlen(name));
	PathCleanupSpec(nullptr, (wchar_t*)fixedName.data());
	fixedName.resize(wcslen(fixedName.data()));

	int initialLen = path.length();
	int i = 0;
	do {
		path.resize(initialLen);
		path += L"\\VapeTextures\\";
		path += fixedName;
		if (i != 0) {
			path += std::to_wstring(i);
		}
		path += extension;
		i++;
	} while (fileExists(path));
	return true;
}

typedef jint(JNICALL* GETKEY)(JNIEnv*, jclass);

typedef jbyteArray(JNICALL* GETTEXTURE)(JNIEnv*, jclass, jstring);

GETKEY fpGetKey;

jint JNICALL DetourGetKey(JNIEnv* jni_env, jclass a_cls)
{
	return key = fpGetKey(jni_env, a_cls);
}

GETTEXTURE fpGetTexture;

jbyteArray JNICALL DetourGetTexture(JNIEnv* jni_env, jclass a_cls, jstring name)
{

	std::cout << "[HOOK] GetTexture hook called." << std::endl;

	jbyteArray texture_array = fpGetTexture(jni_env, a_cls, name);

	std::string texture_name = GetString(jni_env, name);

	XOR(texture_name);

	if (texture_array == nullptr) {
		std::cout << "[HOOK] GetTexture returns null for name: " << texture_name  << " Key: " << key << std::endl;
		return texture_array;
	}

	jint len = jni_env->GetArrayLength(texture_array);
	jbyte* texture = jni_env->GetByteArrayElements(texture_array, NULL);

	{
		std::lock_guard<std::mutex> lock(dumpMutex);
		FILE* fp;
		std::wstring path;
		if (getNextFreeFilePath(path, texture_name.c_str(), L".png")) {
			fp = _wfopen(path.c_str(), L"wb");
			int written = fwrite(texture, sizeof(jbyte), len, fp);
			if (written == 0) {
				std::cout << "Failed to write texture. (is VapeTextures folder present?)" << std::endl;
			}
			else if (written != sizeof(jbyte) * len) {
				std::cout << "[Error] Could not write full bytes of texture: " << texture_name << " Key: " << key << std::endl;
			}
			else {
				std::cout << "Written texture: " << texture_name << " Key: " << key << std::endl;
			}
			fclose(fp);
		}
		else {
			std::cout << "[Error] could not write texture: " << texture_name << " Key: " << key << std::endl;
		}
	}

	jni_env->ReleaseByteArrayElements(texture_array, texture, 0);
	return fpGetTexture(jni_env, a_cls, name);
}


void JNICALL NativeMethodBind(jvmtiEnv* jvmti_env, JNIEnv* jni_env, jthread thread, jmethodID method, void* address, void** new_address_ptr) {
	char* name;
	char* signature;
	char* generic_ptr;
	jvmtiError error = jvmti_env->GetMethodName(method, &name, &signature, &generic_ptr);
	if (error == JVMTI_ERROR_INVALID_METHODID) {
		return;
	}
	if (!strcmp(name, "gk") && !strcmp(signature, "()I")) {
		if (MH_CreateHook(address, DetourGetKey, reinterpret_cast<void**>(&fpGetKey)) != MH_OK) {
			std::cout << "[ERROR] Failed to create GetKey hook." << std::endl;
		}
		else {
			std::cout << "[HOOK] Created GetKey hook." << std::endl;
		}
		if (MH_EnableHook(address) != MH_OK)
		{
			std::cout << "[ERROR] Failed to enable GetKey hook." << std::endl;
		}
		else {
			std::cout << "[HOOK] Enabled GetKey hook." << std::endl;
		}
	}
	else if (!strcmp(name, "gt") && !strcmp(signature, "(Ljava/lang/String;)[B")) {
		if (MH_CreateHook(address, DetourGetTexture, reinterpret_cast<void**>(&fpGetTexture)) != MH_OK) {
			std::cout << "[ERROR] Failed to create GetTexture hook." << std::endl;
		}
		else {
			std::cout << "[HOOK] Created GetTexture hook." << std::endl;
		}
		if (MH_EnableHook(address) != MH_OK)
		{
			std::cout << "[ERROR] Failed to enable GetTexture hook." << std::endl;
		}
		else {
			std::cout << "[HOOK] Enabled GetTexture hook." << std::endl;
		}
	}
}

JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM* vm, char* options, void* reserved) {

	if (AllocConsole()) {
		FILE* f;
		freopen_s(&f, "CONOUT$", "w", stdout);
		SetConsoleTitle(L"Vape Asset Dumper");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
	}
	else {
		MessageBoxA(NULL, "Failed to Allocate Console", "Error", 0);
	}

	JNIEnv* jni;
	jvmtiEnv* jvmti;
	jvmtiError             error;
	jint                   res;
	jvmtiCapabilities      capabilities;
	jvmtiEventCallbacks    callbacks;

	res = vm->GetEnv((void**)&jvmti, JVMTI_VERSION_1_2);

	if (res != JNI_OK) {
		std::cout << "Failed to get JVMTI environment." << std::endl;
	}

	(void)memset(&capabilities, 0, sizeof(capabilities));
	{
		capabilities.can_generate_native_method_bind_events = 1;
	}
	error = jvmti->AddCapabilities(&capabilities);

	if (error != JVMTI_ERROR_NONE) {
		std::cout << "Failed to add capabilities." << std::endl;
	}

	error = jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_NATIVE_METHOD_BIND, NULL);

	if (error != JVMTI_ERROR_NONE) {
		std::cout << "Failed to set event notification mode." << std::endl;
	}
	(void)memset(&callbacks, 0, sizeof(callbacks));
	{
		callbacks.NativeMethodBind = &NativeMethodBind;
	}
	error = jvmti->SetEventCallbacks(&callbacks, (jint)sizeof(callbacks));

	if (error != JVMTI_ERROR_NONE) {
		std::cout << "Failed to set callbacks." << std::endl;
	}

	MH_Initialize();


	std::cout << "Dumper injected" << std::endl;

	return JNI_OK;
}
