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

    void show_occurrences(std::string const&,char const*);

    QString show_highlighting(const std::string& text,const char*pattern);

    void show_occurrences_edit(std::string const&,char const*);

    QString show_highlighting_edit(const std::string& text,const char*pattern,int color);

    void show_highlighting_edit2(const std::string& text,const char*pattern,int color);

    void show_highlighting_edit3(const std::string& text,const char*pattern,int color);





    bool & clear_check(){static bool clear = false;return clear;}

    bool & select_check(){static bool select = false;return select;}




//    friend class QTextEdit;
//    void QTextEdit::mouseReleaseEvent(QMouseEvent *e);

private slots:
    void on_pushButton_clicked();

    void on_actionOpen_triggered();

    void on_tempButton_clicked();

//    void on_tiny_clicked();

    void on_reset_text_clicked();

    void on_actionExit_triggered();

    void clc_released();


public slots:

//    void search_slot(QString str="");

    void search_slot2(QString str="",bool highlight=false,int color=0);

//    void search_slot3(QString str="");

//    void highlight_slot();

    void mte_slot();

    void select_text_slot();

//    void mouse_press_slot();

    void slider_slot(int);
    void slider_slot(int,int);

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
};
#endif // MAINWINDOW_H
