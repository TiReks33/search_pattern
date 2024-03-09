#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog.h"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>

#include <iostream>
#include <stdlib.h>
//#include <cstring>
#include <fstream>
#include <string> //for std::getline
#include <sstream> //std::stringstream

//#define RESET   "\033[0m"
//#define BLACK   "\033[30m"      /* Black */
//#define RED     "\033[31m"      /* Red */
//#define GREEN   "\033[32m"      /* Green */


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , d_(new Dialog)
    , occurrences(0)
{
    ui->setupUi(this);

       ui->pushButton->setFixedHeight(50);
       ui->pushButton->setFixedWidth(50);
       QRect rect(0,0,50,50);
       QRegion region(rect, QRegion::Ellipse);
       ui->pushButton->setMask(region);

       connect(d_,&Dialog::search_signal2, this, &MainWindow::search_slot2);

}

MainWindow::~MainWindow()
{
    delete ui;
    delete d_;
}

size_t strlenpp(const std::string *str)
{
    const char * str_temp=str->c_str();
    size_t t = 0;
    for (; str_temp[t]!='\0';++t);
    return t;

}

size_t strlenpp(const char *str)
{
    size_t t = 0;
    for (; str[t]!='\0';++t);
    return t;
}


size_t strlenpp(char *str)
 {
     size_t t = 0;
     for (; str[t]!='\0';++t);
     return t;
}


// result pattern indexes in text
void MainWindow::show_occurrences(std::string const & text, char const *pattern)
{


    QString result="";

      size_t start_pos=0;
      size_t text_find=0;
      occurrences=0;
      size_t strlen = strlenpp(pattern);


    do    {
            text_find=text.find(pattern,start_pos);

              if(text_find==std::string::npos){if(occurrences>0)result+="<font color=\"black\">.</font>";break;} // DOT
              else if(occurrences>0) result+="<font color=\"black\">, </font>";                             // COMMA
              ++occurrences;
              result+=QString::number(text_find);

        start_pos=/*result_array()[i]*/text_find+strlen;

            } while(1);

      if(occurrences!=0)result+="<br>";
      else {

          result+="Entries not found.:(";

          result+="<br>";
      }

      QString head="<br>Pattern exist in next literal positions(from zero):";

      QString html_final="";

      if(occurrences==0)
      html_final = "<div><font color=\"red\">"+QString(result)+"</font></div>";
      else
      html_final = "<font color=\"green\">"+QString(result)+"</font>";

      Result * r = new Result;

      r->search_results(html_final);

      r->exec();

}

QString MainWindow::show_highlighting(std::string const & text, char const *pattern)
{
    QString before="";
    QString after="";
    QString main_string="";

    size_t start_pos=0;
    size_t text_find=0;

    occurrences=0;

do    {
          text_find=text.find(pattern,start_pos);
//std::cout << "text_find in beginning = " << text_find << "\n";
//std::cout << start_pos<<"\n";
            if(text_find==std::string::npos)break;
            ++occurrences;
before=QString::fromStdString(text.substr(start_pos,text_find - start_pos));
    before.replace(QString("\n"), QString("<br>"));
    before.replace(QString(" "), QString("&nbsp;"));
    before = "<font color=\"black\">"+before+"</font>";
//std::cout << "start_pos from finale: " <<start_pos<<" ";
//std::cout << "text_find from finale: " <<text_find<<"\n";



after=QString::fromStdString(text.substr(/*result_array()[i]*/text_find,strlenpp(pattern)));
    after.replace(QString("\n"), QString("<br>"));
    after.replace(QString(" "), QString("&nbsp;"));
//    after = "<font color=\"red\">"+after+"</font>";
    //after = "<font color=\"black\">"+after+"</font>";
    after ="<abc style=\"background-color: orange;\">"+after+"</abc>";


    main_string+=before+after;

start_pos=/*result_array()[i]*/text_find+strlenpp(pattern);

    } while(1);
//std::cout << "number == " << number<<"\n";
after=QString::fromStdString(text.substr(start_pos));
    after.replace(QString("\n"), QString("<br>"));
    after.replace(QString(" "), QString("&nbsp;"));
    after = "<font color=\"black\">"+after+"</font>";
//std::cout << start_pos << "\n";
    main_string+=after;
    return main_string;
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


void MainWindow::search_slot2(QString pattern,bool highlight)
{
    Qt::Alignment q = ui->textBrowser->alignment();

    QString text = ui->textBrowser->toPlainText();


    std::string str = text.toStdString();


    std::string std_pat = pattern.toStdString();
    char const *pat_cstr = std_pat.c_str();


        ui->textBrowser->clear();
    QString qv = show_highlighting(str,pat_cstr);

    ui->textBrowser->insertHtml(qv); // Highlighting

    ui->textBrowser->setTextColor("black");
    ui->textBrowser->setAlignment(q);

    ui->statusbar->showMessage(QString::number(occurrences)+" matches found");

    if(highlight){

    show_occurrences(str,pat_cstr); // Window with search results

    }
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
