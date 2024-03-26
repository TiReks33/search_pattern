#ifndef MINETEXTEDIT_H
#define MINETEXTEDIT_H

#include <QTextEdit>
#include <QObject>
#include <QWidget>
#include <QTextBrowser>



class MineTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    MineTextEdit();

    void mousePressEvent(QMouseEvent *e);

    void mouseReleaseEvent(QMouseEvent *e);

signals:

void signal();

//void mouse_press_signal();

};

#endif // MINETEXTEDIT_H


