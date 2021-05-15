#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "program.h"
#include "tokenizer.h"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    program = new Program(ui);
    evalstate = new EvalState;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_CommandInputBox_textChanged()
{
    QString InputCommand = ui->CommandInputBox->toPlainText();

    if (InputCommand.right(1)=='\n'){
        ui->ErrorReminder->clear();
        if(program->is_i==false){
            if(InputCommand.isEmpty()) return;
            if(InputCommand.right(1)=='\n') InputCommand.chop(1);
            if(program->type==0){
                if(InputCommand[0]=="+" || InputCommand[0]=="-" || (InputCommand[0]>='0' && InputCommand[0]<='9'))
                    for(QString::iterator itr=InputCommand.begin()+1; itr!=InputCommand.end(); ++itr){
                        if(*itr<'0' || *itr>'9') {
                            ui->ErrorReminder->setText("Invalid input: Command Input need a number");
                            ui->CommandInputBox->clear();
                            return ;
                        }
                    }
                else{
                    ui->ErrorReminder->setText("Invalid input: Command Input need a number");
                    ui->CommandInputBox->clear();
                    return;
                }
                program->int_in = InputCommand.toInt();
                program->is_i = true;
                ui->CommandInputBox->clear();
                return;
            }
            if(program->type==1){
                int len = InputCommand.length();
                if(InputCommand[0]=="\'"){
                    for(int i=1; i<len; ++i){
                        if(InputCommand[i]=="\""){
                            ui->ErrorReminder->setText("Invalid input: invalid character in a string");
                            ui->CommandInputBox->clear();
                            return ;
                        }
                        if(InputCommand[i]=="\'" && i!=len-1){
                            ui->ErrorReminder->setText("Invalid input: not a string(should begin and end up with quota)");
                            ui->CommandInputBox->clear();
                            return ;
                        }
                        if(i==len-1&&InputCommand[i]!="\'"){
                            ui->ErrorReminder->setText("Invalid input: not a string(should begin and end up with quota)");
                            ui->CommandInputBox->clear();
                            return ;
                        }
                    }
                }
                else if(InputCommand[0]=="\""){
                    for(int i=1; i<len; ++i){
                        if(InputCommand[i]=="\'"){
                            ui->ErrorReminder->setText("Invalid input: invalid character in a string");
                            ui->CommandInputBox->clear();
                            return ;
                        }
                        if(InputCommand[i]=="\"" && i!=len-1){
                            ui->ErrorReminder->setText("Invalid input: not a string(should begin and end up with quota)");
                            ui->CommandInputBox->clear();
                            return ;
                        }
                        if(i==len-1&&InputCommand[i]!="\""){
                            ui->ErrorReminder->setText("Invalid input: not a string(should begin and end up with quota)");
                            ui->CommandInputBox->clear();
                            return ;
                        }
                    }
                }
                else {
                    ui->ErrorReminder->setText("Invalid input: not a string(should begin and end up with quota)");
                    ui->CommandInputBox->clear();
                    return ;
                }
                InputCommand = InputCommand.mid(1,InputCommand.length()-2);
                program->str_in = InputCommand;
                program->is_i = true;
                ui->CommandInputBox->clear();
                return;
            }
        }
        if(InputCommand[0]<='9' && InputCommand[0]>='0'){
            if(InputCommand.toInt() != 0) program->Delete_Code(InputCommand.toInt());
            program->Load_Code(InputCommand);
            program->Show_Code();
            ui->CommandInputBox->clear();
            return;
        }
        if(QString::compare(InputCommand,"QUIT")==1)
        {
            exit(0);
        }
        if(QString::compare(InputCommand,"CLEAR")==1)
        {
            ui->CodeBox->clear();
            ui->ResultBox->clear();
            ui->SyntaxTree->clear();
            program->Clear_Code();
            evalstate->clearEval();
            ui->CommandInputBox->clear();
            return;
        }
        if(QString::compare(InputCommand,"HELP")==1)
        {
            QMessageBox help;
            help.setText("hello world!");
            help.show();
            ui->CommandInputBox->clear();
            return;
        }
        if(QString::compare(InputCommand, "LOAD")==1){
            Load_Code();
            ui->CommandInputBox->clear();
            return;
        }
        if(QString::compare(InputCommand, "RUN")==1){
            program->Run_Program(*evalstate);
            ui->CommandInputBox->clear();
            return;
        }
        InputCommand.chop(1);
        ui->CommandInputBox->clear();
        program->Exe_statement(InputCommand, *evalstate);
        /*
        if(InsertCode(InputCommand)==-1)
            ui->ErrorReminder->setText("Invalid command");
        else{
            ui->CodeBox->clear();
            for(int i=0;i<CodeVector.length();++i){
                ui->CodeBox->append(QString::number(LineNumVector[i]).append(CodeVector[i].left(CodeVector[i].length()-1)));
            }
            ui->CodeBox->show();
        }*/
    }
}

