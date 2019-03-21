// 匹配类实现
// Created by 张西海 on 2019-03-19.
//

#include <iostream>
#include "automation.h"

Automation::Automation()
{
    root = new Node;
}

Automation::~Automation()
{
    delete root;
    for (vector<Node*>::iterator node = instances.begin(); node != instances.end(); ++node ) {
        delete *node;
    }
}

void Automation::splitWord(const string &word, int &wordLength, vector <string> &characters)
{
    int wordSize = word.size();

    int i = 0;
    while (i < wordSize) {
        int characterSize = 1;

        if (word[i] & 0x80) {
            char character = word[i];
            character <<= 1;
            do {
                character <<= 1;
                ++characterSize;
            } while (character & 0x80);
        }

        string subWord;
        subWord = word.substr(i, characterSize);
        characters.push_back(subWord);

        i += characterSize;
        ++wordLength;
    }
}

Node* Automation::getNext(Node *current, string &character)
{
    map<string, Node*>::iterator item = current->next.find(character);

    if (item != current->next.end()) {
        return item->second;
    }

    if (current == root) {
        return root;
    }

    return 0;
}

void Automation::add(const string &word, zval *values)
{
    int wordLength = 0;
    vector<string> characters;
    splitWord(word, wordLength, characters);

    Node *temp = root;

    int i = 1;
    string pathWord = "";
    // 遍历字符串
    for (vector<string>::iterator character = characters.begin(); character != characters.end(); ++character) {
        pathWord.append(*character);
        map<string, Node*>::iterator item = temp->next.find(*character);

        if (item != temp->next.end()) {   // 中间节点，指针移到下一节点
            temp = item->second;
        } else {                          // 尾节点，添加新的节点
            Node *n = new Node;
            n->word = pathWord;
            n->wordLength = i;
            n->values = values;
            temp->next.insert(make_pair(*character, n));
            temp = n;
        }

        if (character + 1 == characters.end()) {
            temp->matched = true;
            // cout<< "完整词组:" << temp->word << endl;
        }

        ++i;
    }
}

void Automation::build()
{
    queue<Node*> Q;

    // 将第一级节点的所有失配指针都设置为root 并将所有第一级节点加入队列
    for (map<string, Node*>::iterator item = root->next.begin(); item != root->next.end(); ++ item) {
        item->second->fail = root;
        Q.push(item->second);
    }

    instances.clear();
    //instances.push_back(root);

    // 遍历队列
    while (!Q.empty()) {
        Node* temp = Q.front();
        Q.pop();

        // 遍历当前节点的下一节点
        for (map<string, Node*>::iterator item = temp->next.begin(); item != temp->next.end(); ++ item) {
            // 键值
            string character = item->first;

            // 将当前节点加入队列循环
            Q.push(item->second);

            // 设置失配指针 从上一节点的失配指针查起
            Node  *parent = temp->fail;
            while (!getNext(parent, character)) parent = parent->fail;
            item->second->fail = getNext(parent, character);
            if (!getNext(parent, character)) throw 1;
        }

        // 存储尾节点
        if (temp->matched) {
            instances.push_back(temp);
        }
    }
}

bool Automation::match(const string &buf)
{
    int bufLength = 0;
    vector<string> characters;
    splitWord(buf, bufLength, characters);

    Node *temp = root;
    // 遍历字符串
    for (vector<string>::iterator character = characters.begin(); character != characters.end(); ++character) {
        while (!getNext(temp, *character)) {
            temp = temp->fail;
        }
        temp = getNext(temp, *character);

        if (temp->matched) {
            return true;
        }
    }

    return false;
}

void Automation::search(const string &buf, map<string, Node*> &nodes)
{
    int bufLength = 0;
    vector<string> characters;
    splitWord(buf, bufLength, characters);

    int index = 0;

    Node *temp = root;
    // 遍历字符串
    for (vector<string>::iterator character = characters.begin(); character != characters.end(); ++character) {

        while (!getNext(temp, *character)) {
            temp = temp->fail;
        }

        temp = getNext(temp, *character);

        if (temp->matched) { //如果匹配
            map<string, Node*>::iterator nodeFind = nodes.find(temp->word);
            if (nodeFind == nodes.end()) {
                temp->repeats = 1;
                temp->index   = index + 1 - temp->wordLength ;
                nodes.insert(make_pair(temp->word, temp));
            } else {
                nodeFind->second->repeats += 1;
            }
        }
        ++ index;
    }
}