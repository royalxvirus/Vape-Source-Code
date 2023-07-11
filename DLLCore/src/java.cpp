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

#include "java.h"

PJVM_CTX g_jvmCtx;

// todo use g_jvmCtx and initialize ctx lmfao.
PJVM_CTX
GetJVMContext()
{
	PJVM_CTX ctx = new JVM_CTX();

	jint vms = 0;

	if (!ctx->vm)
		vms = JNI_GetCreatedJavaVMs(&ctx->vm, 1, NULL);

	if (vms && !ctx->vm)
		return NULL;

	g_jvmCtx = ctx;

	JavaVMAttachArgs args{};

	args.version = JNI_VERSION_1_8;
	args.name = NULL;
	args.group = NULL;

	ctx->vm->AttachCurrentThread(reinterpret_cast<void**>(&ctx->jni), &args);

	return ctx;
}

jfieldID
GetFieldByName(PJVM_CTX ctx, jclass cls, const char* name, const char* signature)
{

	jfieldID returnField = NULL;

	if (ctx->jvmti) 
	{
		ctx->vm->GetEnv(reinterpret_cast<void**>(&ctx->jvmti), JVMTI_VERSION_1_1);

		jint count;
		jfieldID* fields;

		ctx->jvmti->GetClassFields(cls, &count, &fields);

		if (count > 0)
		{
			for (jint index = 0; index < count; index++)
			{

				char* fieldName;
				char* fieldSignature;

				jfieldID fieldID = fields[index];

				ctx->jvmti->GetFieldName(cls, fieldID, &fieldName, &fieldSignature, NULL);

				if (fieldSignature && fieldName)
				{
					// does the field match the name and/or signature?
					if (!strstr(fieldName, name) && (!fieldSignature || strstr(fieldSignature, signature)))
						returnField = fieldID;

					ctx->jvmti->Deallocate((unsigned char*)fieldName);
					ctx->jvmti->Deallocate((unsigned char*)fieldSignature);
				}
			}
		}

		// if we can't find the field, use JNI as a last resort.
		if (!returnField && !(returnField = ctx->jni->GetFieldID(cls, name, signature)))
			returnField = ctx->jni->GetStaticFieldID(cls, name, signature);

		ctx->jvmti->Deallocate((unsigned char*)fields);
	}
	else {
		if (!(returnField = ctx->jni->GetFieldID(cls, name, signature)))
			returnField = ctx->jni->GetStaticFieldID(cls, name, signature);
	}

	return returnField;
}

jmethodID
GetMethodByName(PJVM_CTX ctx, jclass cls, const char* name, const char* signature)
{
	jmethodID returnMethod = NULL;

	if (ctx->jvmti)
	{
		ctx->vm->GetEnv(reinterpret_cast<void**>(&ctx->jvmti), JVMTI_VERSION_1_1);

		jint count;
		jmethodID* methods;

		ctx->jvmti->GetClassMethods(cls, &count, &methods);

		if (count > 0)
		{
			for (jint index = 0; index < count; index++)
			{

				char* methodName;
				char* methodSignature;

				jmethodID methodID = methods[index];

				ctx->jvmti->GetMethodName(methodID, &methodName, &methodSignature, NULL);

				if (methodName && methodSignature) 
				{
					// does the method match the name and signature?
					if (!strstr(methodName, name) && !strstr(methodSignature, signature))
						returnMethod = methodID;

					ctx->jvmti->Deallocate((unsigned char*)methodName);
					ctx->jvmti->Deallocate((unsigned char*)methodSignature);
				}
			}
		}

		ctx->jvmti->Deallocate((unsigned char*)methods);

		// if we can't find the method, use JNI as a last resort.
		if (!returnMethod && !(returnMethod = ctx->jni->GetMethodID(cls, name, signature)))
			returnMethod = ctx->jni->GetStaticMethodID(cls, name, signature);
	}
	else
	{
		if (!(returnMethod = ctx->jni->GetMethodID(cls, name, signature)))
			returnMethod = ctx->jni->GetStaticMethodID(cls, name, signature);
	}

	return returnMethod;
}

jvmtiError
EnableClassFileLoadHook() 
{
	PJVM_CTX ctx = GetJVMContext();

	if (!ctx->jvmti)
		ctx->vm->GetEnv(reinterpret_cast<void**>(&ctx->jvmti), JVMTI_VERSION_1_1);

	jvmtiCapabilities caps;
	caps.can_generate_all_class_hook_events = 1;

	ctx->jvmti->AddCapabilities(&caps);

	return ctx->jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_CLASS_FILE_LOAD_HOOK, NULL);
}

jvmtiError
DisableClassFileLoadHook()
{
	PJVM_CTX ctx = GetJVMContext();

	if (!ctx->jvmti)
		ctx->vm->GetEnv(reinterpret_cast<void**>(&ctx->jvmti), JVMTI_VERSION_1_1);

	jvmtiEventCallbacks callbacks{};
	callbacks.ClassFileLoadHook = NULL;

	ctx->jvmti->SetEventCallbacks(&callbacks, sizeof(jvmtiEventCallbacks));

	return ctx->jvmti->SetEventNotificationMode(JVMTI_DISABLE, JVMTI_EVENT_CLASS_FILE_LOAD_HOOK, NULL);
}