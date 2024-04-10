#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog.h"
#include "minetextedit.h"
#include "savedialog.h"
#include "dockwidget.h"
#include "ui_dockwidget.h"

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
#include <QElapsedTimer>
#include <QTextBlock>
#include <QTimer>
#include <QThread>
#include <QDockWidget>

#include <iostream>
#include <stdlib.h>
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
    , r_(new Result)
    , occurrences(0)
    , mte(new MineTextEdit)
    , buffer_("")
    , in_()

    , prog_name_("Search_pattern")
    , file_path_(QDir::homePath())
    , file_name_("")
    , temp_file_path_(QDir::tempPath())
    , temp_file_prefix_("$temp_")
    , temp_subfolder_()
    , full_tmp_f_path_(temp_file_path_)

    //// text file partial read logic
    , fsize_(0)
    , maxfullsize(500000)
    , buf_size   (500000)
    , scroll_buf(0)

    , buf_start(0)

    , end_file(false)

    , format(mte->currentCharFormat())

    , ProcessIsFinished(false)
    , end_text_cursor_pos_(0)
    , text_cursor_isSet_toEnd(false)
    , file_size_limit_(24000000) //(24-24.5mb]
    , need_save_(false)
    , need_save_as_(true)

    , save_(new saveDialog)
    , buttons_isEnabled_(false)
    , play_it_safe_close_(false)

    , options(new DockWidget)
    , hints(new Hints)
{
    ui->setupUi(this);

    setStyleSheet("QMainWindow {background-color: qlineargradient(x1: 0, y1: 0,    x2: 0, y2: 1,    stop: 0 #55a3db, stop: 1 #dfecf6);}"
"QToolBar::separator{ width:2px;background-color: qlineargradient(x1: 0, y1: 0,    x2: 0, y2: 1,    stop: 0 #487ea6, stop: 1 #63a1cd); }"
"");

    ui->toolBar->setStyleSheet("border:none;background-color: qlineargradient(x1: 0, y1: 0,    x2: 0, y2: 1,    stop: 0 #dfecf6, stop: 1 #55a3db);");

mte->verticalScrollBar()->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0,    x2: 0, y2: 1,    stop: 0 #e7ebef, stop: 1 #cde3f2);");

    //"Search button"
       ui->search_button->setFixedSize(67,67);
        QRect rect(0,0,67,67);
       QRegion region(rect, QRegion::Ellipse);
       ui->search_button->setMask(region);
        ui->search_button->setStyleSheet("border: none;"
            "background-color: qlineargradient(x1: 0, y1: 0,    x2: 0, y2: 1,    stop: 0 #e7ebef, stop: 1 #cde3f2);"
                        "color: qlineargradient(x1: 0, y1: 0,    x2: 0, y2: 1,    stop: 0 #5598c9, stop: 1 #a9d5f5);"
                                );


//widget for text screen and button
QWidget *centralWidget1 = new QWidget(this);
centralWidget1->move(0,30);
//QGridLayout * layout1 = new QGridLayout(centralWidget1);
QHBoxLayout * layout1 = new QHBoxLayout(centralWidget1);
centralWidget1->setMinimumSize(640,480);
centralWidget1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
layout1->addWidget(mte);
layout1->addWidget(ui->search_button);
setCentralWidget(centralWidget1);
centralWidget1->show();


mte->setFont(QFont("DejaVu Sans Mono"));

mte->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%=====SIGNALS=====%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

       connect(d_,&Dialog::search_signal2, this, &MainWindow::search_slot);
       connect(mte, &MineTextEdit::signal, this, &MainWindow::mte_slot);


       connect(mte->verticalScrollBar(),SIGNAL(sliderMoved(int)),this/*mte->verticalScrollBar()*/,SLOT(slider_slot(int)),Qt::QueuedConnection);
       connect(mte->verticalScrollBar(),SIGNAL(valueChanged(int)),this/*mte->verticalScrollBar()*/,SLOT(slider_slot(int)),Qt::QueuedConnection);

       //connect(mte,&MineTextEdit::currentCharFormatChanged,this,&MainWindow::format_slot);

       connect(mte,&MineTextEdit::mouse_press_signal,this,&MainWindow::mouse_press_slot);

       connect(this,&MainWindow::add_text_signal,this,&MainWindow::add_text_slot,Qt::QueuedConnection);

       connect(this,&MainWindow::add_text_signal_2limit,this,&MainWindow::add_text_slot_2limit,Qt::QueuedConnection);

       connect(this,&MainWindow::set_position_signal,this,&MainWindow::set_position_slot,Qt::QueuedConnection);

       connect(this,SIGNAL(set_cursor_signal(int)),this,SLOT(set_cursor_slot(int)),Qt::QueuedConnection);

       connect(this,&MainWindow::long_text_add_signal, this, &MainWindow::long_text_add_slot,Qt::QueuedConnection);
       connect(this,SIGNAL(cursor_shape_signal(int)),this,SLOT(cursor_shape_slot(int)));


       connect(this,SIGNAL(total_occur(size_t)),r_,SLOT(total_occur_slot(size_t)));

       connect(mte, &MineTextEdit::textChanged, this, &MainWindow::need_save);

       connect(save_,&saveDialog::save_signal, this, &MainWindow::on_actionSave_triggered);

}

