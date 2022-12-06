/*
 * File: tokenscanner.cpp
 * ----------------------
 * Implementation for the TokenScanner class.
 */

#include <cctype>
#include "error.hpp"
#include "tokenScanner.hpp"
#include "strlib.hpp"


TokenScanner::TokenScanner() {
    initScanner();  //初始化：将所有flag都设为flas
    setInput("");
}

TokenScanner::TokenScanner(std::string str) {
    initScanner();
    setInput(str);
}

TokenScanner::TokenScanner(std::istream &infile) {
    initScanner();
    setInput(infile);
}

TokenScanner::~TokenScanner() {
    delete isp;
    delete savedTokens;
}

void TokenScanner::setInput(std::string str) {
    stringInputFlag = true;
    buffer = str;   //buffer: The original argument string
    if (isp != nullptr) delete isp;                            //？？？？？？？？？如果这个isp不是new出来的，而是对流对象取地址运算得到的，也能delete吗？
    isp = new std::istringstream(buffer);
    delete savedTokens;
    savedTokens = nullptr;
}

void TokenScanner::setInput(std::istream &infile) {
    stringInputFlag = false;
    if (isp != nullptr)delete isp;
    isp = &infile;
    delete savedTokens;
    savedTokens = nullptr;
}

bool TokenScanner::hasMoreTokens() {
    std::string token = nextToken();
    saveToken(token);
    return (token != "");
}

std::string TokenScanner::nextToken() {
    if (savedTokens != nullptr) {  //从链表savedtokens 里面读取头结点的字符串，并且把头结点从链表savedtoken里删除
        StringCell *cp = savedTokens;
        std::string token = cp->str;
        savedTokens = cp->link;
        delete cp;
        return token;
    }
    while (true) {
        if (ignoreWhitespaceFlag) skipSpaces();
        int ch = isp->get();
        if (ch == '/' && ignoreCommentsFlag) {
            ch = isp->get();
            if (ch == '/') {
                while (true) {
                    ch = isp->get();
                    if (ch == '\n' || ch == '\r' || ch == EOF) break;
                }
                continue;
            } else if (ch == '*') {
                int prev = EOF;
                while (true) {
                    ch = isp->get();
                    if (ch == EOF || (prev == '*' && ch == '/')) break;
                    prev = ch;
                }
                continue;
            }
            if (ch != EOF) isp->unget();
            ch = '/';
        }
        if (ch == EOF) return "";
        if ((ch == '"' || ch == '\'') && scanStringsFlag) {
            isp->unget();  //将当前字符读回到流中
            return scanString();
        }
        if (isdigit(ch) && scanNumbersFlag) {
            isp->unget();
            return scanNumber();
        }
        if (isWordCharacter(ch)) {
            isp->unget();
            return scanWord();
        }
        std::string op = std::string(1, ch);
        while (isOperatorPrefix(op)) {   //判断是否是运算符的前缀
            ch = isp->get();
            if (ch == EOF) break;
            op += ch;
        }
        while (op.length() > 1 && !isOperator(op)) {
            isp->unget();
            op.erase(op.length() - 1, 1);
        }
        return op;
    }
}

void TokenScanner::saveToken(std::string token) {  //将字符串token插入链表savedtoken的头结点
    StringCell *cp = new StringCell;
    cp->str = token;
    cp->link = savedTokens;
    savedTokens = cp;
}

void TokenScanner::ignoreWhitespace() {
    ignoreWhitespaceFlag = true;
}

void TokenScanner::ignoreComments() {
    ignoreCommentsFlag = true;
}

void TokenScanner::scanNumbers() {
    scanNumbersFlag = true;
}

void TokenScanner::scanStrings() {
    scanStringsFlag = true;
}

void TokenScanner::addWordCharacters(std::string str) {
    wordChars += str;
}

void TokenScanner::addOperator(std::string op) {  //将op插入链表operators的头部
    StringCell *cp = new StringCell;
    cp->str = op;
    cp->link = operators;
    operators = cp;
}

