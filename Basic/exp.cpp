/*
 * File: exp.cpp
 * -------------
 * This file implements the Expression class and its subclasses.
 */

#include "exp.hpp"


/*
 * Implementation notes: the Expression class
 * ------------------------------------------
 * The Expression class declares no instance variables and needs no code.
 */

Expression::Expression() = default;

Expression::~Expression() = default;

/*
 * Implementation notes: the ConstantExp subclass
 * ----------------------------------------------
 * The ConstantExp subclass declares a single instance variable that
 * stores the value of the constant.  The eval method doesn't use the
 * value of state but needs it to match the general prototype for eval.
 */

ConstantExp::ConstantExp(int value) {
    this->value = value;
}

int ConstantExp::eval_not_delete(EvalState &state, std::string &message) {
    message="";
    return value;
}

int ConstantExp::eval(EvalState &state) {
    return value;
}

std::string ConstantExp::toString() {
    return integerToString(value);
}

ExpressionType ConstantExp::getType() {
    return CONSTANT;
}

int ConstantExp::getValue() const {
    return value;
}

std::string ConstantExp::component_eval(EvalState &state) {
    return "";
}



/*
 * Implementation notes: the IdentifierExp subclass
 * ------------------------------------------------
 * The IdentifierExp subclass declares a single instance variable that
 * stores the name of the variable.  The implementation of eval must
 * look this variable up in the evaluation state.
 */

IdentifierExp::IdentifierExp(std::string name) {
    this->name = name;
}

int IdentifierExp::eval(EvalState &state) {
    if (!state.isDefined(name)) {
        delete this;
        error("VARIABLE NOT DEFINED");
    }
    return state.getValue(name);
}

int IdentifierExp::eval_not_delete(EvalState &state, std::string &message) {
    if (!state.isDefined(name)) {
        message = "VARIABLE NOT DEFINED";
        return -1;
    }
    return state.getValue(name);
}


std::string IdentifierExp::component_eval(EvalState &state) {
    if (!state.isDefined(name)) return "VARIABLE NOT DEFINED";
    return "";
}

std::string IdentifierExp::toString() {
    return name;
}

ExpressionType IdentifierExp::getType() {
    return IDENTIFIER;
}

std::string IdentifierExp::getName() {
    return name;
}


/*
 * Implementation notes: the CompoundExp subclass
 * ----------------------------------------------
 * The CompoundExp subclass declares instance variables for the operator
 * and the left and right subexpressions.  The implementation of eval 
 * evaluates the subexpressions recursively and then applies the operator.
 */

CompoundExp::CompoundExp(std::string op, Expression *lhs, Expression *rhs) {
    this->op = op;
    this->lhs = lhs;
    this->rhs = rhs;
}

CompoundExp::~CompoundExp() {
    delete lhs;
    delete rhs;
}

/*
 * Implementation notes: eval
 * --------------------------
 * The eval method for the compound expression case must check for the
 * assignment operator as a special case.  Unlike the arithmetic operators
 * the assignment operator does not evaluate its left operand.
 */

int CompoundExp::eval(EvalState &state) {
    if (op == "=") {
        if (lhs->getType() != IDENTIFIER) {
            delete this;
            error("Illegal variable in assignment");
        }
        if (lhs->getType() == IDENTIFIER && lhs->toString() == "LET") {
            delete this;
            error("SYNTAX ERROR");
        }
        int val = rhs->eval(state);
        state.setValue(((IdentifierExp *) lhs)->getName(), val);
        return val;
    }
    std::string left_message, right_message;
    left_message = lhs->component_eval(state);
    right_message = rhs->component_eval(state);
    if (!left_message.empty()) {
        delete this;
        error(left_message);
    }
    if (!right_message.empty()) {
        delete this;
        error(right_message);
    }
    int left = lhs->eval(state);
    int right = rhs->eval(state);
    if (op == "+") return left + right;
    if (op == "-") return left - right;
    if (op == "*") return left * right;
    if (op == "/") {
        if (right == 0) {
            delete this;
            error("DIVIDE BY ZERO");
        }
        return left / right;
    }
    return 0;
}

std::string CompoundExp::component_eval(EvalState &state) {
    if (op == "=") {
        if (lhs->getType() != IDENTIFIER) {
            return "Illegal variable in assignment";
        }
        if (lhs->getType() == IDENTIFIER && lhs->toString() == "LET") {
            return "SYNTAX ERROR";
        }
        return rhs->component_eval(state);
    }
//    int left = lhs->eval(state);
    int right = rhs->eval(state);
//    if (op == "+") return "";
//    if (op == "-") return "";
//    if (op == "*") return "";
    if (op == "/") {
        if (right == 0) {
//            delete this;
            return "DIVIDE BY ZERO";
        }
        return "";
    }
    return "";
}

int CompoundExp::eval_not_delete(EvalState &state, std::string &message) {
    if (op == "=") {
        if (lhs->getType() != IDENTIFIER) {
            message="Illegal variable in assignment";
            return -1;
        }
        if (lhs->getType() == IDENTIFIER && lhs->toString() == "LET") {
           message="SYNTAX ERROR";
            return -1;
        }
        int val = rhs->eval(state);
        state.setValue(((IdentifierExp *) lhs)->getName(), val);
        return val;
    }
    std::string left_message, right_message;
    left_message = lhs->component_eval(state);
    right_message = rhs->component_eval(state);
    if (!left_message.empty()) {
        message=left_message;
        return -1;
    }
    if (!right_message.empty()) {
        message=right_message;
        return -1;
    }
    int left = lhs->eval(state);
    int right = rhs->eval(state);
    if (op == "+") return left + right;
    if (op == "-") return left - right;
    if (op == "*") return left * right;
    if (op == "/") {
        if (right == 0) {
            message="DIVIDE BY ZERO";
            return -1;
        }
        return left / right;
    }
    return 0;
}
std::string CompoundExp::toString() {
    return '(' + lhs->toString() + ' ' + op + ' ' + rhs->toString() + ')';
}

ExpressionType CompoundExp::getType() {
    return COMPOUND;
}

std::string CompoundExp::getOp() {
    return op;
}

Expression *CompoundExp::getLHS() {
    return lhs;
}

Expression *CompoundExp::getRHS() {
    return rhs;
}




