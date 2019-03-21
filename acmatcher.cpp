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
#include "node.h"
#include "automation.h"
#include "cacheable.h"

extern "C"
{
    #include "php.h"
    #include "php_ini.h"
    #include "ext/standard/info.h"
    #include "php_acmatcher.h"
};

ZEND_DECLARE_MODULE_GLOBALS(acmatcher)

/**
 * True global resources - no need for thread safety here
 */
static int le_acmatcher;

/**
 * acmatcher_functions[]
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

/**
 * acmatcher_module_entry
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

#ifdef COMPILE_DL_ACMATCHER
    #ifdef ZTS
        ZEND_TSRMLS_CACHE_DEFINE()
    #endif
    ZEND_GET_MODULE(acmatcher)
#endif


/**
 * PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(acmatcher)
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "ACMatcher", acmatcher_functions);
	acmatcher_ce = zend_register_internal_class(&ce TSRMLS_CC);
	return SUCCESS;
}

/**
 * PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(acmatcher)
{
	return SUCCESS;
}


/**
 * PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(acmatcher)
{
    #if defined(COMPILE_DL_ACMATCHER) && defined(ZTS)
	    ZEND_TSRMLS_CACHE_UPDATE();
    #endif
	return SUCCESS;
}

/**
 * PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(acmatcher)
{
	return SUCCESS;
}


/**
 * PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(acmatcher)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "ACMatcher support", "enabled");
	php_info_print_table_header(2, "ACMatcher Version", "2.0.0");
	php_info_print_table_end();
}


/**
 * 创建字典
 * @param string  name        字典名称
 * @param integer seconds     存储时长
 * @param array   dictionary  字典数组
 */
PHP_METHOD(acmatcher, build)
{
    // 字典名称
    char *name;

    // 字典名称长度
    long nameLength;

    // 字典缓存时间设置
    long seconds;

    // 回调方法 用于字典赋值
    zend_fcall_info callback;
    zend_fcall_info_cache callbackCache;


    // 取参数值
    if (zend_parse_parameters (ZEND_NUM_ARGS() TSRMLS_CC, "slf", &name, &nameLength, &seconds, &callback, &callbackCache) == FAILURE) {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "字典 '%s' 初始化失败!", name, nameLength);
        return;
    }


    string dictionaryName = name;

    long now = time(0);

    //获取缓存配置
    Cacheable *cacheable;

    if (ACMATCHER_G(cacheableList).find(dictionaryName) == ACMATCHER_G(cacheableList).end()) { // 如果没有缓存配置则创建
        cacheable = new Cacheable(dictionaryName, seconds);
        ACMATCHER_G(cacheableList).insert(make_pair(dictionaryName, cacheable));
    } else { // 是否延时
        cacheable = ACMATCHER_G(cacheableList).find(dictionaryName)->second;
       cacheable->delay(seconds);
    }

    int automationListExist= ACMATCHER_G(automationList).find(dictionaryName) != ACMATCHER_G(automationList).end();

    // 如果未到缓存时间且缓存存在
    if (now<cacheable->ends && automationListExist)  return;

    // 缓存时间到且缓存存在， 则先删除再重建
    if (automationListExist) {
        ACMATCHER_G(automationList).erase(ACMATCHER_G(automationList).find(dictionaryName));
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
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "字典 '%s' 初始化失败!", name, nameLength);
        return;
    }


    zend_long index, childIndex;
    zend_string *key, *childKey;
    zval *data, *childData;
    Automation *automation = new Automation;
    HashTable *child;
    HashTable *dictionaryArr = Z_ARRVAL_P(&dictionary);

    // 遍历字典创建Tire树
    ZEND_HASH_FOREACH_KEY_VAL(dictionaryArr, index, key, data) {
        ZVAL_DEREF(data);
        if (Z_TYPE_P(data) == IS_STRING) { // 如果值为字符串
            automation->add(Z_STRVAL_P(data), data);
        } else if (Z_TYPE_P(data) == IS_ARRAY) { // 如果值为数组
            child = Z_ARRVAL_P(data);
            ZEND_HASH_FOREACH_KEY_VAL(child, childIndex, childKey, childData) {
                ZVAL_DEREF(childData);
                if (Z_TYPE_P(childData) == IS_STRING) { // 如果值为字符串
                    automation->add(Z_STRVAL_P(childData), data);
                }
            }ZEND_HASH_FOREACH_END();
        }
    }ZEND_HASH_FOREACH_END();

    automation->build();

    ACMATCHER_G(automationList).insert(make_pair(name, automation));

    return;
}


/**
 * 向字典中添加词组
 */
