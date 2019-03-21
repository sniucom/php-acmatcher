// 缓存设置实现
// Created by 张西海 on 2019-03-19.
//
#include "cacheable.h"

Cacheable::Cacheable(string inKey, long inSeconds)
{
    key       = inKey;
    seconds   = inSeconds;
    long now  = time(0);
    starts    = now;
    ends      = now + inSeconds;
}

void Cacheable::delay(long inSeconds)
{
    if (seconds != inSeconds) {
        seconds  = inSeconds;
        ends     = starts + inSeconds;
    }
}