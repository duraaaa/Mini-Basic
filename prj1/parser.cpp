#include "parser.h"
#include <QDebug>

v parse(tokenizer &token_flow, EvalState &eval){
    QStack<Expression*> Operand;
    QStack<CompoundExp*> Operator;
    ExpressionType type, last_type;
    ConstantExp *constant;
    IdentifierExp *identifier;
    CompoundExp *compound, *compound_temp;
    int cons;
    QString name, comp, str;
    last_type = type = NONE;
    token_flow.ignoreSpace();
    while(!token_flow.isEmpty()){
        qDebug()<< token_flow.Get_Code_Line() << endl;
        type = token_flow.token_type();
        if(type==SQUATO || type==DQUATO){
            if(last_type==CONSTANT || last_type==IDENTIFIER){
                v p;
                p.type = -1;
                return p;
            }
            str = token_flow.token_string();
            if(str==nullptr){
                v p;
                p.type = -1;
                return p;
            }
            constant = new ConstantExp(str);
            Operand.push(constant);
            last_type = CONSTANT;
            qDebug()<< (cons) << endl;
        }
        if(type==CONSTANT){
            if(last_type==CONSTANT || last_type==IDENTIFIER){
                v p;
                p.type = -1;
                return p;
            }
            cons = token_flow.token_next().toInt();
            constant = new ConstantExp(cons);
            Operand.push(constant);
            last_type = CONSTANT;
            qDebug()<< (cons) << endl;
        }
        if(type==IDENTIFIER){
            if(last_type==CONSTANT || last_type==IDENTIFIER){
                v p;
                p.type = -1;
                return p;
            }
            name = token_flow.token_next();
            identifier = new IdentifierExp(name);
            Operand.push(identifier);
            last_type = IDENTIFIER;
            qDebug()<< (name) << endl;
        }
        if(type==COMPOUND){
            comp = token_flow.token_next();
            qDebug()<< (comp) << endl;
            if(comp=="("){
                if(last_type==CONSTANT || last_type==IDENTIFIER){
                    v p;
                    p.type = -1;
                    return p;
                }
                last_type = NONE;
            }
            else if(comp==")"){
                if(last_type==COMPOUND){
                    v p;
                    p.type = -1;
                    return p;
                }
                last_type = CONSTANT;
            }
            else{
                if(last_type==COMPOUND){
                    v p;
                    p.type = -1;
                    return p;
                }
                last_type = COMPOUND;
            }
            compound = new CompoundExp(comp,NULL,NULL);
            if(comp=="+" || comp=="-"){
                if(!Operator.isEmpty())
                    while(Operator.top()->toString()!="("){
                        compound_temp = Operator.pop();
                        if(Operand.empty())
                            compound_temp->load_RHS(nullptr);
                        else compound_temp->load_RHS(Operand.pop());
                        if(Operand.empty())
                            compound_temp->load_LHS(nullptr);
                        else compound_temp->load_LHS(Operand.pop());
                        Operand.push(compound_temp);
                        if(Operator.isEmpty()) break;
                    }
                Operator.push(compound);
                continue;
            }
            if(comp=="*" || comp=="/"){
                if(!Operator.isEmpty())
                    while(Operator.top()->toString() =="*"||Operator.top()->toString()=="/"||Operator.top()->toString()=="**"){
                        compound_temp = Operator.pop();
                        if(Operand.empty())
                            compound_temp->load_RHS(nullptr);
                        else compound_temp->load_RHS(Operand.pop());
                        if(Operand.empty())
                            compound_temp->load_LHS(nullptr);
                        else compound_temp->load_LHS(Operand.pop());
                        Operand.push(compound_temp);
                        if(Operator.isEmpty()) break;
                    }
                Operator.push(compound);
                continue;
            }
            if(comp=="**" || comp=="("){
                Operator.push(compound);
                continue;
            }
            if(comp==")"){
                while((compound_temp=Operator.pop())->toString()!="("){
                    //compound_temp = Operator.pop();
                    qDebug()<<compound_temp->toString()<<endl;
                    if(Operand.empty())
                        compound_temp->load_RHS(nullptr);
                    else compound_temp->load_RHS(Operand.pop());
                    if(Operand.empty())
                        compound_temp->load_LHS(nullptr);
                    else compound_temp->load_LHS(Operand.pop());
                    Operand.push(compound_temp);
                }
                continue;
            }
            v p;
            p.type = -1;
            return p;
        }
    }
    while(!Operator.isEmpty()){
        compound_temp = Operator.pop();
        if(!Operand.isEmpty())
            compound_temp->load_RHS(Operand.pop());
        else compound_temp->load_RHS(nullptr);
        if(!Operand.isEmpty())
            compound_temp->load_LHS(Operand.pop());
        else compound_temp->load_LHS(nullptr);

        Operand.push(compound_temp);
    }
    if(Operand.empty()){
        v p;
        p.type = -1;
        return p;
    }
    return Operand.pop()->eval(eval);
}

