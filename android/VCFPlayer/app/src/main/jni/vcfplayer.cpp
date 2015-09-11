
/*
    Copyright 2015 Intel Corporation All Rights Reserved.

    The source code, information and material ("Material") contained herein is
	owned by Intel Corporation or its suppliers or licensors, and title to such
	Material remains with Intel Corporation or its suppliers or licensors.
	The Material contains proprietary information of Intel or its suppliers
	and licensors. The Material is protected by worldwide copyright laws and
	treaty provisions. No part of the Material may be used, copied, reproduced,
	modified, published, uploaded, posted, transmitted, distributed or
	disclosed in any way without Intel's prior express written permission.

	No license under any patent, copyright or other intellectual property rights
	in the Material is granted to or conferred upon you, either expressly,
	by implication, inducement, estoppel or otherwise. Any license under such
	intellectual property rights must be express and approved by Intel in writing.

	Unless otherwise agreed by Intel in writing, you may not remove or alter this
	notice or any other notice embedded in Materials by Intel or Intel's
	suppliers or licensors in any way.
*/
#include <jni.h>

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <string>

#include <android/native_window.h>
#include <android/native_window_jni.h>

#include "native_android_logging.h"
#include "vcf_graph_runner.h"

#define TAG "vcfplayer"
static jfieldID context_field_id;
static pthread_key_t tls_key; // Thread-local storage key
static pthread_t async_thread;
static JavaVM *java_vm;

typedef struct _DecodeCtx {
    jobject appObj;                       // handle to the Class Object of the Application
    ANativeWindow *native_window;         // handle to the App's render view
    VCFGraphRunner *vcfGraphRunner;
    std::string nativeLibDir;
} DecodeCtx;

static JNIEnv *attach_thread(void)
{
    LOGV("attach_thread");
    JNIEnv *env;
    JavaVMAttachArgs args;

    args.version = JNI_VERSION_1_6;
    args.name = NULL;
    args.group = NULL;

    if (java_vm->AttachCurrentThread(&env, &args) < 0) return NULL;

    return env;
}

static void detach_thread (void *env)
{
    LOGV("detach_thread");
    java_vm->DetachCurrentThread();
}

static JNIEnv *get_jni_env (void)
{
    LOGV("get_jni_env");
    JNIEnv *env;

    if ((env = (JNIEnv*)pthread_getspecific(tls_key)) == NULL)
    {
        env = attach_thread();
        pthread_setspecific(tls_key, env);
    }
    return env;
}

