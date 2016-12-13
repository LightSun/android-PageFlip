package com.eschao.android.widget.pageflip.jni;

import android.graphics.Bitmap;

public class PageFlipLib {

    public static int BEGIN_FLIP        = 0;
    public static int FORWARD_FLIP      = 1;
    public static int BACKWARD_FLIP     = 2;
    public static int RESTORE_FLIP      = 3;
    public static int END_FLIP          = 4;
    public static int END_WITH_FORWARD  = 5;
    public static int END_WITH_BACKWARD = 6;
    public static int END_WITH_RESTORE  = 7;

    static {
        System.loadLibrary("libpageflip");
    }

    protected static OnPageFlipListener mListener = null;

    public static void setListener(OnPageFlipListener listener) {
        mListener = listener;
    }

    public static boolean onFingerMove(float x, float y) {
        final boolean canForward = (mListener != null &&
                                    mListener.canFlipForward());
        final boolean canBackward = (mListener != null &&
                                     mListener.canFlipBackward());
        return onFingerMove(x, y, canForward, canBackward);
    }

    public static boolean onFingerUp(float x, float y, int duration) {
        final boolean canForward = (mListener != null &&
                                    mListener.canFlipForward());
        final boolean canBackward = (mListener != null &&
                                     mListener.canFlipBackward());
        return onFingerUp(x, y, duration, canForward, canBackward);
    }

    public static native boolean init();
    public static native boolean cleanup();
    public static native int enableAutoPage(boolean isAuto);
    public static native boolean isAutoPageEnabled();
    public static native int enableClickToFlip(boolean enable);
    public static native int setWidthRatioOfClickToFlip(float ratio);
    public static native int setPixelsOfMesh(int pixelsOfMesh);
    public static native int setSemiPerimeterRatio(float ratio);
    public static native int setMaskAlphaOfFold(int alpha);
    public static native int setShadowColorOfFoldEdges(float startColor,
                                                       float startAlpha,
                                                       float endColor,
                                                       float endAlpha);
    public static native int setShadowColorOfFoldBase(float startColor,
                                                      float startAlpha,
                                                      float endColor,
                                                      float endAlpha);
    public static native int setShadowWidthOfFoldEdges(float min,
                                                       float max,
                                                       float ratio);
    public static native int setShadowWidthOfFoldBase(float min,
                                                      float max,
                                                      float ratio);
    public static native int getPixelsOfMesh();
    public static native int getSurfaceWidth();
    public static native int getSurfaceHeight();
    public static native int getPageWidth(boolean isFirstPage);
    public static native int getPageHeight(boolean isFirstPage);
    public static native int isLeftPage(boolean isFirstPage);
    public static native int isRightPage(boolean isFirstPage);
    public static native int onSurfaceCreated();
    public static native int onSurfaceChanged(int width, int height);

    public static native boolean animating();
    public static native boolean canAnimate(float x, float y);
    public static native boolean isAnimating();
    public static native int abortAnimating();

    public static native int drawFlipFrame();
    public static native int drawPageFrame();

    public static native int getFlipState();
    public static native boolean hasSecondPage();
    public static native boolean isFirstTextureSet(boolean isFirstPage);
    public static native boolean isSecondTextureSet(boolean isFirstPage);
    public static native boolean isBackTextureSet(boolean isFirstPage);
    public static native int setFirstTexture(boolean isFirstPage, Bitmap b);
    public static native int setSecondTexture(boolean isFirstPage, Bitmap b);
    public static native int setBackTexture(boolean isFirstPage, Bitmap b);
    public static native int setGradientLightTexture(Bitmap b);
    public static native int setFirstTextureWithSecond(boolean isFirstPage);
    public static native int setSecondTextureWithFirst(boolean isFirstPage);
    public static native int swapTexturesWith(boolean isFirstPage);
    public static native int deleteUnusedTextures(boolean isFirstPage);

    public static native boolean onFingerDown(float x, float y);
    private static native boolean onFingerMove(float x, float y,
                                               boolean canForward,
                                               boolean canBackward);
    private static native boolean onFingerUp(float x, float y, int duration,
                                             boolean canForward,
                                             boolean canBackward);
}
