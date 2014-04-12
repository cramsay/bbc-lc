#ifndef COMPETITION_H
#define COMPETITION_H

#include <string>
#include <vector>
#include "game.h"
using namespace std;

#define GAME_BUFF_LEN 512

class Competition
{

protected:
    string name;
    int id;
    vector<Game*> games;
    void load_games();

public:
    Competition(int id,string name);
    string get_name();
    vector<Game*> get_games();
    int get_id();

};

#endif // COMPETITION_H
