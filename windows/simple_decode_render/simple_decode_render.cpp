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

#include "vcfapi++.h"
#include <stdio.h>
#include <windows.h>

#define CHECK_RES(RES, MSG) \
if (VCFAPI_STATUS_SUCCESS != RES) {\
    printf(MSG);\
    return RES;\
}


// Globals used to simplify implementation of code accessed by windows message handler
VCFGraph*       g_pVCFGraph         = NULL;
VCFNode*        g_pVCFRenderNode    = NULL;
bool            g_bExecCompleted    = false;
const WCHAR*    g_windowName    = L"RenderWindow";

//
// Message handler for our render window
//
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
    case WM_DESTROY:
        {
            g_pVCFGraph->Stop();
            g_bExecCompleted = true;
            PostQuitMessage(0);
            break;
        }
    case WM_KEYUP:
        {
            if(VK_ESCAPE == wParam)
            {
                g_pVCFGraph->Stop();
                g_bExecCompleted = true;
                PostQuitMessage(0);
            }
            break;
        }
    case WM_SIZE:
        {
            int cx = (int)(short)LOWORD(lParam);
            int cy = (int)(short)HIWORD(lParam);

            VCFNodeRenderCommandSetTarget renderTargetInfo;
            renderTargetInfo.windowsHandle = hWnd;
            renderTargetInfo.x = 0;
            renderTargetInfo.y = 0;
            renderTargetInfo.w = cx;
            renderTargetInfo.h = cy;

            g_pVCFRenderNode->Command(VCFNODE_COMMAND_RENDER_SET_TARGET, &renderTargetInfo);
        }
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

//
// Setup window for VCF rendering
//
VCFAPIStatus SetupWindow(HWND& hWnd)
{
    WNDCLASS window;
    memset(&window, 0, sizeof(window));

    window.lpfnWndProc      = (WNDPROC)WindowProc;
    window.hInstance        = GetModuleHandle(NULL);
    window.hCursor          = LoadCursor(NULL, IDC_ARROW);
    window.lpszClassName    = g_windowName;

    if(!window.hInstance) {
        printf("GetModuleHandle failed");
        return VCFAPI_STATUS_ERR_UNKNOWN;
    }

    ATOM classRef = RegisterClass(&window);
    if(!classRef) {
        printf("Failed to register render window class");
        return VCFAPI_STATUS_ERR_UNKNOWN;
    }

    DWORD dwStyle = WS_OVERLAPPEDWINDOW; // WS_POPUP|WS_BORDER|WS_MAXIMIZE

    hWnd = CreateWindowEx(NULL,
        g_windowName,
        g_windowName,
        dwStyle,
        10,
        10,
        600,
        600,
        NULL,
        NULL,
        window.hInstance,
        NULL);
    if(!hWnd) {
        printf("Failed to create render window");
        return VCFAPI_STATUS_ERR_UNKNOWN;
    }

    return VCFAPI_STATUS_SUCCESS;
}

//
// VCF Graph level callbacks
//
class MyGraphCallbacks : public VCFGraph_Callback
{
public:
    void Status(VCFGraph* pGraph, const char* component, const char* status, const int res)
    {
        // Not implemented since we do not subscribe for this event
    }

    void StatusProgress(VCFGraph* pGraph, const char* sourceComponent, unsigned int numPackets)
    {
        printf("StatusProgress : %d\n", numPackets);
    }

    void ExecutionComplete(VCFGraph* pGraph, VCFGraphExecutionCompleteStatus sts)
    {
         g_bExecCompleted = true;
    }

    void BenchmarkData(VCFGraph* pGraph, const char* data)
    {
        // Not implemented since we do not subscribe for this event
    }
};

//
//
//

int main(int argc, char* argv[])
{
    VCFAPIStatus res = VCFAPI_STATUS_SUCCESS;

    //
    // Create and initialize VCF run-time
    //
    VCFAPI myVCFAPI;
    res = myVCFAPI.Init();
    CHECK_RES(res, "Failed to initialize VCF runtime. Missing RT DLL/so ???\n");

    //
    // Load a VCF graph workload
    //
    res = myVCFAPI.LoadFile("vcf_decode_render.graphml", &g_pVCFGraph);
    CHECK_RES(res, "Failed to load graph\n");

    //
    // Subscribe for graph level events
    //
    MyGraphCallbacks graphcallbacks;
    res = g_pVCFGraph->SubscribeEvents(VCFGRAPH_EVENTMASK_STATUS_PROGRESS | VCFGRAPH_EVENTMASK_EXECUTION_COMPLETE, &graphcallbacks);
    CHECK_RES(res, "Failed to subscribe to graph events\n");

    //
    // VCF Render component must be configured with desired render target (A Window handle)
    // before we can execute the graph
    // To find the VCF node we use the unique name "Render0" for the Render node in the graph
    //
    HWND hWnd = NULL;

    res = g_pVCFGraph->GetNode("VCFRender_W_node_2", &g_pVCFRenderNode); 
    if(VCFAPI_STATUS_SUCCESS == res)
    {
        // Setup window in which the VCF render node will render the decoded frames
        res = SetupWindow(hWnd);
        CHECK_RES(res, "Failed to setup render window\n");

        ShowWindow(hWnd, SW_SHOWDEFAULT);
        UpdateWindow(hWnd);

        // Prepare data describing in what part of the window to render
        VCFNodeRenderCommandSetTarget renderTargetInfo;
        renderTargetInfo.windowsHandle = hWnd;
        renderTargetInfo.x = 20;
        renderTargetInfo.y = 20;
        renderTargetInfo.w = 500;
        renderTargetInfo.h = 350;

        res = g_pVCFRenderNode->Command(VCFNODE_COMMAND_RENDER_SET_TARGET, &renderTargetInfo);
        CHECK_RES(res, "Failed to issue node command\n");
    }
    else {
        printf("Did not find render node in graph. Aborting...\n");
        return VCFAPI_STATUS_ERR_ABORTED;
    }

    //
    // Execute VCF graph
    //
    res = g_pVCFGraph->Run();
    CHECK_RES(res, "Failed to run graph\n");

    //
    // Wait for VCF graph execution to complete 
    // Since we use a Window in this sample we need to handle polling of the message queue, thus we
    // cannot use the blocking VCFAPI:WaitForSingle() call. Instead we use a polling loop which will
    // run until workload completes or user closes the window.
    //
    while(!g_bExecCompleted){
        MSG msg;
        memset(&msg, 0 , sizeof(msg));
        while (msg.message != WM_QUIT && PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {        
            TranslateMessage(&msg);
            DispatchMessage(&msg);        
        }
        Sleep(100);
    }

    printf("VCF Graph execution completed\n");

    // Release VCF graph resources
    g_pVCFGraph->Release();

    // Release window resources
    if(hWnd) {
        DestroyWindow(hWnd);
        UnregisterClass(g_windowName, NULL);
    }

    return res;
}
