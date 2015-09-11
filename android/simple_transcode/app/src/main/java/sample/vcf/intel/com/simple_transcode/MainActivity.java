
/*
 *     Copyright 2015 Intel Corporation All Rights Reserved.
 *
 *     The source code, information and material ("Material") contained herein is
 * 	owned by Intel Corporation or its suppliers or licensors, and title to such
 * 	Material remains with Intel Corporation or its suppliers or licensors.
 * 	The Material contains proprietary information of Intel or its suppliers
 * 	and licensors. The Material is protected by worldwide copyright laws and
 * 	treaty provisions. No part of the Material may be used, copied, reproduced,
 * 	modified, published, uploaded, posted, transmitted, distributed or
 * 	disclosed in any way without Intel's prior express written permission.
 *
 * 	No license under any patent, copyright or other intellectual property rights
 * 	in the Material is granted to or conferred upon you, either expressly,
 * 	by implication, inducement, estoppel or otherwise. Any license under such
 * 	intellectual property rights must be express and approved by Intel in writing.
 *
 * 	Unless otherwise agreed by Intel in writing, you may not remove or alter this
 * 	notice or any other notice embedded in Materials by Intel or Intel's
 * 	suppliers or licensors in any way.
 */

package sample.vcf.intel.com.simple_transcode;

import android.app.Activity;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;

public class MainActivity extends Activity{

    private static final String TAG = "MainActivity";
    private Button      mButtonRun;

    private String mNativeLibraryDir = "";
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // bind UI Button resource to Java Object
        mButtonRun      = (Button)findViewById(R.id.idButtonRun);
        // set OnClickListener
        mButtonRun.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // when the Run Transcode button is clicked call the native code to transcode.
                nativeSimpleTranscode(mNativeLibraryDir);
            }
        });

        // VCF RT for Android requires a path where to find plugins, components
        // The following code shows how to get the path to lib folder of the App.
        // get package manager to get global package info
        PackageManager packageManager = getPackageManager();
        // get the name of applications package name
        String PackagePath = getPackageName();

        try
        {
            // Get the Package Info
            PackageInfo p = packageManager.getPackageInfo(PackagePath, 0);

            ApplicationInfo applicationInfo = packageManager.getApplicationInfo(p.packageName, PackageManager.GET_SHARED_LIBRARY_FILES);
            mNativeLibraryDir = applicationInfo.nativeLibraryDir;
        }
        catch (PackageManager.NameNotFoundException e)
        {
            Log.w(TAG, "Error Package name not found ", e);
        }
    }


    private native String nativeSimpleTranscode(String nativeLibraryPath);
    static
    {
        System.loadLibrary("simple_transcode");
    }
}