MainWindow::~MainWindow()
{
    delete ui;
    delete d_;
    delete mte;
    delete r_;
    delete save_;
    delete options;
    delete hints;
}

void MainWindow::closeEvent(QCloseEvent *event)
{

    if((need_save_&&(!mte->document()->isEmpty()))||play_it_safe_close_){

        this->save_->show();
        this->save_->exec();
        if(save_->is_cancel_button_clicked()){this->save_->is_cancel_button_clicked()=false;event->ignore();return;}
    }

    event->accept();
}

// string length for const char*,const w-char[],std::string,std::wstring
template <class T>
size_t strlenpp(T str)
{
    size_t t = 0;
    for(; str[t]!='\0';++t);
    return t;
}


void MainWindow::buttons_enabled(bool on_off)
{
    ui->menuEdit->setEnabled(on_off);
    ui->actionStart_search->setEnabled(on_off);
    ui->actionStart_search_2->setEnabled(on_off);
    ui->menuSearch->setEnabled(on_off);
    ui->actionSave->setEnabled(on_off);
    ui->actionSave_as->setEnabled(on_off);

    //"Search" button-->
    ui->search_button->setEnabled(true);

    ui->search_button->setStyleSheet("border: none;"
                "background-color: qlineargradient(x1: 0, y1: 0,    x2: 0, y2: 1,    stop: 0 #e7ebef, stop: 1 #cde3f2);"
                            "color: qlineargradient(x1: 0, y1: 0,    x2: 0, y2: 1,    stop: 0 #487ea6, stop: 1 #63a1cd);"
                                    );

    ui->search_button->setToolTip(
                                  "<font style=\"color:black;\">"
                                  "Search function.\nClick to enter the search pattern."
                                  "</font>"
                                  );
    //<--"Search" button

    buttons_isEnabled_=true;
}


// result pattern indexes in text
void MainWindow::search_highlight(std::string const & text, std::string const & pattern,int color)
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
    } else if(color == 3){ color_ = "#43e346";//"green";
    } else if(color == 4){ color_ = "#ff6262";//"red";
    } else if(color == 5){ color_ = "#609aee";//light-blue
    }

    occurrences=0;

bool frst_=false;
  do{
          text_find=text.find(pattern,start_pos);

    if(!frst_){frst_=true;first_occurrence_=text_find;}

            if (text_find==std::string::npos) break;

            ++occurrences;

before=text.substr(start_pos,text_find - start_pos);


after=text.substr(text_find,strlen);


    main_string+=QString::fromStdString(before)

                +"<span style=\"background-color:"+color_+";\">"

                +QString::fromStdString(after)

                +"</span>";

start_pos=text_find+strlen;

    } while(1);

after=text.substr(start_pos);


    main_string+=QString::fromStdString(after);

    buffer_.clear();

    buffer_.append(main_string);

}


