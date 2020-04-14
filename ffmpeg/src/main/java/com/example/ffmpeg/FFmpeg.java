package com.example.ffmpeg;

import android.media.MediaCodec;
import android.media.MediaFormat;
import android.view.Surface;

import com.example.ffmpeg.bean.TimeInfoBean;
import com.example.ffmpeg.listener.MyOnCompleteListener;
import com.example.ffmpeg.listener.MyOnParparedListener;
import com.example.ffmpeg.listener.MyOnTimeInfoListener;
import com.example.ffmpeg.opengl.MyGLSurfaceView;
import com.example.ffmpeg.opengl.MyRender;
import com.example.ffmpeg.util.MyVideoSupportUitl;

import java.nio.ByteBuffer;

public  class FFmpeg {
    private MyOnParparedListener myOnParparedListener;
    private MyOnTimeInfoListener myOnTimeInfoListener;
    private MyOnCompleteListener myOnCompleteListener;
    private static String source;//数据源
    private int duration = 0;
    TimeInfoBean timeInfoBean;

    private MediaFormat mediaFormat;
    private MediaCodec mediaCodec;
    private Surface surface;
    private MediaCodec.BufferInfo info;
    static {
        System.loadLibrary("native-lib");
        System.loadLibrary("avcodec-57");
        System.loadLibrary("avdevice-57");
        System.loadLibrary("avfilter-6");
        System.loadLibrary("avformat-57");
        System.loadLibrary("avutil-55");
        System.loadLibrary("postproc-54");
        System.loadLibrary("swresample-2");
        System.loadLibrary("swscale-4");
    }

    private MyGLSurfaceView myGLSurfaceView;
    public  void setMyGLSurfaceView(MyGLSurfaceView myGLSurfaceView) {
        this.myGLSurfaceView = myGLSurfaceView;
        myGLSurfaceView.getWlRender().setOnSurfaceCreateListener(new MyRender.OnSurfaceCreateListener() {
            @Override
            public void onSurfaceCreate(Surface s) {
                if(surface == null)
                {
                    surface = s;
                }
            }
        });
    }

    /**
     * 数据源url
     * @param source
     */
    public void setSource(String source)
    {
        this.source = source;
    }

// 准备
    public void parpared()
    {
        new Thread(new Runnable() {
            @Override
            public void run() {
                initAvCodec(source);
            }
        }).start();

    }
    public void setWlOnParparedListener(MyOnParparedListener myOnParparedListener)
    {
        this.myOnParparedListener = myOnParparedListener;
    }

    public void onCallParpared()
    {
        if(myOnParparedListener != null)
        {
            myOnParparedListener.onParpared();
        }
    }

    // 开始
    public void start()
    {
        new Thread(new Runnable() {
            @Override
            public void run() {
                startPlay();
            }
        }).start();
    }



//时间
    public void onCallTimeInfo(int currentTime, int totalTime)
    {
        if(myOnTimeInfoListener != null)
        {
            if(timeInfoBean == null)
            {
                timeInfoBean = new TimeInfoBean();
            }
            duration = totalTime;
            timeInfoBean.setCurrentTime(currentTime);
            timeInfoBean.setTotalTime(totalTime);
            myOnTimeInfoListener.onTimeInfo(timeInfoBean);
        }
    }
    public void setMyOnTimeInfoListener(MyOnTimeInfoListener myOnTimeInfoListener) {
        this.myOnTimeInfoListener = myOnTimeInfoListener;
    }

    public int getDuration() {
        return duration;
    }

    // seek
    public void seek(int secds)
    {
        seekPlay(secds);
    }

    // 完成
    public void onCallComplete()
    {
        if(myOnCompleteListener != null)
        {
            stopPlay();
            myOnCompleteListener.onComplete();
        }
    }

    public void setMyOnCompleteListener(MyOnCompleteListener myOnCompleteListener) {
        this.myOnCompleteListener = myOnCompleteListener;
    }

// 是否支持硬解码
    public boolean onCallIsSupportMediaCodec(String ffcodecname)
    {
        return MyVideoSupportUitl.isSupportCodec(ffcodecname);
    }

    /**
     * 初始化MediaCodec
     * @param codecName
     * @param width
     * @param height
     * @param csd_0
     * @param csd_1
     */
    public void initMediaCodec(String codecName, int width, int height, byte[] csd_0, byte[] csd_1)
    {
        if(surface != null)
        {
            try {
                myGLSurfaceView.getWlRender().setRenderType(MyRender.RENDER_MEDIACODEC);
                String mime = MyVideoSupportUitl.findVideoCodecName(codecName);
                mediaFormat = MediaFormat.createVideoFormat(mime, width, height);
                mediaFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, width * height);
                mediaFormat.setByteBuffer("csd-0", ByteBuffer.wrap(csd_0));
                mediaFormat.setByteBuffer("csd-1", ByteBuffer.wrap(csd_1));
                mediaCodec = MediaCodec.createDecoderByType(mime);

                info = new MediaCodec.BufferInfo();
                mediaCodec.configure(mediaFormat, surface, null, 0);
                mediaCodec.start();

            }
            catch (Exception e)
            {
                e.printStackTrace();
            }
        }
    }

   //渲染mediacodec
    public void decodeAVPacket(int datasize, byte[] data)
    {
        if(surface != null && datasize > 0 && data != null && mediaCodec != null)
        {
            try
            {
                int intputBufferIndex = mediaCodec.dequeueInputBuffer(10);
                if(intputBufferIndex >= 0)
                {
                    ByteBuffer byteBuffer = mediaCodec.getInputBuffers()[intputBufferIndex];
                    byteBuffer.clear();
                    byteBuffer.put(data);
                    mediaCodec.queueInputBuffer(intputBufferIndex, 0, datasize, 0, 0);
                }
                int outputBufferIndex = mediaCodec.dequeueOutputBuffer(info, 10);
                while(outputBufferIndex >= 0)
                {
                    mediaCodec.releaseOutputBuffer(outputBufferIndex, true);
                    outputBufferIndex = mediaCodec.dequeueOutputBuffer(info, 10);
                }
            }
            catch (Exception e)
            {
                e.printStackTrace();
            }

        }
    }

    public void stop()
    {
        new Thread(new Runnable() {
            @Override
            public void run() {
                stopPlay();
                releaseMediacodec();
            }
        }).start();
    }

    private void releaseMediacodec()
    {
        if(mediaCodec != null)
        {
            try
            {
                mediaCodec.flush();
                mediaCodec.stop();
                mediaCodec.release();
            }
            catch(Exception e)
            {
                //e.printStackTrace();
            }
            mediaCodec = null;
            mediaFormat = null;
            info = null;
        }

    }
    //渲染YUV
    public void onCallRenderYUV(int width, int height, byte[] y, byte[] u, byte[] v)
    {
        if(myGLSurfaceView != null)
        {
            myGLSurfaceView.getWlRender().setRenderType(MyRender.RENDER_YUV);
            myGLSurfaceView.setYUVData(width, height, y, u, v);
        }
    }
    public  native void  initAvCodec(String source);
    public  native void  startPlay();
    public  native void  pausePlay();
    public  native void  resumePlay();
    public  native void  stopPlay();
    public  native void  seekPlay(int secds);



}