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

#define CHECK_RES(RES, MSG) \
if (VCFAPI_STATUS_SUCCESS != RES) {\
    printf(MSG);\
    return RES;\
}

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
    VCFGraph* myGraph;
    res = myVCFAPI.LoadFile("vcf_transcode.graphml", &myGraph);
    CHECK_RES(res, "Failed to load graph\n");

    //
    // Execute VCF graph
    //
    res = myGraph->Run();
    CHECK_RES(res, "Failed to run graph\n");

    //
    // Wait for VCF graph execution to complete 
    //
    res = myVCFAPI.WaitForSingle(myGraph);
    printf("VCF Graph execution completed (result code = %d)\n", res);

    // Release VCF graph resources
    myGraph->Release();

    return res;
}
