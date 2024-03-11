#ifndef MINETEXTEDIT_H
#define MINETEXTEDIT_H

#include <QTextEdit>
#include <QObject>
#include <QWidget>
#include <QTextBrowser>

//namespace Ui {
//class MineTextEdit;
//}

class MineTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    MineTextEdit();

    //void mousePressEvent(QMouseEvent *e);

    void mouseReleaseEvent(QMouseEvent *e);

signals:

void signal();

//private:
//    Ui::MineTextEdit *ui;
};

#endif // MINETEXTEDIT_H

//MainWindow::MainWindow(QWidget *parent)
//    : QMainWindow(parent)

//    friend class QTextEdit;
//    void QTextEdit::mouseReleaseEvent(QMouseEvent *e);
