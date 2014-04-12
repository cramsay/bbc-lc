#include "competition.h"

Competition::Competition(int id, string name)
{
   this->id = id;
   this->name =name;
    load_games();
}

int Competition::get_id(){
    return id;
}

string Competition::get_name(){
    return name;
}

void Competition::load_games(){

    char buf[GAME_BUFF_LEN];
    int i=0;
    string cmd = "php getGames.php "+to_string(id);

    /* Open the command for reading. */
    FILE* fp = popen(cmd.c_str(), "r");
    if (fp == NULL) {
      printf("Failed to run getGames command\n" );
      return;
    }

    // Ignore the first line (# of games to follow but we don't need this)
    fgets(buf, GAME_BUFF_LEN-1, fp);
    int num_games=atoi(buf);

    /* Read the output a line at a time and fill the game vector*/
    while (!feof(fp)&&i++<num_games){
      fgets(buf, GAME_BUFF_LEN-1, fp);
      string home(buf);
      fgets(buf, GAME_BUFF_LEN-1, fp);
      string away(buf);
      fgets(buf, GAME_BUFF_LEN-1, fp);
      string uid(buf);

      // Remove trailing '\n' s
      home.pop_back();
      away.pop_back();
      uid.pop_back();

      games.push_back(new Game(buf,home,away));
    }

    /* close */
    pclose(fp);

}

vector<Game*> Competition::get_games(){
    return games;
}
