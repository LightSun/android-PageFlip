package com.eschao.android.widget.pageflip.jni;

public class PageFlipLib {

    protected static OnPageFlipListener mListener = null;

    public static void setPageFlipListener(OnPageFlipListener listener) {
        mListener = listener;
    }

    public static boolean canFlipForward() {
        return (mListener != null && mListener.canFlipForward());
    }

    public static boolean canFlipBackward() {
        return (mListener != null && mListener.canFlipBackward());
    }

    public static native boolean init();
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
