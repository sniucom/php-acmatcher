/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2018 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: 神奇的胖子                                                             |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_ACMATCHER_H
#define PHP_ACMATCHER_H

extern zend_module_entry acmatcher_module_entry;
#define phpext_acmatcher_ptr &acmatcher_module_entry

#define PHP_ACMATCHER_VERSION "1.0.0"

#ifdef PHP_WIN32
#	define PHP_ACMATCHER_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_ACMATCHER_API __attribute__ ((visibility("default")))
#else
#	define PHP_ACMATCHER_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#include <map>
#include <queue>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

PHP_MINIT_FUNCTION(acmatcher);
PHP_MSHUTDOWN_FUNCTION(acmatcher);
PHP_RINIT_FUNCTION(acmatcher);
PHP_RSHUTDOWN_FUNCTION(acmatcher);
PHP_MINFO_FUNCTION(acmatcher);

static zend_class_entry *acmatcher_ce;
PHP_METHOD(acmatcher, has);
PHP_METHOD(acmatcher, build);
PHP_METHOD(acmatcher, add);
PHP_METHOD(acmatcher, match);
PHP_METHOD(acmatcher, search);
PHP_METHOD(acmatcher, print);
PHP_METHOD(acmatcher, destroy);

class CacheSets
{
public:
    string cacheKey;
    long starts;
    long seconds;
    long ends;

public:
    CacheSets(string inCacheKey, long inSeconds);
    void delay(long inSeconds);
};

/**
 * 节点类定义
 */
class Node
{

public:
    /**
     * 分词
     */
    string word;

    /**
     * 分词长度
     */
    int wordLength;

    /**
     * 储值
     */
    zval *values;

    /**
     * 词频
     */
    int repeats;

    /**
     * 第一次出现位置，非字符位置
     */
    int index;

    /**
     * 是否匹配
     */
    bool matched;

    /**
     * 下一节点
     */
    map<string, Node*> next;

    /**
     * 失配指针
     */
    Node *fail;

public:
    Node() : word(""), wordLength(0), repeats(0), index(-1), matched(false), fail(0) {};

    Node(string word, int wordLength, zval *values) : word(word), wordLength(wordLength), values(values), repeats(0), index(-1), matched(
            false), fail(0) {};

};

/**
 * 搜索类定义
 */
class Automaton
{

public:
    /**
     * 根节点
     */
    Node *root;

    /**
     * 节点向量
     */
    vector<Node*> instances;

public:
    Automaton();

    /**
     * 获取当前节点的下一节点
     *
     * @param current
     * @param character
     * @return
     */
    Node* getNext(Node *current, string &character);

    /**
     * 拆分分词
     *
     * @param word
     * @param wordLength
     * @param characters
     */
    void splitWord(const string &word, int &wordLength, vector<string> &characters);

    /**
     * 添加节点
     *
     * @param word
     * @param values
     */
    void add(const string &word, zval *values);

    /**
     * 是否匹配
     *
     * @param buf
     * @return
     */
    bool match(const string &buf);

    /**
     * 查找匹配的节点
     *
     * @param buf
     * @return
     */
    void search(const string &buf, map<string, Node*> &nodes);

    /**
     * 设置失配节点
     */
    void build();

};

ZEND_BEGIN_MODULE_GLOBALS(acmatcher)
    std::map<string, Automaton*> automatonList;
    std::map<string, CacheSets*> cacheSetsList;
ZEND_END_MODULE_GLOBALS(acmatcher)


#define ACMATCHER_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(acmatcher, v)

#if defined(ZTS) && defined(COMPILE_DL_ACMATCHER)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#endif	/* PHP_ACMATCHER_H */
