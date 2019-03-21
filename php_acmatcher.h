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


ZEND_BEGIN_MODULE_GLOBALS(acmatcher)
    std::map<string, Automation*> automationList;
    std::map<string, Cacheable*> cacheableList;
ZEND_END_MODULE_GLOBALS(acmatcher)


#define ACMATCHER_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(acmatcher, v)

#if defined(ZTS) && defined(COMPILE_DL_ACMATCHER)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#endif	/* PHP_ACMATCHER_H */
