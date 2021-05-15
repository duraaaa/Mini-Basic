#include <cstdio>
#include <QtCore/qmath.h>
#include "exp.h"

Expression::Expression(){}
Expression::~Expression(){}

ConstantExp::ConstantExp(int val){
    value.type = 0;
    value.v = val;
}

ConstantExp::ConstantExp(QString val){
    value.type = 1;
    value.str = val;
}

v ConstantExp::getConstantValue(){
    return value;
}

ExpressionType ConstantExp::type(){
    return CONSTANT;
}

QString ConstantExp::toString(){
    if(value.type==1)
        return QString::number(value.v);
    else return value.str;
}

v ConstantExp::eval(EvalState &context){
    return value;
}

IdentifierExp::IdentifierExp(QString name){
    this->name = name;
}

QString IdentifierExp::getIdentifierName(){
    return name;
}

ExpressionType IdentifierExp::type(){
    return IDENTIFIER;
}

QString IdentifierExp::toString(){
    return name;
}

v IdentifierExp::eval(EvalState &context){
    if(!context.isEvalExist(name)) perror((name+"is undefined").toLatin1().data());
    return context.getEval(name);
}

CompoundExp::CompoundExp(QString op, Expression *lhs, Expression *rhs){
    this->op = op;
    this->lhs = lhs;
    this->rhs = rhs;
}

QString CompoundExp::toString(){
    return op;
}

ExpressionType CompoundExp::type(){
    return COMPOUND;
}

QString CompoundExp::getOperator(){
    return op;
}

Expression* CompoundExp::getLHS(){
    return lhs;
}

Expression* CompoundExp::getRHS(){
    return rhs;
}

void CompoundExp::load_LHS(Expression *lhs){
    this->lhs = lhs;
}

void CompoundExp::load_RHS(Expression *rhs){
    this->rhs = rhs;
}

v CompoundExp::eval(EvalState &context){
    if(rhs==nullptr){
        v p;
        p.type = -1;
        return p;
    }

    v right = rhs->eval(context);
    if (right.type == -1){
        v p;
        p.type = -1;
        return p;
    }
    if (lhs==nullptr){
        if (op == "-"){
            if(right.type==1){
                v p;
                p.type = -1;
                return p;
            }
            v p;
            p.type = 0;
            p.v = -right.v;
            return p;
        }
        else{
            v p;
            p.type = -1;
            return p;
        }
    }

    if (op == "=") {
       if(right.type==1)
            context.insertEval(lhs->getIdentifierName(),right.str);
       if(right.type==0)
            context.insertEval(lhs->getIdentifierName(),right.v);
       return right;
    }
    v left = lhs->eval(context);
    if (left.type == -1){
        v p;
        p.type = -1;
        return p;
    }
    if(left.type!=right.type){
        v p;
        p.type = -1;
        return p;
    }
    if(left.type==1){
        if (op == "+"){
            v p;
            p.type = 1;
            p.str = left.str + right.str;
            return p;
        }
        else {
            v p;
            p.type = -1;
            return p;
        }
    }
    if (op == "+"){
        v p;
        p.type = 0;
        p.v = left.v + right.v;
        return p;
    }
    if (op == "-"){
        v p;
        p.type = 0;
        p.v = left.v - right.v;
        return p;
    }
    if (op == "*"){
        v p;
        p.type = 0;
        p.v = left.v * right.v;
        return p;
    }
    if (op == "/"){
        if (right.v == 0) {
            perror("Division by 0");
            v p;
            p.type = -1;
            return p;
        }
        v p;
        p.type = 0;
        p.v = left.v / right.v;
        return p;
    }
    if (op == "**"){
        v p;
        p.type = 0;
        p.v = pow(left.v, right.v);
        return p;
    }
    perror("Illegal operator in expression");
    v p;
    p.type = -1;
    return p;
}
