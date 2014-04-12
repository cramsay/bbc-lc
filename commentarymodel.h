#ifndef COMMENTARYMODEL_H
#define COMMENTARYMODEL_H

#include <string>
#include <vector>
#include "competition.h"
#include "game.h"
#include "iobserver.h"



using namespace std;

class CommentaryModel
{
public:
    CommentaryModel();
    vector<Competition*> getComps();
    void registerObserver(IObserver* observer);
    void registerWatchedGame(Game* game);
    void unregisterWatchedGame(Game* game);

protected:
    bool get_events_lock;
    vector<IObserver*> observers;
    vector<Game*> watchedGames;
    vector<Competition*> comps;
    void loadComps();
    static double getMillis();
    void getEvents();
    static void *getEvents_thrd(void *model);
    void notifyObservers();
};

#endif // COMMENTARYMODEL_H