node* syntax_tree(tokenizer &token_flow){
    node *temp, *compound_temp;
    QStack<node*> Operand;
    QStack<node*> Operator;
    ExpressionType type, last_type;
    type = last_type = NONE;
    int cons;
    QString name, comp, str;
    token_flow.ignoreSpace();
    while(!token_flow.isEmpty()){
        type = token_flow.token_type();
        if(type==SQUATO || type==DQUATO){
            if(last_type==CONSTANT||last_type==IDENTIFIER) return nullptr;
            str = token_flow.token_string();
            if(str==nullptr) return nullptr;
            temp = new node(str);
            Operand.push(temp);
            last_type = CONSTANT;
        }
        if(type==CONSTANT){
            if(last_type==CONSTANT||last_type==IDENTIFIER) return nullptr;
            cons = token_flow.token_next().toInt();
            temp = new node(QString::number(cons));
            Operand.push(temp);
            last_type = CONSTANT;
        }
        if(type==IDENTIFIER){
            if(last_type==CONSTANT||last_type==IDENTIFIER) return nullptr;
            name = token_flow.token_next();
            temp = new node(name);
            Operand.push(temp);
            last_type = IDENTIFIER;
        }
        if(type==COMPOUND){
            comp = token_flow.token_next();
            if(comp=="("){
                if(last_type==CONSTANT||last_type==IDENTIFIER) return nullptr;
                last_type = NONE;
            }
            else if(comp==")"){
                if(last_type==COMPOUND) return nullptr;
                last_type = CONSTANT;
            }
            else {
                if(last_type==COMPOUND) return nullptr;
                last_type = COMPOUND;
            }
            temp = new node(comp);
            if(comp=="+" || comp=="-"){
                if(!Operator.isEmpty())
                    while(Operator.top()->name != "("){
                        compound_temp = Operator.pop();
                        if(Operand.empty())
                            compound_temp->right = nullptr;
                        else compound_temp->right = Operand.pop();
                        if(Operand.empty())
                            compound_temp->left = nullptr;
                        else compound_temp->left =  Operand.pop();

                        Operand.push(compound_temp);
                        if(Operator.isEmpty()) break;
                    }
                Operator.push(temp);
            }
            if(comp=="*" || comp=="/"){
                if(!Operator.isEmpty())
                    while(Operator.top()->name =="*"||Operator.top()->name=="/"||Operator.top()->name=="**"){
                        compound_temp = Operator.pop();
                        if(Operand.empty())
                            compound_temp->right = nullptr;
                        else compound_temp->right = Operand.pop();
                        if(Operand.empty())
                            compound_temp->left = nullptr;
                        else compound_temp->left =  Operand.pop();
                        Operand.push(compound_temp);
                        if(Operator.isEmpty()) break;
                    }
                Operator.push(temp);
            }
            if(comp=="**" || comp=="("){
                Operator.push(temp);
            }
            if(comp==")"){
                while((compound_temp=Operator.pop())->name!="("){
                    qDebug()<<(compound_temp)->name<<endl;
                    //compound_temp = Operator.pop();
                    if(Operand.empty())
                        compound_temp->right = (nullptr);
                    else compound_temp->right = (Operand.pop());
                    if(Operand.empty())
                        compound_temp->left = (nullptr);
                    else compound_temp->left = (Operand.pop());
                    Operand.push(compound_temp);
                    /*
                    compound_temp->left = Operand.pop();
                    compound_temp->right = Operand.pop();
                    Operand.push(compound_temp);*/
                }
            }
        }
    }
    while(!Operator.isEmpty()){
        compound_temp = Operator.pop();
        if(!Operand.isEmpty())
            compound_temp->right = Operand.pop();
        else compound_temp->right = nullptr;
        if(!Operand.isEmpty())
            compound_temp->left = Operand.pop();
        else compound_temp->left = nullptr;
        Operand.push(compound_temp);
    }
    if(Operand.empty()) return nullptr;
    return Operand.top();
}
