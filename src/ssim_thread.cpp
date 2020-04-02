#include "ssim_thread.h"
#include "ssim.h"

void ssim_thread(Frame& frame1, Frame& frame2) {
    Scalar ssimScalar = getMSSIM(frame1.GetImageLuma(), frame2.GetImageLuma());
    double ssimY = ssimScalar[0];

    // Set ssim for both frames at once
    frame1.AddCandidate(frame2.GetIndex(), ssimY);
    frame2.AddCandidate(frame1.GetIndex(), ssimY);
}
