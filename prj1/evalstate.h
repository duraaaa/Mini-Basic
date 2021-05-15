#ifndef EVALSTATE_H
#define EVALSTATE_H
#include <qmap.h>

struct v{
    int type;
    int v;
    QString str;
};

class EvalState{
private:
    QMap<QString, v> EvalMap;
public:
    EvalState();
    ~EvalState();
    void insertEval(const QString &var, int value);
    void insertEval(const QString &var, QString value);
    v getEval(const QString &var);
    bool isEvalExist(const QString &var);
    void clearEval(){
        EvalMap.clear();
    }

    class iterator{
    private:
        QMap<QString, v>::iterator itr;
    public:
        iterator(){}
        iterator(QMap<QString,v>::iterator _itr){itr = _itr;}
        iterator(const iterator &_itr){itr = _itr.itr;}
        iterator& operator++(){
            itr++;
            return *this;
        }
        iterator& operator--(){
            itr--;
            return *this;
        }
        iterator& operator=(const iterator &_itr){
            itr = _itr.itr;
            return *this;
        }
        v operator*(){
            return *itr;
        }
        bool operator!=(const iterator &_itr){
            return itr != _itr.itr;
        }
        QString key(){
            return itr.key();
        }
        v value(){
            return itr.value();
        }
        ~iterator(){}
    };
    iterator begin(){
        return iterator(EvalMap.begin());
    }
    iterator end(){
        return iterator(EvalMap.end());
    }
};

#endif // EVALSTATE_H
