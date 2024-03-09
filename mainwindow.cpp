#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog.h"
#include "qt_search_class.h"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>

#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <fstream>
#include <string> //for std::getline
#include <sstream> //std::stringstream

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , d_(new Dialog)
    //, s(new Search)
{
    ui->setupUi(this);
    //ui->pushButton->setText("Test Text");
       ui->pushButton->setFixedHeight(50);
       ui->pushButton->setFixedWidth(50);
       QRect rect(0,0,50,50);
       QRegion region(rect, QRegion::Ellipse);
       ui->pushButton->setMask(region);

       connect(d_,&Dialog::search_signal, this, &MainWindow::search_slot);
       connect(d_,&Dialog::search_signal2, this, &MainWindow::search_slot2);
       connect(d_,&Dialog::highlight_signal, this, &MainWindow::highlight_slot);
      // connect(s,&Search::signal,this,&MainWindow::search_slot3);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete d_;
}


void MainWindow::on_pushButton_clicked()
{
    //QDialog * d = new Dialog;
    //if(!d_)d_ = new Dialog;
    d_->setModal(true);
    d_->show();

    //Dialog * d = new Dialog;
    //d->setModal(true);
//connect(d,SIGNAL(search_signal()), this, SLOT(search_slot()));
    //d.show();

    d_->exec();
}

void MainWindow::on_actionOpen_triggered()
{
    //## 1 file
//    QFile file("/home/alexander/qtcreator_proj/search_pattern/test789.txt");
//    if(!file.open(QIODevice::ReadOnly))
//        QMessageBox::information(0,"info",file.errorString());

//    QTextStream in(&file);

//    ui->textBrowser->setText(in.readAll());

    //##ALL files dialog

    //QString file_name = QFileDialog::getOpenFileName(this, "Open a file", "/");
    QString file_name = QFileDialog::getOpenFileName(this, "Open a file", QDir::homePath());

    //QMessageBox::information(this,"..",file_name);

    QFile file(file_name);
        if(!file.open(QIODevice::ReadOnly))
            QMessageBox::information(0,"info",file.errorString());
        else{
        QTextStream in(&file);

        ui->textBrowser->setText(in.readAll().replace(QString(" "), QString("&nbsp;")).replace(QString("\n"), QString("<br>")));

        ui->pushButton->setEnabled(true);
        ui->pushButton->setStyleSheet("background-color: blue;");

        ui->textBrowser->setStyleSheet("");
        }
}

void MainWindow::on_tempButton_clicked()
{
    //## 1 file
    QFile file("/home/alexander/qtcreator_proj/search_pattern/test789.txt");
    if(!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0,"info",file.errorString());

    QTextStream in(&file);

    ui->textBrowser->setHtml(in.readAll().replace(QString(" "), QString("&nbsp;")).replace(QString("\n"), QString("<br>")));

    ui->pushButton->setEnabled(true);
    ui->pushButton->setStyleSheet("background-color: blue;");
}

void MainWindow::search_slot(QString str)
{
    ui->textBrowser->setText(str);
    //ui->label->setText(str);
}

void MainWindow::search_slot2(QString pattern,bool highlight)
{
    Qt::Alignment q = ui->textBrowser->alignment();

    QString text = ui->textBrowser->toPlainText();
    //QString text_html = ui->textBrowser->toHtml();


    Search ss{};

    std::string str = text.toStdString();
    //char const *cstr=str.c_str();

    std::string std_pat = pattern.toStdString();
    char const *pat_cstr = std_pat.c_str();


    //ss.strstr_multi(cstr,pat_cstr);

        ui->textBrowser->clear();
    QString qv = ss.show_finale_qt_test(str,pat_cstr);
    //for(size_t i=0;i!=qv.size();++i)
    ui->textBrowser->insertHtml(qv); // Highlighting

    ui->textBrowser->setTextColor("black");
    ui->textBrowser->setAlignment(q);

    if(highlight){

    ss.show_mas_qt_test(str,pat_cstr); // Window with search results

    }/*else{
        ui->textBrowser->setTextColor("black");
    }*/
}

void MainWindow::highlight_slot()
{
//        ui->textBrowser->clear();
//        std::vector<QString> qv = ss.show_finale_qt(str,pat_cstr);
//        for(size_t i=0;i!=qv.size();++i)
//        ui->textBrowser->insertHtml(qv[i]);
//        ui->textBrowser->setTextColor("black");
//        ui->textBrowser->setAlignment(q);
}

void MainWindow::search_slot3(QString str)
{
    ui->textBrowser->insertHtml("<div><font color=\"black\">bla-bla-bla</font></div>");
}





void MainWindow::on_tiny_clicked()
{
    QFile file("/home/alexander/cpp/small.txt");
    if(!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0,"info",file.errorString());

    QTextStream in(&file);

    ui->textBrowser->setHtml(in.readAll().replace(QString(" "), QString("&nbsp;")).replace(QString("\n"), QString("<br>")));

    ui->pushButton->setEnabled(true);
    ui->pushButton->setStyleSheet("background-color: blue;");
}

void MainWindow::on_reset_text_clicked()
{

    QString f = ui->textBrowser->toPlainText();
    f.replace(QString("\n"), QString("<br>"));
    f.replace(QString(" "), QString("&nbsp;"));
    f = "<def style=\"background-color: white;\">"+f+"</def>";

    ui->textBrowser->clear();
    ui->textBrowser->setHtml(f);
}
