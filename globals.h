#pragma once

// EPSILON符号
#define EPSILON '@'
// 连接
#define CONCAT '\n'
// 或
#define UNION '|'
// 闭包
#define CLOSURE '*'
// 左括号
#define LBRACKET '('
// 右括号
#define RBRACKET ')'

// 无意义字符，需跳过
#define SKIP_COUNT 3
const char SKIP[SKIP_COUNT] = { ' ', '@', '\n' };

// 保留字
#define RESERVED_COUNT 6
const char RESERVED[RESERVED_COUNT] = {
    EPSILON, CONCAT, UNION, CLOSURE, LBRACKET, RBRACKET
};

int indexOf(const char *source, int count, char target) {
    for (int i = 0; i < count; ++i)
        if (source[i] == target) return i;
    return -1;
}

// 优先级
int privilege(char target) {
    switch (target)
    {
    case CLOSURE:
        return 3;
    case CONCAT:
        return 2;
    case UNION:
        return 1;
    default:
        return 0;
    }
}

// 是否特殊字符
bool reservedSymbol(char target) {
    return indexOf(RESERVED, RESERVED_COUNT, target) > -1;
}

// 是否无意义字符
bool skip(char target) {
    return indexOf(SKIP, SKIP_COUNT, target) > -1;
}