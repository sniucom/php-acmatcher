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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <map>
#include <queue>
#include <vector>
#include <string>
#include <iostream>
#include <time.h>

extern "C"
{
#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_acmatcher.h"
};

ZEND_DECLARE_MODULE_GLOBALS(acmatcher)

/* True global resources - no need for thread safety here */
static int le_acmatcher;

/* {{{ acmatcher_functions[]
 *
 * Every user visible function must have an entry in acmatcher_functions[].
 */
const zend_function_entry acmatcher_functions[] = {
        PHP_ME(acmatcher, has, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
        PHP_ME(acmatcher, build, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
        PHP_ME(acmatcher, add, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
        PHP_ME(acmatcher, match, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
        PHP_ME(acmatcher, search, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
        PHP_ME(acmatcher, print, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
        PHP_ME(acmatcher, destroy, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
        PHP_FE_END	/* Must be the last line in acmatcher_functions[] */
};
/* }}} */

/* {{{ acmatcher_module_entry
 */
zend_module_entry acmatcher_module_entry = {
        STANDARD_MODULE_HEADER,
        "acmatcher",
        acmatcher_functions,
        PHP_MINIT(acmatcher),
        PHP_MSHUTDOWN(acmatcher),
        PHP_RINIT(acmatcher),		/* Replace with NULL if there's nothing to do at request start */
        PHP_RSHUTDOWN(acmatcher),	/* Replace with NULL if there's nothing to do at request end */
        PHP_MINFO(acmatcher),
        PHP_ACMATCHER_VERSION,
        STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_ACMATCHER
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(acmatcher)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("acmatcher.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_acmatcher_globals, acmatcher_globals)
    STD_PHP_INI_ENTRY("acmatcher.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_acmatcher_globals, acmatcher_globals)
PHP_INI_END()
*/
/* }}} */

/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */


/* The previous line is meant for vim and emacs, so it can correctly fold and
   unfold functions in source code. See the corresponding marks just before
   function definition, where the functions purpose is also documented. Please
   follow this convention for the convenience of others editing your code.
*/


/* {{{ php_acmatcher_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_acmatcher_init_globals(zend_acmatcher_globals *acmatcher_globals)
{
	acmatcher_globals->global_value = 0;
	acmatcher_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(acmatcher)
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "ACMatcher", acmatcher_functions);
	acmatcher_ce = zend_register_internal_class(&ce TSRMLS_CC);
	/* If you have INI entries, uncomment these lines
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(acmatcher)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(acmatcher)
{
#if defined(COMPILE_DL_ACMATCHER) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(acmatcher)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(acmatcher)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "ACMatcher support", "enabled");
	php_info_print_table_header(2, "ACMatcher Version", "1.0.0");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */



/**
 * Automation类实现
 */
Automaton::Automaton()
{
	root = new Node;
	instances.push_back(root);
}

Node* Automaton::getNext(Node *current, string &character)
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

void Automaton::splitWord(const string &word, int &wordLength, vector<string> &characters)
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

void Automaton::add(const string &word, zval *values)
{
	int wordLength = 0;
	vector<string> characters;
	splitWord(word, wordLength, characters);


	Node *temp = root;
	int i = 1;
	// 遍历字符串
	for (vector<string>::iterator character = characters.begin(); character != characters.end(); ++character) {
		map<string, Node*>::iterator item = temp->next.find(*character);
		if (item != temp->next.end()) {
			temp = item->second;
		} else {
			Node *n = new Node(word, wordLength, values);
			if (i == wordLength) instances.push_back(n);
			temp->next.insert(make_pair(*character, n));
			temp = n;
			++i;
		}
	}
	temp->matched = true;
}

bool Automaton::match(const string &buf)
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

void Automaton::search(const string &buf, map<string, Node*> &nodes)
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
			map<string, Node*>::iterator nodeFind = nodes.find(*character);
			if (nodeFind == nodes.end()) {
				temp->repeats = 1;
				temp->index   = index + 1 - temp->wordLength ;
				nodes.insert(make_pair(*character, temp));
			} else {
				nodeFind->second->repeats += 1;
			}
		}
		++ index;
	}
}

void Automaton::build()
{
	queue<Node*> Q;

	for (map<string, Node*>::iterator item = root->next.begin(); item != root->next.end(); ++ item) {
		item->second->fail = root;
		Q.push(item->second);
	}

	while (!Q.empty()) {
		Node* temp = Q.front();
		Q.pop();
		for (map<string, Node*>::iterator item = temp->next.begin(); item != temp->next.end(); ++ item) {
			string character = item->first;
			Q.push(item->second);
			Node  *parent = temp->fail;
			while (!getNext(parent, character)) parent = parent->fail;
			item->second->fail = getNext(parent, character);
			if (!getNext(parent, character)) throw 1;
			if (getNext(parent, character)->matched) item->second->matched = true;
		}
	}
}

/**
 * CacheSets类实现
 */
 CacheSets::CacheSets(string inCacheKey, long inSeconds)
 {
 	cacheKey  = inCacheKey;
 	seconds   = inSeconds;
    long now  = time(0);
 	starts    = now;
 	ends      = now + inSeconds;
 }

 void CacheSets::delay(long inSeconds)
 {
 	if (seconds != inSeconds) {
		seconds  = inSeconds;
		ends     = starts + inSeconds;
 	}
 }

/**
 * ACMatcher类实现
 */

/**
 * 创建字典
 */
PHP_METHOD(acmatcher, build)
{
	// 字典名称
	char *name;

	// 字典名称长度
	long nameLength;

	// 字典缓存时间设置
	long  seconds;

	// 回调方法 用于字典赋值
	zend_fcall_info callback;
    zend_fcall_info_cache callbackCache;

	// 取参数值
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,  "slf", &name, &nameLength, &seconds, &callback, &callbackCache) == FAILURE) {
         php_error_docref(NULL TSRMLS_CC, E_NOTICE, "字典 '%s' 初始化失败!", name, nameLength);
         return;
    }

    string dictionaryName = name;

    long now = time(0);

    //获取缓存配置
    CacheSets *cacheSets;

    if (!ACMATCHER_G(cacheSetsList).count(dictionaryName)) { // 如果没有缓存配置则创建
        cacheSets = new CacheSets(dictionaryName, seconds);
        ACMATCHER_G(cacheSetsList).insert(make_pair(dictionaryName, cacheSets));
    } else { // 是否延时
        cacheSets = ACMATCHER_G(cacheSetsList).find(dictionaryName)->second;
        cacheSets->delay(seconds);
    }

    int automatonListCounts = ACMATCHER_G(automatonList).count(dictionaryName);

    // 如果未到缓存时间且缓存存在
    if (now < cacheSets->ends && automatonListCounts)  return;

    // 缓存时间到且缓存存在， 则先删除再重建
    if (automatonListCounts) {
    	ACMATCHER_G(automatonList).erase(dictionaryName);
    }

    // 获取字典
    zval dictionary;
    callback.retval = &dictionary;
    Z_TRY_ADDREF(callback.function_name);
    callback.param_count = 0;
    callback.no_separation = 1;

    // 回调闭包
    int result;
    result = zend_call_function(&callback, &callbackCache);

    if (result == FAILURE) {
    	php_error_docref(NULL TSRMLS_CC, E_NOTICE, "字典 '%s' 初始化失败!", name, nameLength);
        return;
    }


    zend_long index, childIndex;
    zend_string *key, *childKey;
    zval *data, *childData;
    Automaton *automaton = new Automaton;
    HashTable *child;
    HashTable *dictionaryArr = Z_ARRVAL_P(&dictionary);
    // 遍历字典创建Tire树
	ZEND_HASH_FOREACH_KEY_VAL(dictionaryArr, index, key, data) {
        ZVAL_DEREF(data);
        if (Z_TYPE_P(data) == IS_STRING) { // 如果值为字符串
            automaton->add(Z_STRVAL_P(data), data);

        } else if (Z_TYPE_P(data) == IS_ARRAY) { // 如果值为数组
            child = Z_ARRVAL_P(data);
            ZEND_HASH_FOREACH_KEY_VAL(child, childIndex, childKey, childData) {
                ZVAL_DEREF(childData);
                if (Z_TYPE_P(childData) == IS_STRING) { // 如果值为字符串
                    automaton->add(Z_STRVAL_P(childData), data);
                }
            } ZEND_HASH_FOREACH_END();
        }
	} ZEND_HASH_FOREACH_END();

	automaton->build();

	ACMATCHER_G(automatonList).insert(make_pair(name, automaton));

    return;

}


/**
 * 向字典中添加词组
 */
PHP_METHOD(acmatcher, add)
{
    php_printf("预留方法");
}

/**
 * 判断是否匹配
 */
PHP_METHOD(acmatcher, match)
{
    char  *name;
    long   nameLength;
    char  *buf;
    long   bufLength;

    // 取参数值
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,  "ss", &name, &nameLength, &buf, &bufLength) == FAILURE) {
        RETURN_FALSE;
    }

    string dictionaryName = name;
    string bufs = buf;

    if (!ACMATCHER_G(automatonList).count(dictionaryName)) {
        RETURN_FALSE;
    }

    Automaton *automaton;
    automaton = ACMATCHER_G(automatonList).find(dictionaryName)->second;
    if (!automaton->match(bufs)) {
    	RETURN_FALSE;
    }

    RETURN_TRUE;
}

/**
 * 搜索
 */
PHP_METHOD(acmatcher, search)
{
    char  *name;
    long   nameLength;
    char  *buf;
    long   bufLength;

    array_init(return_value);

    // 取参数值
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,  "ss", &name, &nameLength, &buf, &bufLength) == FAILURE) {
        php_error_docref(NULL TSRMLS_CC, E_NOTICE, "参数错误!");
    	return;
    }

    string dictionaryName = name;
    string bufs = buf;

    if (!ACMATCHER_G(automatonList).count(dictionaryName)) {
    	php_printf("未找到字段");
    	return;
    }

    Automaton *automaton;
    automaton = ACMATCHER_G(automatonList).find(dictionaryName)->second;

    map<string, Node*> nodes;
    automaton->search(bufs, nodes);

    int counts = nodes.size();

    // 遍历转数组
	map<string, Node*>::iterator node;
    for ( node = nodes.begin(); node != nodes.end(); ++ node) {
        zval subarray;
        array_init(&subarray);

        string strWord = node->second->word;
        int wordLen = strWord.size();
        char word[wordLen];
        strcpy(word, strWord.c_str());
        char *s = word;
        add_assoc_string(&subarray, "word", s);
        add_assoc_long(&subarray, "word_length", node->second->wordLength);
        add_assoc_long(&subarray, "index", node->second->index);
        add_assoc_long(&subarray, "repeats", node->second->repeats);
        add_assoc_zval(&subarray, "values", node->second->values);

        add_next_index_zval(return_value, &subarray);
    }

    return;

}

/**
 * 是否存在
 */
PHP_METHOD(acmatcher, has)
{
    char  *name;
    long   nameLength;

    // 取参数值
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,  "s", &name, &nameLength) == FAILURE) {
        RETURN_FALSE;
    }

    string dictionaryName = name;

    if (!ACMATCHER_G(automatonList).count(dictionaryName)) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