// Initialize the jni lib
jboolean native_static_init(JNIEnv *env, jclass clazz)
{
    LOGV("native_static_init");

    // get id of the handle to native context in the application activity class
    context_field_id = env->GetFieldID(clazz,"native_context","J");

    if(!context_field_id)
    {
        LOGE("native_static_init failed - GetFieldID");
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

jboolean native_init(JNIEnv *env, jobject thiz, jstring nativeLibraryDir)
{
    LOGV("native_init");
    // Create a Decode Context
    DecodeCtx * pCtx = new DecodeCtx;
    if(!pCtx)
    {
        LOGE("native_init failed - cannot allocate jni context");
        return JNI_FALSE;
    }

    const char *tmp = env->GetStringUTFChars(nativeLibraryDir, NULL);
    pCtx->nativeLibDir = tmp;
    env->ReleaseStringUTFChars(nativeLibraryDir, tmp);

    pCtx->native_window = NULL;                 // native_window will be filled once the available
    pCtx->vcfGraphRunner = NULL;
    pCtx->appObj = env->NewGlobalRef(thiz);     // set the handle to Class object of the Application
    LOGV("Lib Dir = %s", pCtx->nativeLibDir.c_str());
    env->SetLongField(thiz, context_field_id, (jlong)pCtx); // send the Decode Context to the Java App, and store as a native_context

    return JNI_TRUE;
}
void native_close(JNIEnv *env, jobject thiz)
{
    LOGV("native_close");
    DecodeCtx * pCtx = (DecodeCtx*) (jint)env->GetLongField(thiz, context_field_id);

    if(!pCtx)
    {
        LOGE("native_close failed - cannot get the decode context");
        return;
    }

    if(async_thread)
    {
        pthread_join(async_thread, NULL);
    }

    if(pCtx->vcfGraphRunner)
    {
        delete pCtx->vcfGraphRunner;
        pCtx->vcfGraphRunner = NULL;
    }
    if(pCtx->appObj)
    {
        env->DeleteGlobalRef(pCtx->appObj);
    }
    delete pCtx;
}
void native_surface_init(JNIEnv *env, jobject thiz, jobject surface)
{
    LOGV("native_surface_init");
    DecodeCtx * pCtx = (DecodeCtx*) (jint)env->GetLongField(thiz, context_field_id);

    if(!pCtx)
    {
        LOGE("native_surface_init failed - cannot get the decode context");
        return;
    }

    ANativeWindow *new_native_window = ANativeWindow_fromSurface(env, surface);


    if(pCtx->native_window)
    {
        ANativeWindow_release(pCtx->native_window);
        pCtx->native_window = NULL;
    }

    pCtx->native_window = new_native_window;
}
void native_surface_finalize(JNIEnv* env, jobject thiz)
{
    LOGV("native_surface_finalize");
    DecodeCtx * pCtx = (DecodeCtx*) (jint)env->GetLongField(thiz, context_field_id);

    if(!pCtx)
    {
        LOGE("native_surface_finalize failed - cannot get the decode context");
        return;
    }
    if(pCtx->native_window)
    {
        ANativeWindow_release(pCtx->native_window);
    }
    pCtx->native_window = NULL;
}
void* native_decode(void *pData)
{
    LOGV("native_decode");
    int ret_val = 0;
    DecodeCtx *pCtx = (DecodeCtx*) pData;

    pCtx->vcfGraphRunner->play();


    return (void*)&ret_val;
}
void native_play(JNIEnv* env, jobject thiz)
{
    LOGV("native_play");
    DecodeCtx * pCtx = (DecodeCtx*) (jint)env->GetLongField(thiz, context_field_id);

    if(!pCtx)
    {
        LOGE("native_play failed - cannot get the decode context");
        return;
    }

    // create a VCF GraphRunner
    pCtx->vcfGraphRunner = new VCFGraphRunner();
    if(!pCtx->vcfGraphRunner)
    {
        LOGE("native_play failed - cannot create VCF graph runner");
        return;
    }

    // set surface
    pCtx->vcfGraphRunner->set_surface(pCtx->native_window);
    pCtx->vcfGraphRunner->set_JavaVM(java_vm);
    pCtx->vcfGraphRunner->set_NativeLibDir(pCtx->nativeLibDir);
    pthread_create(&async_thread, NULL, &native_decode, pCtx);
}

void native_pause(JNIEnv* env, jobject thiz)
{
    DecodeCtx * pCtx = (DecodeCtx*) (jint)env->GetLongField(thiz, context_field_id);

    if(!pCtx)
    {
        LOGE("native_pause failed - cannot get the decode context");
        return;
    }
    if(pCtx->vcfGraphRunner)
    {
        pCtx->vcfGraphRunner->pause();
    }
    LOGV("native_pause");
}


void native_stop(JNIEnv* env, jobject thiz)
{
    LOGV("native_stop");
    DecodeCtx * pCtx = (DecodeCtx*) (jint)env->GetLongField(thiz, context_field_id);

    if(!pCtx)
    {
        LOGE("native_stop failed - cannot get the decode context");
        return;
    }
    if(pCtx->vcfGraphRunner)
    {
        pCtx->vcfGraphRunner->stop();
    }
}

static JNINativeMethod native_methods[] =
{
    { "nativeStaticInit",       "()Z",                      (void *) native_static_init     },
    { "nativeInit",             "(Ljava/lang/String;)Z",    (void *) native_init            },
    { "nativeClose",            "()V",                      (void *) native_close           },
    { "nativeSurfaceInit",      "(Ljava/lang/Object;)V",    (void *) native_surface_init    },
    { "nativeSurfaceFinalize",  "()V",                      (void *) native_surface_finalize},
    { "nativePlay",             "()V",                      (void *) native_play            },
    { "nativePause",            "()V",                      (void *) native_pause           },
    { "nativeStop",             "()V",                      (void *) native_stop            }
};


jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
    LOGV("JNI_OnLoad");
    java_vm = vm;
    JNIEnv *env = NULL;

    if(vm->GetEnv((void**)&env, JNI_VERSION_1_6 ) != JNI_OK)
    {
        LOGE("JavaVM::GetEnv failed!");
        return 0;
    }
    jclass klass = env->FindClass("sample/vcf/intel/com/vcfplayer/MainActivity");
    env->RegisterNatives ( klass, native_methods, sizeof(native_methods)/sizeof(native_methods[0]));

    pthread_key_create (&tls_key, detach_thread);

    return JNI_VERSION_1_6;
}

void JNI_UnOnLoad(JavaVM *vm, void *reserved)
{
    LOGV("JNI_UnOnLoad");
}