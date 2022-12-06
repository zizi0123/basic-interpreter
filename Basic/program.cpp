/*
 * File: program.cpp
 * -----------------
 * This file is a stub implementation of the program.h interface
 * in which none of the methods do anything beyond returning a
 * value of the correct type.  Your job is to fill in the bodies
 * of each of these methods with an implementation that satisfies
 * the performance guarantees specified in the assignment.
 */

#include "program.hpp"



Program::Program() = default;  //希望仍然保留编译器的默认构造行为

Program::~Program() = default;

void Program::clear() {
    for(auto it=program_map.begin();it!=program_map.end();it++){
        delete it->second.stmt;
    }
    program_map.clear();
}

void Program::addSourceLine(int lineNumber, const std::string &line) {
    node a;
    a.source_line=line;
    auto it=program_map.find(lineNumber);
    if(it!=program_map.end()){ //说明这行statement 之前出现过
        delete it->second.stmt;
        it->second.stmt= nullptr;
        program_map.erase(it);
    }
    program_map.insert(std::pair<int,node>(lineNumber,a));
}

void Program::removeSourceLine(int lineNumber) {
    auto it=program_map.find(lineNumber);
    if(it==program_map.end()) return; //没有找到这个行号，直接返回
    delete it->second.stmt;
    it->second.stmt= nullptr;
    program_map.erase(it);
}

std::string Program::getSourceLine(int lineNumber) {
    auto it=program_map.find(lineNumber);
    if(it==program_map.end()) return "";
    return it->second.source_line;
}

void Program::setParsedStatement(int lineNumber, Statement *stmt) {
    auto it=program_map.find(lineNumber);
    if(it==program_map.end()) {
        error("SYNTAX ERROR");
        return;
    }
    delete it->second.stmt;
    it->second.stmt=stmt;
}

//void Program::removeSourceLine(int lineNumber) {

Statement *Program::getParsedStatement(int lineNumber) {
    return program_map.find(lineNumber)->second.stmt;
}

int Program::getFirstLineNumber() {
    return program_map.begin()->first;
}

int Program::getNextLineNumber(int lineNumber) {
   auto it=program_map.find(lineNumber);
   it++;
    if(it==program_map.end()) return -1;
    return it->first;
}

void Program::execute_all(EvalState & state) {
    auto it=program_map.begin();
    while(it!=program_map.end()){
        auto temp=it;
        it->second.stmt->execute(state,*this,it);  //观察有无跳转，如果已经发生跳转，就不再进行自增运算。
        if(it==temp) it++;
    }
}

void Program::list() {
    for(auto it=program_map.begin();it!=program_map.end();it++){
        std::cout<<it->second.source_line<<'\n';
    }
}

//more func to add
//todo


