#include "tokenizer.h"
#include "QDebug"

tokenizer::tokenizer(){}
tokenizer::tokenizer(const QString &CodeLine){Load_Code_Line(CodeLine);}
tokenizer::~tokenizer(){}

void tokenizer::Load_Code_Line(const QString &CodeLine){
    this->CodeLine = CodeLine;
}

void tokenizer::ignoreSpace(){
    int i=0;
    for(i=0; i<CodeLine.length(); ++i){
        if(CodeLine[i]!=' ') break;
    }
    CodeLine = CodeLine.mid(i);
}

int tokenizer::token_num(){
    ignoreSpace();
    int i=0, num;
    for(i=0; i<CodeLine.length(); ++i){
        if(CodeLine[i]==' ') break;
    }
    num = CodeLine.left(i).toInt();
    if(num==0) {perror("invalid line number"); return -1;}
    CodeLine = CodeLine.right(CodeLine.length()-i);
    return num;
}

QString tokenizer::token_statement(){
    ignoreSpace();
    QString statement;
    int i=0;
    for(i=0; i<CodeLine.length(); ++i){
        if(CodeLine[i]==' ') break;
    }
    statement = CodeLine.left(i);
    CodeLine = CodeLine.mid(i+1);
    return statement;
}

ExpressionType tokenizer::token_type(){
    ignoreSpace();
    if(CodeLine[0]>='0' && CodeLine[0]<='9') return CONSTANT;
    if(CodeLine[0]=='+' || CodeLine[0]=='-' || CodeLine[0]=='*' || CodeLine[0]=='/' || CodeLine[0]=='=' || CodeLine[0]=='(' || CodeLine[0]==')') return COMPOUND;
    if(CodeLine[0]=="\'") return SQUATO;
    if(CodeLine[0]=="\"") return DQUATO;
    return IDENTIFIER;
}

QString tokenizer::token_string(){
    ignoreSpace();
    if(CodeLine[0]=="\'"){
        for(int i=1; i<CodeLine.length(); ++i){
            if(CodeLine[i]=="\"") return nullptr;
            if(CodeLine[i]=="\'") {
                QString next = CodeLine.mid(1,i-1);
                CodeLine = CodeLine.mid(i+1);
                return next;
            }
        }
    }
    if(CodeLine[0]=="\""){
        for(int i=1; i<CodeLine.length(); ++i){
            if(CodeLine[i]=="\'") return nullptr;
            if(CodeLine[i]=="\"") {
                QString next = CodeLine.mid(1,i-1);
                CodeLine = CodeLine.mid(i+1);
                return next;
            }
        }
    }
    return nullptr;
}

QString tokenizer::token_next(){
    ignoreSpace();
    int i=0;
    QString next;
    if(CodeLine.isEmpty()) return CodeLine;
    ExpressionType type = token_type();
    if(type==SQUATO || type==DQUATO){
        next = CodeLine.left(1);
        CodeLine = CodeLine.mid(1);
        return next;
    }
    if(type==CONSTANT){
        for(i=0; i<CodeLine.length(); ++i){
            if(CodeLine[i]>'9' || CodeLine[i]<'0') break;
        }
        next = CodeLine.left(i);
        CodeLine = CodeLine.mid(i);
        return next;
    }
    if(type==IDENTIFIER){
        for(i=0; i<CodeLine.length(); ++i){
            if(CodeLine[i]==' ' || CodeLine[i]=='+' || CodeLine[i]=='-' || CodeLine[i]=='*' || CodeLine[i]=='/' || CodeLine[i]=='=' || CodeLine[i]=='(' || CodeLine[i]==')') break;
        }
        next = CodeLine.left(i);
        CodeLine = CodeLine.mid(i);
        return next;
    }
    if(type==COMPOUND){
        next = CodeLine[0];
        if(next=='*' && CodeLine[1] == '*'){
            next = "**";
        }
        CodeLine = CodeLine.mid(next.length());
        return next;
    }
    return next;
}

bool tokenizer::isEmpty(){
    ignoreSpace();
    return CodeLine.isEmpty();
}
