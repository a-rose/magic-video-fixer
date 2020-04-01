#pragma once

#include "Frame.h"
#include "settings.h"

class Sequence {
private:
    Settings settings;
    map<int, Frame> frames;

    // Video properties
    double fps;
    int width, height;

public:
    Sequence(Settings settings)
        : settings(settings)
    {}

    void Load();
    list<Frame> Solve();
    list<Frame> Solve2();
    void Write(list<Frame>&);

private:
    void LoadFrames();
    void ConvertFrames();
    void ComputeSSIM();
    void LoadSSIM();

    void FilterCorruptedFrames();
    vector<int> FilterCorruptedFrames2();

    void RemoveFrames(const vector<uint>& frames);
    vector<vector<double>> GetCostMatrix();
    map<int, int> ConvertAssignment(vector<int>& assignment, vector<int>& indexes);

    list<Frame> BuildSequence(map<int, int>& assignment, vector<int>& indexes);
    list<Frame> BuildSequence2(vector<int> availableFrames);
    void ListToVideo(list<Frame>& seq);
};
