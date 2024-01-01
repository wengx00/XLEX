#pragma once

#include <iostream>
#include "globals.h"
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <stack>

using namespace std;

struct NfaNode {
    int state = 0;
    bool isEnd = false;
    map<char, vector<NfaNode*>> transfers;

    NfaNode() : state(0) {}
    NfaNode(int state) : state(state) {}
    NfaNode(const NfaNode& node) : state(node.state), isEnd(node.isEnd), transfers(node.transfers) {}
};

struct NfaGraph {
    NfaNode* start;
    NfaNode* end;

    NfaGraph(NfaNode* start = 0, NfaNode* end = 0) : start(start), end(end) {}
    NfaGraph(const NfaGraph& graph) : start(graph.start), end(graph.end) {}

    // 更新节点状态
    void updateState(int offset) {
        map<int, int> visited;
        stack<NfaNode*> prepared; // 待遍历
        vector<NfaNode*> ready; // 就绪
        prepared.push(start);
        while (prepared.size()) {
            NfaNode* cur = prepared.top();
            prepared.pop();
            visited[cur->state] = 1; // 标记已访问
            ready.push_back(cur);
            for (auto& p : cur->transfers)
                for (NfaNode* next : p.second)
                    if (!visited[next->state])
                        prepared.push(next);
        }
        // 执行更新
        for (NfaNode* node : ready)
            node->state += offset;
    }
};

class Nfa {
private:
    NfaGraph fromSymbol(char symbol) { // 根据Symbol生成Nfa子图
        NfaGraph graph(new NfaNode, new NfaNode(1));
        graph.end->isEnd = true;
        graph.start->transfers[symbol].push_back(graph.end);
        return graph;
    }
    NfaGraph setConcat(NfaGraph& t1, NfaGraph& t2) { // 连接Nfa子图
        NfaGraph graph;
        graph.start = t1.start;
        graph.end = t2.end;
        t1.end->isEnd = false;
        // 更新t2子图的结点编号
        t2.updateState(t1.end->state + 1);
        // 加入EPSILON转移
        t1.end->transfers[EPSILON].push_back(t2.start);
        return graph;
    }
    NfaGraph setUnion(NfaGraph& t1, NfaGraph& t2) { // Nfa子图 或运算
        NfaGraph graph(new NfaNode, new NfaNode);
        graph.end->isEnd = true;
        // 更新子图t1、t2的结点编号
        t1.updateState(1);
        t2.updateState(t1.end->state + 1);
        graph.end->state = t2.end->state + 1;
        // 取消原有的终结结点
        t1.end->isEnd = false;
        t2.end->isEnd = false;
        // 加入EPSILON转移
        graph.start->transfers[EPSILON].push_back(t1.start);
        graph.start->transfers[EPSILON].push_back(t2.start);
        t1.end->transfers[EPSILON].push_back(graph.end);
        t2.end->transfers[EPSILON].push_back(graph.end);
        return graph;
    }
    NfaGraph setClosure(NfaGraph& target) { // Nfa子图闭包
        NfaGraph graph(new NfaNode, new NfaNode);
        graph.end->isEnd = true;
        // 更新子图target的结点编号
        target.updateState(1);
        graph.end->state = target.end->state + 1;
        // 取消原有终结结点
        target.end->isEnd = false;
        // 加入EPSILON转移
        graph.start->transfers[EPSILON].push_back(target.start);
        graph.start->transfers[EPSILON].push_back(graph.end);
        target.end->transfers[EPSILON].push_back(target.start);
        target.end->transfers[EPSILON].push_back(graph.end);
        return graph;
    }

    void setAction(char op, stack<NfaGraph>& subgraphs) {
        NfaGraph result;
        if (op == CLOSURE) {
            // 单目运算
            NfaGraph& target = subgraphs.top();
            subgraphs.pop();
            result = setClosure(target);
        }
        else {
            // 双目运算符
            NfaGraph& t2 = subgraphs.top();
            subgraphs.pop();
            NfaGraph& t1 = subgraphs.top();
            subgraphs.pop();
            if (op == CONCAT) result = setConcat(t1, t2);
            else result = setUnion(t1, t2);
        }
        // 压入结果栈
        subgraphs.push(result);
    }

    // 生成顶层NFA图
    void generate(string input) {
        string prepared = ""; // 预处理后的输入字符串
        stack<char> ops; // 符号栈
        stack<NfaGraph> subgraphs; // 子图栈
        for (int i = 0; i < input.size(); ++i) { // 预处理输入字符串（加入CONCAT字符）
            char id = input[i];
            if (skip(id)) continue; // 跳过无意义字符
            prepared.push_back(id);
            if (i + 1 >= input.size() ||
                input[i + 1] == RBRACKET ||
                input[i] == UNION ||
                input[i + 1] == UNION ||
                input[i] == LBRACKET ||
                input[i + 1] == CLOSURE) continue; // 这些情况不用手动加入联结符号
            // 人为加入表示UNION的字符
            prepared.push_back(CONCAT);
        }
        for (int i = 0; i < prepared.size(); ++i) {
            char id = prepared[i]; // 当前Identifier
            if (id == LBRACKET) { // 左括号
                ops.push(id); // 入符号栈
                continue;
            }
            if (id == RBRACKET) { // 右括号
                while (ops.size()) { // 清空和其最近匹配的左括号内的所有操作
                    char op = ops.top();
                    ops.pop();
                    if (op == LBRACKET) break; // 匹配到左括号，退出
                    setAction(op, subgraphs); // 执行操作
                }
                continue;
            }
            if (reservedSymbol(id)) { // 保留字符（运算符）
                while (ops.size()) { // 清空符号栈里优先级比当前高的运算
                    char op = ops.top();
                    if (privilege(id) > privilege(op)) break; // 优先级没当前OP高
                    ops.pop(); // 优先级较高，出栈并执行
                    setAction(op, subgraphs); // 执行操作
                }
                ops.push(id); // 将当前OP压入栈
                continue;
            }
            // 普通Symbol，生成子图
            NfaGraph subgraph = fromSymbol(id);
            subgraphs.push(subgraph);
            symbols.insert(id); // 加入Symbol统计中
        }
        // 清空符号栈
        while (ops.size()) {
            char op = ops.top();
            ops.pop();
            setAction(op, subgraphs);
        }
        this->graph = subgraphs.top(); // 栈顶就是顶层NFA图
    }

    NfaGraph graph; // 顶层NFA图
    set<char> symbols; // 转移字符
public:
    Nfa(string input) {
        generate(input);
    }

    set<char> getSymbols() {
        return symbols;
    }
    NfaGraph getGraph() {
        return graph;
    }
};