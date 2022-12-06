/*
 * File: statement.h
 * -----------------
 * This file defines the Statement abstract type.  In
 * the finished version, this file will also specify subclasses
 * for each of the statement types.  As you design your own
 * version of this class, you should pay careful attention to
 * the exp.h interface, which is an excellent model for
 * the Statement class hierarchy.
 */

#ifndef _statement_h
#define _statement_h

#include <string>
#include <sstream>
#include "evalstate.hpp"
#include "exp.hpp"
#include "Utils/tokenScanner.hpp"
#include "program.hpp"
#include"parser.hpp"
#include "Utils/error.hpp"
#include "Utils/strlib.hpp"

class Program;
class EvalState;
struct node;
/*
 * Class: Statement
 * ----------------
 * This class is used to represent a statement in a program.
 * The model for this class is Expression in the exp.h interface.
 * Like Expression, Statement is an abstract class with subclasses
 * for each of the statement and command types required for the
 * BASIC interpreter.
 */

class Statement {

public:

/*
 * Constructor: Statement
 * ----------------------
 * The base class constructor is empty.  Each subclass must provide
 * its own constructor.
 */

    Statement();

/*
 * Destructor: ~Statement
 * Usage: delete stmt;
 * -------------------
 * The destructor deallocates the storage for this expression.
 * It must be declared virtual to ensure that the correct subclass
 * destructor is called when deleting a statement.
 */

    virtual ~Statement();

/*
 * Method: execute
 * Usage: stmt->execute(state);
 * ----------------------------
 * This method executes a BASIC statement.  Each of the subclasses
 * defines its own execute method that implements the necessary
 * operations.  As was true for the expression evaluator, this
 * method takes an EvalState object for looking up variables or
 * controlling the operation of the interpreter.
 */

    virtual void execute(EvalState &state, Program &program,std::map<int,node>::iterator &it) = 0;

};

class RemStatement:public Statement{

public:
    RemStatement();
    virtual void execute(EvalState &state, Program &program,std::map<int,node>::iterator &it){};
};

class LetStatement:public Statement{

public:
    LetStatement(Expression *exp, std::string var);

    virtual ~LetStatement();

    virtual void execute(EvalState &state, Program &program,std::map<int,node>::iterator &it) ;


private:
    int value;
    std::string var;
    Expression *exp;
};

class IfStatement:public Statement{

public:
    IfStatement(Expression *lsh_exp,std::string op,Expression *rhs_exp,int num);

    ~IfStatement();

    virtual void execute(EvalState &state, Program &program,std::map<int,node>::iterator &it) ;

private:
    Expression *lhs_exp,*rhs_exp;
    std::string op;
    int num;

};

class PrintStatement:public Statement{

public:

    PrintStatement(Expression *exp);

    ~PrintStatement();

    virtual void execute(EvalState &state, Program &program,std::map<int,node>::iterator &it) ;

private:

    Expression * exp;

    int value;

};

class InputStatement:public Statement{

public:

    InputStatement(std::string var);

    virtual void execute(EvalState &state, Program &program,std::map<int,node>::iterator &it) ;

private:

    std::string var;
};

class EndStatement:public Statement{
public:

    EndStatement();

    virtual void execute(EvalState &state, Program &program,std::map<int,node>::iterator &it) ;

};


class GotoStatement:public Statement{
public:

    GotoStatement(int num);

    virtual void execute(EvalState &state, Program &program,std::map<int,node>::iterator &it) ;

private:

    int num;

};
//
//class RunStatement:public Statement{
//
//public:
//
//    RunStatement();
//
//    virtual void execute(EvalState &state, Program &program);
//};
//
//class ListStatement:public Statement{
//
//public:
//
//    ListStatement();
//
//    virtual void execute(EvalState &state, Program &program);
//
//};

//class ClearStatement:public Statement{
//
//public:
//    ClearStatement();
//
//    virtual void execute(EvalState &state, Program &program);
//
//};

//class QuitStatement:public Statement{
//
//public:
//
//    QuitStatement();
//
//    virtual void execute(EvalState &state, Program &program);
//
//};
/*
 * The remainder of this file must consists of subclass
 * definitions for the individual statement forms.  Each of
 * those subclasses must define a constructor that parses a
 * statement from a scanner and a method called execute,
 * which executes that statement.  If the private data for
 * a subclass includes data allocated on the heap (such as
 * an Expression object), the class implementation must also
 * specify its own destructor method to free that memory.
 */

#endif
