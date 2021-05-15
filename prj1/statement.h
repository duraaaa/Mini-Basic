#ifndef STATEMENT_H
#define STATEMENT_H

#include "evalstate.h"
#include "tokenizer.h"
#include "mainwindow.h"

class statement{
public:
    statement();
    virtual ~statement();
    virtual void exe(EvalState &eval)=0;
    tokenizer token_flow;
    int next_line;
    QString state;
};

statement *interpreter(QString &line);

struct CodeLine{
    int line_num;
    QString statement;
    CodeLine(int num, QString state):line_num(num),statement(state){}
};

class ERROR : public statement{
public:
    ERROR(tokenizer &token_flow);
    ~ERROR();
    void exe(EvalState &eval);
};

class REM : public statement{
public:
    REM(tokenizer &token_flow){describe = token_flow.Get_Code_Line();state = "REM"; next_line = -1;}
    ~REM(){}
    QString getREM(){return describe;}
    void exe(EvalState &eval){}
private:
    QString describe;
};

class LET : public statement{
public:
    LET(tokenizer &token_flow);
    ~LET();
    void exe(EvalState &eval);
    QString GetIdName(){return id_name;}
private:
    QString id_name;
};

class PRINT : public statement{
public:
    PRINT(tokenizer &token_flow);
    ~PRINT();
    void exe(EvalState &eval);
    int getNum();
    QString getStr();
    int getType();
private:
    v num;
};

class PRINTF : public statement{
public:
    PRINTF(tokenizer &token_flow);
    ~PRINTF();
    void exe(EvalState &eval);
    int getNum();
    QString getStr();
    int getType();
    QString getS();
private:
    v num;
};

class INPUT : public statement{
public:
    INPUT(tokenizer &token_flow);
    ~INPUT();
    void exe(EvalState &eval);
    QString getName();
private:
    QString name;
};

class INPUTS : public statement{
public:
    INPUTS(tokenizer &token_flow);
    ~INPUTS();
    void exe(EvalState &eval);
    QString getName();
private:
    QString name;
};

class GOTO : public statement{
public:
    GOTO(tokenizer &token_flow);
    ~GOTO();
    void exe(EvalState &eval);
};

class IF : public statement{
public:
    IF(tokenizer &token_flow);
    ~IF();
    void exe(EvalState &eval);
    QString first(){return exp1;}
    QString second(){return exp2;}
    QString exp1,exp2,op;
private:
    int if_next_line;
};

class END : public statement{
public:
    END(tokenizer &token_flow);
    ~END();
    void exe(EvalState &eval);
};

#endif // STATEMENT_H