void MainWindow::search_highlight_w(const std::wstring &text, const std::wstring &pattern, int color)
{
    std::wstring before=L"";
    std::wstring after=L"";
    QString main_string="";

    size_t start_pos=0;
    size_t text_find=0;
    QString color_="";

    size_t strlen = strlenpp(pattern);

           if(!color)    { color_ = "orange";
    } else if(color == 1){ color_ = "pink";
    } else if(color == 2){ color_ = "yellow";
    } else if(color == 3){ color_ = "#43e346";//"green";
    } else if(color == 4){ color_ = "#ff6262";//"red";
    } else if(color == 5){ color_ = "#609aee";//light-blue
    }

    occurrences=0;

bool frst_=false;
  do{
          text_find=text.find(pattern,start_pos);

    if(!frst_){frst_=true;first_occurrence_=text_find;}

            if (text_find==std::string::npos) break;

            ++occurrences;

before=text.substr(start_pos,text_find - start_pos);


after=text.substr(text_find,strlen);


    main_string+=QString::fromStdWString(before)

                +"<span style=\"background-color:"+color_+";\">"

                +QString::fromStdWString(after)

                +"</span>";

start_pos=text_find+strlen;

    } while(1);

after=text.substr(start_pos);


    main_string+=QString::fromStdWString(after);

    buffer_.clear();

    buffer_.append(main_string);
}


void MainWindow::search_highlight_occurrences(std::string const & text, std::string const & pattern,int color)
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
    } else if(color == 3){ color_ = "#43e346";//"green";
    } else if(color == 4){ color_ = "#ff6262";//"red";
    } else if(color == 5){ color_ = "#609aee";//light-blue
    }

    occurrences=0;

    bool frst_=false;
  do{
          text_find=text.find(pattern,start_pos);

          if(!frst_){frst_=true;first_occurrence_=text_find;}

            if (text_find==std::string::npos) {if(occurrences>0)result+=".";break;} // DOT
            else if(occurrences>0) result+=", ";     // COMMA
            ++occurrences;
            result+=QString::number(text_find);

before=text.substr(start_pos,text_find - start_pos);


after=text.substr(text_find,strlen);


    main_string+=QString::fromStdString(before)

                +"<span style=\"background-color:"+color_+";\">"

                +QString::fromStdString(after)

                +"</span>";

start_pos=text_find+strlen;

    } while(1);

if(occurrences==0)
result+="Entries not found.:(";

after=text.substr(start_pos);


    main_string+=QString::fromStdString(after);

    //Result * r=new Result{};

     if(occurrences==0){
     r_->search_results(result,'r');
     emit total_occur(0);
     }else{
     r_->search_results(result);
     emit total_occur(occurrences);
     }
     r_->show();

     buffer_.clear();

     buffer_.append(main_string);

}

void MainWindow::search_highlight_occurrences_w(std::wstring const & text, std::wstring const & pattern,int color)
{

    std::wstring before=L"";
    std::wstring after=L"";
    QString main_string="";
    QString result="";

    size_t start_pos=0;
    size_t text_find=0;
    QString color_="";

    size_t strlen = strlenpp(pattern);

           if(!color)    { color_ = "orange";
    } else if(color == 1){ color_ = "pink";
    } else if(color == 2){ color_ = "yellow";
    } else if(color == 3){ color_ = "#43e346";//"green";
    } else if(color == 4){ color_ = "#ff6262";//"red";
    } else if(color == 5){ color_ = "#609aee";//light-blue
    }

    occurrences=0;

    bool frst_=false;
  do{
          text_find=text.find(pattern,start_pos);

          if(!frst_){frst_=true;first_occurrence_=text_find;}

            if (text_find==std::string::npos) {if(occurrences>0)result+=".";break;} // DOT
            else if(occurrences>0) result+=", ";     // COMMA
            ++occurrences;
            result+=QString::number(text_find);

before=text.substr(start_pos,text_find - start_pos);


after=text.substr(text_find,strlen);


    main_string+=QString::fromStdWString(before)

                +"<span style=\"background-color:"+color_+";\">"

                +QString::fromStdWString(after)

                +"</span>";

start_pos=text_find+strlen;

    } while(1);

if(occurrences==0)
result+="Entries not found.:(";

after=text.substr(start_pos);


    main_string+=QString::fromStdWString(after);

    //Result * r=new Result{};

     if(occurrences==0){
     r_->search_results(result,'r');
     emit total_occur(0);
     }else{
     r_->search_results(result);
     emit total_occur(occurrences);
     }
     r_->show();

     buffer_.clear();

     buffer_.append(main_string);

}


