#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void show_occurrences(std::string const&,char const*);

    QString show_highlighting(const std::string& text,const char*pattern);

private slots:
    void on_pushButton_clicked();

    void on_actionOpen_triggered();

    void on_tempButton_clicked();

    void on_tiny_clicked();

    void on_reset_text_clicked();

public slots:

//    void search_slot(QString str="");

    void search_slot2(QString str="",bool highlight=false);

//    void search_slot3(QString str="");

//    void highlight_slot();

private:
    Ui::MainWindow *ui;
    Dialog * d_;
    size_t occurrences;
    //Search * s;
};
#endif // MAINWINDOW_H
