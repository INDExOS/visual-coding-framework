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
#include "vcf_graph_runner.h"
#include "native_android_logging.h"
#include "vcf_callback.h"
#include <unistd.h>

#define TAG "VCFGraphDecode"




VCFGraphRunner::VCFGraphRunner()
    : m_pNativeWindow(NULL),
    m_bPaused(false),
    m_pVCFRuntime(NULL),
    m_pVCFGraph(NULL)
{
}
VCFGraphRunner::~VCFGraphRunner()
{
    close();
}

VCFAPIStatus VCFGraphRunner::set_surface(ANativeWindow* native_window)
{
    VCFAPIStatus sts = VCFAPI_STATUS_SUCCESS;
    m_pNativeWindow = native_window;
    return sts;
}
VCFAPIStatus VCFGraphRunner::set_JavaVM(void* vm)
{
    m_pJavaVM = vm;
    return VCFAPI_STATUS_SUCCESS;
}
VCFAPIStatus VCFGraphRunner::set_NativeLibDir(std::string path)
{
    m_NativeLibraryDir = path;
    return VCFAPI_STATUS_SUCCESS;
}
VCFAPIStatus VCFGraphRunner::play()
{
    VCFAPIStatus res = VCFAPI_STATUS_SUCCESS;

    //
    // Create and initialize VCF run-time
    //
    VCFAPIConfigItemAndroidEnv *pAndroidEnv = new VCFAPIConfigItemAndroidEnv;
    pAndroidEnv->type = VCFAPIConfigItemTypeAndroidEnv;
    pAndroidEnv->JavaVM = m_pJavaVM;

    VCFAPIConfig vcfConfig;
    vcfConfig.numItems      = 1;
    vcfConfig.pConfigItems  = (VCFAPIConfigItem**)&pAndroidEnv;
    pAndroidEnv->nativeLibraryDir = m_NativeLibraryDir.c_str();

    res = m_pVCFRuntime->Init(&vcfConfig);
    if(VCFAPI_STATUS_SUCCESS != res)
    {
        LOGV("Failed to initialize VCF runtime. Missing libvcfrtcore.so ??? %d\n", res);
        return res;
    }

    //
    // Load a VCF graph workload
    //
    res = m_pVCFRuntime->LoadFile("/storage/emulated/0/Movies/content/android_vcf_decode_render.graphml", &m_pVCFGraph);
    if(VCFAPI_STATUS_SUCCESS != res)
    {
        LOGV("Failed to load graph - android_vcf_decode_render.graphml. - %d", res);
        return res;
    }

    //
    // Subscribe for graph level events
    //
    MyGraphCallback graphcallbacks;
    res = m_pVCFGraph->SubscribeEvents(VCFGRAPH_EVENTMASK_ALL, &graphcallbacks);
    if(VCFAPI_STATUS_SUCCESS != res)
    {
        LOGV("Failed to subscribe to graph events\n");
        return res;
    }

    //
    // VCF Render component must be configured with desired render target (A Window handle)
    // before we can execute the graph
    // To find the VCF node we use the unique name "Render0" for the Render node in the graph
    //
    VCFNode* pVFNode;
    res = m_pVCFGraph->GetNode("VCFRender_A_node_2", &pVFNode);
    if(VCFAPI_STATUS_SUCCESS == res)
    {
        // Prepare data describing in what part of the window the Render component should render
        VCFNodeRenderCommandSetTarget renderTargetInfo;
        renderTargetInfo.windowsHandle = m_pNativeWindow;
        renderTargetInfo.x = 20;
        renderTargetInfo.y = 20;
        renderTargetInfo.w = 300;
        renderTargetInfo.h = 300;
        res = pVFNode->Command(VCFNODE_COMMAND_RENDER_SET_TARGET, &renderTargetInfo);
        if(VCFAPI_STATUS_SUCCESS != res)
        {
            LOGV("Set Render Target Failed.\n");
            return res;
        }
    }
    else
    {
        LOGV("Did not find render component in graph. Aborting...\n");
        return res;
    }


    res = m_pVCFGraph->Run();
    if(VCFAPI_STATUS_SUCCESS != res)
    {
        LOGV("Failed to run graph\n");
        return res;
    }

    //
    // Wait for VCF graph execution to complete
    //
    while(1)
    {
        res = m_pVCFRuntime->WaitForSingle(m_pVCFGraph);
        if(VCFAPI_STATUS_SUCCESS != res)
            return res;
        if(m_bPaused)
        {
            while(m_bPaused)
            {
                sleep(50);
            }
        }
    }

    LOGV("Graph Completed\n");
    //
    // Release VCF graph resources
    //
    m_pVCFGraph->Release();


    return res;
}

VCFAPIStatus VCFGraphRunner::pause()
{
    m_bPaused = !m_bPaused;
    VCFAPIStatus sts = m_pVCFGraph->Pause();
    LOGV("Pause Status = %d",sts);
    return sts;
}

VCFAPIStatus VCFGraphRunner::stop()
{
    return m_pVCFGraph->Stop();
}


VCFAPIStatus VCFGraphRunner::close()
{
    VCFAPIStatus sts = VCFAPI_STATUS_SUCCESS;
    return sts;
}