void MainWindow::on_search_button_clicked()
{
    if(mte->isReadOnly()){mouse_press_slot();mte->signal();}
    temp_subfolder_ = temp_file_path_ + "/";
    //temp_subfolder_ = temp_file_path_ + "/" + prog_name_;

// Checking if temp directory exist -- else create it!
    QDir dir(temp_subfolder_);
    if (!dir.exists())
        dir.mkpath(".");

          full_tmp_f_path_ = temp_subfolder_ + '.' + temp_file_prefix_ + file_name_;
          QFile file(full_tmp_f_path_);

        if(!file.open(QIODevice::WriteOnly)){

            QMessageBox p;
                    p.warning(0,"Error","Temporary data save error. "
            "Please, check read/write permissions in current work directory, "
            "and close search window if you don't want to lose your current changes in file.");
                    qDebug() << "ERROR READING FILE:" << file.fileName();

        }else{

        QTextStream stream(&file);

        stream << mte->toPlainText();

        file.close();

        d_->setModal(true);

        d_->show();

        d_->exec();

        }

}


void MainWindow::on_actionOpen_triggered()
{
    if(need_save_&&(!mte->document()->isEmpty())){
    //this->save_=new saveDialog;
        //this->save_->setModal(true);
        this->save_->show();
        this->save_->exec();
        if(save_->is_cancel_button_clicked()){this->save_->is_cancel_button_clicked()=false;return;}
    }
this->save_->is_cancel_button_clicked()=false;
need_save_as_=false;



    QString open_file = QFileDialog::getOpenFileName(this, "Open a file", QDir::homePath());

    if(!open_file.isEmpty()&& !open_file.isNull()) //"Cancel" in choose dialog case
    {

//qDebug() << "QDir::temp() ::" << QDir::temp() << "QDir::tempPath() ::" << QDir::tempPath();

    QFile file(open_file);
    if(file.size()>file_size_limit_){
        QMessageBox::warning(0,"File size warning",QString("Sorry, current version of programm not "
                               "supporting large files with size bigger then %1 megabytes.:(")
                             .arg(QString::number(static_cast<double>(file_size_limit_/1000000.))));
     return;
    }
        if(!file.open(QIODevice::ReadOnly))
            QMessageBox::information(0,"info",file.errorString());
        else{

buttons_enabled(true);
            end_file=false;
            scroll_buf=0;
            buf_start=0;

            end_text_cursor_pos_=0;
            text_cursor_isSet_toEnd=false;

            mte->clear();
            QTextStream in(&file);
            plain_fsize_=file.size();

            file_path_=QFileInfo(file).path();
            file_name_ = QFileInfo(file).fileName();
            temp_file_path_ = file_path_;

            ui->statusbar->showMessage("File path: "+file_path_+' '+"File Size: "+QString::number(file.size()));

            buffer_.clear();

            //while (!in.atEnd())
                //buffer_.append(in.read(256));
            buffer_.append(in.readAll());

            file.close();


if(plain_fsize_<maxfullsize){

    mte->insertPlainText(buffer_);

 end_file=true;

}else if(plain_fsize_>=maxfullsize){

     if(plain_fsize_<2500000)                 { buf_size= 250000 ; }
else if(plain_fsize_>2500000&&plain_fsize_<5500000) { buf_size= 500000 ; }
else if(plain_fsize_>5500000&&plain_fsize_<10000000){ buf_size= 750000 ; }
else if(plain_fsize_>10000000)                { buf_size= 1000000; }
else {;}


                this->setCursor(Qt::WaitCursor);

                //emit long_text_add_signal();
                       long_text_add_slot();
                       //mte->insertPlainText(buffer_);

        }



format=mte->currentCharFormat();
need_save_=false;
play_it_safe_close_=false;
        }
        qDebug() << "BUFFER SIZE AFTER OPNE FILE::" << buffer_.size();
        qDebug() << "END FILE AFTER OPEN ==::==" << (bool)end_file;
    }


}


