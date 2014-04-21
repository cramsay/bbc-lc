#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QMainWindow>
#include "commentarymodel.h"
#include "game.h"
namespace Ui {
class MainView;
}

class MainView : public QMainWindow, IObserver
{
    Q_OBJECT

public:
    explicit MainView(QWidget *parent = 0,CommentaryModel *model=0);
    ~MainView();
    virtual void refreshObserver() override;

private:
    Ui::MainView *ui;
    CommentaryModel *model;
    Game *selectedGame;

private slots:
    void selectedGameChanged();
    void exitApp();
};

#endif // MAINVIEW_H
