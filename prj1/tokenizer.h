#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <QMainWindow>
#include "exp.h"

class tokenizer{
public:
    tokenizer();
    tokenizer(const QString &CodeLine);
    ~tokenizer();
    void Load_Code_Line(const QString &CodeLine);
    QString Get_Code_Line(){return CodeLine;}
    void ignoreSpace();
    ExpressionType token_type();
    int token_num();
    QString token_statement();
    QString token_string();
    QString token_next();
    bool isEmpty();
private:
    QString CodeLine;
};

#endif // TOKENIZER_H
