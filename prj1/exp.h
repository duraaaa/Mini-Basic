#ifndef EXP_H
#define EXP_H

#include "qmap.h"
#include "evalstate.h"

enum ExpressionType { CONSTANT, IDENTIFIER, COMPOUND, SQUATO, DQUATO, NONE};

class Expression {
public:

   Expression();
   virtual ~Expression();
   virtual v eval(EvalState & context) = 0;
   virtual QString toString() = 0;
   virtual ExpressionType type() = 0;

/* Getter methods for convenience */

   virtual v getConstantValue(){}
   virtual QString getIdentifierName(){}
   virtual QString getOperator(){}
   virtual Expression *getLHS(){}
   virtual Expression *getRHS(){}

};

class ConstantExp: public Expression {

public:

   ConstantExp(int val);
   ConstantExp(QString val);
   ~ConstantExp(){}

   virtual v eval(EvalState & context);
   virtual QString toString();
   virtual ExpressionType type();

   virtual v getConstantValue();

private:

   v value;
};

class IdentifierExp: public Expression {

public:

   IdentifierExp(QString name);
   ~IdentifierExp(){}

   virtual v eval(EvalState & context);
   virtual QString toString();
   virtual ExpressionType type();

   virtual QString getIdentifierName();

private:

   QString name;

};

class CompoundExp: public Expression {

public:

   CompoundExp(QString op, Expression *lhs, Expression *rhs);
   ~CompoundExp(){}

   virtual v eval(EvalState & context);
   virtual QString toString();
   virtual ExpressionType type();

   virtual void load_LHS(Expression *lhs);
   virtual void load_RHS(Expression *rhs);
   virtual QString getOperator();
   virtual Expression *getLHS();
   virtual Expression *getRHS();

private:

   QString op;
   Expression *lhs, *rhs;

};


#endif // EXP_H
