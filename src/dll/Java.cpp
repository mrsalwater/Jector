#include "Java.hpp"

BOOL AttachJVM(JContext *jContext) {
    HMODULE hJVM = GetModuleHandle("jvm.dll");
    if (hJVM == nullptr) {
        return FALSE;
    }

    auto JNI_GetCreatedJavaVMs = (fnJNI_GetCreatedJavaVMs) GetProcAddress(hJVM, "JNI_GetCreatedJavaVMs");
    if (JNI_GetCreatedJavaVMs == nullptr) {
        return FALSE;
    }

    if (JNI_GetCreatedJavaVMs(&jContext->jvm, 1, nullptr) != JNI_OK) {
        return FALSE;
    }

    if (jContext->jvm->AttachCurrentThread((void **) &jContext->jniEnv, nullptr) != JNI_OK) {
        return FALSE;
    }

    return TRUE;
}

BOOL DetachJVM(JContext *jContext) {
    if (jContext->jvm && jContext->jniEnv) {
        if (jContext->jvm->DetachCurrentThread() != JNI_OK) {
            return FALSE;
        }
    }

    jContext->jniEnv = nullptr;
    jContext->jvm = nullptr;

    return TRUE;
}

int InjectJar(JContext *jContext, const char *jarPath, const char *className, const char *methodName, const char *methodSignature) {
    jContext->jniEnv->PushLocalFrame(100);

    jclass jcFile = jContext->jniEnv->FindClass("java/io/File");
    jmethodID jmidFileConstructor = jContext->jniEnv->GetMethodID(jcFile, "<init>", "(Ljava/lang/String;)V");
    jmethodID jmidFileToURI = jContext->jniEnv->GetMethodID(jcFile, "toURI", "()Ljava/net/URI;");

    jclass jcURI = jContext->jniEnv->FindClass("java/net/URI");
    jmethodID jmidURIToURL = jContext->jniEnv->GetMethodID(jcURI, "toURL", "()Ljava/net/URL;");
    jclass jcURL = jContext->jniEnv->FindClass("java/net/URL");

    jclass jcURLClassLoader = jContext->jniEnv->FindClass("java/net/URLClassLoader");
    jmethodID jmidURLClassLoaderConstructor = jContext->jniEnv->GetMethodID(jcURLClassLoader, "<init>","([Ljava/net/URL;)V");
    jmethodID jmidURLClassLoaderLoadClass = jContext->jniEnv->GetMethodID(jcURLClassLoader, "loadClass","(Ljava/lang/String;)Ljava/lang/Class;");

    jstring jsPath = jContext->jniEnv->NewStringUTF(jarPath);
    jobject joFile = jContext->jniEnv->NewObject(jcFile, jmidFileConstructor, jsPath);
    if (joFile == nullptr) return INJ_EJARPATH;
    jobject joURI = jContext->jniEnv->CallObjectMethod(joFile, jmidFileToURI);

    jobject joURL = jContext->jniEnv->CallObjectMethod(joURI, jmidURIToURL);
    jobjectArray joaURL = jContext->jniEnv->NewObjectArray(1, jcURL, joURL);
    jobject joURLClassLoader = jContext->jniEnv->NewObject(jcURLClassLoader, jmidURLClassLoaderConstructor, joaURL);

    jstring jsName = jContext->jniEnv->NewStringUTF(className);
    jobject joClass = jContext->jniEnv->CallObjectMethod(joURLClassLoader, jmidURLClassLoaderLoadClass, jsName);
    if (joClass == nullptr) return INJ_EClASSNAME;
    jobject joClassReference = jContext->jniEnv->NewGlobalRef(joClass);

    jContext->jniEnv->PopLocalFrame(nullptr);

    jmethodID jmidEntry = jContext->jniEnv->GetStaticMethodID((jclass) joClassReference, methodName, methodSignature);
    if (jmidEntry == nullptr) return INJ_EMETHOD;
    jContext->jniEnv->CallStaticVoidMethod((jclass) joClassReference, jmidEntry);

    return INJ_OK;
}
