#ifndef PROGRAM_H
#define PROGRAM_H

#include <QMainWindow>
#include <qmap.h>
#include <QThread>
#include "evalstate.h"
#include "ui_mainwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Program{
private:
    QMap<int, QString> Code_Table;
    QMap<int, int> Pos_Table;
    Ui::MainWindow *ui;
    void draw_tree();
    int draw_sentence(QString &Code_Line);
    int draw_sentence(int line, QString &Code_Line);
    void var_map(EvalState &eval_state);
    int get_line_number(QString &Code_Line);
    void Set_Pos_Table();
    void Set_Highlight(int pos, QColor color);

public:
    bool is_i;
    int int_in;
    QString str_in;
    int type;
    int state;
    int step;
    QList<QTextEdit::ExtraSelection> extras;
    Program(){}
    Program(Ui::MainWindow *_ui);
    ~Program();
    void Load_Code(QString &CodeText);
    void Delete_Code(int no){Code_Table.erase(Code_Table.find(no));}
    void Clear_Code(){Code_Table.clear();}
    void Show_Code();
    int Run_Program(EvalState &eval_state);
    int Exe_statement(QString line, EvalState &eval_state);
private slots:
    void on_CodeBox_textChanged();
};

class IOthread : public QThread{
public:
    IOthread(Program *_pro, EvalState *_evalstate):pro(_pro), evalstate(_evalstate){}
    Program *pro;
    EvalState *evalstate;
    void run(){
        pro->Run_Program(*evalstate);
    }
};

class Exethread : public QThread{
public:
    Exethread(Program *_pro, EvalState *_evalstate, QString _command):pro(_pro), evalstate(_evalstate), command((_command)){}
    Program *pro;
    EvalState *evalstate;
    QString command;
    void run(){
        pro->Exe_statement(command, *evalstate);
    }
};

#endif // PROGRAM_H
