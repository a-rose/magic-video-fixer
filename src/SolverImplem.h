#pragma once

#include <list>
#include <vector>
#include "Frame.h"
#include "settings.h"

class SolverImplem {
protected:
    Settings settings;

public:
    SolverImplem(Settings s)
        : settings(s)
    {}

    virtual ~SolverImplem() {}
    
    virtual list<Frame> Solve(map<int, Frame>& frames) = 0;
};