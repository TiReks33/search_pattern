#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "result.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

private slots:
    void on_buttonBox_accepted();

signals:
    void search_signal(QString str="");

    void search_signal2(QString pattern="",bool highlight=false,int color=0);

    void highlight_signal();

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
