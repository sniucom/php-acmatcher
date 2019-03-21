// 匹配类定义
// Created by 张西海 on 2019-03-19.
//

#ifndef ACMATCHER_AUTOMATION_H
#define ACMATCHER_AUTOMATION_H

#include <map>
#include <queue>
#include <vector>
#include <string>
#include <iostream>
#include "node.h"

extern "C"
{
    #include "php.h"
    #include "php_ini.h"
    #include "ext/standard/info.h"
}

using namespace std;

class Automation
{

public:
    /**
     * 根节点
     */
    Node *root;

    /**
     * 用于存储所所有尾节点
     */
    vector<Node*> instances;

public:
    Automation();
    ~Automation();

    /**
     * 将字符串拆分成数组
     *
     * @param current
     * @param wordLength
     * @param characters
     */
    void splitWord(const string &word, int &wordLength, vector<string> &characters);

    /**
     * 获取当前节点的下一节点
     *
     * @param current    当前节点
     * @param character  键值
     * @return
     */
    Node* getNext(Node* current, string &character);

    /**
     * 添加字典
     *
     * @param word
     */
    void add(const string &word, zval *values);

    /**
     * 创建字典
     */
    void build();

    /**
     * 匹配 词典中任何一个词汇匹配成功都会返回
     *
     * @param buf
     * @return
     */
    bool match(const string &buf);

    /**
     * 返回所有命中的节点信息
     *
     * @param buf
     */
    void search(const string &buf, map<string, Node*> &nodes);

};


#endif //ACMATCHER_AUTOMATION_H
