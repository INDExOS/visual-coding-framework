##########################################################################################
#    Copyright 2015 Intel Corporation All Rights Reserved.
#
#    The source code, information and material ("Material") contained herein is
#	owned by Intel Corporation or its suppliers or licensors, and title to such
#	Material remains with Intel Corporation or its suppliers or licensors.
#	The Material contains proprietary information of Intel or its suppliers
#	and licensors. The Material is protected by worldwide copyright laws and
#	treaty provisions. No part of the Material may be used, copied, reproduced,
#	modified, published, uploaded, posted, transmitted, distributed or
#	disclosed in any way without Intel's prior express written permission.
#
#	No license under any patent, copyright or other intellectual property rights
#	in the Material is granted to or conferred upon you, either expressly,
#	by implication, inducement, estoppel or otherwise. Any license under such
#	intellectual property rights must be express and approved by Intel in writing.
#
#	Unless otherwise agreed by Intel in writing, you may not remove or alter this
#	notice or any other notice embedded in Materials by Intel or Intel's
#	suppliers or licensors in any way.
##########################################################################################
#############################################################
# Makefile
#############################################################
LOCAL_PATH := $(call my-dir)

########################################
# Environment Variables
########################################
VCF_SDK_DIR    := $(INTELVCFSDKROOT)
VCF_API_DIR     := $(VCF_SDK_DIR)/vcfapi
VCF_API_INCLUDE := $(VCF_API_DIR)/include
VCF_API_LIB     := $(VCF_API_DIR)/lib/android/x86/release
VCF_API_REDIST  := $(VCF_API_DIR)/redist/android/x86/release
VCF_COMPONENTS  := $(VCF_API_REDIST)/components
VCF_UTILITIESS  := $(VCF_API_REDIST)/utils

$(info LOCAL_PATH DIR = $(LOCAL_PATH))
include $(CLEAR_VARS)

$(info VCF_SDK_DIR = $(VCF_SDK_DIR))
$(info VCF_API_INCLUDE = $(VCF_API_INCLUDE))
LOCAL_C_INCLUDES := $(VCF_API_INCLUDE)

LOCAL_MODULE            := vcfplayer
LOCAL_SRC_FILES         := vcfplayer.cpp vcf_graph_runner.cpp vcf_callback.cpp
LOCAL_STATIC_LIBRARIES  := vcfrtapi
LOCAL_LDLIBS            := -landroid -ljnigraphics -llog

include $(BUILD_SHARED_LIBRARY)

# Install vcfrtapi libraries
include $(CLEAR_VARS)
LOCAL_MODULE    := libvcfrtapi
LOCAL_SRC_FILES := $(VCF_API_LIB)/libvcfrtapi.a
include $(PREBUILT_STATIC_LIBRARY)

# Install Android Environment
include $(CLEAR_VARS)
LOCAL_MODULE    := libandroid_environment
LOCAL_SRC_FILES := $(VCF_API_REDIST)/libandroid_environment.so
include $(PREBUILT_SHARED_LIBRARY)

# Install audiodecode libraries
include $(CLEAR_VARS)
LOCAL_MODULE    := libnode_vcfaudiodecode
LOCAL_SRC_FILES := $(VCF_COMPONENTS)/libnode_vcfaudiodecode.so
include $(PREBUILT_SHARED_LIBRARY)

# Install audioencode libraries
include $(CLEAR_VARS)
LOCAL_MODULE    := libnode_vcfaudioencode
LOCAL_SRC_FILES := $(VCF_COMPONENTS)/libnode_vcfaudioencode.so
include $(PREBUILT_SHARED_LIBRARY)

# Install decode libraries
include $(CLEAR_VARS)
LOCAL_MODULE    := libnode_vcfvideodecode
LOCAL_SRC_FILES := $(VCF_COMPONENTS)/libnode_vcfvideodecode.so
include $(PREBUILT_SHARED_LIBRARY)

# Install encode libraries
include $(CLEAR_VARS)
LOCAL_MODULE    := libnode_vcfvideoencode
LOCAL_SRC_FILES := $(VCF_COMPONENTS)/libnode_vcfvideoencode.so
include $(PREBUILT_SHARED_LIBRARY)

# Install file libraries
include $(CLEAR_VARS)
LOCAL_MODULE    := libnode_vcffilesource
LOCAL_SRC_FILES := $(VCF_COMPONENTS)/libnode_vcffilesource.so
include $(PREBUILT_SHARED_LIBRARY)

# Install fileout libraries
include $(CLEAR_VARS)
LOCAL_MODULE    := libnode_vcffilesink
LOCAL_SRC_FILES := $(VCF_COMPONENTS)/libnode_vcffilesink.so
include $(PREBUILT_SHARED_LIBRARY)

# Install nullsink libraries
include $(CLEAR_VARS)
LOCAL_MODULE    := libnode_vcfnullsink
LOCAL_SRC_FILES := $(VCF_COMPONENTS)/libnode_vcfnullsink.so
include $(PREBUILT_SHARED_LIBRARY)

# Install vpp libraries
include $(CLEAR_VARS)
LOCAL_MODULE    := libnode_vcfvideoprocess
LOCAL_SRC_FILES := $(VCF_COMPONENTS)/libnode_vcfvideoprocess.so
include $(PREBUILT_SHARED_LIBRARY)

