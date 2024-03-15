#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog.h"
#include "minetextedit.h"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QLayout>
#include <QScrollBar>

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
    , mte(new MineTextEdit)
{
    ui->setupUi(this);

//"Search button"
       ui->pushButton->setFixedHeight(50);
       ui->pushButton->setFixedWidth(50);
       QRect rect(0,0,50,50);
       QRegion region(rect, QRegion::Ellipse);
       ui->pushButton->setMask(region);


//1st widget (main editor window)
       QWidget *centralWidget = new QWidget(this);
    centralWidget->setFixedSize(660,660);
    centralWidget->move(0,30);

    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    //centralWidget->setLayout(layout);

    //layout->addWidget(mte,50,0);
    layout->addWidget(mte,50);

    mte->setReadOnly(false);

//2nd widget (func but)
    QWidget *buttons_widget = new QWidget(this);

    QVBoxLayout * layout2 = new QVBoxLayout(buttons_widget);

    //    //centralWidget->setLayout(layout2);

    layout2->addWidget(ui->tempButton);
    ui->tempButton->setFixedWidth(100);

    ui->reset_text->setText("Reset search\nhighlight");
    layout2->addWidget(ui->reset_text);
    ui->reset_text->setFixedSize(100,50);

    //layout2->addWidget(ui->pushButton);
    buttons_widget->setFixedSize(125,125);
    buttons_widget->move(680,275);
    buttons_widget->show();

//3d widget (for "search button")

    QWidget *search_widget = new QWidget(this);
    QGridLayout * layout3 = new QGridLayout(search_widget);

    layout3->addWidget(ui->pushButton);

    search_widget->setFixedSize(175,175);
    search_widget->move(650,350);
    search_widget->show();


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%=====SIGNALS=====%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

       connect(d_,&Dialog::search_signal2, this, &MainWindow::search_slot2);
       connect(mte, &MineTextEdit::signal, this, &MainWindow::mte_slot);
       //connect(mte, &MineTextEdit::selectionChanged, this, &MainWindow::select_text_slot);

}

MainWindow::~MainWindow()
{
    delete ui;
    delete d_;
    delete mte;
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
void MainWindow::show_occurrences_edit(std::string const & text, char const *pattern)
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


QString MainWindow::show_highlighting_edit(std::string const & text, char const *pattern,int color)
{
    QString before="";
    QString after="";
    QString main_string="";

    size_t start_pos=0;
    size_t text_find=0;
    QString color_="";

    if(!color){ color_ = "orange";
    } else if(color == 1){ color_ = "pink";
    } else if(color == 2){ color_ = "yellow";
    } else if(color == 3){ color_ = "green";
    } else if(color == 4){ color_ = "red";
    }

    occurrences=0;

    //"<def style=\"white-space: pre-wrap;\">"+before+"</def>";
main_string = "<sss style=\"white-space: pre-wrap;\">";
do    {
          text_find=text.find(pattern,start_pos);

            if(text_find==std::string::npos)break;
            ++occurrences;
before=QString::fromStdString(text.substr(start_pos,text_find - start_pos));
    before.replace(QString("\n"), QString("<br>"));
    //before.replace(QString(" "), QString("&nbsp;"));
        before = "<font color=\"black\">"+before+"</font>";




after=QString::fromStdString(text.substr(/*result_array()[i]*/text_find,strlenpp(pattern)));
    after.replace(QString("\n"), QString("<br>"));
    //after.replace(QString(" "), QString("&nbsp;"));


        after ="<abc style=\"background-color:"+color_+";\">"+after+"</abc>";


    main_string+=before+after;

start_pos=/*result_array()[i]*/text_find+strlenpp(pattern);

    } while(1);

after=QString::fromStdString(text.substr(start_pos));
    after.replace(QString("\n"), QString("<br>"));
    //after.replace(QString(" "), QString("&nbsp;"));
        after = "<font color=\"black\">"+after+"</font>";



    main_string+=after+"</sss>";
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
            mte->clear();
            QTextStream in(&file);

            //QString abc=in.readAll();//.replace(QString(" "), QString("&nbsp;")).replace(QString("\n"), QString("<br>"));
            //mte->setStyleSheet("");

            //mte->setText(in.readAll().replace(QString(" "), QString("&nbsp;")).replace(QString("\n"), QString("<br>")));
            mte->setHtml("<opf style=\"white-space: pre-wrap;\">"+in.readAll()
                         .replace(QString("\n"), QString("<br>"))+"</opf>");

//            mte->setHtml("<op style=\"white-space: pre-wrap;\">"+in.readAll()
//                         //.replace(QString(" "), QString("&nbsp;"))
//                         .replace(QString("\n"), QString("<br>"))+"</op>");

        //ui->textBrowser->setText(in.readAll().replace(QString(" "), QString("&nbsp;")).replace(QString("\n"), QString("<br>")));

        ui->pushButton->setEnabled(true);
        ui->pushButton->setStyleSheet("background-color: blue;");

        mte->setStyleSheet("");


ui->statusbar->showMessage("Is read only == "+QString::number(mte->isReadOnly()));
        }

}



