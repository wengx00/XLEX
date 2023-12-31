#include "nfa.hpp"
#include "dfa.hpp"
#include "mdfa.hpp"

using namespace std;

int main() {
    Nfa nfa("ab|cd*");
    Dfa dfa(nfa);
    MDfa mdfa(dfa);
    return 0;
}