#ifndef CURSESVIEW_H
#define CURSESVIEW_H

#include "commentarymodel.h"
#include "game.h"
#include <ncurses.h>

class CursesView : public IObserver
{
public:
    CursesView(CommentaryModel* model);
    virtual ~CursesView();

protected:
    Game* g;
    int selected_game;
    CommentaryModel* model;
    WINDOW *mainwin,*menuwin,*textwin;
    virtual void refreshObserver() override;
    static void* handle_input_thrd(void* view);
};

#endif // CURSESVIEW_H
