#include "program.h"
#include "statement.h"
#include "parser.h"
#include <QDebug>
#include <QQueue>

Program::Program(Ui::MainWindow *_ui){ui = _ui; is_i = true; state = -1; step = 0;}

Program::~Program(){}

int Program::get_line_number(QString &Code_Line){
    int i, num;
    for(i = 0; i<Code_Line.length(); ++i){
        if(Code_Line[i]!=' ') break;
    }
    if(Code_Line[i]>'9' || Code_Line[i]<'0') return -1;
    for(; i<Code_Line.length(); ++i){
        if(Code_Line[i]==' ') break;
    }
    if(i==Code_Line.length()) return -1;
    num = Code_Line.left(i).toInt();
    Code_Line = Code_Line.mid(i+1);
    return num;
}

void Program::Load_Code(QString &CodeText){
    int i=0, num, len=0;
    QStringList Code_List = CodeText.split('\n');
    for(i=0; i<Code_List.length(); ++i){
        len += (Code_List[i]+'\n').length();
        if(Code_List[i].isEmpty()) {len -= 1; continue;}
        num = get_line_number(Code_List[i]);
        qDebug()<<num<<Code_List[i]<<endl;
        if(Code_List[i].isEmpty()) {len -= (QString::number(num)+'\n').length(); continue;}
        if(num!=-1) Code_Table.insert(num, Code_List[i]);
    }
}

void Program::Show_Code(){
    ui->CodeBox->clear();
    for(QMap<int,QString>::iterator itr = Code_Table.begin(); itr!=Code_Table.end(); ++itr){
        if(!itr.value().isEmpty() && itr.value().right(1)=='\n') itr.value().chop(1);
        ui->CodeBox->append(QString::number(itr.key())+" "+itr.value());
    }
    ui->CodeBox->show();
}

int Program::Exe_statement(QString line, EvalState &eval_state){
    statement *st;
    int result;
    st = interpreter(line);
    if(st == nullptr) return -1;
    if(st->state == "ERROR") return -3;
    if(st->state=="END") return -2;
    st->exe(eval_state);
    if(st->state=="PRINT") {
        PRINT *p = (PRINT*)st;
        if(p->getType()==0)
            ui->ResultBox->append(QString::number(p->getNum()));
        if(p->getType()==1)
            ui->ResultBox->append(p->getStr());
        if(p->getType()==-1)
            return -3;
        ui->ResultBox->show();
    }
    if(st->state=="PRINTF") {
        PRINTF *p = (PRINTF*)st;
        if(p->getType()==0)
            ui->ResultBox->append(QString::number(p->getNum()));
        if(p->getType()==1)
            ui->ResultBox->append(p->getStr());
        if(p->getType()==-1)
            return -3;
        ui->ResultBox->show();
    }
    if(st->state=="INPUT") {
        INPUT *p = (INPUT*)st;
        is_i = false;
        type = 0;
        ui->ResultBox->append("?");
        ui->ResultBox->show();
        while(is_i==false){
            QCoreApplication::processEvents();
            QThread::usleep(300);
        }
        //qDebug()<<ui->ResultBox->textCursor().selectedText();
        ui->ResultBox->append(QString::number(int_in));
        ui->ResultBox->show();
        eval_state.insertEval(p->getName(), int_in);
    }
    if(st->state=="INPUTS") {
        INPUTS *p = (INPUTS*)st;
        is_i = false;
        type = 1;
        ui->ResultBox->append("?");
        ui->ResultBox->show();
        qDebug()<<p->getName()<<endl;
        while(is_i==false){
            QCoreApplication::processEvents();
            QThread::usleep(300);
        }
        //qDebug()<<ui->ResultBox->textCursor().selectedText();
        ui->ResultBox->append(str_in);
        ui->ResultBox->show();
        eval_state.insertEval(p->getName(), str_in);
    }
    result = st->next_line;
    delete st;
    var_map(eval_state);

    return result;
}

void Program::var_map(EvalState &eval_state){
    ui->VariableMap->clear();
    for(EvalState::iterator itr = eval_state.begin(); itr!=eval_state.end(); ++itr){
        if(itr.value().type==0)
            ui->VariableMap->append(itr.key()+":"+QString::number(itr.value().v));
        if(itr.value().type==1)
            ui->VariableMap->append(itr.key()+":"+itr.value().str);
    }
    ui->VariableMap->show();
}

void Program::Set_Pos_Table(){
    Pos_Table.clear();
    int len = 0;
    QStringList Code_Lst = ui->CodeBox->toPlainText().split("\n");
    QString tmp;
    for(int i=0; i<Code_Lst.length(); ++i){
        tmp = Code_Lst[i];
        len += (tmp+"\n").length();
        Pos_Table[get_line_number(tmp)] = len-1;
    }
}

