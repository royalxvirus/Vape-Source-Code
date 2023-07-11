// dllmain.cpp : Defines the entry point for the DLL application.

#pragma warning(disable:4996)
#include "dllmain.h"

HHOOK hook = NULL;
HINSTANCE hInst = NULL;

#include <winsock2.h>
#include <ws2tcpip.h>

JNIEnv* env;
JavaVM* vm;
PAPP_CTX app_ctx;
PFORGE_CTX forge_ctx;

std::string GetLastErrorAsString()
{
	//Get the error message ID, if any.
	DWORD errorMessageID = GetLastError();
	if (errorMessageID == 0) {
		return std::string(); //No error message has been recorded
	}

	LPSTR messageBuffer = NULL;
	
	//Ask Win32 to give us the string version of that message ID.
	//The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	//Copy the error message into a std::string.
	std::string message(messageBuffer, size);

	//Free the Win32's string's buffer.
	LocalFree(messageBuffer);

	return message;
}

LONG_PTR prev = NULL;

bool CallOnNotifiction(JNIEnv* env, UINT msg, WPARAM wParam, LPARAM lParam) {
	jclass clsUI = env->FindClass("a/d");
	CHECK_AND_RAISE_JNI_EXCEPTION(env)
	jmethodID midOnNotification = env->GetStaticMethodID(clsUI, "onNotification", "(IJJ)Z");
	CHECK_AND_RAISE_JNI_EXCEPTION(env)
	jboolean result = env->CallStaticBooleanMethod(clsUI, midOnNotification, msg, wParam, lParam);
	CHECK_AND_RAISE_JNI_EXCEPTION(env)
	return result;
}

DWORD idNotificationThread;
HWND hwndWindow;

DWORD WINAPI FinalCleanup(LPVOID) {
	JNIEnv* new_env;
	vm->AttachCurrentThread(reinterpret_cast<void**>(&new_env), NULL);


	/*
	
	  if (GetClassJavaData == (char *)0x0) {
		pcVar4 = (char *)operator_new(0x58);
		MemSet(pcVar4,0,0x58);
		*(undefined8 *)(pcVar4 + 0x28) = 0;
		*(undefined8 *)(pcVar4 + 0x30) = 0;
		*(undefined8 *)(pcVar4 + 0x38) = 0;
		*(undefined4 *)(pcVar4 + 0x40) = 0;
		*(undefined2 *)(pcVar4 + 0x50) = 0;
		GetClassJavaData = pcVar4;
	  }
	  env = local_38;
	  pcVar4 = GetClassJavaData;
	  if (*(char **)(GetClassJavaData + 0x48) != (char *)0x0) {
		MemSet(*(char **)(GetClassJavaData + 0x48),0,
			   (jsize)((longlong)*(int *)(GetClassJavaData + 0x44) << 3));
	  }
	  (*(*env)->DeleteGlobalRef)(env,*(jobject *)(pcVar4 + 8));
	  pp_Var7 = *(jobject **)(pcVar4 + 0x20);
	  uVar6 = (ulonglong)((longlong)*(jobject **)(pcVar4 + 0x28) + (7 - (longlong)pp_Var7)) >> 3;
	  if (*(jobject **)(pcVar4 + 0x28) < pp_Var7) {
		uVar6 = uVar5;
	  }
	  if (uVar6 != 0) {
		do {
		  (*(*env)->DeleteGlobalRef)(env,*pp_Var7);
		  uVar5 = uVar5 + 1;
		  pp_Var7 = pp_Var7 + 1;
		} while (uVar5 != uVar6);
	  }
	  pcVar4 = GetClassJavaData;
	  if (GetClassJavaData != (char *)0x0) {
		pvVar1 = *(void **)(GetClassJavaData + 0x20);
		if (pvVar1 != (void *)0x0) {
		  _Memory = pvVar1;
		  if ((0xfff < (*(longlong *)(GetClassJavaData + 0x30) - (longlong)pvVar1 & 0xfffffffffffffff8U)
			  ) && (_Memory = *(void **)((longlong)pvVar1 + -8),
				   0x1f < (ulonglong)((longlong)pvVar1 + (-8 - (longlong)_Memory)))) {
			_invalid_parameter_noinfo_noreturn();
			pcVar3 = (code *)swi(3);
			(*pcVar3)();
			return;
		  }
		  free(_Memory);
		  *(undefined8 *)(pcVar4 + 0x20) = 0;
		  *(undefined8 *)(pcVar4 + 0x28) = 0;
		  *(undefined8 *)(pcVar4 + 0x30) = 0;
		}
		free(pcVar4);
	  }
	  pvVar1 = DAT_2e409498;
	  GetClassJavaData = (char *)0x0;
	  if (DAT_2e409498 != (void *)0x0) {
		puVar2 = *(undefined2 **)((longlong)DAT_2e409498 + 0x10);
		*puVar2 = 0x3031;
		*(undefined *)(puVar2 + 1) = 0x31;
		*(undefined *)(*(longlong *)((longlong)pvVar1 + 0x10) + 3) = 0;
		send(*(SOCKET *)((longlong)pvVar1 + 8),*(char **)((longlong)pvVar1 + 0x10),3,0);
		send(*(SOCKET *)((longlong)pvVar1 + 8),"\n",1,0);
		MemSet(*(char **)((longlong)pvVar1 + 0x10),0,0x500000);
		shutdown(*(SOCKET *)((longlong)pvVar1 + 8),1);
		closesocket(*(SOCKET *)((longlong)pvVar1 + 8));
		WSACleanup();
		free(*(void **)((longlong)pvVar1 + 0x10));
		pvVar1 = DAT_2e409498;
		free(DAT_2e409498);
		if (pvVar1 != (void *)0x0) {
		  DAT_2e409498 = (void *)0x8123;
		}
	  }
	*/

	jclass clsSystem = env->FindClass("java/lang/System");
	if (clsSystem != NULL) {
		jmethodID midGC = env->GetStaticMethodID(clsSystem, "gc", "()V");
		if (midGC != NULL) {
			env->CallStaticVoidMethod(clsSystem, midGC);
		}
	}

	new_env->DeleteGlobalRef(app_ctx->objClassLoader);

	// TODO
	// for object in app_ctx : new_env->DeleteGlobalRef(object);

	vm->DetachCurrentThread();

	return 0;
}

// TODO deallocate 
DWORD WINAPI Cleanup(LPVOID) {

	JavaVM* vm;
	jvmtiEnv* jvmti_env;
	env->GetJavaVM(&vm);
	vm->GetEnv(reinterpret_cast<void**>(&jvmti_env), JVMTI_VERSION_1_1);

	jvmti_env->ForceGarbageCollection();

	jclass clsSystem = env->FindClass("java/lang/System");
	if (clsSystem != NULL) {
		jmethodID midGC = env->GetStaticMethodID(clsSystem, "gc", "()V");
		if (midGC != NULL) {
			env->CallStaticVoidMethod(clsSystem, midGC);
		}
	}

	// todo
	if (false) {
		HANDLE handle = GetModuleHandleA("jvm.dll");
	}

	// another JVM call here???

	CreateThread(NULL, 0, &FinalCleanup, NULL, 0, NULL);
	ExitThread(0);
}

/*-------------------------------------------------------
	Gets the Forge RenderHandler for FreeCam.

	TODO: global JNI context.
-------------------------------------------------------*/
jint GetForgeVersion(PAPP_CTX app_ctx) {

	jint version = app_ctx->forgeVersion;

	if (version == NULL) {
		jclass clsForgeVersion = LookupMinecraftClass(app_ctx, env, "net/minecraftforge/common/ForgeVersion");
		jfieldID fidMinorVersion = env->GetStaticFieldID(clsForgeVersion, "minorVersion", "I");
		version = env->GetStaticIntField(clsForgeVersion, fidMinorVersion);
		app_ctx->forgeVersion = version;

		std::cout << "Forge version is " << version << std::endl;

	}

	return version;
}

void GetMappingFromSocket() {

}

