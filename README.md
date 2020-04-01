# Magic Video Fixer

by Antoine Rose (https://github.com/a-rose)

## Deps
`cmake make opencv boost`

## Build
Just run `./compile.sh`. Linux only!

To start the program: `./build/magic-video-fixer`.

## Configuration

`config.txt` contains the configurable parameters. Their description can be found in settings.h.

By default, the program will run in mode 1 which means it will load SSIM values from `ssim_y.txt` instead of computing them again. These values are for a specific input video so don't expect the output video to be correct without running mode 0 first. You can change the mode in `config.txt`. 

### `algorithm`

There are two implementations to fix the video:
 - "Challenge algo" uses  a normal distribution + Hungarian algorithm to filter corrupted frames and find out the sequence. This implementation is not finished; it outputs a reconstructed file, but frames are not in order and some corrupted frames are still present in the video.
 - "Naive algo" is much simpler, it uses mean SSIM to find out which frames have low similarity with the rest of the video. It then creates the sequence by associating each frame with its best match in terms of similarity. This get good results, the video can be recreated if we give the first frame of the sequence (`first_frame` in config.txt) but occasionnaly it leaves corrupted images.

The naive algorithm works best with `minimum_ssim = 0.65` and `detection_threshold = 0.78`. It is enabled by default in `config.txt`

## Resources

SSIM algorithm source: https://docs.opencv.org/3.4/d5/dc4/tutorial_video_input_psnr_ssim.html

Hungarian algorithm source: https://github.com/mcximing/hungarian-algorithm-cpp

## TODO

- [ ] Compute SSIM on the Luma channel only. Currently, frames are converted to YUV and only the Y channel's SSIM is kept as it is the most relevant to analyse the image structure. But all channels' SSIM are computed which is a waste of time.
- [ ] Import Hungarian from the repo instead of copying the files
- [ ] docs

## Optimisation

Speedup measured against initial commit

| Improvement | Speedup |
| ------------- |:-------------:| 
| Implemented multi-threading in ComputeSSIM(). Using 6 threads by default.      | x1.8 | 