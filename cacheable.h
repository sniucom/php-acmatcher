// 缓存设置定义类
// Created by 张西海 on 2019-03-19.
// 并非由线程监控缓存是否到期，而是在读取时判断配置
//

#ifndef ACMATCHER_CACHEABLE_H
#define ACMATCHER_CACHEABLE_H

#include <string>
#include <time.h>
#include <iostream>

using namespace std;

class Cacheable
{

public:
    string key;
    long starts;
    long seconds;
    long ends;

public:
    Cacheable(string inKey, long inSeconds);
    void delay(long inSeconds);

};

#endif //ACMATCHER_CACHEABLE_H
