#include "statement.h"
#include "tokenizer.h"
#include "parser.h"
#include <QDebug>

statement::statement(){}
statement::~statement(){}

statement *interpreter(QString &line){
    tokenizer token_flow(line);
    QString state;
    state = token_flow.token_statement();
    if(state == "LET") return new LET(token_flow);
    if(state == "PRINT") return new PRINT(token_flow);
    if(state == "PRINTF") return new PRINTF(token_flow);
    if(state == "INPUT") return new INPUT(token_flow);
    if(state == "INPUTS") return new INPUTS(token_flow);
    if(state == "GOTO") return new GOTO(token_flow);
    if(state == "IF") return new IF(token_flow);
    if(state == "END") return new END(token_flow);
    if(state == "REM") return new REM(token_flow);
    if(state.isEmpty()) return nullptr;
    qDebug() << line << endl;
    perror("invalid statement");
    return new ERROR(token_flow);
}

LET::LET(tokenizer &token_flow){
    id_name = token_flow.token_next();
    if(token_flow.token_next() != "=") {
        perror("lack of equal sign in LET statement");
        next_line = -3;
        state = "ERROR";
        return;
    }
    this->token_flow = token_flow;
    next_line = -1;
    state = "LET";
}
LET::~LET(){}
void LET::exe(EvalState &eval){
    if(next_line==-3) return;
    v i = parse(token_flow, eval);
    if(i.type==-1) {
        next_line=-3;
        state = "ERROR";
        return;
    }
    if(i.type==0)
        eval.insertEval(id_name, i.v);
    if(i.type==1)
        eval.insertEval(id_name, i.str);
}

ERROR::ERROR(tokenizer &token_flow){
    state = "ERROR";
}
ERROR::~ERROR(){}
void ERROR::exe(EvalState &eval){
}

PRINT::PRINT(tokenizer &token_flow){
    this->token_flow = token_flow;
    next_line = -1;
    state = "PRINT";
}
PRINT::~PRINT(){}
void PRINT::exe(EvalState &eval){
    num = parse(token_flow, eval);
    if(num.type==-1) next_line=-3;
}
int PRINT::getNum(){return num.v;}
QString PRINT::getStr(){return num.str;}
int PRINT::getType(){return num.type;}

PRINTF::PRINTF(tokenizer &token_flow){
    if(token_flow.token_type()!=SQUATO && token_flow.token_type()!=DQUATO) {
        next_line = -3; state = "ERROR";}
    else{
        this->token_flow = token_flow;
        next_line = -1;
        state = "PRINTF";
    }
}
PRINTF::~PRINTF(){}
void PRINTF::exe(EvalState &eval){
    if(state=="ERROR") return;
    QString str, tmp;
    QStringList lst;
    tokenizer tmp_token;
    v tmp_v;
    bool lr=false, is_f=false;
    tmp = token_flow.token_string();
    lst = token_flow.Get_Code_Line().split(',');
    for(int i=1; i<lst.length(); ++i){
        is_f = false;
        for(int j=0; j<tmp.length(); ++j){
            if(tmp[j]=="{") {
                if(lr==true){num.type = -1; return;}
                tmp_token.Load_Code_Line(lst[i]);
                tmp_v = parse(tmp_token, eval);
                if(tmp_v.type==-1){num.type = -1; return;}
                if(tmp_v.type==0){str+=QString::number(tmp_v.v);}
                if(tmp_v.type==1){str+=tmp_v.str;}
                lr = true;
            }
            else if(tmp[j]=="}"){
                if(lr==false){num.type = -1; return;}
                tmp = tmp.mid(j+1);
                lr = false;
                is_f = true;
                break;
            }
            else if(lr==false) str += tmp[j];
        }
        if(is_f==false){num.type = -1; return;}
    }
    str += tmp;
    for(int i=0; i<str.length(); ++i){
        if (str[i]=="{" || str[i]=="}") {next_line = -3; return;}
    }
    tmp_token.Load_Code_Line("\""+str+"\"");
    qDebug()<<str<<endl;
    num = parse(tmp_token, eval);
    if(num.type==-1) next_line=-3;
}
int PRINTF::getNum(){return num.v;}
QString PRINTF::getStr(){return num.str;}
int PRINTF::getType(){return num.type;}
QString PRINTF::getS(){return token_flow.Get_Code_Line();}

INPUT::INPUT(tokenizer &token_flow){
    this->token_flow = token_flow;
    next_line = -1;
    state = "INPUT";
    name = token_flow.token_next();
}
INPUT::~INPUT(){}
void INPUT::exe(EvalState &eval){

}
QString INPUT::getName(){
    return name;
}

INPUTS::INPUTS(tokenizer &token_flow){
    this->token_flow = token_flow;
    next_line = -1;
    state = "INPUTS";
    name = token_flow.token_next();
}
INPUTS::~INPUTS(){}
void INPUTS::exe(EvalState &eval){

}
QString INPUTS::getName(){
    return name;
}

GOTO::GOTO(tokenizer &token_flow){
    next_line = token_flow.token_next().toInt();
    if(!token_flow.isEmpty()) {
        perror("invalid syntax for \'GOTO\'");
        state = "ERROR";
        next_line = -3;
    }
    if(next_line<=0) {
        perror("invalid syntax for \'GOTO\'");
        state = "ERROR";
        next_line = -3;
    }
    state = "GOTO";
}
GOTO::~GOTO(){}
void GOTO::exe(EvalState &eval){
}

IF::IF(tokenizer &token_flow){
    this->token_flow = token_flow;
    next_line = -1;
    state = "IF";
}
IF::~IF(){}
void IF::exe(EvalState &eval){
    QString temp;
    v left, right;
    tokenizer *left_token, *right_token;
    temp = token_flow.token_next();
    while(temp!=">" && temp!="<" && temp!="=") {
        exp1.append(temp);
        temp = token_flow.token_next();
    }
    op = temp;
    temp = token_flow.token_next();
    while(temp!="THEN"){
        exp2.append(temp);
        temp = token_flow.token_next();
    }

    left_token = new tokenizer(exp1);
    right_token = new tokenizer(exp2);
    if_next_line = token_flow.token_next().toInt();
    left = parse(*left_token, eval);
    right = parse(*right_token, eval);
    if(left.type!=0 || right.type!=0) {
        next_line=-3;
        return;
    }
    if(op=='>' && left.v>right.v) next_line = if_next_line;
    if(op=='<' && left.v<right.v) next_line = if_next_line;
    if(op=='=' && left.v==right.v) next_line = if_next_line;
}

END::END(tokenizer &token_flow){
    state = "END";
    next_line = -2;
}
END::~END(){}
void END::exe(EvalState &eval){
}
