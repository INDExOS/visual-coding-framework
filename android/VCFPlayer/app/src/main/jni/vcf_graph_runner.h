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
#ifndef __VCF_GRAPH_DECODE_H
#define __VCF_GRAPH_DECODE_H

#include "vcfapi++.h"

#include <android/native_window.h>
#include <string>
//------------------------------ VCF DECODE GRAPH - START ------------------------//

class VCFGraphRunner
{
public:
    VCFGraphRunner();
    virtual ~VCFGraphRunner();

    virtual VCFAPIStatus set_surface(ANativeWindow* native_window);
    virtual VCFAPIStatus set_JavaVM(void* vm);
    virtual VCFAPIStatus set_NativeLibDir(std::string path);
    virtual VCFAPIStatus play();
    virtual VCFAPIStatus pause();
    virtual VCFAPIStatus stop();
    virtual VCFAPIStatus close();

private:
    // Handle to the Java Surface View
    ANativeWindow   *m_pNativeWindow;
    // Handle to VCF
    VCFAPI          *m_pVCFRuntime;
    // Handle to VCF Graph
    VCFGraph       *m_pVCFGraph;
    // state Paused
    bool m_bPaused;

    void *m_pJavaVM;

    std::string m_NativeLibraryDir;

};
//------------------------------ VCF DECODE GRAPH - END --------------------------//

#endif //__VCF_GRAPH_DECODE_H

