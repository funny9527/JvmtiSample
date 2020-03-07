#include <jni.h>
#include "log.h"
#include <stdlib.h>
#include <string>
#include <vector>
#include <memory.h>
#include "include/jvmti.h"
#include "jvmti_helper.h"
#include <pthread.h>
#include <zconf.h>
#include <dlfcn.h>
#include <fstream>
#include "init.h"


void JNICALL OnClassFileLoaded(jvmtiEnv* jvmti_env, JNIEnv* jni_env,
                               jclass class_being_redefined, jobject loader,
                               const char* name, jobject protection_domain,
                               jint class_data_len,
                               const unsigned char* class_data,
                               jint* new_class_data_len,
                               unsigned char** new_class_data) {
    LOGD("OnClassFileLoaded %s", name);
}

void JNICALL OnClassPrepare(jvmtiEnv* jvmti_env, JNIEnv* jni_env,
                            jthread thread, jclass klass) {
    LOGD("OnClassPrepare");
}

void ProfilerInitializationWorker(jvmtiEnv* jvmti, JNIEnv* jni_env, void* ptr) {
    LOGD("worker inited");
}

void JNICALL methodEnter(jvmtiEnv *jvmti_env, JNIEnv* jni_env, jthread thread, jmethodID method) {
    jclass cls;
    jvmti_env->GetMethodDeclaringClass(method, &cls);
    char *cls_name;
    jvmti_env->GetClassSignature(cls, &cls_name, NULL);

    jvmtiThreadInfo info;
    jvmti_env->GetThreadInfo(thread, &info);

    char *m_name;
    char *m_signature;
    jvmti_env->GetMethodName(method, &m_name, &m_signature, NULL);
    LOGD("methodEnter %s %s %s", cls_name, info.name, m_name);
}

void JNICALL methodExit(jvmtiEnv *jvmti_env, JNIEnv* jni_env, jthread thread, jmethodID method,
        jboolean was_popped_by_exception, jvalue return_value) {
    jclass cls;
    jvmti_env->GetMethodDeclaringClass(method, &cls);
    char *cls_name;
    jvmti_env->GetClassSignature(cls, &cls_name, NULL);

    jvmtiThreadInfo info;
    jvmti_env->GetThreadInfo(thread, &info);

    char *m_name;
    char *m_signature;
    jvmti_env->GetMethodName(method, &m_name, &m_signature, NULL);
    LOGD("methodExit %s %s %s", cls_name, info.name, m_name);
}

void JNICALL objectAlloc(jvmtiEnv *jvmti_env, JNIEnv* jni_env, jthread thread, jobject object,
        jclass object_klass, jlong size) {
    LOGD("objectAlloc");
}

void JNICALL objectFree(jvmtiEnv *jvmti_env, jlong tag) {
    LOGD("objectFree");
}

void JNICALL theadStart(jvmtiEnv *jvmti_env, JNIEnv* jni_env, jthread thread) {
    jvmtiThreadInfo info;
    jvmti_env->GetThreadInfo(thread, &info);
    LOGD("theadStart");
}

void JNICALL threadEnd(jvmtiEnv *jvmti_env, JNIEnv* jni_env, jthread thread) {
    jvmtiThreadInfo info;
    jvmti_env->GetThreadInfo(thread, &info);
    LOGD("threadEnd");
}

void JNICALL classLoad(jvmtiEnv *jvmti_env, JNIEnv* jni_env, jthread thread, jclass klass) {
    LOGD("classLoad");
}

void JNICALL garbageCoStart(jvmtiEnv *jvmti_env) {
    LOGD("garbageCoStart");
}

void JNICALL garbageCoEnd(jvmtiEnv *jvmti_env) {
    LOGD("garbageCoEnd");
}

void JNICALL fieldAccess(jvmtiEnv *jvmti_env, JNIEnv* jni_env, jthread thread,
        jmethodID method, jlocation location, jclass field_klass, jobject object, jfieldID field) {
    LOGD("fieldAccess");
}

void JNICALL fieldModify(jvmtiEnv *jvmti_env, JNIEnv* jni_env, jthread thread, jmethodID method, jlocation location,
        jclass field_klass, jobject object, jfieldID field, char signature_type, jvalue new_value) {
    LOGD("fieldModify");
}

