#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <sstream>

#define DEBUG (cout << "[DEBUG] ")

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

struct State {
    State(State *out1, State *out2)
        : m_out1(out1)
        , m_out2(out2) { }

    State()
        : m_out1(nullptr)
        , m_out2(nullptr) { }
    State *m_out1;
    State *m_out2;
    char m_need = 0;

    void print() {
        cout << "State: " << this << endl;
        cout << "need: " << m_need << endl;
        cout << "out1: " << m_out1 << endl;
        cout << "out2: " << m_out2 << endl;
        cout << endl;
    }

    virtual bool check(string::iterator beg, string::iterator end) {
        bool res1 = true, res2= true;
        if (beg == end) {
            return false;
        }
        if (*beg != m_need) {
            return false;
        }
        if (!m_out1 && !m_out2) {
            return true;
        }
        res1 = m_out1->check(beg + 1, end);
        if (m_out2) {
            res2 = m_out2->check(beg + 1, end);
        }
        if (m_out2) {
            return res1 || res2;
        } else {
            return res1;
        }
    }
};

struct QuestionState : public State {
    bool check(string::iterator beg, string::iterator end) override {
        bool res1 = true, res2 = true;
        if (beg == end) {
            return false;
        }
        res1 = m_out1->check(beg, end);
        res2 = m_out2->check(beg, end);
        return res1 || res2;
    }
};

static void printStateChain(State *s) {
    if (!s) {
        return;
    }
    s->print();
    if (s->m_out1)
        printStateChain(s->m_out1);
    if (s->m_out2)
        printStateChain(s->m_out2);
}

static void buildPlainChain(vector<State *> &cache, string &result,
    map<string, State *>&symbol_map, string *sh, int &label_count) {
    cache.clear();
    for (auto j : *sh) {
        State *pState = new State();
        pState->m_need = j;
        cache.push_back(pState);
    }
    if (cache.size() > 1) {
        for (std::size_t s = 1; s < cache.size(); ++s) {
            cache[s - 1]->m_out1 = cache[s];
        }
    }
    stringstream ss;
    ss << "@" << label_count << "@";
    result.append(ss.str());
    symbol_map.insert(std::make_pair(ss.str(), cache.front()));
    ss.str("");
    ++label_count;
}

State *LevelOneParse(string &regexp, map<string, State *>&symbol_map) {
    string *sh = nullptr;
    string result;
    vector<State *> cache;
    for (std::size_t i = 0; i < regexp.size(); ++i) {
        switch (regexp[i]) {
        case '*':
        case '+':
        case '?':
        case '|': {
            if (!sh) {
                result.push_back(regexp[i]);
                break;
            }
            buildPlainChain(cache, result, symbol_map, sh, label_count);
            delete sh;
            sh = nullptr;
            result.push_back(regexp[i]);
            break;
        }
        default:
            if (!sh) {
                sh = new string();
            }
            sh->push_back(regexp[i]);
            break;
        }
    }
    if (sh) {
        buildPlainChain(cache, result, symbol_map, sh, label_count);
        delete sh;
        sh = nullptr;
    }
    // DEBUG << "result: " << result << endl;
    // for (auto i : symbol_map) {
    //     DEBUG << "label " << i.first << endl;
    //     printStateChain(i.second);
    // }
    regexp = result;
    return symbol_map.begin()->second;
}

void LevelTwoParse(string &regexp, map<string, State *>&symbol_map) {
    // replace *, ?, + and concate the corresponding state with the
    // NFA generated in level one parse
    string label;
    string prev_label;
    bool in_label = false;
    // vector<State *> qs_cache;
    for (std::size_t i = 0; i < regexp.size(); ++i) {
        if (regexp[i] == '@' && !in_label) {
            in_label = true;
            prev_label = label;
            label.clear();
        } else if (regexp[i] == '@' && in_label) {
            label.push_back(regexp[i]);
            in_label = false;
            DEBUG << "label: " << label << endl;
            // QuestionState *qs = new QuestionState();
            // qs.m_out1 = symbol_map.at(label);
            // qs.m_out2 = prev_label.empty() ? new State() :
            //     symbol_map.at(prev_label);
        }
        if (in_label) {
            label.push_back(regexp[i]);
        }
        switch (regexp[i]) {
        case '?': {
            QuestionState *qs = new QuestionState();
            State *s1 = prev_label.empty() ? symbol_map.at(label) :
                symbol_map.at(prev_label);
            State *s2 = prev_label.empty() ? new State() :
                symbol_map.at(label);
            qs->m_out1 = s1;
            qs->m_out2 = s2;
            if (!prev_label.empty()) {
                while (s1->m_out1) {
                    s1 = s1->m_out1;
                }
                s1->m_out1 = s2;
            }
            // qs_cache.push_back(qs);
            printStateChain(qs);
            break;
        }
        default: {
            DEBUG << "Error!\n";
            break;
        }
        }
    }
    // if (cache.size() > 1) {
    //     for (std::size_t i = 0; i < cache.size() - 1; ++i) {
    //         State *s1 = nullptr, *s2 = nullptr;
    //         s1 = qs->m_out1;
    //         while (s1->m_out1) {
    //             s1 = s1->m_out1;
    //         }
    //         s1->m_out1 = cache[i + 1];
    //         s2 = qs->m_out2;
    //     }    
    // }    
}

void LevelThreeParse(string &regexp, map<string, State *>symbol_map) {
    // replace | and reorgnize the NFA generated in the first two
    // level parse
}

State *parseRegExp(string &regexp) {
    State *start = nullptr;
    map<string, State *> symbol_map;
    start = LevelOneParse(regexp, symbol_map);
    LevelTwoParse(regexp, symbol_map);
    // LevelThreeParse(regexp, symbol_map);
    return start;
}

bool walk(string &search, State *start) {
    for (auto it = search.begin(), end = search.end();
         it != end; ++it) {
        if (start->check(it, end)) {
            return true;
        }
    }
    return false;
}

int main() {
    string regexp;
    cout << "input your regular expression: ";
    cin >> regexp;
    State *start = parseRegExp(regexp);
    // Walk Part
    // cout << "input your search string: ";
    // string search;
    // cin >> search;
    // cout << (walk(search, start) ? "match" : "no match") << endl;
    return 0;
}
