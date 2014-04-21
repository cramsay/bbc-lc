#include "mainview.h"
#include "ui_mainview.h"
#include "commentarymodel.h"

MainView::MainView(QWidget *parent, CommentaryModel* model) :
    QMainWindow(parent),
    ui(new Ui::MainView)
{
    ui->setupUi(this);

    //Make signal/slot connections
    connect(ui->btnExit,SIGNAL(clicked()),this,SLOT(exitApp()));
    connect(ui->treeGames,SIGNAL(itemSelectionChanged()),this,SLOT(selectedGameChanged()));

    //Handle model registration
    this->model = model;
    model->registerObserver(this);

    //Populate game list tree
    QTreeWidgetItem* itm;
    for(Competition* c : model->getComps()){
        itm = new QTreeWidgetItem;
        itm->setText(0,QString::fromStdString(c->get_name()));

        for(Game* g: c->get_games()){
            QTreeWidgetItem* itm2 = new QTreeWidgetItem;
            itm2->setText(0,QString::fromStdString(g->home_name + " vs " + g->away_name));
            itm2->setToolTip(0,QString::fromStdString(g->id));
            itm->addChild(itm2);
        }

        if(itm->childCount()==0)
            itm->setDisabled(true);

        ui->treeGames->insertTopLevelItem(ui->treeGames->topLevelItemCount(),itm);
    }
}

MainView::~MainView()
{
    delete ui;
}

void MainView::exitApp()
{
    close();
}

void MainView::selectedGameChanged()
{
    //Unregister previous game
    if(selectedGame)
        model->unregisterWatchedGame(selectedGame);
    //Register new game
    QList<QTreeWidgetItem*> selects = ui->treeGames->selectedItems();
    if(selects.count()>0){
        string gid = selects.first()->toolTip(0).toStdString();
        for(Competition* c : model->getComps())
            for(Game* g : c->get_games())
                if(g->id == gid){
                    selectedGame=g;
                    model->registerWatchedGame(selectedGame);
                    refreshObserver();
                    break;
                }
    }
}

 void MainView::refreshObserver(){
    if(selectedGame)
        ui->comText->setText(QString::fromStdString(selectedGame->get_events()));
}
