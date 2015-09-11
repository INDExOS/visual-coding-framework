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

#include "native_android_logging.h"
#include "vcf_callback.h"

#define TAG "VCFCALLBACK"

MyGraphCallback::MyGraphCallback(){}
MyGraphCallback::~MyGraphCallback(){}
void MyGraphCallback::Status(VCFGraph* pGraph, const char* component, const char* status, const int res)
{
	LOGV("Status : Component [%s] Status -> %s (%d)\n", component, status, res);
}
void MyGraphCallback::StatusProgress(VCFGraph* pGraph, const char* sourceComponent, unsigned int numPackets)
{
    LOGV("StatusProgress : %d\n", numPackets);
}
void MyGraphCallback::ExecutionComplete(VCFGraph* pGraph, VCFGraphExecutionCompleteStatus sts)
{
	if (VCFGRAPH_EXECCOMPLETE_STATUS_STOPPED == sts) {
		LOGV("Execution stopped");
	}
	else if (VCFGRAPH_EXECCOMPLETE_STATUS_ABORTED == sts) {
		LOGV("Execution aborted");
	}
	else if (VCFGRAPH_EXECCOMPLETE_STATUS_COMPLETED == sts) {
		LOGV("Execution completed");
	}
}
void MyGraphCallback::BenchmarkData(VCFGraph* pGraph, const char* data)
{
    LOGV("Benchmark Data : %s", data);
}
