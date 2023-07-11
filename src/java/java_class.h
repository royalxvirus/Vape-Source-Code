#pragma once

#include <jni.h>
//#include <string>
//#include <vector>

#define DEFINE_NATIVE_METHOD(name, signature, fn) \
{const_cast<char*>(name), const_cast<char*>(signature), (void*)fn}

/*using namespace std;

class JavaClass {

	public:
		string className;
		~JavaClass() {
			
		}

	protected:
		vector<JNINativeMethod> methods;

		void RegisterNatives(JNIEnv* env) {
			jclass cls = env->FindClass(className.c_str());
			env->RegisterNatives(cls, methods.data(), methods.size());
		}
};*/