void MainWindow::search_slot(QString pattern,bool highlight,int color)
{
    //QTime t;
//    QElapsedTimer t;
//     t.start();

size_t position_ = mte->verticalScrollBar()->value();



    QFile file(full_tmp_f_path_);

  if(!file.open(QIODevice::ReadOnly)){

      QMessageBox p;
              p.warning(0,"Error","Temporary data read error. "
      "...");
              qDebug() << "ERROR READING FILE:" << file.fileName();

  }else{

end_file=false;

scroll_buf=0;
buf_start=0;

end_text_cursor_pos_=0;
text_cursor_isSet_toEnd=false;



  QTextStream stream(&file);
  plain_fsize_=file.size();

  buffer_.clear();

  buffer_.append(stream.readAll()/*.toLatin1()*/);

  file.close();



  if(options->is_option_str()){

          std::string str = buffer_.toStdString();
          std::string std_pat = pattern.toStdString();

          if(!highlight){
              search_highlight(str,std_pat,color);
          }else{
              search_highlight_occurrences(str,std_pat,color);
          }

      } else {

    std::wstring wstr=buffer_.toStdWString();
    std::wstring std_wpat = pattern.toStdWString();

    if(!highlight){
        search_highlight_w(wstr,std_wpat,color);
    }else{
        search_highlight_occurrences_w(wstr,std_wpat,color);
    }

    }





        fsize_ = buffer_.size();

        if(fsize_<maxfullsize){

            //qDebug() << "SLIDER POSITION" << position_;


            mte->clear();
        mte->insertHtml("<pre style=\"white-space: pre-wrap;\">"+buffer_
                     +"</pre>");


        mte->verticalScrollBar()->setSliderPosition(position_);

         end_file=true;

         set_cursor_slot(first_occurrence_);

        }else if(fsize_>=maxfullsize){


        mte->clear();

        //buffer for "split-scroll" viewing
             if(fsize_<2500000)                 { buf_size= 250000 ; }
        else if(fsize_>2500000&&fsize_<5500000) { buf_size= 500000 ; }
        else if(fsize_>5500000&&fsize_<10000000){ buf_size= 750000 ; }
        else if(fsize_>10000000)                { buf_size= 1000000; }
        else {;}

             QStringRef subbuf(&buffer_,buf_start,buf_size);


             mte->insertHtml(

                         "<pre style=\"white-space: pre-wrap;\">"+
                         subbuf.toString()
                            +"</pre>"
                         );


             buf_start+= buf_size;

             size_t max_scroll = mte->verticalScrollBar()->maximum();

             scroll_buf=(max_scroll*0.75);

//             qDebug() << "MAX SCROLL == " << max_scroll;
//             qDebug() << "CURRENT SCROLL == " << position_;

end_text_cursor_pos_=mte->textCursor().position();

             //if(position_<scroll_buf){ //{1}
             if(position_<max_scroll){ //{1}


                 if(first_occurrence_<300000){ //{2}
                            if(first_occurrence_<buf_start){ //{3}
                                    //end_text_cursor_pos_=mte->textCursor().position();
//                            qDebug() << "PRE cursor position" << mte->textCursor().position();
                     //emit set_cursor_signal(first_occurrence_);
                                set_cursor_slot(first_occurrence_);
//                                qDebug() << "AFTER first_occurence" << first_occurrence_;
//                                qDebug() << "AFTER cursor position" << mte->textCursor().position();
//                                qDebug() << "AFTER cursor end" << QTextCursor::End;
                            } else { //{3}
//                                qDebug() << "CASE {3}";
                                //emit add_text_signal_2limit(first_occurrence_);
                                 add_text_slot_2limit(first_occurrence_);
                                }

                 } else { //{2}

             mte->verticalScrollBar()->setSliderPosition(position_);

                 }



                } else { //{1}
//                    qDebug() << "CASE {1}";
                     //emit add_text_signal(position_,max_scroll);
                            add_text_slot(position_,max_scroll);
                //mte->verticalScrollBar()->setSliderPosition(position_);
             }


        }



    //clear_check()=false;

    mte->setReadOnly(true);


    ui->statusbar->showMessage(QString::number(occurrences)+" matches found");

//    qDebug() << ("FILE SIZE IS ::"+QString::number(plain_fsize_)
//                               +"BUFFER AFTER SEARCH SIZE IS ::"+QString::number(fsize_));
//    qDebug() << "FORMAT AFTER OPEN ::" << format.background();

  }
//qDebug("Time elapsed: %d ms", t.elapsed());
//  qDebug() << "Time elapsed: " << t.elapsed() << " ms";
//  qDebug() << "END FILE AFTER SEARCH ==::==" << (bool)end_file;
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

size_t scrollval = mte->verticalScrollBar()->value();

//QElapsedTimer t;
// t.start();

        QTextCursor cursor = mte->textCursor();
        cursor.setCharFormat(format);

        //int positio = mte->cursorForPosition(mte->mapFromGlobal(QCursor::pos())).position();

        int selection_start = cursor.selectionStart();

        int selection_end = cursor.selectionEnd();





            QFile file(full_tmp_f_path_);

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

mte->clear();

          mte->insertPlainText(buffer_);

      }




        mte->setReadOnly(false);