/**
 * 打印字典
 */
PHP_METHOD(acmatcher, print)
{
	char  *name;
	long   nameLength;

    php_printf("ACMatcher V1.0.0<br><br>");

    // 取参数值
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &nameLength) == FAILURE) {
    	php_printf("字典名称必须为字符串!<br><br>");
    	return;
    }

    php_printf("当前字典: ");
    PHPWRITE(name, nameLength);
    php_printf("<br><br>");

    string dictionaryName = name;

    // 打印字典缓存设置
    int cacheSetCount = ACMATCHER_G(cacheSetsList).count(dictionaryName);
    if (cacheSetCount) {
        php_printf("缓存信息:<br>");
        CacheSets *cacheSet = ACMATCHER_G(cacheSetsList).find(dictionaryName)->second;
        php_printf("缓存开始时间戳: %d <br/>", cacheSet->starts);
        php_printf("缓存时长: %d 秒 <br/>", cacheSet->seconds);
        php_printf("缓存结束时间戳: %d <br/><br>", cacheSet->ends);
    }

    // 打印字典
    int automatonCount = ACMATCHER_G(automatonList).count(dictionaryName);
    if (automatonCount) {
        php_printf("字典信息:<br>");
        int n = 0;
        Automaton *automaton;
        automaton = ACMATCHER_G(automatonList).find(dictionaryName)->second;
        for (vector<Node*>::iterator node = automaton->instances.begin(); node != automaton->instances.end(); ++node ) {
        	if ((*node)->word != "") {
                php_printf("%d. %s <br>", ++n, (*node)->word.c_str());
        	}
        }
    }
}

/**
 * 删除缓存
 */
PHP_METHOD(acmatcher, destroy)
{
    char  *name;
    long   nameLength;

    string dictionaryName = name;

    // 取参数值
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &nameLength) == FAILURE) {
        RETURN_FALSE;
    }

    if (ACMATCHER_G(automatonList).count(dictionaryName)) {
        ACMATCHER_G(automatonList).erase(dictionaryName);
    }

    if (ACMATCHER_G(automatonList).count(dictionaryName)) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
