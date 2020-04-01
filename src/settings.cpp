#include <fstream>
#include <sstream>
#include "settings.h"


Settings loadAppSettings(const string& file) {
    Settings settings;
    ifstream config(file);
    string line;
    
    while(getline(config, line)) {
        string param;
        stringstream iss(line);
        iss >> param;

        if(param == "mode") {
            iss >> settings.mode;
        } else if (param == "algo") {
            iss >> settings.algo;
        } else if (param == "input_file") {
            iss >> settings.input_file;
        } else if (param == "output_file") {
            iss >> settings.output_file;
        } else if (param == "ssim_data_file") {
            iss >> settings.ssim_data_file;
        } else if (param == "first_frame") {
            iss >> settings.first_frame;
        } else if (param == "frames_limit") {
            iss >> settings.frames_limit;
        } else if (param == "minimum_ssim") {
            iss >> settings.minimum_ssim;
        } else if (param == "detection_threshold") {
            iss >> settings.detection_threshold;
        }
    }
    return settings;
}