//        qDebug() << "12345 Cursor current pos::==" << selection_start << "::" << selection_end;
        if(selection_start<300000){
           cursor.setPosition(selection_start);
           cursor.setPosition(selection_end, QTextCursor::KeepAnchor);
           mte->setTextCursor(cursor);
        }
          emit set_position_signal(scrollval);



//qDebug() << "MTE time == " << t.elapsed();


    }


}



void MainWindow::slider_slot(int)//(int v)
{

        size_t temp = mte->verticalScrollBar()->value();

        ui->statusbar->showMessage("Slider position: "+QString::number(temp));
        if(end_file){ return;}

        if (temp>=(scroll_buf))
        {
        if(!text_cursor_isSet_toEnd){set_cursor_slot(end_text_cursor_pos_);text_cursor_isSet_toEnd=true;}
//            qDebug()<< "Scroll TEST";

            if((buf_start+buf_size)<=fsize_){
            QStringRef subbuf(&buffer_,buf_start,buf_size);
            mte->insertHtml(
                        "<pre style=\"white-space: pre-wrap;\">"+
                        subbuf.toString()
                            +"</pre>"
                            );

            buf_start+= buf_size;

            size_t max_scroll = mte->verticalScrollBar()->maximum();
             scroll_buf=(max_scroll*0.75);   

            } else if((buf_start+buf_size)>fsize_){
                size_t buf_final_temp = fsize_-buf_start;
                QStringRef subbuf(&buffer_,buf_start,buf_final_temp);
                mte->insertHtml(
                                "<pre style=\"white-space: pre-wrap;\">"+
                            subbuf.toString()
                                +"</pre>"
                                );


                end_file=true;
                scroll_buf=0;
                buf_start=0;


            }

        }


}


//void MainWindow::format_slot(const QTextCharFormat &f)
//{
//    qDebug() << "Current format changed: " << f.background();
//}

void MainWindow::mouse_press_slot()
{
//    cursor_position = mte->cursorForPosition(mte->mapFromGlobal(QCursor::pos())).position();
    cursor_position = mte->cursorForPosition(mte->mapFromGlobal((mte->cursor().pos()))).position();
        //ui->statusbar->setStyleSheet("color:red;");
    ui->statusbar->showMessage("Cursor Position: "+QString::number(cursor_position));
    mte->setCurrentCharFormat(format);
}

void MainWindow::add_text_slot(size_t slider_cur_pos, size_t max_slider_buffer )
{
    qDebug() << "ADD TEXT SIGNAL EMITTED";
    if(end_file) return;
    else
    if(fsize_<maxfullsize) return;                                                 // fsize_ OR plain_fsize_?????????????
    else if(slider_cur_pos>max_slider_buffer )
    {
if(!text_cursor_isSet_toEnd){set_cursor_slot(end_text_cursor_pos_);text_cursor_isSet_toEnd=true;}
        do{

            if((buf_start+buf_size)<=fsize_){

        QStringRef subbuf(&buffer_,buf_start,buf_size);

        mte->insertHtml(
                        "<pre style=\"white-space: pre-wrap;\">"+
                        subbuf.toString()
                        +"</pre>"
                        );

        buf_start+= buf_size;

        max_slider_buffer = mte->verticalScrollBar()->maximum();
        scroll_buf=(max_slider_buffer *0.75);

            } else if((buf_start+buf_size)>fsize_){

                size_t buf_final_temp = fsize_-buf_start;
                QStringRef subbuf(&buffer_,buf_start,buf_final_temp);
                mte->insertHtml(
                                "<pre style=\"white-space: pre-wrap;\">"+
                            subbuf.toString()
                                +"</pre>"
                                );

                end_file=true;
                scroll_buf=0;
                buf_start=0;

                if(slider_cur_pos>max_slider_buffer){
                mte->verticalScrollBar()->setSliderPosition(mte->verticalScrollBar()->maximum());

                return;                                        // return or break??
                }
            }

        }while(slider_cur_pos>max_slider_buffer);

    }

    mte->verticalScrollBar()->setSliderPosition(slider_cur_pos);

}


