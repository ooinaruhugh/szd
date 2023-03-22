#include <iostream>
#include <vector>
#include "util.h"

int main(int argc, char const ** argv) {
    using namespace std;
    vector<int> a{1,2,3,4};
    vector<int> b{-1,-2};

    appendVectorToVector(a,b);
    for (int i : a) { 
        cout << i << ' ';
    }
    cout << endl;

    return 0;
}
