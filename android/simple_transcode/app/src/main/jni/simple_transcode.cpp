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
#include <android/log.h>
#include "vcfapi++.h"
#include <string>

#define TAG "simple_transcode"

#define LOGV(...) ((void)__android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__))
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__))
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__))

static JavaVM *java_vm;

// Sample code for running transcode workload
jstring native_simple_transcode(JNIEnv *env, jobject *thiz, jstring nativeLibraryDir)
{
    VCFAPIStatus res = VCFAPI_STATUS_SUCCESS;

    // Convert JNI jstring object to C++ understandable std::string
    const char *tmp = env->GetStringUTFChars(nativeLibraryDir,NULL);
    std::string strNativeLibraryDir = tmp;
    env->ReleaseStringUTFChars(nativeLibraryDir,tmp);

    // Create VCFAPIConfig that conveys Android ENV to the VCF RT
    VCFAPIConfigItemAndroidEnv *pAndroidEnv = new VCFAPIConfigItemAndroidEnv;
    pAndroidEnv->type = VCFAPIConfigItemTypeAndroidEnv;
    pAndroidEnv->JavaVM = java_vm;
    pAndroidEnv->nativeLibraryDir = strNativeLibraryDir.c_str();

    VCFAPIConfig vcfConfig;
    vcfConfig.numItems      = 1;
    vcfConfig.pConfigItems  = (VCFAPIConfigItem**)&pAndroidEnv;

    //
    // Create and initialize VCF run-time
    //
    VCFAPI myVCFAPI;
    res = myVCFAPI.Init(&vcfConfig);
    if(VCFAPI_STATUS_SUCCESS != res) {
        LOGV("Failed to initialize VCF runtime. Missing RT DLL/so %d\n",res);
        return env->NewStringUTF("Failed to initialize VCF runtime. Missing RT DLL/so ???\n");
    }

    //
    // Load a VCF graph workload
    //
    VCFGraph* myGraph;
    res = myVCFAPI.LoadFile("/storage/emulated/0/Movies/content/android_vcf_transcode.graphml", &myGraph);
    if(VCFAPI_STATUS_SUCCESS != res) {
        LOGV("Failed to load transcode graph %d\n",res);
        return env->NewStringUTF("Failed to load transcode graph. Check if the File exists in /sdcard/Movies/content\n");
    }

    //
    // Execute VCF graph
    //
    res = myGraph->Run();
    if(VCFAPI_STATUS_SUCCESS != res) {
        LOGV("Failed to Run graph %d\n",res);
        return env->NewStringUTF("Failed to run graph\n");
    }

    //
    // Wait for VCF graph execution to complete, This is a blocking call
    // due to blocking nature of this call, this should ideally not be done
    // in the UI thread. We are waiting in the UI thread for simplicity of
    // this sample.
    //
    res = myVCFAPI.WaitForSingle(myGraph);
    LOGV("VCF Graph execution completed (result code = %d)", res);

    // Release VCF graph resources
    myGraph->Release();

    return env->NewStringUTF("native_simple_transcode Done.");
}

// Maps Java side Native Methods to actual C/C++ methods
static JNINativeMethod native_methods[] =
{
    { "nativeSimpleTranscode",       "(Ljava/lang/String;)Ljava/lang/String;",         (void *) native_simple_transcode     }
};

// Explicit JNI Load Library
// This is done for two reasons:
//      1. doing this gets rid of the need to use fully qualified names
//      2. VCF RT internally uses a reference to JAVA VM, JNI_OnLoad gives us access to JavaVM*
jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
    LOGV("JNI_OnLoad");
    JNIEnv *env = NULL;
    java_vm = vm;
    // Get Java ENV from VM
    if(vm->GetEnv((void**)&env, JNI_VERSION_1_6 ) != JNI_OK)
    {
        LOGE("JavaVM::GetEnv failed!");
        return 0;
    }
    // Get reference to the App's Java Class
    jclass klass = env->FindClass("sample/vcf/intel/com/simple_transcode/MainActivity");
    // Register Native methods
    env->RegisterNatives ( klass, native_methods, sizeof(native_methods)/sizeof(native_methods[0]));

    return JNI_VERSION_1_6;
}

// Explicit JNI Unload Library
void JNI_UnOnLoad(JavaVM *vm, void *reserved)
{
    LOGV("JNI_UnOnLoad");
}
