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
    void on_search_button_clicked();

    void on_actionOpen_triggered();

//    void on_reset_text_clicked();

    void on_actionExit_triggered();

//    void clc_released();


//    void on_bigRead_button_clicked();

//    void on_search_clc_button_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

public slots:

    void search_slot(QString str="",bool highlight=false,int color=0);

    void mte_slot();

    void slider_slot(int);

    void format_slot(const QTextCharFormat &f);

    void mouse_press_slot();

    void add_text_slot(size_t,size_t);

    void add_text_slot_2limit(size_t);

    void set_position_slot(size_t);

    void set_cursor_slot(int);

    //void set_cursor_slot(size_t);

    //void set_cursor_slot(QTextCursor);

    //void slider_slot(int,int);

    void long_text_add_slot();

    void cursor_shape_slot(int);

    //void single_shot_slot();

signals:

    void add_text_signal(size_t,size_t);

    void add_text_signal_2limit(size_t); //new

    void set_position_signal(size_t);

    void set_cursor_signal(int);

    void set_cursor_signal(size_t);

    //void set_cursor_signal(QTextCursor);

    void long_text_add_signal();

    void cursor_shape_signal(int);

    //void single_shot_signal();

private:
    Ui::MainWindow *ui;
    Dialog * d_;
    size_t occurrences;

    MineTextEdit *mte;
    QString buffer_;
    QTextStream in_;

    size_t first_occurrence_;

    QString prog_name_;
    QString file_path_;
    QString file_name_;
    QString temp_file_path_;
    QString temp_file_prefix_;
    QString temp_subfolder_;
    QString full_tmp_f_path_;

    //QString search_;

    // text file partial read logic
    // (by slider (vertical scroll-bar)
    size_t plain_fsize_;
    size_t fsize_;
    size_t maxfullsize;
    size_t buf_size;
    size_t scroll_buf;
    size_t buf_start;
    bool end_file;
    //bool is_split;
    QTextCharFormat format;

    bool ProcessIsFinished;

    int cursor_position;

    size_t end_text_cursor_pos_;
    bool text_cursor_isSet_toEnd;
};
#endif // MAINWINDOW_H
