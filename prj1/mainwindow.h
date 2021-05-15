#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "program.h"
#include "evalstate.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:

    void on_CommandInputBox_textChanged();

    void on_ClearButton_clicked();

    void on_RunButton_clicked();

    void on_LoadButton_clicked();

    void append_result_box(QString *mes);

    void on_DebugButton_clicked();

private:
    Program *program;
    EvalState *evalstate;
    Ui::MainWindow *ui;
    void Load_Code();
};
#endif // MAINWINDOW_H
