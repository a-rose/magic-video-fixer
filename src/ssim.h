#pragma once

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Scalar getMSSIM( const Mat& i1, const Mat& i2);