int TokenScanner::getPosition() const {
    if (savedTokens == nullptr) {
        return int(isp->tellg());
    } else {
        return int(isp->tellg()) - savedTokens->str.length();    //?????????????????????????如果savedtoken链表里有很多节点呢？以及对于空格的处理？
    }
    return -1;     //？？？？？？？？？？？？？？？？？？？？？？？？？？？？调用多次就返回-1是什么意思以及是如何做到的？
}

bool TokenScanner::isWordCharacter(char ch) const {
    return isalnum(ch) || wordChars.find(ch) != std::string::npos;    //要求能在wordchar字符串中找到它
};

void TokenScanner::verifyToken(std::string expected) {
    std::string token = nextToken();
    if (token != expected) {
        std::string msg = "Found \"" + token + "\"" +
                          " when expecting \"" + expected + "\"";
        error(msg);
    }
};

TokenType TokenScanner::getTokenType(std::string token) const {
    if (token == "") return TokenType(EOF);
    char ch = token[0];
    if (isspace(ch)) return SEPARATOR;       //?????????????separator是什么，它的第一个字符为什么是空格？
    if (ch == '"' || (ch == '\'' && token.length() > 1)) return STRING;
    if (isdigit(ch)) return NUMBER;
    if (isWordCharacter(ch)) return WORD;
    return OPERATOR;
};

std::string TokenScanner::getStringValue(std::string token) const {
    std::string str = "";
    int start = 0;
    int finish = token.length();
    if (finish > 1 && (token[0] == '"' || token[0] == '\'')) {
        start = 1;
        finish--;
    }
    for (int i = start; i < finish; i++) {
        char ch = token[i];
        if (ch == '\\') {
            ch = token[++i];
            if (isdigit(ch) || ch == 'x') {
                int base = 8;
                if (ch == 'x') {
                    base = 16;
                    i++;
                }
                int result = 0;
                int digit = 0;
                while (i < finish) {
                    ch = token[i];
                    if (isdigit(ch)) {
                        digit = ch - '0';
                    } else if (isalpha(ch)) {
                        digit = toupper(ch) - 'A' + 10;
                    } else {
                        digit = base;
                    }
                    if (digit >= base) break;
                    result = base * result + digit;
                    i++;
                }
                ch = char(result);
                i--;
            } else {
                switch (ch) {
                    case 'a':
                        ch = '\a';
                        break;
                    case 'b':
                        ch = '\b';
                        break;
                    case 'f':
                        ch = '\f';
                        break;
                    case 'n':
                        ch = '\n';
                        break;
                    case 'r':
                        ch = '\r';
                        break;
                    case 't':
                        ch = '\t';
                        break;
                    case 'v':
                        ch = '\v';
                        break;
                    case '"':
                        ch = '"';
                        break;
                    case '\'':
                        ch = '\'';
                        break;
                    case '\\':
                        ch = '\\';
                        break;
                }
            }
        }
        str += ch;
    }
    return str;
}

int TokenScanner::getChar() {
    return isp->get();
}

void TokenScanner::ungetChar(int ch) {
    isp->unget();
}

/* Private methods */

void TokenScanner::initScanner() {
    ignoreWhitespaceFlag = false;
    ignoreCommentsFlag = false;
    scanNumbersFlag = false;
    scanStringsFlag = false;
    operators = nullptr;
}

/*
 * Implementation notes: skipSpaces
 * --------------------------------
 * Advances the position of the scanner until the current character is
 * not a whitespace character.
 */

void TokenScanner::skipSpaces() {   //在输入流中跳过空格
    while (true) {
        int ch = isp->get(); //从输入流*isp中读入一个字符，包括空白字符和EOF
        if (ch == EOF) return;
        if (!isspace(ch)) {
            isp->unget();//将*isp向后移动一个字节。返回*isp. 为什么要向后移动一个字节？
            return;
        }
    }
}

/*
 * Implementation notes: scanWord
 * ------------------------------
 * Reads characters until the scanner reaches the end of a sequence
 * of word characters.
 */

