#include "cursesview.h"
#include "competition.h"
#include "game.h"
#include <vector>
#include <iostream>
#include <ncurses.h>
#include <pthread.h>

CursesView::CursesView(CommentaryModel* model)
{
    this->model = model;
    selected_game=0;

    //Grab competitions list
    vector<Competition*> comps = model->getComps();
    Competition* c = comps.at(1);
    g=c->get_games().at(0);

    //Init ncurses settings
    mainwin = initscr();			/* Start curses mode 		*/
    raw();				/* Line buffering disabled	*/
    keypad(stdscr, TRUE);		/* We get F1, F2 etc..		*/
    noecho();			/* Don't echo() while we do getch */

    //Init sub windows
    menuwin = subwin(mainwin,40,40,0,1);
    box(menuwin,0,0);
    mvwprintw(menuwin,0,1,"Games List");
    textwin = subwin(mainwin,40,82,0,42);
    box(textwin,0,0);
    mvwprintw(textwin,0,1,"Commentary");

    int y=1;
    for(Competition* comp : comps)
        for (Game* game : comp->get_games())
            mvwprintw(menuwin,y++,1,(game->home_name+"vs "+game->away_name).c_str());
    refresh();

    //Start up input handling thread
    pthread_t ui_handler;
    pthread_create(&ui_handler,NULL,handle_input_thrd,(void*)this);

    //Register to the CommentaryModel (must be ready to update
    //as soon as you call registerObserver!)
    model->registerObserver(this);
    model->registerWatchedGame(g);
}

CursesView::~CursesView()
{
    delwin(mainwin);
    delwin(menuwin);
    delwin(textwin);
    endwin();
}

void CursesView::refreshObserver()
{
    wclear(textwin);
   mvwprintw(textwin,1,1,g->get_events().c_str());
   box(textwin,0,0);
   mvwprintw(textwin,0,1,("Commentary: "+g->home_name+"vs "+g->away_name).c_str());
   wrefresh(textwin);
   refresh();
}

void* CursesView::handle_input_thrd(void* view)
{
    CursesView* v = (CursesView*) view;
    while(true){
        int c = getch();
        if(KEY_UP==c)
            v->selected_game--;
        else if(KEY_DOWN==c)
            v->selected_game++;

        v->model->unregisterWatchedGame(v->g);
        int i=0;
        for(Competition* comp : v->model->getComps())
            for (Game* game : comp->get_games())
                if(v->selected_game==i++){
                    v->g=game;
                    v->model->registerWatchedGame(v->g);
                    break;
                }
    }

    return NULL;
}
