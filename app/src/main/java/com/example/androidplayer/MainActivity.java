package com.example.androidplayer;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;

import com.example.ffmpeg.FFmpeg;
import com.example.ffmpeg.opengl.MyGLSurfaceView;

import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {

    private MyGLSurfaceView myGLSurfaceView;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Button btn = (Button) findViewById(R.id.btn);
        final FFmpeg ffmpeg = new FFmpeg();
        ffmpeg.initAvCodec("http://vfx.mtime.cn/Video/2019/03/19/mp4/190319212559089721.mp4");
        btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        ffmpeg.startPlay();
                    }
                }).start();
            }
        });
        myGLSurfaceView = findViewById(R.id.myglsurfaceview);
        ffmpeg.setMyGLSurfaceView(myGLSurfaceView);
    }
}