std::string TokenScanner::scanWord() {   //找读取输入流中的下一个单词。如果下一个不是单词，返回空字符串。
    std::string token = "";
    while (true) {
        int ch = isp->get();
        if (ch == EOF) break;
        if (!isWordCharacter(ch)) {
            isp->unget();
            break;
        }
        token += char(ch);
    }
    return token;
}

/*
 * Implementation notes: scanNumber
 * --------------------------------
 * Reads characters until the scanner reaches the end of a legal number.
 * The function operates by simulating what computer scientists
 * call a finite-state machine.  The program uses the variable
 * <code>state</code> to record the history of the process and
 * determine what characters would be legal at this point in time.
 */

std::string TokenScanner::scanNumber() {
    std::string token = "";
    NumberScannerState state = INITIAL_STATE;
    while (state != FINAL_STATE) {
        int ch = isp->get();
        int xch = 'e';
        switch (state) {
            case INITIAL_STATE:
                if (!isdigit(ch)) {
                    error("Internal error: illegal call to scanNumber");
                }
                state = BEFORE_DECIMAL_POINT;
                break;
            case BEFORE_DECIMAL_POINT:
                if (ch == '.') {
                    state = AFTER_DECIMAL_POINT;
                } else if (ch == 'E' || ch == 'e') {
                    state = STARTING_EXPONENT;
                    xch = ch;
                } else if (!isdigit(ch)) {
                    if (ch != EOF) isp->unget();
                    state = FINAL_STATE;
                }
                break;
            case AFTER_DECIMAL_POINT:
                if (ch == 'E' || ch == 'e') {
                    state = STARTING_EXPONENT;
                    xch = ch;
                } else if (!isdigit(ch)) {
                    if (ch != EOF) isp->unget();
                    state = FINAL_STATE;
                }
                break;
            case STARTING_EXPONENT:
                if (ch == '+' || ch == '-') {
                    state = FOUND_EXPONENT_SIGN;
                } else if (isdigit(ch)) {
                    state = SCANNING_EXPONENT;
                } else {
                    if (ch != EOF) isp->unget();
                    isp->unget();
                    state = FINAL_STATE;
                }
                break;
            case FOUND_EXPONENT_SIGN:
                if (isdigit(ch)) {
                    state = SCANNING_EXPONENT;
                } else {
                    if (ch != EOF) isp->unget();
                    isp->unget();
                    isp->unget();
                    state = FINAL_STATE;
                }
                break;
            case SCANNING_EXPONENT:
                if (!isdigit(ch)) {
                    if (ch != EOF) isp->unget();
                    state = FINAL_STATE;
                }
                break;
            default:
                state = FINAL_STATE;
                break;
        }
        if (state != FINAL_STATE) {
            token += char(ch);
        }
    }
    return token;
}

/*
 * Implementation notes: scanString
 * --------------------------------
 * Reads and returns a quoted string from the scanner, continuing until
 * it scans the matching delimiter.  The scanner generates an error if
 * there is no closing quotation mark before the end of the input.
 */

std::string TokenScanner::scanString() {
    std::string token = "";
    char delim = isp->get();
    token += delim;
    bool escape = false;
    while (true) {
        int ch = isp->get();
        if (ch == EOF) error("TokenScanner found unterminated string");
        if (ch == delim && !escape) break;
        escape = (ch == '\\') && !escape;
        token += ch;
    }
    return token + delim;
}

/*
 * Implementation notes: isOperator, isOperatorPrefix
 * --------------------------------------------------
 * These methods search the list of operators and return true if the
 * specified operator is either in the list or a prefix of an operator
 * in the list, respectively.  This code could be made considerably more
 * efficient by implementing operators as a trie.
 */

bool TokenScanner::isOperator(std::string op) {     //判断所给运算符是否在链表operator里出现
    for (StringCell *cp = operators; cp != nullptr; cp = cp->link) {
        if (op == cp->str) return true;
    }
    return false;
}

bool TokenScanner::isOperatorPrefix(std::string op) {
    for (StringCell *cp = operators; cp != nullptr; cp = cp->link) {
        if (startsWith(cp->str, op)) return true;
    }
    return false;
}