void JNICALL eventMonitorContendedEnter(jvmtiEnv *jvmti_env, JNIEnv* jni_env, jthread thread, jobject object) {
    LOGD("eventMonitorContendedEnter");
}

void JNICALL eventMonitorContendedEntered(jvmtiEnv *jvmti_env, JNIEnv* jni_env, jthread thread, jobject object) {
    LOGD("eventMonitorContendedEntered");
}

void JNICALL eventMonitorWait(jvmtiEnv *jvmti_env, JNIEnv* jni_env, jthread thread, jobject object, jlong timeout) {
    LOGD("eventMonitorWait");
}

void JNICALL eventMonitorWaited(jvmtiEnv *jvmti_env, JNIEnv* jni_env, jthread thread, jobject object, jboolean timeout) {
    LOGD("eventMonitorWaited");
}


void checkCapability(jvmtiEnv* jvmti_env) {
    jvmtiCapabilities capa;
    jvmti_env->GetCapabilities(&capa);

    jvmtiEventCallbacks callbacks;
    memset(&callbacks, 0, sizeof(callbacks));

    if (capa.can_generate_all_class_hook_events) {
        callbacks.ClassFileLoadHook = OnClassFileLoaded;
        profiler::SetEventNotification(jvmti_env, JVMTI_ENABLE, JVMTI_EVENT_CLASS_FILE_LOAD_HOOK);
    } else {
        LOGD("not support class hook event");
    }

    if (capa.can_retransform_classes) {
        callbacks.ClassPrepare = OnClassPrepare;
        profiler::SetEventNotification(jvmti_env, JVMTI_ENABLE, JVMTI_EVENT_CLASS_PREPARE);
    } else {
        LOGD("not support class retransform");
    }

    if (capa.can_generate_method_entry_events) {
        callbacks.MethodEntry = methodEnter;
        profiler::SetEventNotification(jvmti_env, JVMTI_ENABLE, JVMTI_EVENT_METHOD_ENTRY);
    } else {
        LOGD("not support method entry event");
    }

    if (capa.can_generate_method_exit_events) {
        callbacks.MethodExit = methodExit;
        profiler::SetEventNotification(jvmti_env, JVMTI_ENABLE, JVMTI_EVENT_METHOD_EXIT);
    } else {
        LOGD("not support method exit event");
    }

    callbacks.ThreadStart = theadStart;
    profiler::SetEventNotification(jvmti_env, JVMTI_ENABLE, JVMTI_EVENT_THREAD_START);
    callbacks.ThreadEnd = threadEnd;
    profiler::SetEventNotification(jvmti_env, JVMTI_ENABLE, JVMTI_EVENT_THREAD_END);

    profiler::CheckJvmtiError(jvmti_env,
                              jvmti_env->SetEventCallbacks(&callbacks, sizeof(callbacks)), std::string("SetEventCallbacks"));
}


extern "C" JNIEXPORT jint JNICALL Agent_OnAttach(JavaVM* vm, char* options,
                                                 void* reserved) {
    LOGD("Agent_OnAttach");
    jvmtiEnv* jvmti_env = profiler::CreateJvmtiEnv(vm);
    if (jvmti_env == nullptr) {
        return JNI_ERR;
    }

    if (options == nullptr) {
        LOGD("Config file parameter was not specified");
        return JNI_ERR;
    }

    profiler::SetAllCapabilities(jvmti_env);

    JNIEnv* jni_env = profiler::GetThreadLocalJNI(vm);

    checkCapability(jvmti_env);

    jint count;
    jclass* loadedCls;
    jvmti_env->GetLoadedClasses(&count, &loadedCls);

    LOGD("loaded count %d", count);
    for (int i = 0; i < count; i++) {
        char* sig;
        jvmti_env->GetClassSignature(loadedCls[i], &sig, NULL);
        LOGD("loaded %s", sig);
    }

    jvmti_env->RunAgentThread(profiler::AllocateJavaThread(jvmti_env, jni_env),
                              &ProfilerInitializationWorker, "",
                              JVMTI_THREAD_NORM_PRIORITY);

    return JNI_OK;
}