void Program::Set_Highlight(int pos, QColor color){
    QTextCursor cursor(ui->CodeBox->document());
    QTextEdit::ExtraSelection h;
    if(!extras.empty()) extras.pop_back();
    if(pos<0) {ui->CodeBox->setExtraSelections(extras); return;}
    h.cursor = cursor;
    h.cursor.setPosition(pos);
    h.cursor.movePosition(QTextCursor::StartOfLine);
    h.cursor.movePosition(QTextCursor::EndOfLine);
    h.format.setProperty(QTextFormat::FullWidthSelection, true);
    h.format.setBackground(color);
    extras.append(h);
    ui->CodeBox->setExtraSelections(extras);
}

int Program::Run_Program(EvalState &eval_state){
    eval_state.clearEval();
    draw_tree();
    if(state==1){
        QTextEdit::ExtraSelection h;
        QTextCursor cursor(ui->CodeBox->document());
        h.cursor = cursor;
        h.cursor.setPosition(0);
        h.format.setProperty(QTextFormat::FullWidthSelection, true);
        h.format.setBackground(QColor(100,255,100));
        extras.append(h);
    }
    QString line;
    QMap<int,QString>::iterator Code_itr;

    Set_Pos_Table();

    Code_itr = Code_Table.begin();
    while(true){
        if(state==1){
            while(step==0){
                QCoreApplication::processEvents();
                QThread::usleep(300);
            }
            if(state==1){
                ui->SyntaxTree->clear();
                draw_sentence(Code_itr.key(), Code_itr.value());
                ui->SyntaxTree->show();
            }
            if(state==0){
                draw_tree();
            }
            Set_Highlight(Pos_Table[Code_itr.key()], QColor(100,255,100));
        }

        if(Code_itr == Code_Table.end()) {Set_Highlight(-1, QColor(100,255,100)); return 0;}
        line = Code_itr.value();
        int next = Exe_statement(line, eval_state);
        if(state==1){
            step=0;
            ui->RunButton->setEnabled(true);
            ui->DebugButton->setEnabled(true);
        }
        qDebug()<<next<<endl;
        if(next == -1) {Code_itr++;}
        else if(next == -2) {Set_Highlight(-1, QColor(100,255,100)); return 0;}
        else if(next == -3) {
            ui->ResultBox->append("Error:"+line+":\n Invalid Statement or Invalid Expression");
            Set_Highlight(Pos_Table[Code_itr.key()], QColor(255,100,100));
            return -3;
        }
        else Code_itr = Code_Table.find(next);
    }
    state = -1;
    Set_Highlight(-1, QColor(100,255,100));
    return 0;
}

int Program::draw_sentence(QString &Code_Line){
    int num;
    num = get_line_number(Code_Line);
    return draw_sentence(num, Code_Line);
}

