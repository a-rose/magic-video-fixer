#pragma once

#include "SolverImplem.h"

class NaiveSolver : public SolverImplem {
public:
    NaiveSolver(Settings s)
        : SolverImplem(s)
    {}

    list<Frame> Solve(map<int, Frame>& frames);

private:
    list<Frame> BuildSequence(map<int, Frame>& frames, vector<int>& available_frames);
};