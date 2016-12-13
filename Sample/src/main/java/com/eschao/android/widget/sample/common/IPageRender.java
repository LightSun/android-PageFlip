package com.eschao.android.widget.sample.common;

import com.eschao.android.widget.pageflip.OnPageFlipListener;

/**
 * Created by chao on 13/12/2016.
 */

public interface IPageRender extends OnPageFlipListener {

    int getPageNo();
    void release();
    boolean onFingerUp(float x, float y);
    boolean onFingerMove(float x, float y);
    void onDrawFrame();
    void onSurfaceChanged(int width, int height);
    boolean onEndedDrawing(int what);
}
