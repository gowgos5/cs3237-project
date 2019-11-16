/*******************************************************************************
 * Copyright (C) 2016 Maxim Integrated Products, Inc., All Rights Reserved.
 * <p>
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * <p>
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * <p>
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 * <p>
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 * <p>
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 * ******************************************************************************
 */
package graphing;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.view.View;

import java.util.ArrayList;

/**
 * Provides simple line graphing for one or multiple lines on a single graph
 */
public class LineGraphing extends View {

    private Bitmap mBitmap;
    private Paint mPaint = new Paint();
    private Canvas mCanvas = new Canvas();

    private float mLastX;
    private float mScale;
    private float mLastValue;
    private float mYOffset;
    /// the width of this view
    private float mWidth;
    /// the height of this view
    private float mHeight;

    private Paint linePaint = new Paint();
    private Paint pointPaint = new Paint();
    private static final int DEFAULT_LINE_STROKE_WIDTH_DP = 2;
    protected float density;
    ArrayList<LinePointCollection> pointCollections;
    float runningMaxY;
    float runningMinY;

    /**
     * Default constuctor for a given context
     * @param context
     */
    public LineGraphing(Context context) {
        super(context);
        init();
    }

    /**
     * Constructor that accomidated a contect and set of attributes
     * @param context
     * @param attrs
     */
    public LineGraphing(Context context, AttributeSet attrs) {
        super(context, attrs);
        this.density = context.getResources().getDisplayMetrics().density;
        init();
    }

    /**
     * Init this view with render paint defaults
     */
    private void init() {
        mPaint.setFlags(Paint.ANTI_ALIAS_FLAG);
        linePaint.setAntiAlias(true);
        linePaint.setStyle(Paint.Style.STROKE);
        linePaint.setStrokeCap(Paint.Cap.ROUND);
        linePaint.setStrokeWidth(2); //dp2px(density, DEFAULT_LINE_STROKE_WIDTH_DP));
        pointPaint.setAntiAlias(true);
        pointPaint.setStyle(Paint.Style.FILL);
        pointCollections = new ArrayList<>();
        mLastX = 0;
    }

    /**
     * Calculate the pixel width for a given density
     * @param density
     * @param dp
     * @return
     */
    public static int dp2px(float density, int dp) {
        if (dp == 0) {
            return 0;
        }
        return (int) (dp * density + 0.5f);
    }

    /**
     * Specify the point collection to use for this graph
     * @param _pointCollection
     */
    public void addPointCollection(LinePointCollection _pointCollection) {
        pointCollections.add(_pointCollection);
    }

    /**
     * Calculate the extremes of all point collections
     * Used to autoscale graph
     */
    public void calculateGraphExtremes() {
        runningMaxY = Float.MIN_VALUE;
        runningMinY = Float.MAX_VALUE;
        for (LinePointCollection pointCollection : pointCollections) {
            if (pointCollection.getRunningMaxY() > runningMaxY)
                runningMaxY = pointCollection.getRunningMaxY();
            if (pointCollection.getRunningMinY() < runningMinY)
                runningMinY = pointCollection.getRunningMinY();
        }
    }

    /**
     * Plot all of the point collections and invalidate the view to force a view refresh
     */
    public void plotPointCollection() {
        calculateGraphExtremes();
        // clear the canvas
        mCanvas.drawColor(Color.WHITE);
        for (LinePointCollection pointCollection : pointCollections) {
            // space out graph on x based on number of points to show and the width of the graph
            linePaint.setColor(pointCollection.getColor());
            mLastValue = pointCollection.getDroppedPointY();
            mLastX = 0;
            for (LinePoint point : pointCollection.getPoints()) {
                plotDataPoint(mCanvas, point, runningMaxY, runningMinY, pointCollection.getMaxCount(), 10);
                drawPoint(mCanvas, point, 3);
            }
        }
        invalidate();
    }

    /**
     * Plot a single point on the graph
     * @param canvas Canvas to paint to
     * @param point Point to plot
     * @param max The calculated max for the set
     * @param min The calculated min for the set
     * @param numberPointsAcross Number of points to plot horizontally on the graph
     * @param margin The top and bottom pixel margin to use
     */
    private void plotDataPoint(Canvas canvas, LinePoint point, float max, float min, float numberPointsAcross, float margin) {
        final Paint paint = linePaint;
        float newX;
        float v;
        float stepX;

        // scale the graph horizontally
        stepX = mWidth / numberPointsAcross;
        newX = mLastX + stepX;
        // scale the graph vertically
        v = max - point.y;
        v = (v / (max - min)) * (mHeight - margin) + margin / 2;

        point.rawX = newX;
        point.rawY = v;

        canvas.drawLine(mLastX, mLastValue, newX, v, paint);
        mLastValue = v;
        mLastX += stepX;
    }

    /**
     * Draw the X,Y point on the graph using a circle
     * @param canvas Canvas to use to when drawing
     * @param point Point that contains the raw pixel X,Y point
     * @param pointRadius The radius of the circular dot to draw
     */
    private void drawPoint(Canvas canvas, LinePoint point,
                           float pointRadius) {
        canvas.drawCircle(point.rawX, point.rawY, pointRadius, pointPaint);
    }

    /**
     * Called by the view framework when the view is initially drawn or the size changes
     * @param w New width of the view
     * @param h New height of the view
     * @param oldw Old width of the view
     * @param oldh Old height of the view
     */
    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        mBitmap = Bitmap.createBitmap(w, h, Bitmap.Config.RGB_565);
        mCanvas.setBitmap(mBitmap);
        mCanvas.drawColor(0xFFFFFFFF);
        mYOffset = h;
        mWidth = w;
        mHeight = h;
        super.onSizeChanged(w, h, oldw, oldh);
    }

    /**
     * Called by the view when a draw is needed
     * @param canvas Drawing canvas for this view
     */
    @Override
    protected void onDraw(Canvas canvas) {
        synchronized (this) {
            if (mBitmap != null) {
                canvas.drawBitmap(mBitmap, 0, 0, null);
            }
        }
    }
}