void MainWindow::add_text_slot_2limit(size_t limit_pos) //!!
{

    do{
if(end_file)break;

        if((buf_start+buf_size)<=fsize_){

    QStringRef subbuf(&buffer_,buf_start,buf_size);

    mte->insertHtml(
                    "<pre style=\"white-space: pre-wrap;\">"+
                    subbuf.toString()
                    +"</pre>"
                    );

    buf_start+= buf_size;

    scroll_buf=((mte->verticalScrollBar()->maximum())*0.75);

        } else if((buf_start+buf_size)>fsize_){

            size_t buf_final_temp = fsize_-buf_start;
            QStringRef subbuf(&buffer_,buf_start,buf_final_temp);
            mte->insertHtml(
                            "<pre style=\"white-space: pre-wrap;\">"+
                            subbuf.toString()
                            +"</pre>"
                            );

            end_file=true;
            scroll_buf=0;

            if(buf_start>=first_occurrence_)set_cursor_slot(first_occurrence_);
            else set_cursor_slot(QTextCursor::End);

            buf_start=0;

            this->setCursor(Qt::ArrowCursor);

            return;
        }


    }while(limit_pos>buf_start);

    if(first_occurrence_<=buf_start)set_cursor_slot(first_occurrence_);
    else set_cursor_slot(QTextCursor::End);

}


void MainWindow::set_position_slot(size_t scrollval)
{
    qDebug() << "SIGNAL SET POSITION EMITED";
    mte->verticalScrollBar()->setSliderPosition(scrollval);
}


void MainWindow::set_cursor_slot(int positio)
{
    QTextCursor cursor = mte->textCursor();
    cursor.setPosition(positio);
    mte->setTextCursor(cursor);
    qDebug() << "SIGNAL CURSOR EMITED";
}

void MainWindow::long_text_add_slot()
{

    do{

        if(end_file)break;

        if((buf_start+buf_size)<=plain_fsize_){

    QStringRef subbuf(&buffer_,buf_start,buf_size);

    mte->insertPlainText(
                    subbuf.toString()
                    );

    buf_start+= buf_size;

        } else if((buf_start+buf_size)>plain_fsize_){

            size_t buf_final_temp = plain_fsize_-buf_start;
            QStringRef subbuf(&buffer_,buf_start,buf_final_temp);
            mte->insertPlainText(
                        subbuf.toString()
                            );

            end_file=true;
            scroll_buf=0;
            buf_start=0;

            this->setCursor(Qt::ArrowCursor);

            break;
        }


    }while(1);

    this->setCursor(Qt::ArrowCursor);

}

void MainWindow::cursor_shape_slot(int shape)
{
    switch(shape){
    case 0:  this->setCursor(Qt::ArrowCursor); break;
    case 2:  this->setCursor(Qt::WaitCursor); break;
    default: this->setCursor(Qt::ArrowCursor); break;
    }
}


// DESIGN //
void MainWindow::on_actionStart_search_triggered()
{
    on_search_button_clicked();
}

void MainWindow::on_actionStart_search_2_triggered()
{
    on_search_button_clicked();
}

void MainWindow::on_actionNew_triggered()
{
    if(need_save_&&(!mte->document()->isEmpty())){
    //this->save_=new saveDialog;
        //this->save_->setModal(true);
        this->save_->show();
        this->save_->exec();
        if(save_->is_cancel_button_clicked()){this->save_->is_cancel_button_clicked()=false;return;}
    }



                this->save_->is_cancel_button_clicked()=false;
                buttons_enabled(true);

                end_file=true;
                scroll_buf=0;
                buf_start=0;

                end_text_cursor_pos_=0;
                text_cursor_isSet_toEnd=true;


                mte->clear();

                //need_save_=true;
                need_save_as_=true;
play_it_safe_close_=false;

}

void MainWindow::on_actionCut_triggered()
{
    mte->cut();
}

void MainWindow::on_actionUndo_triggered()
{
    mte->undo();
}

void MainWindow::on_actionRedo_triggered()
{
    mte->redo();
}

void MainWindow::on_actionCopy_triggered()
{
    mte->copy();
}

void MainWindow::on_actionPaste_triggered()
{
    mte->paste();
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(0,"");
}

