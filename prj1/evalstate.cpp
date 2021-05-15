#include "evalstate.h"

EvalState::EvalState(){}
EvalState::~EvalState(){}

void EvalState::insertEval(const QString &var, int value){
    v tmp;
    tmp.type = 0;
    tmp.v = value;
    EvalMap.insert(var, tmp);
}

void EvalState::insertEval(const QString &var, QString value){
    v tmp;
    tmp.type = 1;
    tmp.str = value;
    EvalMap.insert(var, tmp);
}

v EvalState::getEval(const QString &var){
    if(isEvalExist(var)) return (EvalMap.find(var)).value();
    else {
        v p;
        p.type = -1;
        return p;
    }
}

bool EvalState::isEvalExist(const QString &var){
    if(EvalMap.isEmpty()) return false;
    if(EvalMap.contains(var)) return true;
    else return false;
}
