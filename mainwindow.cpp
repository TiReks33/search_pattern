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
#include <QElapsedTimer>
#include <QTextBlock>
#include <QTimer>
#include <QThread>

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
    , file_size_limit_(24000000) //[24mb24.5]
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

//2nd widget (func but)




//3d widget (for "search button")
    QWidget *search_widget = new QWidget(this);
    QGridLayout * layout3 = new QGridLayout(search_widget);

    layout3->addWidget(ui->search_button);

    search_widget->setFixedSize(175,175);
    search_widget->move(600,350);
    search_widget->show();

//    QPushButton *clc_button = new QPushButton(this);
//    clc_button->setText("clear screen");
//    clc_button->setFixedSize(100,50);
//    clc_button->move(700,500);
//    clc_button->show();

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%=====SIGNALS=====%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

       connect(d_,&Dialog::search_signal2, this, &MainWindow::search_slot);
       connect(mte, &MineTextEdit::signal, this, &MainWindow::mte_slot);
       //connect(clc_button, SIGNAL (released()),this, SLOT (clc_released()));

       //mte->verticalScrollBar()->
       mte->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

       connect(mte->verticalScrollBar(),SIGNAL(sliderMoved(int)),this/*mte->verticalScrollBar()*/,SLOT(slider_slot(int)),Qt::QueuedConnection);
       connect(mte->verticalScrollBar(),SIGNAL(valueChanged(int)),this/*mte->verticalScrollBar()*/,SLOT(slider_slot(int)),Qt::QueuedConnection);

       connect(mte,&MineTextEdit::currentCharFormatChanged,this,&MainWindow::format_slot);

       connect(mte,&MineTextEdit::mouse_press_signal,this,&MainWindow::mouse_press_slot);

       connect(this,&MainWindow::add_text_signal,this,&MainWindow::add_text_slot,Qt::QueuedConnection);

       connect(this,&MainWindow::add_text_signal_2limit,this,&MainWindow::add_text_slot_2limit,Qt::QueuedConnection);

       connect(this,&MainWindow::set_position_signal,this,&MainWindow::set_position_slot,Qt::QueuedConnection);

       connect(this,SIGNAL(set_cursor_signal(int)),this,SLOT(set_cursor_slot(int)),Qt::QueuedConnection);

       connect(this,&MainWindow::long_text_add_signal, this, &MainWindow::long_text_add_slot,Qt::QueuedConnection);
       connect(this,SIGNAL(cursor_shape_signal(int)),this,SLOT(cursor_shape_slot(int)));
       //connect(this,SIGNAL(single_shot_signal()),this,SLOT(single_shot_slot()), Qt::QueuedConnection);

       connect(this,SIGNAL(total_occur(size_t)),r_,SLOT(total_occur_slot(size_t)));

mte->setFont(QFont("DejaVu Sans Mono"));
//this->mte->setUndoRedoEnabled(false);
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



void MainWindow::on_search_button_clicked()
{
    //QString file_dir = QFileDialog::getOpenFileName(this, "Open a file", QDir::homePath());
        //QFile file("/home/alexander/cpp/$temp1");
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

    QString open_file = QFileDialog::getOpenFileName(this, "Open a file", QDir::homePath());

qDebug() << "QDir::temp() ::" << QDir::temp() << "QDir::tempPath() ::" << QDir::tempPath();

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

//end_file=true; //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1


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

ui->search_button->setEnabled(true);
ui->search_button->setStyleSheet("background-color: blue;");

//"<span style=\"background-color:"+color_+";\">"

//                +QString::fromStdString(after)

//                +"</span>";


ui->search_button->setToolTip("<font style=\"background-color:yellow;\">Search function. Click to enter the search pattern.</font>");

//mte->setStyleSheet("");

format=mte->currentCharFormat();

    }
        qDebug() << "BUFFER SIZE AFTER OPNE FILE::" << buffer_.size();
        qDebug() << "END FILE AFTER OPEN ==::==" << (bool)end_file;
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
      //ui->search_button->setEnabled(false);
end_file=false;

scroll_buf=0;
buf_start=0;

end_text_cursor_pos_=0;
text_cursor_isSet_toEnd=false;

      //on_search_clc_button_clicked();

//            clear_check()=true;

  QTextStream stream(&file);
  plain_fsize_=file.size();

  buffer_.clear();

  buffer_.append(stream.readAll());

  file.close();





    std::string str = buffer_.toStdString();


    std::string std_pat = pattern.toStdString();
    char const *pat_cstr = std_pat.c_str();



        if(!highlight){
            search_highlight(str,pat_cstr,color);
        }else{
            search_highlight_occurrences(str,pat_cstr,color);
        }


        fsize_ = buffer_.size();
