#include "interpreter.h"

#include <string>
#include <sstream>
#include <iomanip>

Interpreter::Interpreter(MainWindow *parent, CodeTorus *torus)
{
    this->parent = parent;
    this->torus = torus;
    st = new std::stack<char>();
}

void Interpreter::step()
{
    char c = torus->getCurrentChar();
    if (inputMode && c != '"'){
        push(c);
        torus->next();
        return;
    }

    switch (c) {
    case('>'): {  // direction right
        torus->changeDirection(CodeTorus::RIGHT);
        torus->next();
        return;
    }
    case('v'): {  // direction down
        torus->changeDirection(CodeTorus::DOWN);
        torus->next();
        return;
    }
    case('<'): {  // direction left
        torus->changeDirection(CodeTorus::LEFT);
        torus->next();
        return;
    }
    case('^'): {  // direction up
        torus->changeDirection(CodeTorus::UP);
        torus->next();
        return;
    }
    case(' '): {  // do nothing
        torus->next();
        return;
    }
    case('#'): {  // trampoline
        torus->next();
        torus->next();
        return;
    }
    case('"'): {  // accept text input
        inputMode = !inputMode;
        torus->next();
        return;
    }
    case('g'): {  // get value
        int y = (int)pop();
        int x = (int)pop();
        push(torus->getChar(x,y));

        torus->next();
        return;
    }
    case('p'): {  // put value
        int y = (int) pop();
        int x = (int) pop();
        char v = pop();
        torus->putChar(v, x, y);

        torus->next();
        return;
    }
    }
}

QString Interpreter::stackToQString()
{
    if (st->empty()) return QString("");
    std::stack<char> *backStack = new std::stack<char>();
    char tmp;
    std::string preOut("");

    while (!st->empty()) {
        backStack->push(st->top());
        st->pop();
    }

    tmp = backStack->top();
    st->push(tmp);
    backStack->pop();

    if (tmp >= ' ' && tmp <= '~') preOut.insert(preOut.end(), tmp);
    else {
        std::stringstream ss;
        ss << "0x" << std::hex << std::setw(2) << std::setfill('0') << (int)tmp;
        std::string sttmp;
        ss >> sttmp;
        preOut.append(sttmp.c_str());
    }

    while (!backStack->empty()) {
        tmp = backStack->top();
        st->push(tmp);
        backStack->pop();

        preOut.append(" , ");
        if (tmp >= ' ' && tmp <= '~') preOut.insert(preOut.end(), tmp);
        else {
            std::stringstream ss;
            ss << "0x" << std::hex << std::setw(2) << std::setfill('0') << (int)tmp;
            std::string sttmp;
            ss >> sttmp;
            preOut.append(sttmp.c_str());
        }
    }
    delete backStack;

    return QString(preOut.c_str());
}

void Interpreter::push(char c)
{
    st->push(c);
}

char Interpreter::pop()
{
    if (st->empty()) return (char) 0;

    char tmp = st->top();
    st->pop();
    return tmp;
}
