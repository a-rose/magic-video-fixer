#pragma once

#include "SolverImplem.h"

class ChallengeSolver : public SolverImplem {
public:
    ChallengeSolver(Settings s)
        : SolverImplem(s)
    {}

    list<Frame> Solve(map<int, Frame>& frames);

private:
    void FilterCorruptedFrames(map<int, Frame>& frames);
    list<Frame> BuildSequence(map<int, Frame>& frames, map<int, int>& assignment, vector<int>& indexes);

    void RemoveFrames(map<int, Frame>& frames, const vector<uint>& rejected_frames);
    vector<vector<double>> GetCostMatrix(map<int, Frame>& frames);
    map<int, int> ConvertAssignment(vector<int>& assignment, vector<int>& indexes);
};