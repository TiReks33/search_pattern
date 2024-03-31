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
//#include <QTime>
#include <QElapsedTimer>

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

    , prog_name_("Search_pattern")
    , file_path_(QDir::homePath())
    , file_name_("")
    , temp_file_path_(QDir::tempPath())
    , temp_file_prefix_("/.$temp_")
    , temp_subfolder_()
    , full_tmp_f_path_(temp_file_path_)

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

    , format(mte->currentCharFormat())
{
    ui->setupUi(this);
//"Search button"
       ui->search_button->setFixedHeight(50);
       ui->search_button->setFixedWidth(50);
       QRect rect(0,0,50,50);
       QRegion region(rect, QRegion::Ellipse);
       ui->search_button->setMask(region);


//1st widget (main editor window)
       QWidget *centralWidget = new QWidget(this);
    centralWidget->setFixedSize(660,660);
    centralWidget->move(0,30);

    QVBoxLayout *layout = new QVBoxLayout(centralWidget);


    layout->addWidget(mte,50);

    mte->setReadOnly(false);
    //mte->setUndoRedoEnabled(false);

//2nd widget (func but)
    QWidget *buttons_widget = new QWidget(this);

    QVBoxLayout * layout2 = new QVBoxLayout(buttons_widget);


    ui->reset_text->setText("Reset search\nhighlight");
    layout2->addWidget(ui->reset_text);
    ui->reset_text->setFixedSize(100,50);

    buttons_widget->setFixedSize(125,125);
    buttons_widget->move(680,275);
    buttons_widget->show();


//3d widget (for "search button")
    QWidget *search_widget = new QWidget(this);
    QGridLayout * layout3 = new QGridLayout(search_widget);

    layout3->addWidget(ui->search_button);

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

       //mte->verticalScrollBar()->
       mte->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

       connect(mte->verticalScrollBar(),SIGNAL(sliderMoved(int)),this/*mte->verticalScrollBar()*/,SLOT(slider_slot(int)),Qt::QueuedConnection);
       connect(mte->verticalScrollBar(),SIGNAL(valueChanged(int)),this/*mte->verticalScrollBar()*/,SLOT(slider_slot(int)),Qt::QueuedConnection);

       connect(mte,&MineTextEdit::currentCharFormatChanged,this,&MainWindow::format_slot);

       connect(mte,&MineTextEdit::mouse_press_signal,this,&MainWindow::mouse_press_slot);

       connect(this,&MainWindow::add_text_signal,this,&MainWindow::add_text_slot,Qt::QueuedConnection);

       connect(this,&MainWindow::set_position_signal,this,&MainWindow::set_position_slot,Qt::QueuedConnection);

       connect(this,SIGNAL(set_cursor_signal(int)),this,SLOT(set_cursor_slot(int)),Qt::QueuedConnection);

       connect(this,SIGNAL(set_cursor_signal(QTextCursor)),this,SLOT(set_cursor_slot(QTextCursor)),Qt::QueuedConnection);

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

size_t strlenpp(std::string const *str)
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

//main_string = "<sss style=\"white-space: pre-wrap;background-color:white;\">";
//main_string = "<sssss style=\"white-space: pre-wrap;\">"; //!!!!!!!!!!!!!!!!!!DELETE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
bool frst_=false;
  do{
          text_find=text.find(pattern,start_pos);
    if(!frst_){frst_=true;first_occurrence_=text_find;}
            if (text_find==std::string::npos) break;

            ++occurrences;

before=text.substr(start_pos,text_find - start_pos);


after=text.substr(text_find,strlen);


    main_string+=QString::fromStdString(before)

//                .replace(QString("\n"), QString("<br>"))

                +"<span style=\"background-color:"+color_+";\">"

                +QString::fromStdString(after)

//                .replace(QString("\n"), QString("<br>"))

                +"</span>";

start_pos=text_find+strlen;

    } while(1);

after=text.substr(start_pos);


    main_string+=QString::fromStdString(after);

//                .replace(QString("\n"), QString("<br>"))

//                +"</sssss>";


    //return main_string;

    buffer_.clear();
    //mte->clearMask();

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


    main_string+=QString::fromStdString(before)

//                .replace(QString("\n"), QString("<br>"))

                +"<abc style=\"background-color:"+color_+";\">"

                +QString::fromStdString(after)

//                .replace(QString("\n"), QString("<br>"))

                +"</abc>";

start_pos=text_find+strlen;

    } while(1);

