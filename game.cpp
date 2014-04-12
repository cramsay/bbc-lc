#include "game.h"

Game::Game(string id, string home_name, string away_name)
{
    this->id=id;
    this->away_name=away_name;
    this->home_name=home_name;
}

string Game::get_events()
{
    return events;
}

bool Game::reload_events()
{
    string newEvents;
    char line[EVENT_BUFF_LEN];

    string cmd = "php getEvents.php " + id;
    FILE* fp = popen(cmd.c_str(),"r");
    if (fp == NULL) {
      printf("Failed to run command\n" );
      return false;
    }

    while(!feof(fp)){
        fgets(line,EVENT_BUFF_LEN-1,fp);
        newEvents+=line;
    }

    if(events==newEvents)
        return false;
    else {
        events=newEvents;
        return true;
    }
}