qDebug() << "123456 FSIZE_::" << fsize_;

        if(fsize_<maxfullsize){

            qDebug() << "SLIDER POSITION" << position_;


            mte->clear();
        mte->insertHtml("<pre style=\"white-space: pre-wrap;\">"+buffer_
                     +"</pre>");


        mte->verticalScrollBar()->setSliderPosition(position_);

         end_file=true;                                          //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!111

         set_cursor_slot(first_occurrence_);

        }else if(fsize_>=maxfullsize){


            //is_split=true;
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

             qDebug() << "MAX SCROLL == " << max_scroll;
             qDebug() << "CURRENT SCROLL == " << position_;
end_text_cursor_pos_=mte->textCursor().position();
             //if(position_<scroll_buf){ //{1}
             if(position_<max_scroll){ //{1}


                 if(first_occurrence_<300000){ //{2}
                            if(first_occurrence_<buf_start){ //{3}
                                    //end_text_cursor_pos_=mte->textCursor().position();
                            qDebug() << "PRE cursor position" << mte->textCursor().position();
                     //emit set_cursor_signal(first_occurrence_);
                                set_cursor_slot(first_occurrence_);
                                qDebug() << "AFTER first_occurence" << first_occurrence_;
                                qDebug() << "AFTER cursor position" << mte->textCursor().position();
                                qDebug() << "AFTER cursor end" << QTextCursor::End;
                            } else { //{3}
                                qDebug() << "CASE {3}";
                                //emit add_text_signal_2limit(first_occurrence_);
                                 add_text_slot_2limit(first_occurrence_);
                                }

                 } else { //{2}

             mte->verticalScrollBar()->setSliderPosition(position_);

                 }



                } else { //{1}
                    qDebug() << "CASE {1}";
                     //emit add_text_signal(position_,max_scroll);
                            add_text_slot(position_,max_scroll);
                //mte->verticalScrollBar()->setSliderPosition(position_);
             }


        }



    clear_check()=false;

    mte->setReadOnly(true);


    ui->statusbar->showMessage(QString::number(occurrences)+" matches found");

    qDebug() << ("FILE SIZE IS ::"+QString::number(plain_fsize_)
                               +"BUFFER AFTER SEARCH SIZE IS ::"+QString::number(fsize_));
    qDebug() << "FORMAT AFTER OPEN ::" << format.background();

  }
//qDebug("Time elapsed: %d ms", t.elapsed());
  qDebug() << "Time elapsed: " << t.elapsed() << " ms";
  qDebug() << "END FILE AFTER SEARCH ==::==" << (bool)end_file;
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

        //ui->search_button->setEnabled(true);

        qDebug() << "12345 Cursor current pos::==" << selection_start << "::" << selection_end;
        if(selection_start<300000){
           cursor.setPosition(selection_start);
           cursor.setPosition(selection_end, QTextCursor::KeepAnchor);
           mte->setTextCursor(cursor);
        }
          emit set_position_signal(scrollval);



qDebug() << "MTE time == " << t.elapsed();


    }


}



void MainWindow::slider_slot(int)//(int v)
{

        size_t temp = mte->verticalScrollBar()->value();

        ui->statusbar->showMessage(QString::number(temp));
        if(end_file){ return;}
        if (temp>=(scroll_buf)) //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!111
        {
        if(!text_cursor_isSet_toEnd){set_cursor_slot(end_text_cursor_pos_);text_cursor_isSet_toEnd=true;}
            qDebug()<< "Scroll TEST";

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

            qDebug() << "SCROLL BUFFFFFFUUUUUU" << scroll_buf;

            } else if((buf_start+buf_size)>fsize_){
                size_t buf_final_temp = fsize_-buf_start;
                QStringRef subbuf(&buffer_,buf_start,buf_final_temp);
                mte->insertHtml(
                                "<pre style=\"white-space: pre-wrap;\">"+
                            subbuf.toString()
                                +"</pre>"
                                );

                qDebug() << "buf_start" << buf_start << "buf_size" << buf_size << "fsize_" << fsize_ << "buf_final_temp" << buf_final_temp
                         << "scroll_buf" << scroll_buf << "temp" << temp;

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
    //cursor_position = mte->cursorForPosition(mte->mapFromGlobal(QCursor::pos())).position();
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
 //if(end_file)return;
    //if(!text_cursor_isSet_toEnd){set_cursor_slot(end_text_cursor_pos_);text_cursor_isSet_toEnd=true;}
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
    //cursor.setPosition(mte->cursorForPosition(mte->mapFromGlobal(QCursor::pos())).position());
    cursor.setPosition(positio);
    mte->setTextCursor(cursor);
    qDebug() << "SIGNAL CURSOR EMITED";
}

void MainWindow::long_text_add_slot()
{
//    if(end_file)return;

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

//void MainWindow::single_shot_slot()
//{

//}



//void MainWindow::on_pushButton_clicked()
//{
//    this->setCursor(Qt::ArrowCursor);
//}

//void MainWindow::on_pushButton_2_clicked()
//{
//    this->setCursor(Qt::WaitCursor);
//}
