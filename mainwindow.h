#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dialog.h"
#include <QTextEdit>
#include "minetextedit.h"
#include <QTextStream>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void closeEvent(QCloseEvent *event);



    void search_highlight(const std::string& text,const char*pattern,int color);

    void search_highlight_occurrences(const std::string& text,const char*pattern,int color);





    bool & clear_check(){static bool clear = false;return clear;}

    bool & select_check(){static bool select = false;return select;}


private slots:
    void on_pushButton_clicked();

    void on_actionOpen_triggered();

    void on_tempButton_clicked();

    void on_reset_text_clicked();

    void on_actionExit_triggered();

    void clc_released();


public slots:

    void search_slot(QString str="",bool highlight=false,int color=0);

    void mte_slot();

    void slider_slot(int);

    //void slider_slot(int,int);

private:
    Ui::MainWindow *ui;
    Dialog * d_;
    size_t occurrences;

    MineTextEdit *mte;
    QString buffer_;
    QTextStream in_;
    QString temp_file_path_;
    QString temp_file_name_;
    //QString search_;

    // text file partial read logic
    // (by slider (vertical scroll-bar)
    size_t fsize_;
    size_t maxfullsize;
    size_t buf_size;
    size_t scroll_buf;
    size_t buf_start;
    bool end_file;

};
#endif // MAINWINDOW_H
