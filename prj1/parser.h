#ifndef PARSER_H
#define PARSER_H

#include "evalstate.h"
#include "exp.h"
#include "tokenizer.h"
#include <qstack.h>

struct node{
    QString name;
    node *left, *right;
    int layer;
    node(QString _name="", node *_left=NULL, node *_right=NULL, int _layer=-1):name(_name),left(_left),right(_right),layer(_layer){}
};

v parse(tokenizer &token_flow, EvalState &eval);
node* syntax_tree(tokenizer &token_flow);

#endif // PARSER_H
