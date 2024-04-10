#include "minetextedit.h"
#include <QMouseEvent>
#include <QTextBrowser>

MineTextEdit::MineTextEdit()
{}



void MineTextEdit::mousePressEvent(QMouseEvent *e)
{
    QTextEdit::mousePressEvent(e);
    if(e->button()==Qt::LeftButton)
    {
    emit mouse_press_signal();
    }
}



void MineTextEdit::mouseReleaseEvent(QMouseEvent *e)
{
    QTextEdit::mouseReleaseEvent(e);
    if(e->button()==Qt::LeftButton)
    {
        emit signal();
    }
    //QTextEdit::mouseReleaseEvent(e);
}