# Install render libraries
include $(CLEAR_VARS)
LOCAL_MODULE    := libnode_vcfrender
LOCAL_SRC_FILES := $(VCF_COMPONENTS)/libnode_vcfrender.so
include $(PREBUILT_SHARED_LIBRARY)

# Install libva.so libraries
include $(CLEAR_VARS)
LOCAL_MODULE    := libva
LOCAL_SRC_FILES := $(VCF_API_REDIST)/libva.so
include $(PREBUILT_SHARED_LIBRARY)

# Install libva-android.so libraries
include $(CLEAR_VARS)
LOCAL_MODULE    := libva-android
LOCAL_SRC_FILES := $(VCF_API_REDIST)/libva-android.so
include $(PREBUILT_SHARED_LIBRARY)

# Install TBB libraries
include $(CLEAR_VARS)
LOCAL_MODULE    := libtbb
LOCAL_SRC_FILES := $(VCF_API_REDIST)/libtbb.so
include $(PREBUILT_SHARED_LIBRARY)

# Install sysinfo libraries
include $(CLEAR_VARS)
LOCAL_MODULE    := libutilities_sysinfo
LOCAL_SRC_FILES := $(VCF_UTILITIESS)/libutilities_vcfsysinfo.so
include $(PREBUILT_SHARED_LIBRARY)

# Install buffermemorymanager libraries
include $(CLEAR_VARS)
LOCAL_MODULE    := libutilities_vcfbuffermemorymanager
LOCAL_SRC_FILES := $(VCF_UTILITIESS)/libutilities_vcfbuffermemorymanager.so
include $(PREBUILT_SHARED_LIBRARY)

# Install devicemanager libraries
include $(CLEAR_VARS)
LOCAL_MODULE    := libutilities_vcfdevicemanager
LOCAL_SRC_FILES := $(VCF_UTILITIESS)/libutilities_vcfdevicemanager.so
include $(PREBUILT_SHARED_LIBRARY)


# Install mediasdkmanager libraries
include $(CLEAR_VARS)
LOCAL_MODULE    := libutilities_vcfmediasdkmanager
LOCAL_SRC_FILES := $(VCF_UTILITIESS)/libutilities_vcfmediasdkmanager.so
include $(PREBUILT_SHARED_LIBRARY)

# Install surfacememmgr libraries
include $(CLEAR_VARS)
LOCAL_MODULE    := libutilities_vcfsurfacememmgr
LOCAL_SRC_FILES := $(VCF_UTILITIESS)/libutilities_vcfsurfacememmgr.so
include $(PREBUILT_SHARED_LIBRARY)

# Install vcfrtcore libraries
include $(CLEAR_VARS)
LOCAL_MODULE    := libvcfrtcore
LOCAL_SRC_FILES := $(VCF_API_REDIST)/libvcfrtcore.so
include $(PREBUILT_SHARED_LIBRARY)

# Install ffmpeg libavcodec libararies
include $(CLEAR_VARS)
LOCAL_MODULE    := libavcodec
LOCAL_SRC_FILES := $(VCF_API_REDIST)/libavcodec-56.so
include $(PREBUILT_SHARED_LIBRARY)

# Install ffmpeg libavfilter libararies
include $(CLEAR_VARS)
LOCAL_MODULE    := libavfilter-5
LOCAL_SRC_FILES := $(VCF_API_REDIST)/libavfilter-5.so
include $(PREBUILT_SHARED_LIBRARY)

# Install ffmpeg libavformat libararies
include $(CLEAR_VARS)
LOCAL_MODULE    := libavformat
LOCAL_SRC_FILES := $(VCF_API_REDIST)/libavformat-56.so
include $(PREBUILT_SHARED_LIBRARY)

# Install ffmpeg libavutil libararies
include $(CLEAR_VARS)
LOCAL_MODULE    := libavutil
LOCAL_SRC_FILES := $(VCF_API_REDIST)/libavutil-54.so
include $(PREBUILT_SHARED_LIBRARY)

# Install FGD libararies
include $(CLEAR_VARS)
LOCAL_MODULE    := libfgd_utils
LOCAL_SRC_FILES := $(VCF_API_REDIST)/libfgd_utils.so
include $(PREBUILT_SHARED_LIBRARY)

# Install FGD libararies
include $(CLEAR_VARS)
LOCAL_MODULE    := libgraph
LOCAL_SRC_FILES := $(VCF_API_REDIST)/libgraph.so
include $(PREBUILT_SHARED_LIBRARY)

# Install FGD libararies
include $(CLEAR_VARS)
LOCAL_MODULE    := libplugin_manager
LOCAL_SRC_FILES := $(VCF_API_REDIST)/libplugin_manager.so
include $(PREBUILT_SHARED_LIBRARY)

# Install FGD libararies
include $(CLEAR_VARS)
LOCAL_MODULE    := libxml_parser
LOCAL_SRC_FILES := $(VCF_API_REDIST)/libxml_parser.so
include $(PREBUILT_SHARED_LIBRARY)

# Install FGD libararies
include $(CLEAR_VARS)
LOCAL_MODULE    := libfgd_graphml_reader
LOCAL_SRC_FILES := $(VCF_API_REDIST)/libfgd_graphml_reader.so
include $(PREBUILT_SHARED_LIBRARY)
