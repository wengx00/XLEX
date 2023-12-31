#pragma once

#include <iostream>
#include "globals.h"
#include "dfa.hpp"
#include <cstring>
#include <string>
#include <queue>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <stack>

struct MDfaNode {
    int state;
    bool isEnd;
    set<DfaNode*> dfaNodes; // 持有的DFA结点
    map<char, int> transfer;

    void bindDfaNodes(set<DfaNode*> dfaNodes) {
        this->dfaNodes = dfaNodes;
        for (DfaNode* node : dfaNodes) {
            if (node->isEnd) {
                isEnd = true;
                break;
            }
        }
    }

    MDfaNode(int state) : state(state), isEnd(false) {}

    bool operator== (const MDfaNode& node) {
        if (node.dfaNodes.size() != dfaNodes.size()) return false;
        for (DfaNode* dfaNode : node.dfaNodes)
            if (!dfaNodes.count(dfaNode)) return false;
        return true;
    }
};

class MDfa {
private:
    Dfa& dfa;
    vector<MDfaNode*> nodes;

    void minimize() { // 最小化
        set<char> symbols = dfa.getNfa().getSymbols();
        set<DfaNode*> left, right; // 两个拆分
        vector<set<DfaNode*>> completed; // 已完成拆分
        vector<set<DfaNode*>> prepared; // 待拆分
        map<int, set<DfaNode*>> destination; // 转移目的地，做暂存使用
        // 根据是否终结节点初始化
        for (DfaNode* node : dfa.getNodes()) {
            if (node->isEnd) right.insert(node);
            else left.insert(node);
        }
        prepared.push_back(left);
        prepared.push_back(right);
        while (prepared.size()) {
            for (const char symbol : symbols) {
                destination.clear();
                set<DfaNode*> cur = prepared.front();
                if (cur.empty()) continue; // 空集合
                if (cur.size() == 1) {
                    completed.push_back(cur); // 长度为1，无需拆分
                    prepared.erase(prepared.begin(), prepared.begin() + 1);
                    continue;
                }
                for (DfaNode* node : cur) {
                    if (!node->transfers.count(symbol)) { // 不存在该转移
                        destination[-1].insert(node);
                        continue;
                    }
                    int target = node->transfers[symbol]; // 下一个DFA状态
                    for (int i = 0; i < prepared.size(); ++i) {
                        for (DfaNode* state : prepared[i]) {
                            if (state->state == target) { // 移进的目标是这个集合
                                destination[i].insert(node);
                            }
                        }
                    }
                    for (int i = 0; i < completed.size(); ++i) {
                        for (DfaNode* state : completed[i]) {
                            if (state->state == target) { // 移进的目标是这个集合
                                destination[prepared.size() + i].insert(node);
                            }
                        }
                    }
                }
                prepared.erase(prepared.begin(), prepared.begin() + 1); // 把cur出队
                // 找内奸
                if (destination.size() > 1) { // 有内奸
                    for (auto& p : destination) {
                        prepared.push_back(p.second);
                    }
                    continue;
                }
                // 没有内奸
                completed.push_back(cur);
            }
        }
        // 根据划分结果生成MDFA结点
        MDfaNode* startNode = new MDfaNode(0);
        set<DfaNode*> divideOfStartNode;
        for (set<DfaNode*> divide : completed) {
            for (DfaNode* node : divide) {
                if (node->state == 0) {
                    divideOfStartNode = divide;
                    break;
                }
            }
            if (divideOfStartNode.size()) break; // 已经找到起始划分
        }
        startNode->bindDfaNodes(divideOfStartNode); // 绑定划分
        nodes.push_back(startNode);
        for (int i = 0; i < nodes.size(); ++i) { // 生成MDFA结点
            MDfaNode* cur = nodes[i];
            for (const char symbol : symbols) {
                set<int> next = forward(cur->dfaNodes, symbol); // 获取转移目标
                if (next.empty()) continue;
                for (set<DfaNode*> divide : completed) { // 寻找和目标等价的划分
                    bool matched = false;
                    for (DfaNode* node : divide) {
                        if (next.count(node->state)) {
                            matched = true;
                            break;
                        }
                    }
                    if (!matched) continue; // 不匹配
                    MDfaNode* instance = new MDfaNode(nodes.size());
                    instance->bindDfaNodes(divide);
                    for (MDfaNode* exist : nodes) { // 寻找是否已经存在该结点
                        if (*exist == *instance) {
                            instance = exist;
                            break;
                        }
                    }
                    if (instance->state == nodes.size()) // 不存在的结点
                        nodes.push_back(instance);
                    cur->transfer[symbol] = instance->state;
                    break;
                }
            }
        }
    }

    set<int> forward(set<DfaNode*> source, char symbol) {
        set<int> result;
        for (DfaNode* node : source) {
            if (!node->transfers.count(symbol)) continue; // 当前DFA结点上不存在该转移关系
            result.insert(node->transfers[symbol]);
        }
        return result;
    }

public:
    MDfa(Dfa& dfa) : dfa(dfa) {
        minimize();
    };

    vector<MDfaNode*> getNodes() {
        return nodes;
    }
};