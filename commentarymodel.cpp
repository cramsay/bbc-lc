#include "commentarymodel.h"
#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define SLEEP(d) usleep(d*1000)
#define SLEEP_DATA 10000

CommentaryModel::CommentaryModel()
{
    get_events_lock=false;
    loadComps();
    pthread_t thrd_data;
    pthread_create(&thrd_data,NULL,getEvents_thrd,(void*)this);
}

double CommentaryModel::getMillis()
{
    struct timeval  tv;
    gettimeofday(&tv, NULL);

    return (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 ;
}

vector<Competition*> CommentaryModel::getComps()
{
    return comps;
}

void CommentaryModel::loadComps(){

    ifstream comp_file ("competitions.lst");
    string line;
    int c_id=0;
    string c_name;

    if (!comp_file.is_open()) {
      printf("Failed to open the competitions.lst file\n" );
      return;
    }

    while(getline(comp_file,line))
    {
        c_name = line.substr(line.find(' ')+1,line.length());
        c_id = atoi(line.substr(0,line.find(' ')).c_str());
        comps.push_back(new Competition(c_id,c_name));
    }

}

 void CommentaryModel::registerObserver(IObserver* observer)
 {
     observers.push_back(observer);
     return;
 }

 void CommentaryModel::registerWatchedGame(Game* game)
 {
     watchedGames.push_back(game);
     getEvents();
 }

 void CommentaryModel::unregisterWatchedGame(Game* game)
 {
     for(int i=0;i<watchedGames.size();i++){
        Game* g = watchedGames.at(i);
         if(g==game){
            watchedGames.erase(watchedGames.begin()+i);
            break;
         }
     }
 }

 void* CommentaryModel::getEvents_thrd(void * model)
 {
   CommentaryModel* m = (CommentaryModel*) model;

   while(true){
    //Store starting time
    double startMS=getMillis();

     m->getEvents();

     //Sleep for remaining time
     double r = startMS+SLEEP_DATA-getMillis();
     if(r>0)
         SLEEP(r);
   }
   return NULL;
 }

 void CommentaryModel::getEvents(){
     bool newData=false;

     if(get_events_lock)
         return;
     else
         get_events_lock=true;


     //Reload each tracked game event list
     for(Game* g :watchedGames)
        if(g->reload_events())
            newData=true;

     //Inform the observers of any updates
     if(newData)
         notifyObservers();

     get_events_lock = false;
 }

 void CommentaryModel::notifyObservers()
 {
     for (IObserver* ob : observers)
         ob->refreshObserver();
 }
