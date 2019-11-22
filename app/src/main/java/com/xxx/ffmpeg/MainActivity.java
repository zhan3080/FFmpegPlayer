package com.xxx.ffmpeg;


import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import com.xxx.ffmpeglibrary.FFmpegBridge;

public class MainActivity extends Activity {
    private static final String TAG = "MainActivity";
    private Button button1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        button1 = findViewById(R.id.button_test);
        button1.setOnClickListener(listener);
    }

    private View.OnClickListener listener = new View.OnClickListener() {
        @Override
        public void onClick(View view) {
            switch (view.getId()){
                case R.id.button_test:
                    Log.i(TAG,"onClick button_test");
                    ffmpegTest();
                    break;
                default:
                    break;
            }
        }
    };

    private void ffmpegTest(){
        FFmpegBridge bridge = new FFmpegBridge();
        bridge.sayHello();
    }
}
