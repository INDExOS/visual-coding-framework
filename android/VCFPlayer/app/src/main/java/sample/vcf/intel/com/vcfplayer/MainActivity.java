/*
 * Copyright 2015 Intel Corporation All Rights Reserved.
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

package sample.vcf.intel.com.vcfplayer;

import android.app.Activity;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;

public class MainActivity extends Activity implements View.OnClickListener, SurfaceHolder.Callback
{

    private static final String TAG = "VCFPlayerActivity";

    private Button mButtonPlay;
    private Button mButtonPause;
    private Button mButtonStop;
    private SurfaceView mSurfaceViewPlayback;

    private String mNativeLibraryDir;

    // handle to native context
    private long native_context;


    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Log.v(TAG, "onCreate");

        // get package manager to get global package info
        PackageManager packageManager = getPackageManager();
        // get the name of applications package name
        String PackagePath = getPackageName();

        String PathLib = "";
        try
        {
            // Get the Package Info
            PackageInfo p = packageManager.getPackageInfo(PackagePath, 0);
            // Get Application packagepath directory
            PackagePath = p.applicationInfo.dataDir;

            ApplicationInfo applicationInfo = packageManager.getApplicationInfo(p.packageName, PackageManager.GET_SHARED_LIBRARY_FILES);
            mNativeLibraryDir = applicationInfo.nativeLibraryDir;

        }
        catch (PackageManager.NameNotFoundException e)
        {
            Log.w(TAG, "Error Package name not found ", e);
        }

        bindUIElements();
    }

    @Override
    protected void onStart()
    {
        super.onStart();
        Log.v(TAG,"onStart");
    }

    @Override
    protected void onRestart()
    {
        super.onRestart();
        Log.v(TAG,"onRestart");
    }

    @Override
    protected void onResume()
    {
        super.onResume();
        nativePause();
        Log.v(TAG,"onResume");
    }

    @Override
    protected void onPause()
    {
        super.onPause();
        nativeStop();
        Log.v(TAG,"onPause");
    }

    @Override
    protected void onStop()
    {
        super.onStop();
        nativePause();
        Log.v(TAG,"onStop");
    }

    @Override
    protected void onDestroy()
    {
        nativeClose();
        super.onDestroy();
        Log.v(TAG,"onStop");
    }


    @Override
    public void surfaceCreated(SurfaceHolder holder)
    {
        Log.v("Surface", "Created");
        nativeSurfaceInit(holder.getSurface());
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height)
    {
        Log.v("Surface", "Changed");
        nativeSurfaceInit(holder.getSurface());
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder)
    {
        Log.v("Surface", "Destroyed");
        nativeSurfaceFinalize();
    }

    @Override
    public void onClick(View v)
    {
        switch (v.getId())
        {
            case R.id.idButtonPlay:
                Log.v(TAG,"Play");
                nativePlay();
                break;
            case R.id.idButtonPause:
                nativePause();
                Log.v(TAG,"Pause");
                break;
            case R.id.idButtonStop:
                nativeStop();
                Log.v(TAG,"Stop");
                break;
        }
    }

    private void bindUIElements()
    {
        mButtonPlay             = (Button) findViewById(R.id.idButtonPlay);
        mButtonPause            = (Button) findViewById(R.id.idButtonPause);
        mButtonStop             = (Button) findViewById(R.id.idButtonStop);
        mSurfaceViewPlayback    = (SurfaceView)findViewById(R.id.idSurfaceViewPlayback);

        if(!nativeInit(mNativeLibraryDir))
        {
            Log.e(TAG, "Error, Could not Initialize Native Lib");
            return;
        }

        mButtonPlay.setOnClickListener(this);
        mButtonPause.setOnClickListener(this);
        mButtonStop.setOnClickListener(this);
        mSurfaceViewPlayback.getHolder().addCallback(this);

    }

    static private native boolean nativeStaticInit();
    private native boolean nativeInit(String NativeLibraryDir);
    private native void nativeClose();
    private native void nativeSurfaceInit(Object surface);
    private native void nativeSurfaceFinalize();
    private native void nativePlay();
    private native void nativePause();
    private native void nativeStop();


    static
    {
        System.loadLibrary("vcfplayer");
        nativeStaticInit();
    }
}
