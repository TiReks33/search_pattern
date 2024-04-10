#include "dockwidget.h"
#include "ui_dockwidget.h"
#include <QDebug>

DockWidget::DockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::DockWidget)
{
    ui->setupUi(this);
    ui->radioButton_2->setChecked(true);
    //qDebug() << ui->radioButton->isChecked() <<"::"<< ui->radioButton_2->isChecked();
}

DockWidget::~DockWidget()
{
    delete ui;
}

bool DockWidget::is_option_str()
{
    return ui->radioButton->isChecked();
}

bool DockWidget::is_option_wstr()
{
    return ui->radioButton_2->isChecked();
}
