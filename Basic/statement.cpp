/*
 * File: statement.cpp
 * -------------------
 * This file implements the constructor and destructor for
 * the Statement class itself.  Your implementation must do
 * the same for the subclasses you define for each of the
 * BASIC statements.
 */

#include "statement.hpp"


/* Implementation of the Statement class */


Statement::Statement() = default;

Statement::~Statement() = default;

/* Implementation notes: the LetStatement subclass */

RemStatement::RemStatement() {}


bool IsLegalInteger_state(std::string a){
    for(int i=0;i<a.length();i++){
        if(!('0'<=a[i] && a[i]<='9' || a[i]=='-')) return false;
    }
    return true;
}

LetStatement::LetStatement(Expression *exp,std::string var){
    this->exp=exp;
    this->var=var;
}

LetStatement::~LetStatement() {
    delete exp;
}

void LetStatement::execute(EvalState &state, Program &program,std::map<int,node>::iterator &it) {
    std::string error_message;
    value=exp->eval_not_delete(state, error_message);
    if(!error_message.empty()) error(error_message);
    state.setValue(var,value);
}


IfStatement::IfStatement(Expression *lhs_exp, std::string op,Expression *rhs_exp, int num) {
    this->lhs_exp=lhs_exp;
    this->op=op;
    this->rhs_exp=rhs_exp;
    this->num=num;
}

void IfStatement::execute(EvalState &state, Program &program,std::map<int,node>::iterator &it) {
    bool flag=false;
    std::string error_message;
    int value1=lhs_exp->eval_not_delete(state,error_message);
    if(!error_message.empty()){
        std::cout<<error_message<<'\n';
        return;
    }
    int value2=rhs_exp->eval_not_delete(state,error_message);
    if(!error_message.empty()){
        std::cout<<error_message<<'\n';
        return;
    }
    if(op=="=" && value1==value2) flag= true;
    if(op=="<" && value1<value2) flag= true;
    if(op==">" && value1>value2) flag= true;
    if(flag){
        auto itt=program.program_map.find(num);
        if(itt==program.program_map.end()){
            std::cout<<"LINE NUMBER ERROR\n";
        }else{
            it=itt;
        }
    }

}

IfStatement::~IfStatement() {
    delete lhs_exp;
    delete rhs_exp;
}

PrintStatement::PrintStatement(Expression *exp) {
    this->exp=exp;
}

PrintStatement::~PrintStatement() {
    delete exp;
}

void PrintStatement::execute(EvalState &state, Program &program,std::map<int,node>::iterator &it) {
    std::string error_message;
    value=exp->eval_not_delete(state, error_message);
    if(!error_message.empty()) error(error_message);
    std::cout << value<<'\n';
}



InputStatement::InputStatement(std::string var) {
    this->var=var;
}

void InputStatement::execute(EvalState &state, Program &program,std::map<int,node>::iterator &it) {
    std::cout<<" ? ";
    std::string value;
    getline(std::cin, value);
    while(!IsLegalInteger_state(value)){
        std::cout<<"INVALID NUMBER\n";
        std::cout<<" ? ";
        getline(std::cin, value);
    }
    int value_int= stringToInteger(value);
    state.setValue(var,value_int);
}

EndStatement::EndStatement() {}

void EndStatement::execute(EvalState &state, Program &program,std::map<int,node>::iterator &it) {
    it=program.program_map.end();
};

GotoStatement::GotoStatement(int num) {
    this->num=num;
}

void GotoStatement::execute(EvalState &state, Program &program,std::map<int,node>::iterator &it) {
    auto itt=program.program_map.find(num);
    if(itt==program.program_map.end()) {
        std::cout<<"LINE NUMBER ERROR\n";
    }else{
        it=itt;
    }
}

//RunStatement::RunStatement() {}
//
//void RunStatement::execute(EvalState &state, Program &program) {
//
//
//}

//ListStatement::ListStatement() {}
//
//void ListStatement::execute(EvalState &state, Program &program) {
//}
//
//ClearStatement::ClearStatement() {}
//
//void ClearStatement::execute(EvalState &state, Program &program) {
//}

//QuitStatement::QuitStatement() {}
//
//void QuitStatement::execute(EvalState &state, Program &program) {
//    exit(0);
//}


