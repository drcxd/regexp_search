#include "State.h"

#include <string>
#include <iostream>
#include <vector>
#include <deque>
#include <map>
#include <set>
#include <sstream>

#define DEBUG (cout << "[DEBUG] ")

using std::vector;
using std::string;
using std::cin;
using std::cout;
using std::endl;
using std::map;
using std::stringstream;
using std::set;
using std::deque;

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

// global label counter
int label_count = 0;

// Utilitiy functions
static void printStateChain(State *s);
static void buildPlainChain(vector<State *> &cache, string &result,
    map<string, State *>&sumbol_map, string *sh, int &label_count);
static void buildQuantityChain(vector<string> &cat_labels,
    map<string, State*> &symbol_map, string &result,
    set<string> labels);
static string getFirstLabel(const string &result);

// Main work functions
State *LevelOneParse(string &regexp, map<string, State *>&symbol_map);
State *LevelTwoParse(string &regexp, map<string, State *>&symbol_map);
void LevelThreeParse(string &regexp, map<string, State *>&symbol_map);
State *parseRegExp(string &regexp);
bool walk(string &search, State *start);

int main() {
    string regexp;
    cout << "input your regular expression: ";
    cin >> regexp;
    State *start = parseRegExp(regexp);
    // Walk Part
    cout << "input your search string: ";
    string search;
    cin >> search;
    cout << (walk(search, start) ? "match" : "no match") << endl;
    return 0;
}

void printStateChain(State *s) {
    if (!s) {
        return;
    }
    s->print();
    if (s->m_out1)
        printStateChain(s->m_out1);
    if (s->m_out2)
        printStateChain(s->m_out2);
}

void buildPlainChain(vector<State *> &cache, string &result,
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
    ss << "@" << "1_" << label_count << "@";
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

State *LevelTwoParse(string &regexp, map<string, State *>&symbol_map) {
    // replace *, ?, + and concate the corresponding state with the
    // NFA generated in level one parse
    DEBUG << "level two regexp: " << regexp << endl;
    bool in_label = false;
    set<string> labels;
    vector<State *> cache;
    string label;
    string result;
    for (std::size_t i = 0; i < regexp.size(); ++i) {
        result.push_back(regexp[i]);
        if (regexp[i] == '@' && !in_label) {
            in_label = true;
            label.clear();
        }
        if (regexp[i] == '@' && in_label && !label.empty()) {
            label.push_back(regexp[i]);
            in_label = false;
            labels.insert(label);
        }
        if (in_label) {
            label.push_back(regexp[i]);
        }
        switch (regexp[i]) {
        default: {
            // DEBUG << "scan plain character: " << regexp[i] << endl;
            break;
        }
        case '?': {
            result.pop_back();
            QuestionState *qs = new QuestionState();
            qs->m_out1 = symbol_map.at(label);
            stringstream ss;
            ss << "@2_" << label_count << "@";
            ++label_count;
            string new_label = ss.str();
            symbol_map.erase(label);
            symbol_map.insert(std::make_pair(new_label, qs));
            labels.erase(label);
            labels.insert(new_label);
            size_t old_label_pos = result.find(label);
            if (old_label_pos == std::string::npos) {
                DEBUG << "Fatal Error: Can't find label: " << label
                      << "in result: " << result << endl;
                DEBUG << "Program Terminated!\n";
                exit(0);
            }
            result.replace(old_label_pos, label.length(), new_label);
            break;
        }
        case '|': {
            break;
        }
        }
    }
    DEBUG << "After level 2 parse the result is " << result << endl;
    // parse it again and concatenate any adjacent labels
    regexp = result;
    result.clear();
    in_label = false;
    vector<string> cat_labels;
    for (std::size_t i = 0; i < regexp.size(); ++i) {
        result.push_back(regexp[i]);
        if (regexp[i] == '@' && !in_label) {
            in_label = true;
            label.clear();
        }
        if (regexp[i] == '@' && in_label && !label.empty()) {
            label.push_back(regexp[i]);
            in_label = false;
            labels.insert(label);
            cat_labels.push_back(label);
        }
        if (in_label) {
            label.push_back(regexp[i]);
        }
        switch (regexp[i]) {
        default: {
            break;
        }
        case '|': {
            if (cat_labels.size() < 2) {
                break;
            }
            buildQuantityChain(cat_labels, symbol_map, result, labels);
            break;
        }
        }
    }
    if (cat_labels.size() > 1) {
        buildQuantityChain(cat_labels, symbol_map, result, labels);
    }
    DEBUG << "After level 2 reparse result is " << result << endl;
    State *start = symbol_map.at(getFirstLabel(result));
    return start;
}

void LevelThreeParse(string &regexp, map<string, State *>&symbol_map) {
    // replace | and reorgnize the NFA generated in the first two
    // level parse
}

State *parseRegExp(string &regexp) {
    State *start = nullptr;
    map<string, State *> symbol_map;
    start = LevelOneParse(regexp, symbol_map);
    start = LevelTwoParse(regexp, symbol_map);
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

void buildQuantityChain(vector<string> &cat_labels, map<string, State*> &symbol_map,
    string &result, set<string> labels) {
    State *head = nullptr;
    for (std::size_t i = 0; i < cat_labels.size() - 1; ++i) {
        State *s1 = symbol_map.at(cat_labels[i]);
        State *s2 = symbol_map.at(cat_labels[i + 1]);
        if (i == 0) {
            head = s1;
        }
        if (QuestionState *qs = dynamic_cast<QuestionState *>(s1)) {
            qs->m_out2 = s2;
            while (s1->m_out1) {
                s1 = s1->m_out1;
            }
            s1->m_out1 = s2;
        } else {
            s1->m_out1 = s2;
        }
    }
    stringstream ss;
    ss << "@2_" << label_count << "@";
    ++label_count;
    string new_label = ss.str();
    string old_label = cat_labels.front();
    std::size_t old_pos = result.find(old_label);
    if (old_pos == std::string::npos) {
        DEBUG << "Fatal Error: Can't find label: " << old_label
              << "in result: " << result << endl;
        DEBUG << "Program Terminated!\n";
        exit(0);
    }
    result.replace(old_pos, old_label.length(), new_label);
    symbol_map.erase(old_label);
    symbol_map.insert(std::make_pair(new_label, head));
    labels.erase(old_label);
    labels.insert(new_label);
    for (std::size_t i = 1; i < cat_labels.size(); ++i) {
        old_label = cat_labels[i];
        old_pos = result.find(old_label);
        if (old_pos == std::string::npos) {
            DEBUG << "Fatal Error: Can't find label: " << old_label
                  << "in result: " << result << endl;
            DEBUG << "Program Terminated!\n";
            exit(0);
        }
        result.replace(old_pos, old_label.length(), "");
        symbol_map.erase(old_label);
        labels.erase(old_label);
    }
    cat_labels.clear();
}

string getFirstLabel(const string &result) {
    string label;
    bool in_label = false;
    for (std::size_t i = 0; i < result.size(); ++i) {
        if (result[i] == '@' && !in_label) {
            in_label = true;
        }
        if (result[i] == '@' && in_label && !label.empty()) {
            label.push_back(result[i]);
            in_label = false;
            break;
        }
        if (in_label) {
            label.push_back(result[i]);
        }
    }
    return label;
}
