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
#include <QDebug>
#include <QCloseEvent>
#include <QString>
#include <QFileInfo>
//#include <QPalette>
//#include <QTextCharFormat>

#include <iostream>
#include <stdlib.h>
//#include <cstring>
#include <fstream>
#include <string> //for std::getline
#include <sstream> //std::stringstream
//#include <cerrno>

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
    , buffer_("")
    , in_()
    , temp_file_path_(QDir::homePath())
    , temp_file_name_("/$temp1")

    //// text file partial read logic
    , fsize_(0)
    , maxfullsize(500000)
    , buf_size   (500000)
//    , maxfullsize(25000000)
//    , buf_size(250000)
    , scroll_buf(0)

    , buf_start(0)
    //, buf_end(0)

    , end_file(false)
    //, is_split(false)
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
    //mte->setUndoRedoEnabled(false);

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

    QPushButton *clc_button = new QPushButton(this);
    clc_button->setText("clear screen");
    clc_button->setFixedSize(100,50);
    clc_button->move(700,500);
    clc_button->show();

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%=====SIGNALS=====%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

       connect(d_,&Dialog::search_signal2, this, &MainWindow::search_slot);
       connect(mte, &MineTextEdit::signal, this, &MainWindow::mte_slot);
       connect(clc_button, SIGNAL (released()),this, SLOT (clc_released()));

//connect(mte->verticalScrollBar(),SIGNAL(sliderMoved(int)),this/*mte->verticalScrollBar()*/,SLOT(slider_slot(int)));
//connect(mte->verticalScrollBar(),SIGNAL(valueChanged(int)),this/*mte->verticalScrollBar()*/,SLOT(slider_slot(int)));
       //connect(mte->verticalScrollBar(),SIGNAL(sliderPressed(int)),this/*mte->verticalScrollBar()*/,SLOT(slider_slot(int)));
       //connect(mte->verticalScrollBar(),SIGNAL(sliderReleased(int)),this/*mte->verticalScrollBar()*/,SLOT(slider_slot(int)));

       //???connect(mte->verticalScrollBar(),SIGNAL(actionTriggered(int)),this/*mte->verticalScrollBar()*/,SLOT(slider_slot(int)));

       //connect(mte->verticalScrollBar(),SIGNAL(rangeChanged(int,int)),this/*mte->verticalScrollBar()*/,SLOT(slider_slot(int,int)));

//mte->verticalScrollBar()->
       mte->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

       connect(mte->verticalScrollBar(),SIGNAL(sliderMoved(int)),this/*mte->verticalScrollBar()*/,SLOT(slider_slot(int)),Qt::QueuedConnection);
       connect(mte->verticalScrollBar(),SIGNAL(valueChanged(int)),this/*mte->verticalScrollBar()*/,SLOT(slider_slot(int)),Qt::QueuedConnection);

       connect(mte,&MineTextEdit::currentCharFormatChanged,this,&MainWindow::format_slot);

       connect(mte,&MineTextEdit::mouse_press_signal,this,&MainWindow::mouse_press_slot);

mte->setFont(QFont("DejaVu Sans Mono"));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete d_;
    delete mte;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox p;
    p.warning(0,"123","123");
    //p.exec();
    //cleanUp();
           event->accept();
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

void MainWindow::search_highlight(std::string const & text, char const *pattern,int color)
{
    std::string before="";
    std::string after="";
    QString main_string="";

    size_t start_pos=0;
    size_t text_find=0;
    QString color_="";

    size_t strlen = strlenpp(pattern);

           if(!color)    { color_ = "orange";
    } else if(color == 1){ color_ = "pink";
    } else if(color == 2){ color_ = "yellow";
    } else if(color == 3){ color_ = "green";
    } else if(color == 4){ color_ = "red";
    }

    occurrences=0;

main_string = "<sss style=\"white-space: pre-wrap;background-color:white;\">";

  do{
          text_find=text.find(pattern,start_pos);

            if (text_find==std::string::npos) break;

            ++occurrences;

before=text.substr(start_pos,text_find - start_pos);


after=text.substr(text_find,strlen);


    main_string+=QString::fromStdString(before)

//                .replace(QString("\n"), QString("<br>"))

                +"<abc style=\"background-color:"+color_+";\">"

                +QString::fromStdString(after)

//                .replace(QString("\n"), QString("<br>"))

                +"</abc>";

start_pos=text_find+strlen;

    } while(1);

after=text.substr(start_pos);


    main_string+=QString::fromStdString(after)

//                .replace(QString("\n"), QString("<br>"))

                +"</sss>"+"<redd style=\"background-color:red;\">"+"</redd>";

    //return main_string;

    buffer_.clear();
    mte->clearMask();

    //buffer_=main_string;
    buffer_.append(main_string);
    //mte->insertHtml(buffer_);


}