/*-------------------------------------------------------
	Sets up Forge context with JNI classes, fields & methods. 
	
	TODO: global JNI context.
-------------------------------------------------------*/
void SetupForgeEventListener(PFORGE_CTX forge_ctx, PAPP_CTX app_ctx) {
	jint forgeVersion = GetForgeVersion(app_ctx);

	if (forgeVersion > 13) {
		if (forgeVersion < 26) {
			forge_ctx->clsEvent = GetClassByName(app_ctx, env, "net/minecraftforge/fml/common/eventhandler/Event");
			forge_ctx->clsListenerList = GetClassByName(app_ctx, env, "net/minecraftforge/fml/common/eventhandler/ListenerList");
			forge_ctx->clsListenerListInstance = GetClassByName(app_ctx, env, "net/minecraftforge/fml/common/eventhandler/ListenerList$ListenerListInst");
			forge_ctx->clsEventPriority = GetClassByName(app_ctx, env, "net/minecraftforge/fml/common/eventhandler/EventPriority");

			forge_ctx->fidEventPriority = GetFieldByName(app_ctx, env, forge_ctx->clsEventPriority, "NORMAL", "Lnet/minecraftforge/fml/common/eventhandler/EventPriority;");

			forge_ctx->midGetListenerList = GetMethodByName(app_ctx, env, forge_ctx->clsEvent, "getListenerList", "()Lnet/minecraftforge/fml/common/eventhandler/ListenerList;");
			forge_ctx->midRegister = GetMethodByName(app_ctx, env, forge_ctx->clsListenerList, "register", "(ILnet/minecraftforge/fml/common/eventhandler/EventPriority;Lnet/minecraftforge/fml/common/eventhandler/IEventListener;)V");

			forge_ctx->fidLists = GetFieldByName(app_ctx, env, forge_ctx->clsListenerList, "lists", "[Lnet/minecraftforge/fml/common/eventhandler/ListenerList$ListenerListInst;");
			forge_ctx->fidAllLists = GetFieldByName(app_ctx, env, forge_ctx->clsListenerList, "allLists", "Lcom/google/common/collect/ImmutableList;");
		}
		else {
			forge_ctx->clsEvent = GetClassByName(app_ctx, env, "net/minecraftforge/eventbus/api/Event");
			forge_ctx->clsListenerList = GetClassByName(app_ctx, env, "net/minecraftforge/eventbus/ListenerList");
			forge_ctx->clsListenerListInstance = GetClassByName(app_ctx, env, "net/minecraftforge/eventbus/ListenerList$ListenerListInst");
			forge_ctx->clsEventPriority = GetClassByName(app_ctx, env, "net/minecraftforge/eventbus/api/EventPriority");

			forge_ctx->fidEventPriority = GetFieldByName(app_ctx, env, forge_ctx->clsEventPriority, "NORMAL", "Lnet/minecraftforge/eventbus/api/EventPriority;");

			forge_ctx->midGetListenerList = GetMethodByName(app_ctx, env, forge_ctx->clsEvent, "getListenerList", "()Lnet/minecraftforge/eventbus/ListenerList;");
			forge_ctx->midRegister = GetMethodByName(app_ctx, env, forge_ctx->clsListenerList, "register", "(ILnet/minecraftforge/eventbus/api/EventPriority;Lnet/minecraftforge/eventbus/api/IEventListener;)V");
		}
	}
	else {
		forge_ctx->clsEvent = GetClassByName(app_ctx, env, "cpw/mods/fml/common/eventhandler/Event");
		forge_ctx->clsListenerList = GetClassByName(app_ctx, env, "cpw/mods/fml/common/eventhandler/ListenerList");
		forge_ctx->clsListenerListInstance = GetClassByName(app_ctx, env, "net/minecraftforge/fml/common/eventhandler/ListenerList$ListenerListInst");
		forge_ctx->clsEventPriority = GetClassByName(app_ctx, env, "cpw/mods/fml/common/eventhandler/EventPriority");

		forge_ctx->fidEventPriority = GetFieldByName(app_ctx, env, forge_ctx->clsEventPriority, "NORMAL", "Lcpw/mods/fml/common/eventhandler/EventPriority;");

		forge_ctx->midGetListenerList = GetMethodByName(app_ctx, env, forge_ctx->clsEvent, "getListenerList", "()Lcpw/mods/fml/common/eventhandler/ListenerList;");
		forge_ctx->midRegister = GetMethodByName(app_ctx, env, forge_ctx->clsListenerList, "register", "(ILcpw/mods/fml/common/eventhandler/EventPriority;Lcpw/mods/fml/common/eventhandler/IEventListener;)V");

		forge_ctx->midUnregister = GetMethodByName(app_ctx, env, forge_ctx->clsListenerList, "unregisterAll", "(ILcpw/mods/fml/common/eventhandler/IEventListener;)V");

		forge_ctx->fidLists = GetFieldByName(app_ctx, env, forge_ctx->clsListenerList, "lists", "[Lcpw/mods/fml/common/eventhandler/ListenerList;");
		forge_ctx->fidAllLists = GetFieldByName(app_ctx, env, forge_ctx->clsListenerList, "allLists", "Lcom/google/common/collect/ImmutableList;");
	}

	forge_ctx->midBuildCache = GetMethodByName(app_ctx, env, forge_ctx->clsListenerListInstance, "buildCache", "()V");
}

jobject RENDERHANDLE;

const jbyte* G_CLASS_BYTES;

HWND windowHandle;
const char* windowSearchName;

// TODO
bool StuffWithForgeVersion(PAPP_CTX ctx, PJVM_CTX jvm, psocket_ctx socket) {

	if (!ctx->isVanilla)
		goto SHID;

	// TODO replace env with ctx->jvm_ctx->jni
	if ( !LookupMinecraftClass(ctx, env, "net/minecraftforge/common/ForgeVersion") ) {
		ctx->isVanilla = true;

		SHID:
		if (true) {
			jclass clsObject = env->FindClass("java/lang/Object");
			jmethodID midToString = env->GetMethodID(clsObject, "toString", "()Ljava/lang/String;");

			jclass referenceClass = env->FindClass("net/minecraft/client/main/Main");

			if (!referenceClass) {
				referenceClass = GetClassByName(ctx, env, "Laaa;");
			}

			jobject objClassLoader;

			// TODO get JVMTI env from ctx->jvm_ctx->jvmti
			JavaVM* vm;
			jvmtiEnv* jvmti_env;
			env->GetJavaVM(&vm);
			vm->GetEnv(reinterpret_cast<void**>(&jvmti_env), JVMTI_VERSION_1_1);

			jvmti_env->GetClassLoader(referenceClass, &objClassLoader);
			ctx->objClassLoader = env->NewGlobalRef(objClassLoader);
		}

		char* WindowName;
		/*char* SubStr;
		SubStr = "Minecraft 1.";
		EnumWindows(EnumProc, GetCurrentProcessId());
		GetWindowTextA(hWnd, String, 256);
		if (!String[0])
		{
			SubStr = " Client";
			EnumWindows(EnumProc, GetCurrentProcessId());
			GetWindowTextA(hWnd, String, 256);
		}*/

		if (strstr(WindowName, "1.7.10")) {
			ctx->forgeVersion = 13;
			//SendPacket(v14, v13, 400);
			//SendPacket(a3, v24, 200);
		}
		if (strstr(WindowName, "1.8.9")) {
			ctx->forgeVersion = 15;
			//SendPacket(v14, v13, 400);
			//SendPacket(a3, v24, 200);
		}
		if (strstr(WindowName, "1.12.2")) {
			ctx->forgeVersion = 23;
			//SendPacket(v14, v13, 400);
			//SendPacket(a3, v24, 200);
		}

		//SendPacket(socket, v13, 400);
		//SendPacket(socket, v36, 201);
		return 0;
	}

	//SendPacket(socket, v13, 400u);
	//SendPacket(socket, v27, 202u);
	//SendPacket(a1->socket, v28, qword_2E409848);

	return FALSE;
}

