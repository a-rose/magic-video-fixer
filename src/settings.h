#pragma once

#include <string>

using namespace std;

// Default settings
struct Settings{
    // Mode:
    //  0: Load frames from input_file, compute SSIM and write it to ssim_data_file, write the sequence to output_file
    //  1: Load frames from input_file, Load SSIM from ssim_data_file, write the sequence to output_file
    //  2: Load SSIM from ssim_data_file
    int mode = 2;

    // Algorithm (see README.md)
    //  0: challenge algo
    //  1: naive algo
    int algo = 1;

    // Video to fix
    string input_file = "corrupted_video.mp4";
    
    // Output file to write
    string output_file = "fixed_video.mp4";
    
    // SSIM backup file. Using mode 1 and 2, reading SSIM from this file
    // avoids computing it again.
    string ssim_data_file = "ssim_y.txt";

    // Number of threads to compute SSIM in parallel
    int nb_threads_ssim = 6;
    
    // Limits the number of frames to process
    int frames_limit = 999;

    // First frame in the sequence
    int first_frame = 30;
    
    // Minimum mean SSIM to consider a frame is not corrupted
    double minimum_ssim = 0.65;

    // Normal distribution quantile to detect a frame as a good candidate
    double detection_threshold = 0.75;
};

Settings loadAppSettings(const string& config_file);