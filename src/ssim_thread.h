#pragma once

#include <thread>
#include "Frame.h"

using namespace cv;

void ssim_thread(Frame& frame1, Frame& frame2);
