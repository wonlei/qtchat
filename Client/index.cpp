
#include "index.h"
#include "ui_index.h"
#include<QDebug>

Friend *Index::getfriend()
{
    return ui->friendpage;
}

File *Index::getfile()
{
    return ui->filepage;
}

Index::Index(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Index)
{
    ui->setupUi(this);

}

Index::~Index()
{
    delete ui;
}

Index &Index::getInstance()
{
    static Index instance;
    return instance;
}

void Index::on_friend_PB_clicked()
{
    ui->stackedWidget->setCurrentWidget(getfriend());
}

void Index::on_file_PB_clicked()
{
    ui->stackedWidget->setCurrentWidget(getfile());
}
