/*
 * File: Basic.cpp
 * ---------------
 * This file is the starter project for the BASIC interpreter.
 */

#include <cctype>
#include <iostream>
#include <string>
#include "exp.hpp"
#include "parser.hpp"
#include "program.hpp"
#include "Utils/error.hpp"
#include "Utils/tokenScanner.hpp"
#include "Utils/strlib.hpp"


/* Function prototypes */

void processLine(std::string line, Program &program, EvalState &state);

bool IsLegalWord(std::string a);

bool IsLegalInteger(std::string a);

/* Main program */

int main() {
    EvalState state;
    Program program;
    //cout << "Stub implementation of BASIC" << endl;
    while (true) {
        try {
            std::string input;
            getline(std::cin, input);
            if (input.empty())
                return 0;
            processLine(input, program, state);
        } catch (ErrorException &ex) {
            std::cout << ex.getMessage() << std::endl;
        }
    }
    return 0;
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.  In this version of
 * implementation, the program reads a line, parses it as an expression,
 * and then prints the result.  In your implementation, you will
 * need to replace this method with one that can respond correctly
 * when the user enters a program line (which begins with a number)
 * or one of the BASIC commands, such as LIST or RUN.
 */

void processLine(std::string line, Program &program, EvalState &state) {
    TokenScanner scanner;
    scanner.ignoreWhitespace();
    scanner.scanNumbers();
    scanner.setInput(line);
    int line_num = 0;
    std::string str_first = scanner.nextToken(),str_command;
    if (scanner.getTokenType(str_first) == NUMBER) {//如果有行号
        line_num = stringToInteger(str_first);
        if (!scanner.hasMoreTokens()) {
            program.removeSourceLine(line_num);
            return;
        }else{
            str_command=scanner.nextToken();
        }
    }else{
        str_command=str_first;
    }

    Statement *stmt;
    if (str_command == "REM") { //REMARK
        if (line_num == 0) return;
        stmt = new RemStatement();
    } else if (str_command == "LET") {  //LET
        std::string var;
        var = scanner.nextToken();  //获取变量名
        if (scanner.getTokenType(var) != WORD || !IsLegalWord(var)) {
            error("SYNTAX ERROR"); //会自动构建一个异常类型并且抛出
        }
        std::string op;
        op = scanner.nextToken();
        if (op != "=") {
            error("SYNTAX ERROR"); //会自动构建一个异常类型并且抛出
        }
        Expression *exp;
        exp = parseExp(scanner);
        if (line_num == 0) {
            int value;
            value = exp->eval(state); //get右值；
            delete exp;
            state.setValue(var, value); //立刻执行
        } else {
            stmt = new LetStatement(exp, var);
        }
    } else if (str_command == "PRINT") {  //PRINT
        Expression *exp_print = parseExp(scanner);
        if (line_num == 0) {
            int value_print = exp_print->eval(state);
            delete exp_print;
            std::cout << value_print<<'\n';
        } else {
            stmt = new PrintStatement(exp_print);
        }
    } else if (str_command == "INPUT") {
        std::string var = scanner.nextToken();
        if(scanner.getTokenType(var)!=WORD || !IsLegalWord(var)) error("SYNTAX ERROR");
        if(line_num==0){  //立刻执行
            std::cout<<" ? ";
            std::string value;
            getline(std::cin, value);
            while(!IsLegalInteger(value)){
                std::cout<<"INVALID NUMBER\n";
                std::cout<<" ? ";
                getline(std::cin, value);
            }
            int value_int= stringToInteger(value);
            state.setValue(var,value_int);
        }else{
            stmt = new InputStatement(var);
        }
    } else if (str_command == "END") {
        if (line_num == 0) {
            exit(0);
        } else {
            stmt = new EndStatement();
        }
    } else if (str_command == "GOTO") {
        int num = stringToInteger(scanner.nextToken());
        stmt = new GotoStatement(num);
    } else if (str_command == "RUN") {
        program.execute_all(state);
    } else if (str_command == "LIST") {
        program.list();
    } else if (str_command == "CLEAR") {
        program.clear();
        state.Clear();
    } else if (str_command == "QUIT") {
        exit(0);
    } else if (str_command == "IF") {
        if (line_num == 0) error("SYNTAX ERROR");
        std::string lhs, op, rhs, temp;
        int num;
        temp = scanner.nextToken();
        while (temp!="=" && temp!=">" && temp!="<") {
            lhs += temp;
            temp = scanner.nextToken();
        }
        op = temp;
        temp = scanner.nextToken();
        while (temp != "THEN") {
            rhs += temp;
            temp = scanner.nextToken();
        }
        temp = scanner.nextToken();
        num = stringToInteger(temp);
        TokenScanner lhss, rhss;
        lhss.ignoreWhitespace();
        rhss.ignoreWhitespace();
        lhss.scanNumbers();
        rhss.scanNumbers();
        lhss.setInput(lhs);
        rhss.setInput(rhs);
        Expression *lhs_exp,*rhs_exp;
        lhs_exp=parseExp(lhss);
        rhs_exp= parseExp(rhss);
        stmt = new IfStatement(lhs_exp, op, rhs_exp, num);
    }

    if (line_num != 0) {
        program.addSourceLine(line_num, line);
        program.setParsedStatement(line_num, stmt);
    }
}

bool IsLegalWord(std::string a){
    if(a=="REM") return false;
    if(a=="LET") return false;
    if(a=="PRINT") return false;
    if(a=="END") return false;
    if(a=="GOTO") return false;
    if(a=="IF") return false;
    if(a=="THEN") return false;
    if(a=="RUN") return false;
    if(a=="LIST") return false;
    if(a=="CLEAR") return false;
    if(a=="QUIT") return false;
    if(a=="HELP") return false;
    return true;
}

bool IsLegalInteger(std::string a){
    for(int i=0;i<a.length();i++){
        if(!('0'<=a[i] && a[i]<='9'|| a[i]=='-')) return false;
    }
    return true;
}