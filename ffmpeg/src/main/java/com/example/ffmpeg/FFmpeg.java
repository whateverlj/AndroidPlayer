package com.example.ffmpeg;

import com.example.ffmpeg.opengl.MyGLSurfaceView;

public  class FFmpeg {

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

    private  MyGLSurfaceView myGLSurfaceView;
    public  void setMyGLSurfaceView(MyGLSurfaceView myGLSurfaceView) {
        this.myGLSurfaceView = myGLSurfaceView;
    }

    public void onCallRenderYUV(int width, int height, byte[] y, byte[] u, byte[] v)
    {
        if(myGLSurfaceView != null)
        {
            myGLSurfaceView.setYUVData(width, height, y, u, v);
        }
    }
    public  native void  initAvCodec(String source);
    public  native void  startPlay();

}