#ifndef GAME_H
#define GAME_H

#include <string>
using namespace std;

#define EVENT_BUFF_LEN 1024

class Game
{
protected:
    string events;

public:
    Game(string id, string home_name, string away_name);
    string home_name,away_name,id;
    string get_events();
    bool reload_events();
};

#endif // GAME_H