void MainWindow::on_tempButton_clicked()
{
    //## 1 file
    QFile file("/home/alexander/qtcreator_proj/search_pattern/test789.txt");
    if(!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0,"info",file.errorString());

    QTextStream in(&file);

    //"<def style=\"white-space: pre-wrap;\">"+before+"</def>";
    mte->setHtml("<op style=\"white-space: pre-wrap;\">"+in.readAll()
                 //.replace(QString(" "), QString("&nbsp;"))
                 .replace(QString("\n"), QString("<br>"))+"</op>");

    ui->pushButton->setEnabled(true);
    ui->pushButton->setStyleSheet("background-color: blue;");

    ui->statusbar->showMessage("Is read only == "+QString::number(mte->isReadOnly()));
}


void MainWindow::search_slot2(QString pattern,bool highlight,int color)
{

    QString text = mte->toPlainText()
            .replace(QString("\n"), QString("<br>"));

    std::string str = text.toStdString();


    std::string std_pat = pattern.toStdString();
    char const *pat_cstr = std_pat.c_str();


        //ui->textBrowser->clear();
        mte->clear();
    QString qv = show_highlighting_edit(str,pat_cstr,color);

//    ui->textBrowser->insertHtml(qv); // Highlighting
    mte->insertHtml(qv); // Highlighting

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%
    clear_check()=false;
    mte->setReadOnly(true);
    //%%%%%%%%%%%%%%%%%%%%%%%%%%

//    ui->textBrowser->setTextColor("black");

    mte->setTextColor("black");

//    //ui->textBrowser->setAlignment(q);

    ui->statusbar->showMessage(QString::number(occurrences)+" matches found");

    if(highlight){

    show_occurrences_edit(str,pat_cstr); // Window with search results

    }
}




void MainWindow::on_reset_text_clicked()
{

    if(!clear_check())
    {
        QString f = mte->toPlainText();
        f.replace(QString("\n"), QString("<br>"));
        //f.replace(QString(" "), QString("&nbsp;"));
        f = "<def style=\"background-color: white;white-space: pre-wrap;\">"+f+"</def>";

        mte->clear();
        mte->setHtml(f);
        clear_check()=true;
    }

}


void MainWindow::on_actionExit_triggered()
{
    QCoreApplication::quit();
}

void MainWindow::mte_slot()
{

    if(mte->isReadOnly())
    {

        QTextCursor cursor = mte->textCursor();

        int curpos = cursor.position();

        QScrollBar * scroll = mte->verticalScrollBar();
            int scrollval = scroll->value();
            int scrollhor = mte->horizontalScrollBar()->value();
            scroll->setValue(scrollval);

    mte->setHtml("<def style=\"background-color: white;white-space: pre-wrap;\">"
+mte->toPlainText().replace(QString("\n"), QString("<br>"))
                 //.replace(QString(" "), QString("&nbsp;"))
                 +"</def>");

    //int coord2 = mte->cursorForPosition(mte->mapFromGlobal(QCursor::pos())).position();


    cursor.setPosition(curpos);

    mte->verticalScrollBar()->setValue(scrollval);
        mte->horizontalScrollBar()->setValue(scrollhor);

        mte->setTextCursor(cursor);

        //cursor.clearSelection();

//int coord = mte->cursorForPosition(QCursor::pos()).position();
//int coord2 = mte->cursorForPosition(QWidget::mapFromGlobal(QCursor::pos())).position();
//int coord2 = mte->cursorForPosition(mte->mapFromGlobal(QCursor::pos())).position();

        mte->setReadOnly(false);

////!!TEst
//        QTextCursor c = mte->textCursor();
//        c.setPosition(5); //startPos
//        c.setPosition(10, //endPos
//            QTextCursor::KeepAnchor);
//        mte->setTextCursor(c);
////!!TEst
//c.select
    }

}


void MainWindow::select_text_slot()
{


//    QTextCursor cursor = mte->textCursor();



//if(!select_check()){

//select_check()=true;
//}



}

