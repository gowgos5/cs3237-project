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

import java.util.ArrayList;
import java.util.List;

/**
 * Contains a list of points, the list will only hold a max number of
 * points before the old value is deleted and new values are shifted in
 */
public class LinePointCollection {
    /// list of points in this collection
    List<LinePoint> points;
    /// the list of points has a max count
    int maxCount;
    /// color used to render the points and connecting lines
    int color;
    /// the updated calculated max of all Y values in the list
    float runningMaxY;
    /// the updated calculated min of all Y values in the list
    float runningMinY;

    float pointDroppedOff;

    /**
     * Constructor used to specify the color of the line and tbe max number of points in this collection
     * @param _color Color to use when rendering this collection
     * @param _maxCount Max number of points held before deleting old points and "shifting" in new
     */
    public LinePointCollection(int _color, int _maxCount) {
        color = _color;
        maxCount = _maxCount;
        points = new ArrayList<>();
        pointDroppedOff = 0;
    }

    /**
     * Accessor to get the list of points
     * @return Return the list of points in this collection
     */
    public List<LinePoint> getPoints() {
        return points;
    }

    public float getDroppedPointY() {
        return pointDroppedOff;
    }

    /**
     * Accessor to get the color of this collection
     * @return Color that is used in this collection
     */
    public int getColor() {
        return color;
    }

    /**
     * Calculate the max and min Y extremes for the current cached points
     */
    private void calculateExtremes() {
        runningMaxY = Float.MIN_VALUE;
        runningMinY = Float.MAX_VALUE;
        for (LinePoint point : points) {
            if (point.y > runningMaxY) runningMaxY = point.y;
            if (point.y < runningMinY) runningMinY = point.y;
        }
    }

    /**
     * Accessor to get the min Y value
     * @return Smallest Y value in the list of points
     */
    public float getRunningMinY() {
        return runningMinY;
    }

    /**
     * Accessor to get the max Y value
     * @return Largest Y Value in the list of points
     */
    public float getRunningMaxY() {
        return runningMaxY;
    }

    /**
     * Get the max number of points that will be contained in this collection
     * @return Return the max count
     */
    public float getMaxCount() {
        return maxCount;
    }

    /**
     * Add a Y point value in this collection...
     * note currently we only care about the Y value,
     * the x running values are programically calculated
     * @param y The Y value to use for this point
     */
    public void addpoint(float y) {
        LinePoint point = new LinePoint();
        point.x = 0;
        point.y = y;
        points.add(point);
        if (points.size() > maxCount) {
            // get the point removed for plotting first point
            pointDroppedOff = points.get(0).rawY;
            points.remove(0);
        }
        calculateExtremes();
    }
}
