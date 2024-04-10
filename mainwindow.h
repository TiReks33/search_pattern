#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dialog.h"
#include <QTextEdit>
#include "minetextedit.h"
#include <QTextStream>
#include <savedialog.h>
#include "dockwidget.h"
#include "hints.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // close-event override for save purpose after closing
    void closeEvent(QCloseEvent *event);

    // save/save-as enable after some actions
    void buttons_enabled(bool on_off=true);

    // search function for 1-byte encoding
    void search_highlight(const std::string& text,const std::string& pattern,int color);

    // search function for 1+2-more byte encoding (w_char)
    void search_highlight_w(const std::wstring& text,const std::wstring& pattern,int color);

    // search function for 1-byte encoding (with result window)
    void search_highlight_occurrences(const std::string& text,const std::string& pattern,int color);

    // search function for 1+2-more byte encoding (with result window) (w_char)
    void search_highlight_occurrences_w(const std::wstring& text,const std::wstring& pattern,int color);

    //bool & clear_check(){static bool clear = false;return clear;}

    //bool & select_check(){static bool select = false;return select;}


private slots:

    // function saves current file changes to the
    // temporary file before search
    void on_search_button_clicked();

    // opening new file and all the addition stuffs
    // (flags etc.)
    void on_actionOpen_triggered();

    void on_actionExit_triggered();

    void on_actionStart_search_triggered();

    void on_actionStart_search_2_triggered();

    // create new file
    void on_actionNew_triggered();

    void on_actionCut_triggered();

    void on_actionUndo_triggered();

    void on_actionRedo_triggered();

    void on_actionCopy_triggered();

    void on_actionPaste_triggered();

    void on_actionAbout_Qt_triggered();

    // info window
    void on_actionAbout_Search_pattern_triggered();

    // save file
    void on_actionSave_triggered();

    // save as
    void on_actionSave_as_triggered();

    void on_actionOptions_triggered();

    void on_actionOptions_2_triggered();

    void on_actionHints_tips_triggered();

public slots:

    // handle signal from the 'dialog' window
    void search_slot(QString str="",bool highlight=false,int color=0);

    // this slot handle mouse clicking on the screen after search highlighting
    // (revert plain text after setting hmtl color tags with entries of pattern)
    void mte_slot();

    // mechanism that loads html-tagged text with search results part-by-part
    // (for memory optimization of 'Rich' text in qTextEdit)
    void slider_slot(int);

    //void format_slot(const QTextCharFormat &f);

    // overriding slot for handling left mouse button-pressed scenario
    void mouse_press_slot();

    // function scrolls html-tagged text to the current QTextEdit->Slider position after search complete
    void add_text_slot(size_t,size_t);

    // function may scroll html-tagged text to the current QTextEdit->textSlider position after search
    void add_text_slot_2limit(size_t);

    // func sets scrollbar to the needed position
    void set_position_slot(size_t);

    // func sets cursor to the needed position
    void set_cursor_slot(int);

    // function for open file (fast insert for large files without lagging; uses fast 'subbuf' method)
    void long_text_add_slot();

    // cursor icon change
    void cursor_shape_slot(int);

    // after writing some text in window, this slot was handled by 'textEdit->textChanged() SIGNAL
    void need_save();

signals:

    void add_text_signal(size_t,size_t);

    void add_text_signal_2limit(size_t);

    void set_position_signal(size_t);

    void set_cursor_signal(int);

    void set_cursor_signal(size_t);


    void long_text_add_signal();

    void cursor_shape_signal(int);


    void total_occur(size_t);

private:
    Ui::MainWindow *ui;
    Dialog * d_;
    Result * r_;
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


    // text file partial read logic
    // (by slider (vertical scroll-bar)
    size_t plain_fsize_;
    size_t fsize_;
    size_t maxfullsize;
    size_t buf_size;
    size_t scroll_buf;
    size_t buf_start;
    bool end_file;

    QTextCharFormat format;

    bool ProcessIsFinished;

    int cursor_position;

    size_t end_text_cursor_pos_;
    bool text_cursor_isSet_toEnd;

    int file_size_limit_;

    bool need_save_;
    bool need_save_as_;

    saveDialog* save_;
    bool buttons_isEnabled_;
    bool play_it_safe_close_;

    DockWidget* options;
    Hints* hints;
};
#endif // MAINWINDOW_H
