// 字典树
// Created by 张西海 on 2019-03-19.
//

#ifndef ACMATCHER_NODE_H
#define ACMATCHER_NODE_H

#include <map>
#include <string>
#include <iostream>

using namespace std;

extern "C"
{
    #include "php.h"
    #include "php_ini.h"
    #include "ext/standard/info.h"
}

class Node
{

public:
    /**
     * 字
     */
    string word;

    /**
     * 下一节点
     */
    map<string, Node*> next;

    /**
     * 失配指针
     */
    Node *fail;

    /**
     * 是否匹配，即是否为根节点
     */
    bool matched;

    /**
     * 命中的词频
     */
    int repeats;

    /**
     * 字长
     * 注意: 非字符长
     */
    int wordLength;

    /**
     * 第一次出现位置
     */
    int index;

    /**
     * 值
     */
    zval *values;

public:
    Node() : word(""), fail(0), matched(false), repeats(0), wordLength(0), index(-1) {};

};


#endif //ACMATCHER_NODE_H