void MainWindow::on_actionAbout_Search_pattern_triggered()
{
QString logo="";
QString abc=":/rec/icons/rec/icons/accesibility_window_abc.png";
abc="<img src='"+abc+"' height='32' width='32'>";
QString tux=":/rec/icons/rec/icons/tux_color.png";
tux="<img src='"+tux+"' height='48' width='48'>";
QString input=":/rec/icons/rec/icons/input_devices.png";
input="<img src='"+input+"' height='32' width='32'>";
QString screen=":/rec/icons/rec/icons/monitor_windows.png";
screen="<img src='"+screen+"' height='32' width='32'>";
QString loop=":/rec/icons/rec/icons/search2.png";
loop="<img src='"+loop+"' height='32' width='32'>";
QString note=":/rec/icons/rec/icons/notepad-5.png";
note="<img src='"+note+"' height='32' width='32'>";
QString mouse=":/rec/icons/rec/icons/mouse-0.png";
mouse="<img src='"+mouse+"' height='32' width='32'>";
QString kbmouse=":/rec/icons/rec/icons/kbmouse.png";
kbmouse="<img src='"+kbmouse+"' height='32' width='32'>";
QString success=":/rec/icons/rec/icons/success.png";
success="<img src='"+success+"' height='32' width='32'>";

    QFile file(":/rec/icons/rec/icons/logo.txt");
//qDebug() << dir;
        if(!file.open(QIODevice::ReadOnly))
            ;
        else{
            QTextStream in(&file);
        logo="<pre style=\"white-space: pre-wrap;\">"+in.readAll()+"</pre>";
        file.close();
        }

    QMessageBox::about(0,"Search Pattern about",note+loop+tux+kbmouse+abc+success+
                         "<p style=\"font-family:'Serif';font-size:22px;\">Plain text edit with "
                         "multiply occurrences search functional. "
                         "Program uses <b>std::string</b>'s very optimized methods in search algorithm;"
                         " was made JFF and Qt/C++ learning basic technics goal.:)</p><br>"
                                          +logo);
}

void MainWindow::need_save()
{
    need_save_=true;

    if(((!buttons_isEnabled_)&&(!mte->document()->isEmpty()))){buttons_enabled(true);buttons_isEnabled_=true;}
    if(buttons_isEnabled_)play_it_safe_close_=true;
    //mte->blockSignals(false);
}

void MainWindow::on_actionSave_triggered()
{
    if(need_save_as_){

        on_actionSave_as_triggered();
        //return;
    } else
    {


        QString subfolder_ = file_path_+'/'+file_name_;
        //temp_subfolder_ = temp_file_path_ + "/" + prog_name_;

        // Checking if temp directory exist -- else create it!
        QDir dir(subfolder_);
        if (!dir.exists())
            dir.mkpath(".");

              QFile file(subfolder_);

            if(!file.open(QIODevice::WriteOnly)){

                QMessageBox p;
                        p.warning(0,"Error","Save data error. "
                "Please, check read/write permissions in current file directory, ");
                        qDebug() << "ERROR READING FILE:" << file.fileName();
                        return;

            }else{

            QTextStream stream(&file);

            stream << mte->toPlainText();

            file.close();

            }


    }

    need_save_=false;
}

void MainWindow::on_actionSave_as_triggered()
{

    QString save_as_file = QFileDialog::getSaveFileName(this, "Save as", QDir::homePath());

    if(!save_as_file.isEmpty()&& !save_as_file.isNull()) //"Cancel" in choose dialog case
    {

    QFile file(save_as_file);

    if(!file.open(QIODevice::WriteOnly))
        QMessageBox::information(0,"info",file.errorString());
    else{
        buttons_enabled(true);

        end_file=true;
        scroll_buf=0;
        buf_start=0;

        end_text_cursor_pos_=0;
        text_cursor_isSet_toEnd=true;


        QTextStream stream(&file);

        stream << mte->toPlainText();


        file_path_=QFileInfo(file).path();
        file_name_ = QFileInfo(file).fileName();
        temp_file_path_ = file_path_;

//        qDebug() << "SAVE AS PATH AFTER==" << file_path_ << "::" << "SAVE AS NAME AFTER" << file_name_ <<
//                    "SAVE AS TEMP FILE PATH AFTER==" << temp_file_path_;

        file.close();

    }

    need_save_as_=false;

    }

}

void MainWindow::on_actionOptions_triggered()
{
    //DockWidget * options = new DockWidget;
    options->show();
}

void MainWindow::on_actionOptions_2_triggered()
{
    on_actionOptions_triggered();
}

void MainWindow::on_actionHints_tips_triggered()
{
    hints->show();
}
