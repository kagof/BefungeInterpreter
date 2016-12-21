#include "interpreter.h"

#include <string>
#include <sstream>
#include <iomanip>
#include <random>

Interpreter::Interpreter(MainWindow *parent, CodeTorus *torus)
{
    this->parent = parent;
    this->torus = torus;
    st = new std::stack<char>();
    stringmode = false;
}

void Interpreter::step()
{
    char c = torus->getCurrentChar();
    if (stringmode && c != '"'){
        push(c);
        torus->next();
        return;
    }

    switch (c) {
    case(' '): {  // do nothing
        torus->next();
        return;
    }
    case('+'): {  // add
        int a = (int) pop();
        int b = (int) pop();
        push((char)(a+b));
        torus->next();
        return;
    }
    case('-'): {  // subtract
        int a = (int) pop();
        int b = (int) pop();
        push((char)(b-a));
        torus->next();
        return;
    }
    case('*'): {  // multiply
        int a = (int) pop();
        int b = (int) pop();
        push((char)(a*b));
        torus->next();
        return;
    }
    case('/'): {  // divide
        int a = (int) pop();
        int b = (int) pop();
        if (a!=0) push((char)(b/a));
        //TODO: else: ask for input if a=0
        torus->next();
        return;
    }
    case('%'): {  // modulo
        int a = (int) pop();
        int b = (int) pop();
        push((char)(b%a));
        torus->next();
        return;
    }
    case('!'): {  // logical not
        int a = (int) pop();
        if (a==0) push((char)1);
        else push((char)0);
        torus->next();
        return;
    }
    case('`'): {  // greater than
        int a = (int) pop();
        int b = (int) pop();
        if (b>a) push((char)1);
        else push((char)0);
        torus->next();
        return;
    }
    case('>'): {  // direction right
        torus->changeDirection(CodeTorus::RIGHT);
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
    case('v'): {  // direction down
        torus->changeDirection(CodeTorus::DOWN);
        torus->next();
        return;
    }
    case('?'): {  // random direction
        //create & seed a random number generator (between 0 and 3)
        std::random_device rd;
        std::mt19937 rand_gen(rd());
        std::uniform_int_distribution<> range(0, 3);
        torus->changeDirection(static_cast<CodeTorus::direction>(range(rand_gen)));
        torus->next();
        return;
    }
    case('_'): {  // horizontal if
        int a = (int) pop();
        if (a == 0) torus->changeDirection(CodeTorus::RIGHT);
        else torus->changeDirection(CodeTorus::LEFT);
        torus->next();
        return;
    }
    case('|'): {  // vertical if
        int a = (int) pop();
        if (a == 0) torus->changeDirection(CodeTorus::DOWN);
        else torus->changeDirection(CodeTorus::UP);
        torus->next();
        return;
    }
    case('"'): {  // accept text input (stringmode)
        stringmode = !stringmode;
        torus->next();
        return;
    }
    case(':'): {  // duplicate
        char c = pop();
        if (!st->empty()) push(c);  // only actually want to push it twice if the stack was empty
        push(c);
        torus->next();
        return;
    }
    case('\\'): {  // swap
        char c = pop();
        char d = pop();
        push(c);
        push(d);
        torus->next();
        return;
    }
    case('$'): {  // pop & discard
        pop();
        torus->next();
        return;
    }
    case('.'): {  // pop & output as integer
        int i = (int)pop();
        parent->output(i);
        torus->next();
        return;
    }
    case(','): {  // pop & output as ASCII char
        char c = pop();
        parent->output(c);
        torus->next();
        return;
    }
    case('#'): {  // trampoline
        torus->next();
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
    case('&'): {  // get integer input
        //TODO: this case
        torus->next();
        return;
    }
    case('~'): {  // get ASCII char input
        //TODO: this case
        torus->next();
        return;
    }
    case('@'): {  // End program
        parent->programFinished();
        return;
    }
    case('0'): {  // push this number
        push(char(0));
        torus->next();
        return;
    }
    case('1'): {  // push this number
        push(char(1));
        torus->next();
        return;
    }
    case('2'): {  // push this number
        push(char(2));
        torus->next();
        return;
    }
    case('3'): {  // push this number
        push(char(3));
        torus->next();
        return;
    }
    case('4'): {  // push this number
        push(char(4));
        torus->next();
        return;
    }
    case('5'): {  // push this number
        push(char(5));
        torus->next();
        return;
    }
    case('6'): {  // push this number
        push(char(6));
        torus->next();
        return;
    }
    case('7'): {  // push this number
        push(char(7));
        torus->next();
        return;
    }
    case('8'): {  // push this number
        push(char(8));
        torus->next();
        return;
    }
    case('9'): {  // push this number
        push(char(9));
        torus->next();
        return;
    }
    }
    //TODO: if we get here then we hit an invalid character.
}

QString Interpreter::stackToQString()
{
    if (st->empty()) return QString("");
    std::stack<char> *backStack = new std::stack<char>();
    char tmp;
    std::string preOut("");

    // first push the whole stack to a temporary stack, so we can read it bottom to top
    while (!st->empty()) {
        backStack->push(st->top());
        st->pop();
    }

    tmp = backStack->top();
    st->push(tmp);
    backStack->pop();

    // add decimal value of first character
    std::stringstream ss;
    ss << std::dec << (int)tmp;
    std::string sttmp;
    ss >> sttmp;
    preOut.append(sttmp.c_str());

    // if it is a visible character, show that in the stack.
    if (tmp >= ' ' && tmp <= '~') {
        preOut.append(" (");
        preOut.insert(preOut.end(), tmp);
        preOut.append(")");
    }

    //for all other characters ( this is just so that our stack representation will be comma separated)
    while (!backStack->empty()) {
        tmp = backStack->top();
        st->push(tmp);
        backStack->pop();

        preOut.append(" , ");

        //add decimal value
        std::stringstream ss;
        ss << std::dec << (int)tmp;
        std::string sttmp;
        ss >> sttmp;
        preOut.append(sttmp.c_str());

        // if it is a visible character, show that in the stack.
        if (tmp >= ' ' && tmp <= '~') {
            preOut.append(" (");
            preOut.insert(preOut.end(), tmp);
            preOut.append(")");
        }
    }
    delete backStack;

    return QString(preOut.c_str());
}

void Interpreter::push(char c)
{
    st->push(c);
    parent->setStackBoxText(stackToQString());
}

char Interpreter::pop()
{
    if (st->empty()) return (char) 0;

    char tmp = st->top();
    st->pop();
    parent->setStackBoxText(stackToQString());
    return tmp;
}
