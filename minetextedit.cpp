#include "minetextedit.h"
#include <QMouseEvent>
#include <QTextBrowser>

MineTextEdit::MineTextEdit()
    //:QTextEdit(parent)
    //,ui(new Ui::MineTextEdit)
{
    //ui->setupUi(this);
}

void MineTextEdit::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button()==Qt::LeftButton)
    {
        emit signal();
    }
}


//Dialog::Dialog(QWidget *parent) :
//    QDialog(parent),
//    ui(new Ui::Dialog)
//{
//    ui->setupUi(this);
//}
