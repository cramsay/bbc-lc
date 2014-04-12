#include <iostream>
#include "commentarymodel.h"
#include "competition.h"
#include "cursesview.h"
#include <vector>

using namespace std;

int main()
{
    cout << "Grabbing games list from the web...\nPlease be patient!" << endl;
    CommentaryModel c;
    CursesView cv(&c);

    while(true);
    return 0;
}

