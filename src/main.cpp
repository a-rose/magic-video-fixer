#include <iostream>
#include "Sequence.h"
#include "settings.h"

using namespace cv;
using namespace std;

int main(int argc, char* argv[]) {
    cout << "Magic Video Fixer !" << endl;

	Settings settings = loadAppSettings("config.txt");
    Sequence seq(settings);
    list<Frame> frameList;

    seq.Load();
    frameList = seq.Solve();
    seq.Write(frameList);

    cout << "Exiting normally" << endl;
    return 0;
}
