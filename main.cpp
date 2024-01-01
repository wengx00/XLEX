#include "nfa.hpp"
#include "dfa.hpp"
#include "mdfa.hpp"

using namespace std;

int main() {
    Nfa nfa("b*a(bb)*a(a|(bb)*)*b*");
    Dfa dfa(nfa);
    MDfa mdfa(dfa);
    string lexCode = mdfa.lex();
    cout << lexCode << '\n';
    return 0;
}