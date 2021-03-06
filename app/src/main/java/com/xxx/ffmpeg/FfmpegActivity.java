package com.xxx.ffmpeg;


import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.xxx.ffmpeglibrary.FFmpegBridge;

public class FfmpegActivity extends Activity {
    private static final String TAG = "FfmpegActivity";
    private Button button1;
    private Button button2;
    private Button button3;
    private Button button4;
    private Button button5;
    private Button button6;
    private TextView mInput;
    private TextView mOutput;
    private EditText mInputPath;
    private EditText mOutputPath;
    private FFmpegBridge bridge;
    private SurfaceView mSurfaceView;
    private Surface mSurface;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_ffmpeg);
        button1 = findViewById(R.id.button1);
        button1.setOnClickListener(listener);
        button2 = findViewById(R.id.button2);
        button2.setOnClickListener(listener);
        button3 = findViewById(R.id.button3);
        button3.setOnClickListener(listener);
        button4 = findViewById(R.id.button4);
        button4.setOnClickListener(listener);
        button5 = findViewById(R.id.button5);
        button5.setOnClickListener(listener);
        button6 = findViewById(R.id.button6);
        button6.setOnClickListener(listener);
        mInputPath = findViewById(R.id.inputpath);
        mOutputPath = findViewById(R.id.outputpath);
        mSurfaceView = findViewById(R.id.videoid);
        bridge = new FFmpegBridge();
        checkPermission();
    }

    private View.OnClickListener listener = new View.OnClickListener() {
        @Override
        public void onClick(View view) {
            switch (view.getId()) {
                case R.id.button1:
                    Log.i(TAG, "onClick button1");
                    decode();
                    break;
                case R.id.button2:
                    Log.i(TAG, "onClick button2");
                    stream();
                    break;
                case R.id.button3:
                    Log.i(TAG, "onClick button3");
                    getAvcodecinfo();
                    break;
                case R.id.button4:
                    Log.i(TAG, "onClick button4");
                    getAvfilterinfo();
                    break;
                case R.id.button5:
                    Log.i(TAG, "onClick button5");
                    getConfigurationinfo();
                    break;
                case R.id.button6:
                    Log.i(TAG, "onClick button6");
                    setSurface(mSurface);
//                    pausePlayer();
//                    stopPlayer();
//                    resumePlayer();
                    break;
                default:
                    break;
            }
        }
    };


    private void decode() {
        String folderPath = Environment.getExternalStorageDirectory().getPath();
//        String folderPath = "/sdcard";
        final String inputUrl = folderPath + "/testFFmpeg/" + mInputPath.getText().toString();
        final String outputUrl = folderPath + "/testFFmpeg/" + mOutputPath.getText().toString();
        Log.i(TAG, "inputUrl:" + inputUrl);
        Log.i(TAG, "outputUrl:" + outputUrl);

        new Thread(new Runnable() {
            @Override
            public void run() {
                bridge.decode(inputUrl, outputUrl);
            }
        }).start();
    }

    private void stream() {
        String folderPath = Environment.getExternalStorageDirectory().getPath();
//        String folderPath = "/sdcard";
        final String inputUrl = folderPath + "/testFFmpeg/" + mInputPath.getText().toString();
        final String outputUrl = mOutputPath.getText().toString();
        Log.i(TAG, "inputUrl:" + inputUrl);
        Log.i(TAG, "outputUrl:" + outputUrl);

        new Thread(new Runnable() {
            @Override
            public void run() {
                bridge.stream(inputUrl, outputUrl);
            }
        }).start();
    }

    private void getUrlprotocolinfo() {
        String s = bridge.getUrlprotocolinfo();
    }

    private void getAvformatinfo() {
        String s = bridge.getAvformatinfo();
    }

    private void getAvcodecinfo() {
        String s = bridge.getAvcodecinfo();
    }

    private void getAvfilterinfo() {
        String s = bridge.getAvfilterinfo();
    }

    private void getConfigurationinfo() {
        String s = bridge.getConfigurationinfo();
    }

    private  int setSurface(Surface view){
       return bridge.setSurface(view);
    }

    private  int pausePlayer(){
        return bridge.pausePlayer();
    }

    private  int resumePlayer(){
        return bridge.resumePlayer();
    }

    private  int stopPlayer(){
        return bridge.stopPlayer();
    }


    @Override
    protected void onResume() {
        super.onResume();
        if(mSurfaceView != null){
            mSurfaceView.getHolder().addCallback(callback);
        }
        resumePlayer();
    }

    @Override
    protected void onPause() {
        super.onPause();
        pausePlayer();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        stopPlayer();
    }

    private SurfaceHolder.Callback callback = new SurfaceHolder.Callback() {
        @Override
        public void surfaceCreated(SurfaceHolder surfaceHolder) {
            Log.i(TAG, "surfaceCreated ");
//            setSurface(surfaceHolder.getSurface());
            mSurface = surfaceHolder.getSurface();
        }

        @Override
        public void surfaceChanged(SurfaceHolder surfaceHolder, int format, int width, int height) {
            Log.v(TAG, "surfaceChanged, format is " + format + ", width is "
                    + width + ", height is" + height);
        }

        @Override
        public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
            Log.i(TAG, "surfaceDestroyed ");
        }
    };


    private void checkPermission() {

        if (Build.VERSION.SDK_INT >= 23) {
            int write = checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE);
            int read = checkSelfPermission(Manifest.permission.READ_EXTERNAL_STORAGE);
            Log.i(TAG, "checkPermission write:" + write + ",read:" + read);
            if (write != PackageManager.PERMISSION_GRANTED || read != PackageManager.PERMISSION_GRANTED) {
                requestPermissions(new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.READ_EXTERNAL_STORAGE}, 300);
            } else {
                Log.i(TAG, "hasPermission");
            }
        } else {
            Log.i(TAG, "------------- Build.VERSION.SDK_INT < 23 ------------");
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        if (requestCode == 300) {
            Log.i(TAG, "--------------requestCode == 300->" + requestCode + "," + permissions.length + "," + grantResults.length);
        } else {
            Log.i(TAG, "--------------requestCode != 300->" + requestCode + "," + permissions + "," + grantResults);
        }
    }
}
