package com.example.module_rtsp.widget;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceView;

/**
 * create by liaohailong
 * 2025/1/12 11:16
 * desc:
 */
public class AutoFitSurfaceView extends SurfaceView {

    private static final String TAG = AutoFitSurfaceView.class.getSimpleName();
    private float aspectRatio = 0f;

    public AutoFitSurfaceView(Context context) {
        this(context, null);
    }

    public AutoFitSurfaceView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    /**
     * Constructor that takes a context, attribute set, and default style.
     *
     * @param context  The context associated with this view.
     * @param attrs    The attribute set.
     * @param defStyle The default style to apply.
     */
    public AutoFitSurfaceView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
    }

    /**
     * Sets the aspect ratio for this view. The size of the view will be
     * measured based on the ratio calculated from the parameters.
     *
     * @param width  Camera resolution horizontal size.
     * @param height Camera resolution vertical size.
     */
    public void setAspectRatio(int width, int height) {
        if (width <= 0 || height <= 0) {
            throw new IllegalArgumentException("Size cannot be negative");
        }
        aspectRatio = (float) width / height;
        getHolder().setFixedSize(width, height);
        requestLayout();
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        int width = MeasureSpec.getSize(widthMeasureSpec);
        int height = MeasureSpec.getSize(heightMeasureSpec);

        if (aspectRatio == 0f) {
            setMeasuredDimension(width, height);
        } else {
            // Performs center-crop transformation of the camera frames
            float actualRatio = (width > height) ? aspectRatio : 1f / aspectRatio;
            int newWidth = width;
            int newHeight = Math.round(width * actualRatio);

            Log.d(TAG, "Measured dimensions set: " + newWidth + " x " + newHeight);
            setMeasuredDimension(newWidth, newHeight);
        }
    }
}