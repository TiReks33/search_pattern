#include "hints.h"
#include "ui_hints.h"

Hints::Hints(QWidget *parent) :
    QStackedWidget(parent),
    ui(new Ui::Hints)
{
    ui->setupUi(this);
    this->setCurrentWidget(ui->page);
}

Hints::~Hints()
{
    delete ui;
}

void Hints::on_pushButton_2_clicked()
{
    this->setCurrentWidget(ui->page_2);
}

void Hints::on_pushButton_clicked()
{
    this->setCurrentWidget(ui->page);
}
