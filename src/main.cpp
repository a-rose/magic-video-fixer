#include <iostream>
#include "Sequence.h"

using namespace cv;
using namespace std;

int main(int argc, char* argv[]) {
    cout << "Magic Video Fixer !" << endl;

    Sequence seq("config.txt");
    list<Frame> frameList;

    try {
        seq.Load();
        frameList = seq.Solve();

        if(frameList.size() > 0) {
            cout << "Sequence complete:" << endl;
            for(Frame& frame : frameList) {
                cout << frame.GetIndex() << " -> ";
            }
            cout << endl << endl;

            seq.Write(frameList);
        } else {
            cout << "The reconstructed sequence is empty" << endl;
        }
    } catch (const exception& ex) {
        cout << "Exception occured: " << ex.what() << endl;
    } catch (const string& ex) {
        cout << "Exception occured: " << ex << endl;
    } catch (...) {
        cout << "Uncaught exception occured !" << endl;
    }

    cout << "Exiting normally" << endl;
    return 0;
}
