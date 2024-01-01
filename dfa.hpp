#pragma once

#include <iostream>
#include "globals.h"
#include "nfa.hpp"
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <stack>

using namespace std;

struct DfaNode {
    int state;
    bool isEnd = false;
    map<char, int> transfers; // 转移
    set<NfaNode*> nfaNodes;
    DfaNode() : state(0) {}
    DfaNode(int state) : state(state) {}
    DfaNode(const DfaNode& node) : state(node.state), transfers(node.transfers), nfaNodes(node.nfaNodes) {}

    void bindNfaNodes(set<NfaNode*> nodes) {
        nfaNodes = nodes;
        for (NfaNode* node : nodes) {
            if (node->isEnd) {
                isEnd = true;
                return;
            }
        }
    }

    bool operator== (const DfaNode& node) {
        if (nfaNodes.size() != node.nfaNodes.size()) return false;
        for (NfaNode* nfaNode : nfaNodes) {
            if (!node.nfaNodes.count(nfaNode)) return false;
        }
        return true;
    }
};

class Dfa {
private:
    void generate() { // 生成DFA图
        NfaGraph nfaGraph = nfa.getGraph();
        set<char> symbols = nfa.getSymbols();
        DfaNode* start = new DfaNode();
        start->bindNfaNodes(epsilonClosure(nfaGraph.start)); // 将NFA节点列表绑定进DFA状态中
        nodes.push_back(start); // 加入初始节点
        for (int i = 0; i < nodes.size(); ++i) {
            DfaNode* cur = nodes[i];
            for (char symbol : symbols) {
                set<NfaNode*> nfaNodesOfSymbol;
                for (NfaNode* next : cur->nfaNodes) {
                    set<NfaNode*> nfaNodesOfNext = forward(next, symbol);
                    nfaNodesOfSymbol.insert(nfaNodesOfNext.begin(), nfaNodesOfNext.end());
                }
                if (nfaNodesOfSymbol.empty()) continue;
                // 判断是否新增
                DfaNode* instance = new DfaNode(nodes.size());
                instance->bindNfaNodes(nfaNodesOfSymbol);
                // 子集构造法
                for (DfaNode* exist : nodes) {
                    if (*exist == *instance) {
                        instance = exist;
                        break;
                    }
                }
                if (instance->state == nodes.size()) { // 需要新增节点
                    nodes.push_back(instance);
                }
                // 加入转移关系
                cur->transfers[symbol] = instance->state;
            }
        }
    }

    set<NfaNode*> forward(NfaNode* source, char symbol) { // 以symbol步进
        set<NfaNode*> result;
        for (NfaNode* next : source->transfers[symbol]) {
            result.insert(next);
            // 同时要加入其EPSILON闭包
            set<NfaNode*> closureOfNext = epsilonClosure(next);
            result.insert(closureOfNext.begin(), closureOfNext.end());
        }
        return result;
    }

    set<NfaNode*> epsilonClosure(NfaNode* source) { // NFA节点的EPSILON闭包
        set<NfaNode*> closure;
        stack<NfaNode*> prepared; // DFS栈
        map<int, int> visited;
        prepared.push(source);
        while (prepared.size()) {
            NfaNode* cur = prepared.top();
            prepared.pop();
            if (visited[cur->state]) continue;
            closure.insert(cur);
            visited[cur->state] = 1;
            for (NfaNode* next : cur->transfers[EPSILON])
                prepared.push(next);
        }
        return closure;
    }

    vector<DfaNode*> nodes;
    Nfa& nfa;
public:
    Dfa(Nfa& nfa) : nfa(nfa) {
        generate();
    }
    Nfa getNfa() {
        return nfa;
    }
    vector<DfaNode*> getNodes() {
        return nodes;
    }
};