void MainWindow::Load_Code(){
    QString file_name = QFileDialog::getOpenFileName(this, "open file", "", "", 0), temp;
    if(!file_name.isNull()){
        QFile file(file_name);
        if(!file.open(QIODevice::ReadOnly|QFile::Text)){
            static QMessageBox err_mes;
            err_mes.setText("invalid path");
            err_mes.show();
        }
        else{
            ui->CodeBox->clear();
            program->Clear_Code();
            while(!file.atEnd()){
                temp = file.readLine();
                program->Load_Code(temp);
            }
            program->Show_Code();
        }
    }
}

void MainWindow::on_ClearButton_clicked()
{
    ui->CodeBox->clear();
    ui->ResultBox->clear();
    ui->SyntaxTree->clear();
    program->Clear_Code();
    evalstate->clearEval();
}

void MainWindow::on_RunButton_clicked()
{
    if(program->state == -1){
        ui->LoadButton->setEnabled(false);
        ui->RunButton->setEnabled(false);
        ui->ClearButton->setEnabled(false);
        ui->DebugButton->setEnabled(false);

        ui->ResultBox->clear();
        ui->SyntaxTree->clear();
        ui->VariableMap->clear();

        program->state = 0;
        program->Run_Program(*evalstate);
        program->state = -1;
        evalstate->clearEval();
        program->extras.clear();

        ui->LoadButton->setEnabled(true);
        ui->RunButton->setEnabled(true);
        ui->ClearButton->setEnabled(true);
        ui->DebugButton->setEnabled(true);
    }
    if(program->state == 1){
        program->state = 0;
        program->step = 1;
    }
}

void MainWindow::append_result_box(QString *mes){
    ui->ResultBox->append(*mes);
    ui->ResultBox->show();
}

void MainWindow::on_LoadButton_clicked()
{
    Load_Code();
}

void MainWindow::on_DebugButton_clicked()
{
    if(program->state==-1){
        int run_state;

        ui->ClearButton->setEnabled(false);
        ui->LoadButton->setEnabled(false);

        ui->ResultBox->clear();
        ui->SyntaxTree->clear();
        ui->VariableMap->clear();

        program->state = 1;
        run_state = program->Run_Program(*evalstate);
        evalstate->clearEval();
        program->state = -1;
        program->extras.clear();

        ui->ClearButton->setEnabled(true);
        ui->LoadButton->setEnabled(true);

        if(run_state==-3){
            static QMessageBox err_mes;
            err_mes.setText("Error has occured when Debugging! Exit Debug mode.");
            err_mes.show();
        }
        if(run_state==0){
            static QMessageBox mes;
            mes.setText("Debugging is complete! Exit Debug mode.");
            mes.show();
        }
    }
    if(program->state==1){
        ui->RunButton->setEnabled(false);
        ui->DebugButton->setEnabled(false);

        program->step = 1;
    }
}
