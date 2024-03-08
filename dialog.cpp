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
//    QString str;
//    str = ui->textEdit->toPlainText();
//    std::string std_str = str.toStdString();
//    char const * cstr;
//    cstr = std_str.c_str();
// //     char const * cstr2;
// //    cstr2 = str.toStdString().c_str();
//    QMessageBox::warning(this,"123",cstr);

                                                                //std::string std_str = str.toStdString();
                                                                //char const * cstr = std_str.c_str();
        //QMessageBox::information(this,"Success",cstr);

        //emit search_signal(cstr);
        //emit search_signal2(cstr);

        emit search_signal2(str,ui->highlight_result->checkState());
//        if(ui->highlight_result->isChecked())
//            emit highlight_signal();
        }
}

void Dialog::on_tempButton_clicked()
{
    QString str;
    str = ui->textEdit->toPlainText();
    char const *cstr=str.toStdString().c_str();


    //emit search_signal(str);
}

void Dialog::on_highlight_result_stateChanged(int arg1)
{

}