PHP_METHOD(acmatcher, add)
{
    // 字典名称
    char *name;

    // 字典名称长度
    long nameLength;

    // 回调方法 用于字典赋值
    zend_fcall_info callback;
    zend_fcall_info_cache callbackCache;

    // 取参数值
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,  "sf", &name, &nameLength, &callback, &callbackCache) == FAILURE) {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "向字典 '%s' 添加新关键词失败!", name, nameLength);
        return;
    }

    string dictionaryName = name;

    // 获取字典
    zval dictionary;
    callback.retval = &dictionary;
    Z_TRY_ADDREF(callback.function_name);
    callback.param_count = 0;
    callback.no_separation = 1;

    // 回调闭包
    int result;
    result = zend_call_function(&callback, &callbackCache);


    // 字典不存在
    if (ACMATCHER_G(automationList).find(dictionaryName) == ACMATCHER_G(automationList).end()) {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "字典 '%s' 尚未创建,无法添加新关键词!", name, nameLength);
    }

    zend_long index, childIndex;
    zend_string *key, *childKey;
    zval *data, *childData;
    Automation *automation = ACMATCHER_G(automationList).find(dictionaryName)->second;
    HashTable *child;
    HashTable *dictionaryArr = Z_ARRVAL_P(&dictionary);

    // 遍历字典创建Tire树
    ZEND_HASH_FOREACH_KEY_VAL(dictionaryArr, index, key, data) {
        ZVAL_DEREF(data);
        if (Z_TYPE_P(data) == IS_STRING) { // 如果值为字符串
            automation->add(Z_STRVAL_P(data), data);
        } else if (Z_TYPE_P(data) == IS_ARRAY) { // 如果值为数组
            child = Z_ARRVAL_P(data);
            ZEND_HASH_FOREACH_KEY_VAL(child, childIndex, childKey, childData) {
                ZVAL_DEREF(childData);
                if (Z_TYPE_P(childData) == IS_STRING) { // 如果值为字符串
                    automation->add(Z_STRVAL_P(childData), data);
                }
            } ZEND_HASH_FOREACH_END();
        }
    } ZEND_HASH_FOREACH_END();

    automation->build();

    ACMATCHER_G(automationList).insert(make_pair(name, automation));

    return;
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

    if (ACMATCHER_G(automationList).find(dictionaryName) == ACMATCHER_G(automationList).end()) {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "字典 '%s' 尚未创建!", name, nameLength);
    }

    Automation *automation;
    automation = ACMATCHER_G(automationList).find(dictionaryName)->second;
    if (!automation->match(bufs)) {
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

    if (ACMATCHER_G(automationList).find(dictionaryName) == ACMATCHER_G(automationList).end()) {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "字典 '%s' 尚未创建!", name, nameLength);
    }

    Automation *automation;
    automation = ACMATCHER_G(automationList).find(dictionaryName)->second;

    map<string, Node*> nodes;
    automation->search(bufs, nodes);

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

    if (ACMATCHER_G(automationList).find(dictionaryName) != ACMATCHER_G(automationList).end()) {
        RETURN_TRUE;
    }

    RETURN_FALSE;
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
    int cacheableCount = ACMATCHER_G(cacheableList).count(dictionaryName);
    if (cacheableCount) {
        php_printf("缓存信息:<br>");
        Cacheable *cacheable = ACMATCHER_G(cacheableList).find(dictionaryName)->second;
        php_printf("缓存开始时间戳: %d <br/>", cacheable->starts);
        php_printf("缓存时长: %d 秒 <br/>", cacheable->seconds);
        php_printf("缓存结束时间戳: %d <br/><br>", cacheable->ends);
    }

    // 打印字典
    int automationCount = ACMATCHER_G(automationList).count(dictionaryName);
    if (automationCount) {
        php_printf("字典信息:<br>");
        int n = 0;
        Automation *automation;
        automation = ACMATCHER_G(automationList).find(dictionaryName)->second;
        for (vector<Node*>::iterator node = automation->instances.begin(); node != automation->instances.end(); ++node ) {
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

    // 取参数值
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &nameLength) == FAILURE) {
        RETURN_FALSE;
    }

    string dictionaryName = name;

    if (ACMATCHER_G(automationList).find(dictionaryName) != ACMATCHER_G(automationList).end()) {
        map<string, Automation*>::iterator automation = ACMATCHER_G(automationList).find(dictionaryName);
        delete automation->second;
        ACMATCHER_G(automationList).erase(automation);
    }

    if (ACMATCHER_G(cacheableList).find(dictionaryName) == ACMATCHER_G(cacheableList).end()) {
        map<string, Cacheable*>::iterator cacheable = ACMATCHER_G(cacheableList).find(dictionaryName);
        delete cacheable->second;
        ACMATCHER_G(cacheableList).erase(cacheable);
    }


    if (ACMATCHER_G(automationList).find(dictionaryName) == ACMATCHER_G(automationList).end()) {
        RETURN_TRUE;
    }

    RETURN_FALSE;
}
