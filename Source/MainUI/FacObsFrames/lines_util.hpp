#ifndef LINES_UTIL_HPP
#define LINES_UTIL_HPP

#include <opencv2/opencv.hpp>
#include "../pch.h"
struct Line {
    cv::Point start;
    cv::Point end;
    cv::Vec3b color;
};

inline void setPixelFast(cv::Mat& img, int x, int y, const cv::Vec3b& color) {
    uchar* ptr = img.ptr<uchar>(y) + x * 3;
    ptr[0] = color[0];
    ptr[1] = color[1];
    ptr[2] = color[2];
}

void drawLineOptimized(cv::Mat& img, int x0, int y0, int x1, int y1, const cv::Vec3b& color) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    int err = dx - dy;

    while (true) {
        setPixelFast(img, x0, y0, color);

        if (x0 == x1 && y0 == y1) break;

        int e2 = 2 * err;

        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }

        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

#endif // LINES_UTIL_HPP