void MainWindow::search_highlight_occurrences(std::string const & text, char const *pattern,int color)
{

    std::string before="";
    std::string after="";
    QString main_string="";
    QString result="";

    size_t start_pos=0;
    size_t text_find=0;
    QString color_="";

    size_t strlen = strlenpp(pattern);

           if(!color)    { color_ = "orange";
    } else if(color == 1){ color_ = "pink";
    } else if(color == 2){ color_ = "yellow";
    } else if(color == 3){ color_ = "green";
    } else if(color == 4){ color_ = "red";
    }

    occurrences=0;

main_string = "<sss style=\"white-space: pre-wrap;\">";

  do{
          text_find=text.find(pattern,start_pos);

            if (text_find==std::string::npos) {if(occurrences>0)result+=".";break;} // DOT
            else if(occurrences>0) result+=", ";     // COMMA
            ++occurrences;
            result+=QString::number(text_find);

before=text.substr(start_pos,text_find - start_pos);


after=text.substr(text_find,strlen);


    main_string+=QString::fromStdString(before).replace(QString("\n"), QString("<br>"))

                +"<abc style=\"background-color:"+color_+";\">"

                +QString::fromStdString(after).replace(QString("\n"), QString("<br>"))

                +"</abc>";

start_pos=text_find+strlen;

    } while(1);

if(occurrences==0)
result+="Entries not found.:(";

after=text.substr(start_pos);


    main_string+=QString::fromStdString(after).replace(QString("\n"), QString("<br>"))+"</sss>";

    Result r{};

     if(occurrences==0)
     r.search_results(result,'r');
     else
     r.search_results(result);

    //return main_string;
     buffer_.clear();
     //buffer_=main_string;
     buffer_.append(main_string);
     //mte->insertHtml(buffer_);

    r.exec();


}



void MainWindow::on_pushButton_clicked()
{
    //QString file_dir = QFileDialog::getOpenFileName(this, "Open a file", QDir::homePath());
        //QFile file("/home/alexander/cpp/$temp1");
          QFile file(temp_file_path_ + temp_file_name_);

        if(!file.open(QIODevice::WriteOnly)){

            QMessageBox p;
                    p.warning(0,"Error","Temporary data save error. "
            "Please, close search window if you don't want to lose your current changes.");
                    qDebug() << "ERROR READING FILE:" << file.fileName();

        }else{
        QTextStream stream(&file);

        stream << mte->toPlainText();

        file.close();


//    {
//        qDebug() << "ERROR READING FILE";
//    }

    }
        d_->setModal(true);
    d_->show();

    d_->exec();
        //}
}