if(occurrences==0)
result+="Entries not found.:(";

after=text.substr(start_pos);


    main_string+=QString::fromStdString(after)

//            .replace(QString("\n"), QString("<br>"))

            +"</sss>";

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



void MainWindow::on_search_button_clicked()
{
    //QString file_dir = QFileDialog::getOpenFileName(this, "Open a file", QDir::homePath());
        //QFile file("/home/alexander/cpp/$temp1");

    temp_subfolder_ = temp_file_path_ + "/";
    //temp_subfolder_ = temp_file_path_ + "/" + prog_name_;

    // Checking if temp directory exist -- else create it!
    QDir dir(temp_subfolder_);
    if (!dir.exists())
        dir.mkpath(".");

          full_tmp_f_path_ = temp_subfolder_ + temp_file_prefix_ + file_name_;
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

//    {
//        qDebug() << "ERROR READING FILE";
//    }

    //}
        d_->setModal(true);
    d_->show();

    d_->exec();
        }
}


void MainWindow::on_actionOpen_triggered()
{

    QString open_file = QFileDialog::getOpenFileName(this, "Open a file", QDir::homePath());

qDebug() << "QDir::temp() ::" << QDir::temp() << "QDir::tempPath() ::" << QDir::tempPath();

    QFile file(open_file);
        if(!file.open(QIODevice::ReadOnly))
            QMessageBox::information(0,"info",file.errorString());
        else{
            mte->clear();
            QTextStream in(&file);
            plain_fsize_=file.size();

            file_path_=QFileInfo(file).path();
            file_name_ = QFileInfo(file).fileName();
            temp_file_path_ = file_path_;

            ui->statusbar->showMessage("File path: "+file_path_+' '+"File Size: "+QString::number(file.size()));

            buffer_.clear();

            buffer_.append(in.readAll());

            file.close();

end_file=true; //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1



//             mte->setHtml("<opf style=\"white-space: pre-wrap;\">"+buffer_
//                                                                  +"</opf>");
             mte->setPlainText(buffer_);

             // search_button "activate" trigger
        ui->search_button->setEnabled(true);
        ui->search_button->setStyleSheet("background-color: blue;");

        //mte->setStyleSheet("");

        format=mte->currentCharFormat();
        }

}





