#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <sstream>

using std::vector;
using std::string;
using std::cin;
using std::cout;
using std::endl;
using std::map;
using std::stringstream;

// Simple regular expression: only consists of

// The literal character: a-z, A-Z, 0-9

// Parenthesis: () no capture function, only used to sepcify the
// calculate sequnce

// Quantity character: *, ?, +

// Alternation character: |

// The regular expressions are consists of operans and operators, the
// literal characters are the baisc operands, while all the other
// characters are operators.

// Concating lietral characters consists of unit

// First the program will sacn the regualr expression and figure out
// all the units

// Then according to the units and operators we make a NFA

// Finally we let the test string go through the NFA to see if there
// is a match

int label_count = 0;

enum StateType {
    SINGLE,
    SPLIT,
    MATCH,
    ERROR,
};
struct State {
    State(StateType type, State *out1, State *out2)
        : m_type(type)
        , m_out1(out1)
        , m_out2(out2) { }

    State(StateType type)
        : m_type(type)
        , m_out1(nullptr)
        , m_out2(nullptr) { }

    State()
        : m_type(ERROR)
        , m_out1(nullptr)
        , m_out2(nullptr){ }
    StateType m_type;
    State *m_out1;
    State *m_out2;
    char need = 0;
    int m_count = 0;
};



static void LevelOneParse(string &regexp, map<string, State *>symbol_map) {
    string *sh = nullptr;
    std::vector<State *> cache;
    std::size_t start = 0;
    std::size_t end = 0;
    for (std::size_t i = 0; i < regexp.size(); ++i) {
        switch (regexp[i]) {
        case '*':
        case '+':
        case '?':
        case '|': {
            end = i;
            cache.clear();
            for (auto j : *sh) {
                State *pState = new State(SINGLE);
                pState->need = j;
                cache.push_back(pState);
            }
            if (cache.size() > 1) {
                for (std::size_t s = 1; s < cache.size(); ++s) {
                    cache[s - 1]->m_out1 = cache[s];
                }
            }
            stringstream ss;
            ss << "@" << label_count << "@";
            regexp.replace(start, end, ss.str());
            ss.str("");
            delete sh;
            sh = nullptr;
            break;
        }
        default:
            if (!sh) {
                sh = new string();
                start = i;
            }
            sh->push_back(regexp[i]);
            break;
        }
    }
}

static void LevelTwoParse(string &regexp, map<string, State *>symbol_map) {
    // replace *, ?, + and concate the corresponding state with the
    // NFA generated in level one parse
}

static void LevelThreeParse(string &regexp, map<string, State *>symbol_map) {
    // replace | and reorgnize the NFA generated in the first two
    // level parse
}

static void parseRegExp(string &regexp) {
    map<string, State *> symbol_map;
    LevelOneParse(regexp, symbol_map);
    // LevelTwoParse(regexp, symbol_map);
    // LevelThreeParse(regexp, symbol_map);
}

int main() {
    string regexp;
    cin >> regexp;
    parseRegExp(regexp);
    cout << regexp;
    return 0;
}
