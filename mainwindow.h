#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dialog.h"
#include "qt_search_class.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    friend class Search;

private slots:
    void on_pushButton_clicked();

    void on_actionOpen_triggered();

    void on_tempButton_clicked();

    void on_tiny_clicked();

public slots:

    void search_slot(QString str="");

    void search_slot2(QString str="",bool highlight=false);

    void search_slot3(QString str="");

    void highlight_slot();

private:
    Ui::MainWindow *ui;
    Dialog * d_;
    //Search * s;
};
#endif // MAINWINDOW_H