void MainWindow::on_actionOpen_triggered()
{

    QString file_name = QFileDialog::getOpenFileName(this, "Open a file", QDir::homePath());



    QFile file(file_name);
        if(!file.open(QIODevice::ReadOnly))
            QMessageBox::information(0,"info",file.errorString());
        else{
            mte->clear();
            QTextStream in(&file);
            //fsize_=file.size();

            temp_file_path_=QFileInfo(file).path();


            ui->statusbar->showMessage("File path: "+temp_file_path_+' '+"File Size: "+QString::number(file.size()));

            buffer_.clear();

//            while(!in.atEnd()) {
//                buffer_.append(in.read(500000));
//            }
            buffer_.append(in.readAll());
//                  buffer_.append(in.readAll());
            file.close();


end_file=true; //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1

//            QTextCharFormat tcf;
//            QColor qc;
//            qc.fromRgba()
//            tcf.setBackground();
//            mte->setCurrentCharFormat()

             mte->setHtml("<opf style=\"white-space: pre-wrap;\">"+buffer_
                                                                  +"</opf>");
             //mte->setPlainText(buffer_);

        ui->pushButton->setEnabled(true);
        ui->pushButton->setStyleSheet("background-color: blue;");

        //mte->setStyleSheet("");

        format=mte->currentCharFormat();
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





void MainWindow::search_slot(QString pattern,bool highlight,int color)
{

    QFile file(temp_file_path_ + temp_file_name_);

  if(!file.open(QIODevice::ReadOnly)){

      QMessageBox p;
              p.warning(0,"Error","Temporary data read error. "
      "...");
              qDebug() << "ERROR READING FILE:" << file.fileName();

  }else{
      ui->search_clc_button->setEnabled(true);
      on_search_clc_button_clicked();

  QTextStream stream(&file);
  fsize_=file.size();

  buffer_.clear();
  //mte->clear();

  buffer_.append(stream.readAll());


  file.close();





    std::string str = buffer_.toStdString();


    std::string std_pat = pattern.toStdString();
    char const *pat_cstr = std_pat.c_str();





//        int scrollval = mte->verticalScrollBar()->value();
//        QScrollBar scroll;scroll.setValue(scrollval);
//        mte->clear();

        if(!highlight){
            search_highlight(str,pat_cstr,color);
        }else{
            search_highlight_occurrences(str,pat_cstr,color);
        }
auto bfs = buffer_.size();
fsize_=bfs;

        if(fsize_<maxfullsize){
            int scrollval = mte->verticalScrollBar()->value();
            QScrollBar scroll;scroll.setValue(scrollval);
            mte->clear();
        mte->setHtml("<opf style=\"white-space: pre-wrap;\">"+buffer_
//                     .replace(QString("\n"), QString("<br>"))
                     +"</opf>");
        mte->verticalScrollBar()->setValue(scrollval);
         end_file=true;

        }else if(fsize_>=maxfullsize){

            //is_split=true;
        mte->clear();
        //buffer for "split-scroll" viewing
             if(fsize_<2500000)                 buf_size= 250000;
        else if(fsize_>2500000&&fsize_<5500000) buf_size= 500000;
        else if(fsize_>5500000&&fsize_<10000000)buf_size= 750000;
        else if(fsize_>10000000)                buf_size= 1000000;

             QStringRef subbuf(&buffer_,buf_start,buf_size);
             mte->insertHtml("<opf style=\"white-space: pre-wrap;\">"+subbuf.toString()
                                      /*.replace(QString("\n"), QString("<br>"))*/
                                      +"</opf>");
             buf_start+= buf_size;
             //buf_end = buf_start + buf_size;
             size_t max_scroll = mte->verticalScrollBar()->maximum();
             scroll_buf=(max_scroll*0.75);

             //end_file=false; //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1

        }


//    mte->verticalScrollBar()->setValue(scrollval);

    clear_check()=false;

    mte->setReadOnly(true);

    //mte->setTextColor("black");

    ui->statusbar->showMessage(QString::number(occurrences)+" matches found");

    ui->statusbar->showMessage("FILE SIZE IS ::"+QString::number(fsize_)
                               +"BUFFER AFTER SEARCH SIZE IS ::"+QString::number(bfs));
    qDebug() << "FORMAT AFTER OPEN ::" << format.background();

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
    //QCoreApplication::quit();
   close();
}


void MainWindow::mte_slot()
{

    if(mte->isReadOnly())
    {

end_file=true;

        //is_split=false;

        QTextCursor cursor = mte->textCursor();

        //int curpos = cursor.position();
        //int selection_start = cursor.selectionStart();

        //int selection_end = cursor.selectionEnd();

        //qDebug() << "selection start position == " << selection_start << Qt::endl;
        //qDebug() << "selection end position == " << selection_end << Qt::endl;

        QScrollBar * scroll = mte->verticalScrollBar();
            int scrollval = scroll->value();
            int scrollhor = mte->horizontalScrollBar()->value();
            scroll->setValue(scrollval);

            qDebug() << "scrollval == " << scrollval << Qt::endl;
            qDebug() << "scrollhor == " << scrollhor << Qt::endl;


//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&






//buffer_.clear();
//buffer_=mte->toPlainText();
//mte->setHtml("<def style=\"background-color: white;white-space: pre-wrap;\">"+buffer_+"</def>");
////mte->setStyleSheet("background-color: white");

            QFile file(temp_file_path_ + temp_file_name_);

          if(!file.open(QIODevice::ReadOnly)){

              QMessageBox p;
                      p.warning(0,"Error","Temporary data read error. "
              "...");
                      qDebug() << "ERROR READING FILE:" << file.fileName();

          }else{
          QTextStream stream(&file);
          buffer_.clear();

          buffer_.append(stream.readAll());


          file.close();

          //mte->setHtml("<def style=\"background-color: white;white-space: pre-wrap;\">"+buffer_+"</def>");

          mte->setPlainText(buffer_);

      }









//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


//mte->setHtml("<def style=\"background-color: white;white-space: pre-wrap;\">"
//            +buffer_
//             +"</def>");



    //int coord2 = mte->cursorForPosition(mte->mapFromGlobal(QCursor::pos())).position();


    //cursor.setPosition(curpos);
//    cursor.setPosition(selection_start);

//mte->verticalScrollBar()->setValue(scrollval);
        //mte->horizontalScrollBar()->setValue(scrollhor);

//        cursor.setPosition(selection_end, QTextCursor::KeepAnchor);

//mte->setTextCursor(cursor);






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

        //qDebug() << "selection start position == " << cursor.selectionStart() << Qt::endl;
        //qDebug() << "selection end position == " << cursor.selectionEnd() << Qt::endl;
    }
    /*QTextCursor y=mte->textCursor();
    int yp=y.position();
    ui->statusbar->showMessage(QString::number(yp))*/;
}



void MainWindow::slider_slot(int)//(int v)
{
    //ui->statusbar->showMessage("simple slider signal == "+QString::number(v));

       //end_file=true;
       // qDebug()<<"BUFFER SIZE ==" << buf_size;
        size_t temp = mte->verticalScrollBar()->value();
        //qDebug()<< "MAXIMUM ==" << ui->textEdit->verticalScrollBar()->maximum();
    //    size_t max_scroll = ui->textEdit->verticalScrollBar()->maximum();
    //    scroll_buf=(max_scroll*0.75);
        //ui->statusbar->showMessage(QString::number(ui->textEdit->verticalScrollBar()->value()));
        ui->statusbar->showMessage(QString::number(temp));
        if(end_file){ return;}
        if (temp>=(scroll_buf))
        {
            //ui->textEdit->verticalScrollBar()->setEnabled(false);

            if(buf_start+buf_size<=fsize_){
            QStringRef subbuf(&buffer_,buf_start,buf_size);
            mte->insertHtml("<opf style=\"white-space: pre-wrap;\">"+subbuf.toString()/*.replace(QString("\n"), QString("<br>"))*/+"</opf>");
            buf_start+= buf_size;
        //end_file=true;
            size_t max_scroll = mte->verticalScrollBar()->maximum();
             scroll_buf=(max_scroll*0.75);

             //ui->textEdit->verticalScrollBar()->setEnabled(true);

            } else if((buf_start+buf_size)>fsize_){
                size_t buf_final_temp = fsize_-buf_start;
                QStringRef subbuf(&buffer_,buf_start,buf_final_temp);
                mte->insertHtml("<opf style=\"white-space: pre-wrap;\">"+subbuf.toString()/*.replace(QString("\n"), QString("<br>"))*/+"</opf>");
                end_file=true;

                //%%%%%%%%%%%%%%%%
                scroll_buf=0;
                buf_start=0;
                //%%%%%%%%%%%%%%%%

                //is_split=false;

            }
            //scroll_buf*=2;
        }



}

void MainWindow::format_slot(const QTextCharFormat &f)
{
    qDebug() << "Current format changed: " << f.background();
}

void MainWindow::mouse_press_slot()
{
    mte->setCurrentCharFormat(format);
}

//void MainWindow::slider_slot(int min, int max)
//{
//    //ui->statusbar->showMessage("min == "+QString::number(min)+"max == "+QString::number(max));
//}

void MainWindow::clc_released()
{

    mte->clear();

}

void MainWindow::on_bigRead_button_clicked()
{
    if(end_file) return;
    else
    if(fsize_<maxfullsize) return;

    else if (fsize_>=maxfullsize)
    {
        /*if(end_file){return;
        } else */if(buf_start+buf_size<=fsize_){
        QStringRef subbuf(&buffer_,buf_start,buf_size);
        mte->insertHtml("<opf style=\"white-space: pre-wrap;\">"+subbuf.toString()/*.replace(QString("\n"), QString("<br>"))*/+"</opf>");
        buf_start+= buf_size;

        size_t max_scroll = mte->verticalScrollBar()->maximum();
        scroll_buf=(max_scroll*0.75);
        } else if((buf_start+buf_size)>fsize_){
            size_t buf_final_temp = fsize_-buf_start;
            QStringRef subbuf(&buffer_,buf_start,buf_final_temp);
            mte->insertHtml("<opf style=\"white-space: pre-wrap;\">"+subbuf.toString()/*.replace(QString("\n"), QString("<br>"))*/+"</opf>");
            end_file=true;
            //%%%%%%%%%%%%%%%%
            scroll_buf=0;
            buf_start=0;
            //%%%%%%%%%%%%%%%%
        }
    }
}

void MainWindow::on_search_clc_button_clicked()
{
    mte->clear();
    buffer_.clear();
    end_file=false;
    //%%%%%%%%%%%%%%%%
    scroll_buf=0;
    buf_start=0;
    //%%%%%%%%%%%%%%%%
}
