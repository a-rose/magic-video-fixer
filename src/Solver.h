#pragma once

#include <list>
#include <vector>
#include "Frame.h"
#include "settings.h"

using namespace std;

class Solver {
protected:
    Settings settings;

public:
    Solver(Settings s)
        : settings(s)
    {}
    
    virtual list<Frame> Solve(map<int, Frame>& frames) = 0;
};
