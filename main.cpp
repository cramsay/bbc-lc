#include "mainview.h"
#include <QApplication>
#include "commentarymodel.h"


int main(int argc, char *argv[])
{
    CommentaryModel model;
    QApplication a(argc, argv);
    MainView w(0,&model);
    w.show();

    return a.exec();
}