void MainWindow::search_slot(QString pattern,bool highlight,int color)
{
    //QTime t;
    QElapsedTimer t;
     t.start();

size_t position_ = mte->verticalScrollBar()->value();



    QFile file(full_tmp_f_path_);

  if(!file.open(QIODevice::ReadOnly)){

      QMessageBox p;
              p.warning(0,"Error","Temporary data read error. "
      "...");
              qDebug() << "ERROR READING FILE:" << file.fileName();

  }else{
      ui->search_clc_button->setEnabled(true);
      on_search_clc_button_clicked();

  QTextStream stream(&file);
  plain_fsize_=file.size();

  buffer_.clear();
  //mte->clear();

  buffer_.append(stream.readAll());
    //buffer_=stream.readAll();

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

//auto bfs = buffer_.size();
//fsize_=bfs;
        fsize_ = buffer_.size();

        if(plain_fsize_<maxfullsize){
            //int scrollval = mte->verticalScrollBar()->value();
            //QScrollBar scroll;scroll.setValue(scrollval);


            qDebug() << "SLIDER POSITION" << position_;

            mte->clear();
        mte->insertHtml("<pre style=\"white-space: pre-wrap;\">"+buffer_
//                     .replace(QString("\n"), QString("<br>"))
                     +"</pre>");
        //mte->verticalScrollBar()->setValue(scrollval);

        mte->verticalScrollBar()->setSliderPosition(position_);

         end_file=true;

        }else if(plain_fsize_>=maxfullsize){


            //is_split=true;
        mte->clear();
        //buffer for "split-scroll" viewing
             if(plain_fsize_<2500000)                 { buf_size= 250000 ; }
        else if(plain_fsize_>2500000&&plain_fsize_<5500000) { buf_size= 500000 ; }
        else if(plain_fsize_>5500000&&plain_fsize_<10000000){ buf_size= 750000 ; }
        else if(plain_fsize_>10000000)                { buf_size= 1000000; }
        else {;}

             QStringRef subbuf(&buffer_,buf_start,buf_size);


             mte->insertHtml(

                         "<pre style=\"white-space: pre-wrap;\">"+
                         subbuf.toString()
//                                      .replace(QString("\n"), QString("<br>"))
                                      +"</pre>"
                         );

//             mte->insertHtml(subbuf.toString());

             buf_start+= buf_size;
             //buf_end = buf_start + buf_size;
             size_t max_scroll = mte->verticalScrollBar()->maximum();

             scroll_buf=(max_scroll*0.75);

             qDebug() << "MAX SCROLL == " << max_scroll;
             qDebug() << "CURRENT SCROLL == " << position_;

             if(position_<max_scroll)
             mte->verticalScrollBar()->setSliderPosition(position_);
             else{

                     emit add_text_signal(position_,max_scroll);

                //mte->verticalScrollBar()->setSliderPosition(position_);
             }
             //end_file=false; //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1

        }


//    mte->verticalScrollBar()->setValue(scrollval);

    clear_check()=false;

    mte->setReadOnly(true);

    //mte->setTextColor("black");

    ui->statusbar->showMessage(QString::number(occurrences)+" matches found");

    qDebug() << ("FILE SIZE IS ::"+QString::number(plain_fsize_)
                               +"BUFFER AFTER SEARCH SIZE IS ::"+QString::number(fsize_));
    qDebug() << "FORMAT AFTER OPEN ::" << format.background();

  }
//qDebug("Time elapsed: %d ms", t.elapsed());
  qDebug() << "Time elapsed: " << t.elapsed() << " ms";
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

size_t scrollval = mte->verticalScrollBar()->value();

QElapsedTimer t;
 t.start();

       // QTextCursor cursor = mte->textCursor();
       // cursor.setCharFormat(format);

        //int positio = mte->cursorForPosition(mte->mapFromGlobal(QCursor::pos())).position();

      //  int selection_start = cursor.selectionStart();

      //  int selection_end = cursor.selectionEnd();





            QFile file(full_tmp_f_path_);

          if(!file.open(QIODevice::ReadOnly)){

              QMessageBox p;
                      p.warning(0,"Error","Temporary data read error. "
              "...");
                      qDebug() << "ERROR READING FILE:" << file.fileName();

          }else{

          //positio = mte->cursorForPosition(mte->mapFromGlobal(QCursor::pos())).position();
         // qDebug() << "POSITION == " << positio;
          //mte->setReadOnly(false);



          QTextStream stream(&file);
          buffer_.clear();

          buffer_.append(stream.readAll());

          //buffer_=stream.readAll();

          file.close();

          //mte->setHtml("<def style=\"background-color: white;white-space: pre-wrap;\">"+buffer_+"</def>");
mte->clear();
          mte->insertPlainText(buffer_);
          //mte->setPlainText(buffer_);
//QTextCursor kkk = mte->textCursor();
//kkk.setPosition(positio);
//mte->setTextCursor(kkk);
//mte->verticalScrollBar()->setSliderPosition(scrollval);
      }


       //  cursor.setPosition(selection_start);



       // cursor.setPosition(selection_end, QTextCursor::KeepAnchor);

        //mte->setTextCursor(cursor);

//mte->setReadOnly(false);
          emit set_position_signal(scrollval);
          //emit set_cursor_signal(scrollval);



        //mte->verticalScrollBar()->setSliderPosition(scrollval);


        //emit set_cursor_signal(cursor);


        //mte->setVerticalScrollBar(scroll);
mte->setReadOnly(false);



qDebug() << "MTE time == " << t.elapsed();


//        this->mte_slot();
//mte->verticalScrollBar()->setValue(scrollval);
    }

