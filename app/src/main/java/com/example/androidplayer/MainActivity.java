package com.example.androidplayer;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.TextView;

import com.example.ffmpeg.FFmpeg;
import com.example.ffmpeg.bean.TimeInfoBean;
import com.example.ffmpeg.listener.MyOnCompleteListener;
import com.example.ffmpeg.listener.MyOnParparedListener;
import com.example.ffmpeg.listener.MyOnTimeInfoListener;
import com.example.ffmpeg.opengl.MyGLSurfaceView;
import com.example.ffmpeg.util.TimeUtil;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.widget.ContentLoadingProgressBar;

public class MainActivity extends AppCompatActivity {
    ContentLoadingProgressBar progressBar;
    private MyGLSurfaceView myGLSurfaceView;
    private SeekBar seekBar;
    TextView tvTime;
    private boolean isSeek = false;
    private int position;
    Handler handler;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Button startBtn = (Button) findViewById(R.id.start_btn);
        Button pauseBtn = (Button) findViewById(R.id.pause_btn);
        Button stopBtn = (Button) findViewById(R.id.stop_btn);
        Button resumeBtn = (Button) findViewById(R.id.resume_btn);
         tvTime = (TextView) findViewById(R.id.time);
         progressBar = (ContentLoadingProgressBar) findViewById(R.id.progressBar);
        seekBar = findViewById(R.id.seekbar);
        final FFmpeg ffmpeg = new FFmpeg();
        myGLSurfaceView = findViewById(R.id.myglsurfaceview);
        ffmpeg.setMyGLSurfaceView(myGLSurfaceView);
         handler = new Handler(){
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                if(msg.what == 1)
                {
                    TimeInfoBean wlTimeInfoBean = (TimeInfoBean) msg.obj;
                    tvTime.setText(TimeUtil.secdsToDateFormat(wlTimeInfoBean.getTotalTime(), wlTimeInfoBean.getTotalTime())
                            + "/" + TimeUtil.secdsToDateFormat(wlTimeInfoBean.getCurrentTime(), wlTimeInfoBean.getTotalTime()));
                    if( !isSeek && wlTimeInfoBean.getTotalTime() > 0)
                    {
                        seekBar.setProgress(wlTimeInfoBean.getCurrentTime() * 100 / wlTimeInfoBean.getTotalTime());
                    }
                }
            }
        };
        ffmpeg.setWlOnParparedListener(new MyOnParparedListener() {
            @Override
            public void onParpared() {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        progressBar.hide();
                        ffmpeg.start();
                    }
                });

            }
        });
        ffmpeg.setMyOnCompleteListener(new MyOnCompleteListener() {
            @Override
            public void onComplete() {
                Log.e("ffmpeg","播放完成了");
            }
        });
        startBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        progressBar.show();
                        ffmpeg.setSource("rtmp://58.200.131.2:1935/livetv/hunantv");
//                        ffmpeg.setSource("http://vfx.mtime.cn/Video/2019/02/04/mp4/190204084208765161.mp4");
                        ffmpeg.parpared();
                    }
                }).start();
            }
        });

        pauseBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                ffmpeg.pausePlay();
            }
        });

        resumeBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                ffmpeg.resumePlay();
            }
        });

        stopBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                ffmpeg.stop();
            }
        });

        seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                position = progress * ffmpeg.getDuration() / 100;
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                isSeek = true;
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                ffmpeg.seek(position);
                isSeek = false;
            }
        });

        ffmpeg.setMyOnTimeInfoListener(new MyOnTimeInfoListener() {
            @Override
            public void onTimeInfo(TimeInfoBean timeInfoBean) {
                Log.e("timeInfoBean",timeInfoBean.toString());
                Message message = Message.obtain();
                message.what = 1;
                message.obj = timeInfoBean;
                handler.sendMessage(message);
            }
        });

    }
}
