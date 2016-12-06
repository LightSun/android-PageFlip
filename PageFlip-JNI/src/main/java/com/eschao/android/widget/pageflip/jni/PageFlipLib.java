package com.eschao.android.widget.pageflip.jni;

public class PageFlipLib {

    public static native boolean onSurfaceCreated();
    public static native boolean onSurfaceChanged(int width, int height);

    public static native boolean onFingerDown(float x, float y);
    public static native boolean onFingerMove(float x, float y);
    public static native boolean onFingerUp(float x, float y, int duration);

    public static native boolean animating();
    public static native boolean isAnimating();
    public static native void abortAnimating();

    public static native void drawFlipFrame();
    public static native void drawPageFrame();

}