//    auto cccurs = mte->textCursor();
//    cccurs.setPosition(first_occurrence_);
//    mte->setTextCursor(cccurs);

//    mte->verticalScrollBar()->setValue(2000);

}



void MainWindow::slider_slot(int)//(int v)
{

        size_t temp = mte->verticalScrollBar()->value();

        ui->statusbar->showMessage(QString::number(temp));
        if(end_file){ return;}
        if (temp>=(scroll_buf))
        {

            qDebug()<< "Scroll TEST";

            if(buf_start+buf_size<=fsize_){
            QStringRef subbuf(&buffer_,buf_start,buf_size);
            mte->insertHtml(
                        "<pre style=\"white-space: pre-wrap;\">"+
                        subbuf.toString()
//                            .replace(QString("\n"), QString("<br>"))
                            +"</pre>"
                            );
//            mte->insertHtml(subbuf.toString());
            buf_start+= buf_size;

            size_t max_scroll = mte->verticalScrollBar()->maximum();
             scroll_buf=(max_scroll*0.75);



            } else if((buf_start+buf_size)>fsize_){
                size_t buf_final_temp = fsize_-buf_start;
                QStringRef subbuf(&buffer_,buf_start,buf_final_temp);
                mte->insertHtml(
                                "<pre style=\"white-space: pre-wrap;\">"+
                            subbuf.toString()
//                                .replace(QString("\n"), QString("<br>"))
                                +"</pre>"
                                );
//                mte->insertHtml(subbuf.toString());
                end_file=true;
                scroll_buf=0;
                buf_start=0;


            }

        }


}


void MainWindow::format_slot(const QTextCharFormat &f)
{
    qDebug() << "Current format changed: " << f.background();
}

void MainWindow::mouse_press_slot()
{
    cursor_position = mte->cursorForPosition(mte->mapFromGlobal(QCursor::pos())).position();
    ui->statusbar->showMessage(QString::number(cursor_position));
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
        do{
        QStringRef subbuf(&buffer_,buf_start,buf_size);

        mte->insertHtml(
                        "<pre style=\"white-space: pre-wrap;\">"+
                        subbuf.toString()
                        +"</pre>"
                        );

        buf_start+= buf_size;

        max_slider_buffer = mte->verticalScrollBar()->maximum();
        scroll_buf=(max_slider_buffer *0.75);
        }while(slider_cur_pos>max_slider_buffer );
    }
    mte->verticalScrollBar()->setSliderPosition(slider_cur_pos);
}


void MainWindow::set_position_slot(size_t scrollval)
{
    qDebug() << "SIGNAL SET POSITION EMITED";
    mte->verticalScrollBar()->setSliderPosition(scrollval);
}

void MainWindow::set_cursor_slot(int positio)
{
    QTextCursor cursor = mte->textCursor();
    //cursor.setPosition(mte->cursorForPosition(mte->mapFromGlobal(QCursor::pos())).position());
    cursor.setPosition(positio);
    mte->setTextCursor(cursor);
    qDebug() << "SIGNAL CURSOR EMITED";
}

void MainWindow::set_cursor_slot(size_t positio)
{
    QTextCursor cursor = mte->textCursor();
    //cursor.setPosition(mte->cursorForPosition(mte->mapFromGlobal(QCursor::pos())).position());
    cursor.setPosition(positio);
    mte->setTextCursor(cursor);
    qDebug() << "SIGNAL CURSOR EMITED";
}

void MainWindow::set_cursor_slot(QTextCursor cursor)
{
    mte->setTextCursor(cursor);
}




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
