package com.xxx.ffmpeg;


import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.xxx.ffmpeglibrary.FFmpegBridge;

public class MainActivity extends Activity {
    private static final String TAG = "MainActivity";
    private Button button1;
    private Button button2;
    private Button button3;
    private Button button4;
    private Button button5;
    private TextView mTextView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
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
        mTextView = findViewById(R.id.info);
    }

    private View.OnClickListener listener = new View.OnClickListener() {
        @Override
        public void onClick(View view) {
            switch (view.getId()){
                case R.id.button1:
                    Log.i(TAG,"onClick button1");
                    getUrlprotocolinfo();
                    break;
                case R.id.button2:
                    Log.i(TAG,"onClick button2");
                    getAvformatinfo();
                    break;
                case R.id.button3:
                    Log.i(TAG,"onClick button3");
                    getAvcodecinfo();
                    break;
                case R.id.button4:
                    Log.i(TAG,"onClick button4");
                    getAvfilterinfo();
                    break;
                case R.id.button5:
                    Log.i(TAG,"onClick button5");
                    getConfigurationinfo();
                    break;
                default:
                    break;
            }
        }
    };

    private void getUrlprotocolinfo(){
        FFmpegBridge bridge = new FFmpegBridge();
        String s = bridge.getUrlprotocolinfo();
        mTextView.setText(s);
    }

    private void getAvformatinfo(){
        FFmpegBridge bridge = new FFmpegBridge();
        String s = bridge.getAvformatinfo();
        mTextView.setText(s);
    }

    private void getAvcodecinfo(){
        FFmpegBridge bridge = new FFmpegBridge();
        String s = bridge.getAvcodecinfo();
        mTextView.setText(s);
    }

    private void getAvfilterinfo(){
        FFmpegBridge bridge = new FFmpegBridge();
        String s = bridge.getAvfilterinfo();
        mTextView.setText(s);
    }

    private void getConfigurationinfo(){
        FFmpegBridge bridge = new FFmpegBridge();
        String s = bridge.getConfigurationinfo();
        mTextView.setText(s);
    }
}
