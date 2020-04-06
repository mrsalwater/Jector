#ifndef JECTOR_DLL_JAVA_HPP
#define JECTOR_DLL_JAVA_HPP

#include <Windows.h>
#include <jni.h>

#define INJ_OK            0
#define INJ_EJARPATH      (-1)
#define INJ_EClASSNAME    (-2)
#define INJ_EMETHOD       (-3)

typedef jint(JNICALL *fnJNI_GetCreatedJavaVMs)(JavaVM **, jsize, jsize *);

struct JContext {
    JavaVM *jvm;
    JNIEnv *jniEnv;
};

BOOL AttachJVM(JContext *jContext);

BOOL DetachJVM(JContext *jContext);

int InjectJar(JContext *jContext, const char *jarPath, const char *className, const char *methodName, const char *methodSignature);

#endif //JECTOR_DLL_JAVA_HPP