//void Vape_send_2(PSOCKET_CTX param_1, void* param_2, int param_3) {
//
//}

typedef struct jvm_ctx {
	JavaVM* vm;
	JNIEnv* jni;
	jvmtiEnv* jvmti;
} * pjvm_ctx;

typedef struct vape_main_ctx {
	pjvm_ctx jvm_ctx;
	jint forgeMinorVersion;
	jint loadedClassesCount;
	jclass* loadedClasses;
	std::map<int, void*> map; // forge version, something.
} *pvape_main_ctx;

pjvm_ctx GetJVMContext() {
	pjvm_ctx ctx{};

	jint vms;

	if (!ctx->vm)
		vms = JNI_GetCreatedJavaVMs(&ctx->vm, 1, NULL);
	
	if (vms && !ctx->vm)
		return NULL;

	JavaVMAttachArgs args{};

	args.version = JNI_VERSION_1_8;
	args.name = NULL;
	args.group = NULL;

	ctx->vm->AttachCurrentThread(reinterpret_cast<void**>(&ctx->jni), &args);
}

typedef struct socket_ctx {
	char* buffer;
	SOCKET socket;
	int key;
} *psocket_ctx;

// TODO: pull to DllMain
BOOL Initialize() {

	pjvm_ctx jvm = GetJVMContext();

	pvape_main_ctx ctx{};

	ctx->jvm_ctx = jvm;

	jvmtiEnv* jvmti_env = jvm->jvmti;

	if (!jvmti_env)
		jvm->vm->GetEnv(reinterpret_cast<void**>(&jvm->jvmti), JVMTI_VERSION_1_1);
	

	jvmti_env->GetLoadedClasses(&ctx->loadedClassesCount, &ctx->loadedClasses);

	psocket_ctx socket_ctx{};

	if (InitializeClassLoader(ctx)) {

		WSADATA data;
		WSAStartup(MAKEWORD(2, 2), &data);

		addrinfo hints{};
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		addrinfo* result;
		getaddrinfo("127.0.0.1", NULL, &hints, &result);

		SOCKET s;

		if (result) {
			while (s != INVALID_SOCKET) {
				s = ::socket(result->ai_family, result->ai_socktype, result->ai_protocol);
				socket_ctx->socket = s;
				if (connect(s, result->ai_addr, result->ai_addrlen) == INVALID_SOCKET)
				{
					closesocket(socket_ctx->socket);
					socket_ctx->socket = INVALID_SOCKET;
					result = result->ai_next;
					if (result) continue;
				}
				goto CLOSE_SOCKET;
			}
		}
		else {
		CLOSE_SOCKET:
			freeaddrinfo(result);
			s = socket_ctx->socket;
			if (s != INVALID_SOCKET)
			{
				const char opt = '\x01';
				setsockopt(socket_ctx->socket, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
			}
		}
	}

	if (StuffWithForgeVersion(ctx, jvm, socket_ctx)) {
		//SendPacket(qword_2E409498, v12, 300u);
		//v14 = SocketRecieve(v13);
		//v15 = atoi(v14);
		//free(v14);
		//v13[6] = v15; // set version in socket_ctx
		//SendPacket(v16, v17, 500u);

		//SetClasses(v25);
		//CallEntryPoint(v25);
		//while (((*Block->jni)->ExceptionCheck)(Block->jni))
		//	((*Block->jni)->ExceptionClear)(Block->jni);
		//while (v25->running)
		//	Sleep(0x32u);
		//CleanupAndUnregisterNatives(v25);
		//j_j_free(v25);
	}

	while (jvm->jni->ExceptionCheck())
		jvm->jni->ExceptionClear();

	if (jvm->vm && jvm->jni)
	{
		if (jvm->jvmti)
			jvm->jvmti->DisposeEnvironment();
		
		jvm->vm->DetachCurrentThread();
		jvm->vm = NULL;
		jvm->jni = NULL;
	}

	free(jvm);
	Sleep(1000);
	HANDLE Thread = CreateThread(NULL, 0, &Cleanup, NULL, 0, NULL);
	BOOL result = CloseHandle(Thread);

	return result;
}

typedef struct forge_version_struct {
	int forgeVersion;
	const char* name;
} *pforge_version_struct;

int InitializeClassLoader(pvape_main_ctx ctx) {

	forge_version_struct legacy{ 13, "1.7.10" };
	forge_version_struct eight{ 15, "1.8.9" };
	forge_version_struct twelve{ 23, "1.12.2" };
	forge_version_struct sixteen{ 34, "1.7.10" };

	// send all to socket

	//ctx->map.insert({1, ""});

	JNIEnv* jni = ctx->jvm_ctx->jni;

	jclass clsClassLoader = jni->FindClass("java/lang/ClassLoader");

	jmethodID midFindClass = jni->GetMethodID(clsClassLoader, 
		"findClass", 
		"(Ljava/lang/String;)Ljava/lang/Class;");

	jclass clsLaunch = jni->FindClass("net/minecraft/launchwrapper/Launch");

	const char* classLoaderClassName;

	if (clsLaunch) {
		classLoaderClassName = "net/minecraft/launchwrapper/LaunchClassLoader";
	}
	else {
		clsLaunch = jni->FindClass("cpw/mods/modlauncher/Launcher");
		classLoaderClassName = "cpw/mods/modlauncher/TransformingClassLoader";
	}

	jclass clsLoader = env->FindClass(classLoaderClassName);

	if (clsLoader) {
		// ctx->isForge = true; ?
		jfieldID fidClassLoader = jni->GetStaticFieldID(clsLoader, 
			"classLoader", 
			"Lnet/minecraft/launchwrapper/LaunchClassLoader;");
		jobject objClassLoader = jni->GetStaticObjectField(clsLoader, fidClassLoader);
		jni->NewGlobalRef(objClassLoader);
		midFindClass = jni->GetMethodID(clsLoader, "findClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	}
	else {
		// ctx->isVanilla = true;
	}

	return 1;
}

#define STR_REPLACE(str, ch, rp)\
char* buf;						\
int len = strlen(str);			\
int index = 0;					\
if (len) {						\
	buf = str;					\
	do {						\
		if (*buf == ch)			\
			*buf = rp;			\
		++buf;					\
	} while (len > ++index);	\
}								\

/**
 * Lookup a class using Minecraft's class loader.
*/
jclass LookupMinecraftClass(PAPP_CTX ctx, JNIEnv* env, const char* name) {

	jobject globClass;

	char formattedName[1024];
	sprintf(formattedName, "%s", name);

	// TODO: replace '.' in formattedName with '/' (somewhat properly)
	STR_REPLACE(formattedName, '.', '/');

	if (ctx->objClassLoader) 
	{
		globClass = env->NewGlobalRef(
			env->CallObjectMethod(ctx->objClassLoader, ctx->midFindClass, env->NewStringUTF(formattedName)));

		if (globClass)
			goto PROCESS;
	}

	if (env->ExceptionOccurred())
		env->ExceptionClear();

	globClass = env->NewGlobalRef(env->FindClass(formattedName));

	if (globClass) 
	{
	PROCESS:
		ctx->classes.insert(ctx->classes.begin(), globClass);
	}
	else if (env->ExceptionOccurred())
		env->ExceptionClear();

	return reinterpret_cast<jclass>(globClass);
}

jvmtiError RemoveClassFileLoadHook() 
{

	JavaVM* vm;
	jvmtiEnv* jvmti_env;
	env->GetJavaVM(&vm);
	vm->GetEnv(reinterpret_cast<void**>(&jvmti_env), JVMTI_VERSION_1_1);

	jvmtiEventCallbacks callbacks{};
	callbacks.ClassFileLoadHook = NULL;

	jvmti_env->SetEventCallbacks(&callbacks, sizeof(jvmtiEventCallbacks));

	return jvmti_env->SetEventNotificationMode(JVMTI_DISABLE, JVMTI_EVENT_CLASS_FILE_LOAD_HOOK, NULL);
}

jvmtiError SetClassFileLoadHook() 
{
	JavaVM* vm;
	jvmtiEnv* jvmti_env;
	env->GetJavaVM(&vm);
	vm->GetEnv(reinterpret_cast<void**>(&jvmti_env), JVMTI_VERSION_1_1);

	jvmtiCapabilities capabilities{};

	capabilities.can_generate_all_class_hook_events = 1;

	jvmti_env->GetCapabilities(&capabilities);

	jvmtiEventCallbacks callbacks{};

	(void)memset(&callbacks, 0, sizeof(callbacks));
	{
		callbacks.ClassFileLoadHook = [](jvmtiEnv*, JNIEnv* jni_env, jclass, jobject, const char* name, jobject, jint class_data_len, const unsigned char* class_data, jint*, unsigned char**)
		{
			//if (lastName != NULL && !strcmp(lastName, name)) {
			//	query_class = jni_env->NewByteArray(class_data_len);
			//	jni_env->SetByteArrayRegion(query_class, 0, class_data_len, (jbyte*)class_data);
			//	lastName = NULL;
			//}
		};
	}

	jvmti_env->SetEventCallbacks(&callbacks, sizeof(callbacks));

	return jvmti_env->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_CLASS_FILE_LOAD_HOOK, NULL);
}

jclass GetClassByName(PAPP_CTX ctx, JNIEnv* env, const char* name) {

	JavaVM* vm;
	jvmtiEnv* jvmti_env;
	env->GetJavaVM(&vm);
	vm->GetEnv(reinterpret_cast<void**>(&jvmti_env), JVMTI_VERSION_1_1);

	char nameStartChar = *name;

	char formattedName[2048];

	// check if ctx->jvm_ctx->jvmti is null.
	if (false) 
	{
		if (nameStartChar == 'L') {
			sprintf(formattedName, "%s", name + 1);
		}
		else sprintf(formattedName, "%s", name);
	
		if (strstr(formattedName, "net/minecraft")) {
			return LookupMinecraftClass(ctx, env, formattedName);
		}
		else return env->FindClass(formattedName);
		
	}

	jclass returnClass;

	const char* formatString;
	
	  if (nameStartChar == 'L' || (formatString = "L%s;", nameStartChar == '['))
		  formatString = "%s";

	sprintf(formattedName, "L%s;", name);

	if (strstr(formattedName, "net/minecraft") || ctx->isVanilla && (strstr(formattedName, "netty") || strstr(formattedName, "lwjgl"))) {
	}

	if (ctx->loadedClassesCount > 0) {
		for (int index = 0; index < ctx->loadedClassesCount; index++) {

			jclass loadedClass = ctx->loadedClasses[index];

			char* signature;
			jvmti_env->GetClassSignature(loadedClass, &signature, NULL);

			if (signature) {

				if (!strcmp(signature, formattedName)) {
					returnClass = loadedClass;
				}

				jvmti_env->Deallocate((unsigned char*)signature);
			}
		}

		if (returnClass) return returnClass;
	}

//	do {
//
//	} while (formattedName[0] != 'L');

//	if (formattedName[0] != '[') {
//
//	}

}

// TODO: 1.7.10 support.
// |- cpw/mods/modlauncher/TransformingClassLoader
void InitializeMinecraftClassLoaderContext(PAPP_CTX ctx) {


	// cpw/mods/modlauncher/Launcher
	jclass clsLaunch = env->FindClass("net/minecraft/launchwrapper/Launch");
	CHECK_AND_RAISE_JNI_EXCEPTION(env)

	// cpw/mods/modlauncher/TransformingClassLoader
	jclass clsLaunchClassLoader = env->FindClass("Lnet/minecraft/launchwrapper/LaunchClassLoader;");
	CHECK_AND_RAISE_JNI_EXCEPTION(env)

	jclass clsClassLoader = env->FindClass("java/lang/ClassLoader");
	CHECK_AND_RAISE_JNI_EXCEPTION(env)

	jfieldID fidClassLoader = env->GetStaticFieldID(clsLaunch, "classLoader", "Lnet/minecraft/launchwrapper/LaunchClassLoader;");
	CHECK_AND_RAISE_JNI_EXCEPTION(env)
		jmethodID midFindClass = env->GetMethodID(clsClassLoader, "findClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	CHECK_AND_RAISE_JNI_EXCEPTION(env)

	jobject objClassLoader = env->GetStaticObjectField(clsLaunch, fidClassLoader);
	CHECK_AND_RAISE_JNI_EXCEPTION(env)

	ctx->objClassLoader = env->NewGlobalRef(objClassLoader);
	ctx->midFindClass = midFindClass;

	//return TRUE;
}

/*-------------------------------------------------------
	Gets a field by it's name and signature from the specified class.
--------------------------------------------------------*/
jfieldID GetFieldByName(PAPP_CTX ctx, JNIEnv* env, jclass cls, const char* name, const char* signature) {

	jfieldID returnField;

	// check if JVMTI is available.
	if (true) {
		JavaVM* vm;
		jvmtiEnv* jvmti_env;
		env->GetJavaVM(&vm);
		vm->GetEnv(reinterpret_cast<void**>(&jvmti_env), JVMTI_VERSION_1_1);

		jint count;
		jfieldID* fields;

		jvmti_env->GetClassFields(cls, &count, &fields);

		if (count > 0) 
		{
			for (int index = 0; index < count; index++) 
			{

				char* fieldName;
				char* fieldSignature;

				jfieldID fieldID = fields[index];

				jvmti_env->GetFieldName(cls, fieldID, &fieldName, &fieldSignature, NULL);

				if (fieldSignature && fieldName) 
				{

					// does the field match the name and/or signature?
					if (!strstr(fieldName, name) && (!fieldSignature || strstr(fieldSignature, signature)))
						returnField = fieldID;
					
					jvmti_env->Deallocate((unsigned char*)fieldName);
					jvmti_env->Deallocate((unsigned char*)fieldSignature);
				}
			}

			if (!returnField && !(returnField = env->GetFieldID(cls, name, signature))) {
				returnField = env->GetStaticFieldID(cls, name, signature);
			}
		}
	}
	else {
		if (!(returnField = env->GetFieldID(cls, name, signature))) {
			returnField = env->GetStaticFieldID(cls, name, signature);
		}
	}

	return returnField;
}

void RemoveTacosShitQuoteOnQuoteAnticheat() {
	EnumWindows([](HWND hWnd, LPARAM lParam) -> BOOL {
		DWORD pid = NULL;
		GetWindowThreadProcessId(hWnd, &pid);
		char buffer[512];
		GetWindowTextA(hWnd, buffer, 512);
		if (strstr(strlwr(buffer), "clicker")) {
			SetWindowTextA(hWnd, "Legitimate Application");
		}
		free(buffer);
		return FALSE;
		}, NULL);
}

/*-------------------------------------------------------
	Gets a method by it's name and signature from the specified class.
-------------------------------------------------------*/
jmethodID GetMethodByName(PAPP_CTX ctx, JNIEnv* env, jclass cls, const char* name, const char* signature) {

	jmethodID returnMethod;

	// check if JVMTI is available.
	if (true) 
	{

		// Grab from ctx?
		JavaVM* vm;
		jvmtiEnv* jvmti_env;
		env->GetJavaVM(&vm);
		vm->GetEnv(reinterpret_cast<void**>(&jvmti_env), JVMTI_VERSION_1_1);

		jint count;
		jmethodID* methods;

		jvmti_env->GetClassMethods(cls, &count, &methods);

		if (count > 0) 
		{
			for (int index = 0; index < count; index++) 
			{

				char* methodName;
				char* methodSignature;

				jmethodID methodID = methods[index];

				jvmti_env->GetMethodName(methodID, &methodName, &methodSignature, NULL);

				if (methodName && methodSignature) {
					if (!strstr(methodName, name)) {
						if (!strstr(methodSignature, signature)) {
							returnMethod = methodID;
						}
					}
					jvmti_env->Deallocate((unsigned char*)methodName);
					jvmti_env->Deallocate((unsigned char*)methodSignature);
				}
			}
		}
	}
	else 
	{
		if (!(returnMethod = env->GetMethodID(cls, name, signature)))
		{
			returnMethod = env->GetStaticMethodID(cls, name, signature);
		}
	}

	return returnMethod;
}
//#include <WinUser.h>

void AttatchToJVM() {

	std::cout << "AttatchToJVM" << std::endl;

	jint result = NULL;

	if (vm == NULL) {
		result = JNI_GetCreatedJavaVMs(&vm, 1, NULL);
	}

	if (result != JNI_OK && vm == NULL) {
		return;
	}

	JavaVMAttachArgs args{};

	args.version = JNI_VERSION_1_8;
	args.name = const_cast<char*>("");
	args.group = NULL;

	vm->AttachCurrentThread(reinterpret_cast<void**>(&env), &args);

	/*
	  int iVar1;
	  JavaVM *ppJVar2;
	  undefined auStack72 [32];
	  undefined4 local_28 [2];
	  undefined1 *local_20;
	  undefined8 local_18;
	  ulonglong local_10;
  
	  local_10 = DAT_2e2d3480 ^ (ulonglong)auStack72;
	  ppJVar2 = (JavaVM *)operator_new(0x20);
	  iVar1 = 0;
	  *(undefined *)(ppJVar2 + 2) = 0;
	  ppJVar2[3] = (JavaVM)0x0;
	  if (PTR_DAT_2e409890 == (JavaVM *)0x0) {
		iVar1 = JNI_GetCreatedJavaVMs(&PTR_DAT_2e409890,1,0);
	  }
	  *ppJVar2 = (JavaVM)PTR_DAT_2e409890;
	  if ((iVar1 != 0) && (PTR_DAT_2e409890 == (JavaVM *)0x0)) {
		MeanExit(local_10 ^ (ulonglong)auStack72);
		return;
	  }
	  local_28[0] = 0x10008;
	  local_20 = &DAT_2e2bd3db;
	  local_18 = 0;
	  (*(*PTR_DAT_2e409890)->AttachCurrentThread)(PTR_DAT_2e409890,ppJVar2 + 1,local_28);
  */
}

void RegisterAClassNatives(jclass cls) {

	JNINativeMethod natives[] = {
		DEFINE_NATIVE_METHOD("exit", "(Z)V",									Exit),
		DEFINE_NATIVE_METHOD("dc",    "()V",									DiscordCallback),
		DEFINE_NATIVE_METHOD("fs",    "()V",									FS),
		DEFINE_NATIVE_METHOD("gc",    "(Ljava/lang/String;)Ljava/lang/Class;",  GetClass),
		DEFINE_NATIVE_METHOD("gt",    "(Ljava/lang/String;)[B",					GetTexture),
		DEFINE_NATIVE_METHOD("gk",    "()I",									GetKey),
		DEFINE_NATIVE_METHOD("gs",    "()Ljava/lang/String;",					GetSettings),
		DEFINE_NATIVE_METHOD("ss",    "(Ljava/lang/String;)V",					SaveSettings),
		DEFINE_NATIVE_METHOD("mb",	  "(I)V",									n_MessageBox),
		DEFINE_NATIVE_METHOD("gcj",	  "(Ljava/lang/String;)Ljava/lang/Class;",  GetClassJava),
		DEFINE_NATIVE_METHOD("gcs",   "(Ljava/lang/Class;)Ljava/lang/String;",  GetClassSignature),
		DEFINE_NATIVE_METHOD("cs",	  "(I)Ljava/lang/String;",					CopyString),
		DEFINE_NATIVE_METHOD("rl",    "()V",									Reload),
		DEFINE_NATIVE_METHOD("grh",   "()Ljava/lang/Object;",					GetRenderHandler),
		DEFINE_NATIVE_METHOD("cpy",   "(Ljava/lang/String;)V",					ClipboardCopy),
		DEFINE_NATIVE_METHOD("p",     "(Ljava/lang/String;)V",					Print),
		DEFINE_NATIVE_METHOD("gks",   "(I)S",									n_GetKeyState),
		DEFINE_NATIVE_METHOD("gco",   "(Ljava/lang/Class;)[Ljava/lang/Object;",	GetClassObjects),
		DEFINE_NATIVE_METHOD("trn",   "(DDD)[D",								Translate),
		DEFINE_NATIVE_METHOD("gcb",   "(Ljava/lang/Class;)[B",					GetClassBytes),
		DEFINE_NATIVE_METHOD("scb",   "(Ljava/lang/Class;[B)I",					SetClassBytes),
		DEFINE_NATIVE_METHOD("gcf",   "(Ljava/lang/Class;)[Ljava/lang/String;",	GetClassFields),
		DEFINE_NATIVE_METHOD("gcm",   "(Ljava/lang/Class;)[Ljava/lang/String;",	GetClassMethods),
		DEFINE_NATIVE_METHOD("ds",    "(ILjava/lang/String;DDI)I",				DrawString),
		DEFINE_NATIVE_METHOD("gsw",   "(ILjava/lang/String;)D",					GetStringWidth),
		DEFINE_NATIVE_METHOD("gsh",   "(ILjava/lang/String;)D",					GetStringHeight),
		DEFINE_NATIVE_METHOD("smd",   "(II)V",									SendMouseDown),
		DEFINE_NATIVE_METHOD("gp",    "(Ljava/lang/String;)Ljava/lang/String;",	GetProfile),
		DEFINE_NATIVE_METHOD("su",    "(Ljava/lang/String;)V",					SetUsername),
		DEFINE_NATIVE_METHOD("iv",    "()Z",									IsVanilla),
		DEFINE_NATIVE_METHOD("gvc",   "(Ljava/lang/String;)Ljava/lang/Class;",	GetVanillaClass),
		DEFINE_NATIVE_METHOD("gmv",   "()I",									GetMinorVersion),
		DEFINE_NATIVE_METHOD("rsc",   "()V",									RSC),
		DEFINE_NATIVE_METHOD("mf",    "(IILjava/lang/String;)I",				MakeFont),
		DEFINE_NATIVE_METHOD("updc",  "(Ljava/lang/String;Ljava/lang/String;)V",UpdateDiscord),
		DEFINE_NATIVE_METHOD("dsv2",  "(ILjava/lang/String;DDIF)I",				DrawStringV2),
		DEFINE_NATIVE_METHOD("gswv2", "(ILjava/lang/String;)D",					GetStringWidthV2),
		DEFINE_NATIVE_METHOD("gshv2", "(ILjava/lang/String;)D",					GetStringHeightV2),
		DEFINE_NATIVE_METHOD("mfv2",  "(IILjava/lang/String;)I",				MakeFontV2),
		DEFINE_NATIVE_METHOD("mvk",   "(II)I",									n_MapVirtualKey),
		DEFINE_NATIVE_METHOD("gkn",   "(J)Ljava/lang/String;",					GeyKeyName),
		DEFINE_NATIVE_METHOD("rs",    "(IDD)V",									RenderState)
	};

	env->RegisterNatives(cls, natives, sizeof(natives) / sizeof(JNINativeMethod));

	if (!app_ctx->isVanilla) {
		// TODO getting listener shit again?
	}
}

void RegisterBClassNatives(jclass cls) {

	JNINativeMethod natives[] = {
		DEFINE_NATIVE_METHOD("a",   "(ILjava/lang/Class;Ljava/lang/String;Ljava/lang/String;Z)V",  GetMethod),
		DEFINE_NATIVE_METHOD("b",   "(ILjava/lang/Class;Ljava/lang/String;Ljava/lang/String;Z)V",  GetMethodMapped),
		DEFINE_NATIVE_METHOD("c",   "(ILjava/lang/Object;[Ljava/lang/Object;)V",				   InvokeMethodVoid),
		DEFINE_NATIVE_METHOD("d",   "(ILjava/lang/Object;[Ljava/lang/Object;)Z",				   InvokeBooleanMethod),
		DEFINE_NATIVE_METHOD("e",   "(ILjava/lang/Object;[Ljava/lang/Object;)C",				   InvokeCharMethod),
		DEFINE_NATIVE_METHOD("f",   "(ILjava/lang/Object;[Ljava/lang/Object;)S",				   InvokeShortMethod),
		DEFINE_NATIVE_METHOD("g",   "(ILjava/lang/Object;[Ljava/lang/Object;)I",				   InvokeIntMethod),
		DEFINE_NATIVE_METHOD("h",   "(ILjava/lang/Object;[Ljava/lang/Object;)J",				   InvokeLongMethod),
		DEFINE_NATIVE_METHOD("i",   "(ILjava/lang/Object;[Ljava/lang/Object;)F",				   InvokeFloatMethod),
		DEFINE_NATIVE_METHOD("j",   "(ILjava/lang/Object;[Ljava/lang/Object;)D",				   InvokeDoubleMethod),
		DEFINE_NATIVE_METHOD("k",   "(ILjava/lang/Object;[Ljava/lang/Object;)Ljava/lang/Object;",  InvokeObjectMethod),
		DEFINE_NATIVE_METHOD("l",   "(ILjava/lang/Object;[Ljava/lang/Object;)[Z",				   InvokeBooleanArrayMethod),
		DEFINE_NATIVE_METHOD("m",   "(ILjava/lang/Object;[Ljava/lang/Object;)[C",				   InvokeCharArrayMethod),
		DEFINE_NATIVE_METHOD("n",   "(ILjava/lang/Object;[Ljava/lang/Object;)[S",				   InvokeShortArrayMethod),
		DEFINE_NATIVE_METHOD("o",   "(ILjava/lang/Object;[Ljava/lang/Object;)[I",				   InvokeIntArrayMethod),
		DEFINE_NATIVE_METHOD("p",   "(ILjava/lang/Object;[Ljava/lang/Object;)[J",				   InvokeLongArrayMethod),
		DEFINE_NATIVE_METHOD("q",   "(ILjava/lang/Object;[Ljava/lang/Object;)[F",				   InvokeFloatArrayMethod),
		DEFINE_NATIVE_METHOD("r",   "(ILjava/lang/Object;[Ljava/lang/Object;)[D",				   InvokeDoubleArrayMethod),
		DEFINE_NATIVE_METHOD("s",   "(ILjava/lang/Object;[Ljava/lang/Object;)[Ljava/lang/Object;", InvokeObjectArrayMethod),
		DEFINE_NATIVE_METHOD("t",   "(ILjava/lang/Class;Ljava/lang/String;Ljava/lang/String;Z)V",  GetField),
		DEFINE_NATIVE_METHOD("u",   "(ILjava/lang/Class;Ljava/lang/String;Ljava/lang/String;Z)V",  GetFieldMapped),
		DEFINE_NATIVE_METHOD("v",   "(ILjava/lang/Object;)Z",									   GetBooleanField),
		DEFINE_NATIVE_METHOD("w",   "(ILjava/lang/Object;)C",									   GetCharField),
		DEFINE_NATIVE_METHOD("x",   "(ILjava/lang/Object;)S",									   GetShortField),
		DEFINE_NATIVE_METHOD("y",   "(ILjava/lang/Object;)I",									   GetIntField),
		DEFINE_NATIVE_METHOD("z",   "(ILjava/lang/Object;)J",									   GetLongField),
		DEFINE_NATIVE_METHOD("aa",  "(ILjava/lang/Object;)F",									   GetFloatField),
		DEFINE_NATIVE_METHOD("bb",  "(ILjava/lang/Object;)D",									   GetDoubleField),
		DEFINE_NATIVE_METHOD("cc",  "(ILjava/lang/Object;)Ljava/lang/Object;",					   GetObjectField),
		DEFINE_NATIVE_METHOD("dd",  "(ILjava/lang/Object;)[Z",									   GetBooleanArrayField),
		DEFINE_NATIVE_METHOD("ee",  "(ILjava/lang/Object;)[C",									   GetCharArrayField),
		DEFINE_NATIVE_METHOD("ff",  "(ILjava/lang/Object;)[S",									   GetShortArrayField),
		DEFINE_NATIVE_METHOD("gg",  "(ILjava/lang/Object;)[I",									   GetIntArrayField),
		DEFINE_NATIVE_METHOD("hh",  "(ILjava/lang/Object;)[J",									   GetLongArrayField),
		DEFINE_NATIVE_METHOD("ii",  "(ILjava/lang/Object;)[F",									   GetFloatArrayField),
		DEFINE_NATIVE_METHOD("jj",  "(ILjava/lang/Object;)[D",									   GetDoubleArrayField),
		DEFINE_NATIVE_METHOD("kk",  "(ILjava/lang/Object;)[Ljava/lang/Object;",					   GetObjectArrayField),
		DEFINE_NATIVE_METHOD("ll",  "(ILjava/lang/Object;Z)V",									   SetBooleanField),
		DEFINE_NATIVE_METHOD("mm",  "(ILjava/lang/Object;C)V",									   SetCharField),
		DEFINE_NATIVE_METHOD("nn",  "(ILjava/lang/Object;S)V",									   SetShortField),
		DEFINE_NATIVE_METHOD("oo",  "(ILjava/lang/Object;I)V",									   SetIntField),
		DEFINE_NATIVE_METHOD("pp",  "(ILjava/lang/Object;J)V",									   SetLongField),
		DEFINE_NATIVE_METHOD("qq",  "(ILjava/lang/Object;F)V",									   SetFloatField),
		DEFINE_NATIVE_METHOD("rr",  "(ILjava/lang/Object;D)V",									   SetDoubleField),
		DEFINE_NATIVE_METHOD("ss",  "(ILjava/lang/Object;Ljava/lang/Object;)V",					   SetObjectField),
		DEFINE_NATIVE_METHOD("tt",  "(ILjava/lang/Object;[Z)V",									   SetBooleanArrayField),
		DEFINE_NATIVE_METHOD("uu",  "(ILjava/lang/Object;[C)V",									   SetCharArrayField),
		DEFINE_NATIVE_METHOD("vv",  "(ILjava/lang/Object;[S)V",									   SetShortArrayField),
		DEFINE_NATIVE_METHOD("ww",  "(ILjava/lang/Object;[I)V",									   SetIntArrayField),
		DEFINE_NATIVE_METHOD("xx",  "(ILjava/lang/Object;[J)V",									   SetLongArrayField),
		DEFINE_NATIVE_METHOD("yy",  "(ILjava/lang/Object;[F)V",									   SetFloatArrayField),
		DEFINE_NATIVE_METHOD("zz",  "(ILjava/lang/Object;[D)V",									   SetDoubleArrayField),
		DEFINE_NATIVE_METHOD("aaa", "(ILjava/lang/Object;[Ljava/lang/Object;)V",				   SetObjectArrayField),
		DEFINE_NATIVE_METHOD("bbb", "(ILjava/lang/Object;[Ljava/lang/Object;)V",				   InvokeNonVirtualVoidMethod),
		DEFINE_NATIVE_METHOD("ccc", "(ILjava/lang/Class;[Ljava/lang/Object;)Ljava/lang/Object;",   InvokeConstructor),
		DEFINE_NATIVE_METHOD("ddd", "(ILjava/lang/Object;[Ljava/lang/Object;)B",				   InvokeByteMethod),
		DEFINE_NATIVE_METHOD("eee", "(ILjava/lang/Object;[Ljava/lang/Object;)[B",				   InvokeByteArrayMethod),
		DEFINE_NATIVE_METHOD("fff", "(ILjava/lang/Object;)B",									   GetByteField),
		DEFINE_NATIVE_METHOD("ggg", "(ILjava/lang/Object;B)V",									   SetByteField),
		DEFINE_NATIVE_METHOD("hhh", "(ILjava/lang/Object;)[B",									   GetByteArrayField),
		DEFINE_NATIVE_METHOD("iii", "(ILjava/lang/Object;[B)V",									   SetByteArrayField),
		DEFINE_NATIVE_METHOD("gfn", "(I)Ljava/lang/String;",									   GetFieldName),
		DEFINE_NATIVE_METHOD("gmn", "(I)Ljava/lang/String;",									   GetMethodName),
	};

	env->RegisterNatives(cls, natives, sizeof(natives) / sizeof(JNINativeMethod));
}

#include <TlHelp32.h>

typedef struct threads {
	int threadsCount;

} *pthreads;

void SuspendAllThreads(DWORD** ptr_threads) {
	HANDLE snapshot = INVALID_HANDLE_VALUE;

	if (INVALID_HANDLE_VALUE != (snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, NULL)))
	{
		THREADENTRY32 entry = { 0 };
		PDWORD pids = NULL;
		SIZE_T pidCount = 0;

		entry.dwSize = sizeof(THREADENTRY32);

		if (Thread32First(snapshot, &entry))
		{
			while (Thread32Next(snapshot, &entry))
			{
				if (entry.dwSize >= 0x10
					&& entry.th32OwnerProcessID == GetCurrentProcessId()
					&& entry.th32ThreadID != GetCurrentThreadId())
				{
					
				}
			}
		}

		CloseHandle(snapshot);
	}

	int e = (THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT | THREAD_SET_CONTEXT | THREAD_QUERY_INFORMATION);
	int t = 0x5A;

	struct _CONTEXT ctx;

	for (int i = 0; i < sizeof(*ptr_threads) / sizeof(DWORD); i++) {
		HANDLE thread = OpenThread(THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT | THREAD_SET_CONTEXT | THREAD_QUERY_INFORMATION, NULL, *ptr_threads[i]);
		if (thread) {
			SuspendThread(thread);
			ctx.ContextFlags = CONTEXT_CONTROL;
			if (GetThreadContext(thread, &ctx)) {
				
			}
		}
	}

}

#define FORGE_1_7 13
#define FORGE_1_8 15
#define FORGE_1_12 26

jbyte* FORGE_1_7_10_HANDLER = {};
jbyte* FORGE_1_8_9_HANDLER = {};
jbyte* FORGE_1_12_2_HANDLER = {};


// todo 
jclass DefineForgeEventHandler(PAPP_CTX ctx) {

	jint forgeVersion = GetForgeVersion(ctx);

	jclass clsClassLoader = env->FindClass("java/lang/ClassLoader");
	jmethodID midDefineClass = env->GetMethodID(clsClassLoader, "defineClass", "([BII)Ljava/lang/Class;");

	jsize _Size;
	jbyte* pjVar5;

	if (forgeVersion == FORGE_1_7) {
		pjVar5 = FORGE_1_7_10_HANDLER;
		_Size = SizeofResource(NULL, FindResourceA(NULL, "", ""));
	}
	else {
		if (forgeVersion < FORGE_1_12) {
			pjVar5 = FORGE_1_8_9_HANDLER;
			_Size = SizeofResource(NULL, FindResourceA(NULL, "", ""));
		}
		else {
			pjVar5 = FORGE_1_12_2_HANDLER;
			_Size = SizeofResource(NULL, FindResourceA(NULL, "", ""));
		}
	}

	jbyteArray array = env->NewByteArray(_Size);
	env->SetByteArrayRegion(array, 0, _Size, reinterpret_cast<jbyte*>(pjVar5));
	jclass cls = (jclass)env->CallObjectMethod(ctx->objClassLoader, midDefineClass, array);

	env->DeleteLocalRef(array);

	// free();
}

#define GET_CONTSTRUCTOR(x) \
env->GetMethodID(x, "<init>", "()V")

void InitializeOrCallEntryPoint(PAPP_CTX ctx) {
	jmethodID midStart = env->GetStaticMethodID(ctx->clsMain, "start", "()V");
	RegisterAClassNatives(ctx->clsMain);
	RegisterBClassNatives(ctx->clsMappings);

	if (!app_ctx->isVanilla) {
		SetupForgeEventListener(forge_ctx, app_ctx);

		/* --------------------------------------------------------------------------------------------------------- */

		jint forgeVersion = GetForgeVersion(ctx);

		jclass clsClassLoader = env->FindClass("java/lang/ClassLoader");
		jmethodID midDefineClass = env->GetMethodID(clsClassLoader, "defineClass", "([BII)Ljava/lang/Class;");

		std::cout << "clsEventHandler, eeeeeek!" << std::endl;

		jclass clsEventHandler = [clsClassLoader, midDefineClass, forgeVersion, ctx]() -> jclass {

			jsize _Size;
			jbyte* pjVar5;

			if (forgeVersion == FORGE_1_7) {
				pjVar5 = FORGE_1_7_10_HANDLER;
				_Size = sizeof(FORGE_1_7_10_HANDLER) / sizeof(char);
			}
			else {
				if (forgeVersion < FORGE_1_12) {
					pjVar5 = FORGE_1_8_9_HANDLER;
					_Size = sizeof(FORGE_1_8_9_HANDLER) / sizeof(char);
				}
				else {
					pjVar5 = FORGE_1_12_2_HANDLER;
					_Size = sizeof(FORGE_1_12_2_HANDLER) / sizeof(char);
				}
			}

			jbyteArray array = env->NewByteArray(_Size);
			env->SetByteArrayRegion(array, 0, _Size, reinterpret_cast<jbyte*>(pjVar5));
			jclass cls = (jclass)env->CallObjectMethod(ctx->objClassLoader, midDefineClass, array);
			jclass globCls = (jclass) env->NewGlobalRef(cls);
			env->DeleteLocalRef(globCls);

			// -- ??? -- ??? -- ??? -- ??? -- ??? -- ??? -- ??? -- ???
			jbyte* buf = (jbyte*)malloc(_Size);
			pjVar5 = buf;
			while (_Size != 0) {
				_Size = _Size - 1;
				*pjVar5 = '\0';
				pjVar5 = pjVar5 + 1;
			}
			env->SetByteArrayRegion(array, 0, _Size, buf);
			env->DeleteLocalRef(array);
			free(buf);
			// -- ??? -- ??? -- ??? -- ??? -- ??? -- ??? -- ??? -- ???

			return globCls;
		}();

		jmethodID midEventHandlerInit = GET_CONTSTRUCTOR(clsEventHandler);

		auto reg = [clsEventHandler, midEventHandlerInit](JNIEnv* env, jclass cls_c, jclass cls, jint busId) {

			jclass clsEvent = (jclass) env->NewGlobalRef(cls);
			jobject objVapeListener = env->NewGlobalRef(env->NewObject(clsEventHandler, midEventHandlerInit));

			jmethodID midInit = GET_CONTSTRUCTOR(clsEvent);

			jobject objEvent = env->NewObject(clsEvent, midInit);
			jobject objListenerList = env->CallObjectMethod(objEvent, forge_ctx->midGetListenerList);
			jobject objEventPriority = env->GetStaticObjectField(forge_ctx->clsEventPriority, forge_ctx->fidEventPriority);

			env->CallVoidMethod(objListenerList, forge_ctx->midRegister, busId, objEventPriority, objVapeListener);

			// TODO more stuff here.

			// add clsEvent to array?

		};

		/* --------------------------------------------------------------------------------------------------------- */


		/* --------------------------------------------------------------------------------------------------------- */

		env->RegisterNatives(ctx->clsEventRegistry, new JNINativeMethod[] {
			DEFINE_NATIVE_METHOD("reg", "(Ljava/lang/Class;I)V", &reg)
		}, 1);

		jmethodID midCall = env->GetStaticMethodID(forge_ctx->clsVapeListener, "call", "(Ljava/lang/Object;)V");

		auto handler = [midCall](JNIEnv* env, jclass cls_c, jobject objEvent) {
			env->CallStaticObjectMethod(forge_ctx->clsVapeListener, midCall, objEvent);
		};

		env->RegisterNatives(clsEventHandler, new JNINativeMethod[]{
			DEFINE_NATIVE_METHOD("a", "(Ljava/lang/Object;)V", &handler)
		}, 1);

		/* --------------------------------------------------------------------------------------------------------- */

		if (forgeVersion < FORGE_1_12) {
			jclass clsRenderHandler = env->DefineClass("g", app_ctx->objClassLoader, G_CLASS_BYTES, sizeof(G_CLASS_BYTES) / sizeof(unsigned char*));
			RENDERHANDLE = env->NewGlobalRef(env->NewObject(clsRenderHandler, GET_CONTSTRUCTOR(clsRenderHandler)));
		}
	}

	// Possible mapping setup?

	env->CallStaticVoidMethod(ctx->clsMain, midStart);
}

LPVOID SOCKET_PORT;
bool finished;

typedef struct CLS_DEFINE_CTX {
	const char* name;
	const jbyte* buf;
	jsize len;
} * PCLS_DEFINE_CTX;

void SetClasses(PAPP_CTX ctx) {

	jobject objClassLoader;

	if (!ctx->isVanilla) {
		jclass clsSecureClassLoader = LookupMinecraftClass(ctx, env, "java/security/SecureClassLoader");
		if (env->ExceptionOccurred() != NULL) {
			env->ExceptionDescribe();
		}
		jmethodID midInit = env->GetMethodID(clsSecureClassLoader, "<init>", "(Ljava/lang/ClassLoader;)V");
		jobject objInstance = env->NewObject(clsSecureClassLoader, midInit, ctx->objClassLoader);
		jobject instance2 = env->NewObject(clsSecureClassLoader, midInit, objInstance);
		objClassLoader = env->NewGlobalRef(instance2);
	}
	else {
		objClassLoader = ctx->objClassLoader;
	}

	// ctx->something = objClassLoader;

	if (env->ExceptionOccurred() != NULL) {
		env->ExceptionDescribe();
	}

	ctx->clsMain = env->FindClass("a/a");
	ctx->clsMappings = env->FindClass("a/b");
	ctx->clsEventRegistry = env->FindClass("a/c");

	//int uVar4;

	////Vape_send_2(param_1->field_0x10);
	//Vape_send_2(param_1->field_0x10);
	//pcVar9 = SocketRecieve((socket_ctx*)param_1->field_0x10);
	//uVar4 = atoi(pcVar9);
	//free(pcVar9);

	/*Sleep(1000);

	if (0 < uVar4) {

	}

	PCLS_DEFINE_CTX c;
	jclass cls = env->DefineClass(c->name, objClassLoader, c->buf, c->len);
	if (!env->ExceptionCheck()) {
		if (!strstr(c->name, "a/a")) {
			ctx->clsMain = cls;
		}
		if (!strstr(c->name, "a/b")) {
			ctx->clsMappings = cls;
		}
		if (!strstr(c->name, "a/c")) {
			ctx->clsEventRegistry = cls;
		}
	}
	else {
		env->ExceptionClear();
	}*/

	// iterating through vector?

	// 

	// ctx->cls = env->NewGlobalRef(cls);

}



void Unregister(PAPP_CTX ctx) {

	env->UnregisterNatives(ctx->clsMain);
	env->UnregisterNatives(ctx->clsMappings);

	env->DeleteGlobalRef(RENDERHANDLE);

	//env->UnregisterNatives();
}

void DeleteForgeListeners(PFORGE_CTX ctx) {
	jfieldID fidListenerList;
	jint forgeVersion = GetForgeVersion(app_ctx);

	if (forgeVersion < FORGE_1_7 + 1) {
		fidListenerList = env->GetStaticFieldID(ctx->clsEvent, "LISTENER_LIST", "Lcpw/mods/fml/common/eventhandler/ListenerList;");
	}
	else if (forgeVersion < FORGE_1_12) {
		fidListenerList = env->GetStaticFieldID(ctx->clsEvent, "LISTENER_LIST", "Lnet/minecraftforge/fml/common/eventhandler/ListenerList;");
	}
	else {
		fidListenerList = env->GetStaticFieldID(ctx->clsEvent, "LISTENER_LIST", "Lnet/minecraftforge/eventbus/ListenerList;");
	}

	jobject objListenerList = env->GetStaticObjectField(ctx->clsEvent, fidListenerList);

	if (objListenerList) {
		jobjectArray objLists = (jobjectArray)env->GetObjectField(objListenerList, forge_ctx->fidLists);
		if (objLists) {
			jint countListeners = env->GetArrayLength(objLists);
			for (jint index = 0; index < countListeners; index++) {
				jobject objListener = env->GetObjectArrayElement(objLists, index);
				if (objListener)
					env->CallVoidMethod(objListener, forge_ctx->midBuildCache);
			}
		}
	}

	env->DeleteGlobalRef(ctx->clsEvent);
}


void SomeMoreEventShite(PFORGE_CTX forge_ctx) {
	// todo
	for (;;) {
		jobject listenerList = env->CallObjectMethod(NULL, forge_ctx->midGetListenerList);
		if (listenerList != NULL) {
			env->CallVoidMethod(listenerList, forge_ctx->midUnregister);
		}
	}

	DeleteForgeListeners(forge_ctx);
}

void MainInitialization() {

	SetWindowLongPtr(GetForegroundWindow(), GWLP_WNDPROC, (LONG_PTR)&[](HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT {
		JavaVM* vm;
		JNIEnv* new_env;
		env->GetJavaVM(&vm);

		if (vm->AttachCurrentThread(reinterpret_cast<void**>(&new_env), NULL) == JNI_OK) {
			if (!CallOnNotifiction(new_env, msg, wParam, lParam)) {
				return CallWindowProc(reinterpret_cast<WNDPROC>(prev), hWnd, msg, wParam, lParam);
			}
			else {
				return NULL;
			}
			vm->DetachCurrentThread();
		}
		else {
			std::cout << "[ERROR] WndProc's AttachCurrentThread failed." << std::endl;
		}
		return CallWindowProc(reinterpret_cast<WNDPROC>(prev), hWnd, msg, wParam, lParam);
	});

	std::cout << "MainInitialization" << std::endl;

	AttatchToJVM();

	APP_CTX ctx{};
	FORGE_CTX forge{};
	//SOCKET_CTX

	app_ctx = &ctx;
	forge_ctx = &forge;

	JavaVM* vm;
	jvmtiEnv* jvmti_env;
	env->GetJavaVM(&vm);
	vm->GetEnv(reinterpret_cast<void**>(&jvmti_env), JVMTI_VERSION_1_1);

	jvmti_env->GetLoadedClasses(&ctx.loadedClassesCount, &ctx.loadedClasses);

	InitializeMinecraftClassLoaderContext(&ctx);

	// SocketOpen


	bool res = StuffWithForgeVersion(&ctx);

	(&ctx)->running = true;

	SetClasses(&ctx);
	InitializeOrCallEntryPoint(&ctx);
	while (env->ExceptionOccurred()) 
		env->ExceptionClear();

	bool running = (&ctx)->running;
	while (running) {
		Sleep(50);
		running = (&ctx)->running;
	}
	Unregister(&ctx);

	Sleep(1000);
	HANDLE hObject = CreateThread(NULL, 0, &Cleanup, NULL, 0, NULL);
	CloseHandle(hObject);
	finished = 1;
}

void CallInitializationAndWaitForFinish(LPVOID args) {

	std::cout << "CallInitializationAndWaitForFinish" << std::endl;


	if (args != 0) {
		SOCKET_PORT = args;
	}
	MainInitialization();
	while (!finished) {
		Sleep(100);
	}
}

DWORD WINAPI StartThread(LPVOID args) {
	// locks?

	std::cout << "StartThread" << std::endl;


	CallInitializationAndWaitForFinish(args);
	FreeLibraryAndExitThread(hInst, 0);
}

extern "C" __declspec(dllexport) BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		HANDLE tHandle = CreateThread(NULL, 0, &StartThread, lpvReserved, 0, NULL);
		CloseHandle(tHandle);
		hInst = hinstDLL;
		break;
	}
	return TRUE;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
	return JNI_VERSION_1_8;
}