int Program::draw_sentence(int line, QString &Code_Line){
    int num = line;
    QString temp;
    statement *state;
    node *head;
    QQueue<node*> queue;

    state = interpreter(Code_Line);
    if(state==nullptr) {
        return 0;
    }
    if(state->state == "LET"){
        ui->SyntaxTree->append(QString::number(num)+" LET =");
        ui->SyntaxTree->append("\t"+((LET*)state)->GetIdName());
        head = syntax_tree(state->token_flow);
        if(head==nullptr) {
            ui->SyntaxTree->append("error");
            return 2;
        }
        head->layer = 1;
        queue.enqueue(head);
        while(!queue.empty()){
            temp.clear();
            node *temp_node = queue.dequeue();
            for(int i=0; i<temp_node->layer; ++i){
                temp+="\t";
            }
            temp+=temp_node->name;
            ui->SyntaxTree->append(temp);
            if(temp_node->left!=NULL) {
                temp_node->left->layer = temp_node->layer+1;
                queue.enqueue(temp_node->left);
            }
            if(temp_node->right!=NULL) {
                temp_node->right->layer = temp_node->layer+1;
                queue.enqueue(temp_node->right);
            }
            delete temp_node;
        }
    }
    if(state->state == "PRINT"){
        ui->SyntaxTree->append(QString::number(num)+" PRINT");
        head = syntax_tree(state->token_flow);
        if(head == nullptr) {
            ui->SyntaxTree->append("error");
            return 2;
        }
        head->layer = 1;
        queue.enqueue(head);
        while(!queue.empty()){
            temp.clear();
            node *temp_node = queue.dequeue();
            for(int i=0; i<temp_node->layer; ++i){
                temp+="\t";
            }
            temp+=temp_node->name;
            ui->SyntaxTree->append(temp);
            if(temp_node->left!=NULL) {
                temp_node->left->layer = temp_node->layer+1;
                queue.enqueue(temp_node->left);
            }
            if(temp_node->right!=NULL) {
                temp_node->right->layer = temp_node->layer+1;
                queue.enqueue(temp_node->right);
            }
            delete temp_node;
        }
    }
    if(state->state == "PRINTF"){
        ui->SyntaxTree->append(QString::number(num)+" PRINTF");
        ui->SyntaxTree->append("\t"+((PRINTF*)state)->getS());
    }
    if(state->state == "INPUT"){
        ui->SyntaxTree->append(QString::number(num)+" INPUT");
        ui->SyntaxTree->append("\t"+((INPUT*)state)->getName());
    }
    if(state->state == "INPUTS"){
        ui->SyntaxTree->append(QString::number(num)+" INPUTS");
        ui->SyntaxTree->append("\t"+((INPUTS*)state)->getName());
    }
    if(state->state == "GOTO"){
        ui->SyntaxTree->append(QString::number(num)+" GOTO");
        ui->SyntaxTree->append("\t"+QString::number(((GOTO*)state)->next_line));
    }
    if(state->state == "IF"){
        ui->SyntaxTree->append(QString::number(num)+" IF THEN");
        QString temp, op;
        tokenizer *left_token, *right_token;
        temp = ((IF*)state)->token_flow.token_next();
        while(temp!=">" && temp!="<" && temp!="=") {
            ((IF*)state)->exp1.append(temp);
            temp = ((IF*)state)->token_flow.token_next();
        }
        op = temp;
        temp = ((IF*)state)->token_flow.token_next();
        while(temp!="THEN"){
            ((IF*)state)->exp2.append(temp);
            temp = ((IF*)state)->token_flow.token_next();
        }

        left_token = new tokenizer(((IF*)state)->exp1);
        right_token = new tokenizer(((IF*)state)->exp2);

        head = syntax_tree(*left_token);
        if(head == nullptr) {
            ui->SyntaxTree->append("error");
            return 2;
        }
        head->layer = 1;
        queue.enqueue(head);
        while(!queue.empty()){
            temp.clear();
            node *temp_node = queue.dequeue();
            for(int i=0; i<temp_node->layer; ++i){
                temp+="\t";
            }
            temp+=temp_node->name;
            ui->SyntaxTree->append(temp);
            if(temp_node->left!=NULL) {
                temp_node->left->layer = temp_node->layer+1;
                queue.enqueue(temp_node->left);
            }
            if(temp_node->right!=NULL) {
                temp_node->right->layer = temp_node->layer+1;
                queue.enqueue(temp_node->right);
            }
            delete temp_node;
        }

        ui->SyntaxTree->append("\t"+op);

        head = syntax_tree(*right_token);
        if(head == nullptr) {
            ui->SyntaxTree->append("error");
            return 2;
        }
        head->layer = 1;
        queue.enqueue(head);
        while(!queue.empty()){
            temp.clear();
            node *temp_node = queue.dequeue();
            for(int i=0; i<temp_node->layer; ++i){
                temp+="\t";
            }
            temp+=temp_node->name;
            ui->SyntaxTree->append(temp);
            if(temp_node->left!=NULL) {
                temp_node->left->layer = temp_node->layer+1;
                queue.enqueue(temp_node->left);
            }
            if(temp_node->right!=NULL) {
                temp_node->right->layer = temp_node->layer+1;
                queue.enqueue(temp_node->right);
            }
            delete temp_node;
        }

        ui->SyntaxTree->append("\t"+state->token_flow.token_next());
    }
    if(state->state=="END"){
        ui->SyntaxTree->append(QString::number(num)+" END");
    }
    if(state->state == "ERROR"){
        ui->SyntaxTree->append(QString::number(num)+" ERROR");
        return 2;
    }
    return 0;
}

void Program::draw_tree(){
    //node *head;
    QQueue<node*> queue;
    ui->SyntaxTree->clear();
    QString Code = ui->CodeBox->toPlainText(), temp;
    QStringList CodeList = Code.split("\n");
    //statement *state;

    QTextBrowser *code = ui->CodeBox;
    QTextCursor cursor(code->document());
    QList<QPair<int,QColor>> highlights;
    int len=-1, err;
    for(QStringList::iterator itr = CodeList.begin(); itr != CodeList.end(); ++itr){
        len += (*itr+"\n").length();
        err = draw_sentence(*itr);
        if(err!=0){
            highlights.append(QPair<int,QColor>(len,QColor(255,100,100)));
        }
    }
    for (auto &line : highlights) {
         QTextEdit::ExtraSelection h;
         h.cursor = cursor;
         h.cursor.setPosition(line.first);
         //h.cursor.movePosition(QTextCursor::Left);
         h.cursor.movePosition(QTextCursor::StartOfLine);
         h.cursor.movePosition(QTextCursor::EndOfLine);
         h.format.setProperty(QTextFormat::FullWidthSelection, true);
         h.format.setBackground(line.second);
         extras.append(h);
    }
    code->setExtraSelections(extras);

    ui->SyntaxTree->show();
}

void Program::on_CodeBox_textChanged()
{

}
