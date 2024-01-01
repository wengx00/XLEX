#include "nfa.hpp"
#include "dfa.hpp"
#include "mdfa.hpp"

using namespace std;

int main() {
    Nfa nfa("ab|de*");
    Dfa dfa(nfa);
    MDfa mdfa(dfa);
    string lexCode = mdfa.lex();
    cout << lexCode << '\n';
    return 0;
}