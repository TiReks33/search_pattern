#include "dialog.h"
#include "mainwindow.h"
#include "ui_dialog.h"
#include <QMessageBox>
//#include <QFile>
//#include <QTextStream>
#include "result.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->textEdit->setText("a");
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_buttonBox_accepted()
{
    QString str = ui->textEdit->toPlainText();
    if(str=="")
        QMessageBox::warning(this,"Try again", "Pattern of search is empty.");
    else{

        emit search_signal2(str,ui->highlight_result->checkState(),
                            ui->comboBox->currentIndex());

        }
}


