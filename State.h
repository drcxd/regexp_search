#pragma once

#include <string>
#include <iostream>

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
        std::cout << "State: " << this << std::endl;
        std::cout << "need: " << m_need << std::endl;
        std::cout << "out1: " << m_out1 << std::endl;
        std::cout << "out2: " << m_out2 << std::endl;
        std::cout << std::endl;
    }

    virtual bool check(std::string::iterator beg, std::string::iterator end) {
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
    bool check(std::string::iterator beg, std::string::iterator end) override {
        bool res1 = true, res2 = true;
        // if (beg == end) {
        //     return false;
        // }
        res1 = m_out1->check(beg, end);
        if (m_out2) {
            res2 = m_out2->check(beg, end);
        } else {
            std::cout << "[Warning] QuestionState m_out2 is nullptr, use default true\n";
        }
        return res1 || res2;
    }
};

struct AsteriskState : public State {
    bool check(std::string::iterator beg, std::string::iterator end) override {
        bool res1 = true, res2 = true;
        res1 = m_out1->check(beg, end);
        if (m_out2) {
            res2 = m_out2->check(beg, end);
        } else {
            std::cout << "[Warning] AsteriskState m_out2 is nullptr, use default true\n";
        }
        return res1 || res2;
    }
};

struct PlusState : public State {
    bool check(std::string::iterator beg, std::string::iterator end) override {
        bool res1 = true, res2 = true;
        res1 = m_out1->check(beg, end);
        if (m_out2) {
            res2 = m_out2->check(beg, end);
        } else {
            std::cout << "[Warning] PlusState m_out2 is nullptr, use default true\n";
        }
        return res1 || res2;
    }
};

struct AlternationState : public State {
    bool check(std::string::iterator beg, std::string::iterator end) override {
        bool res1 = true, res2 = true;
        res1 = m_out1->check(beg, end);
        res2 = m_out2->check(beg, end);
        return res1 || res2;
    }
};
