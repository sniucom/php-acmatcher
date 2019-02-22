### ACMatcher V1.0.0

Tire树AC自动机PHP扩展

字典含对应值存储。

> 仅在进程内常驻内存。 进程间版本尚在测试。

### 安装

要求: PHP \>= 7.0

```
phpize
./confugure
make
sudo make install
```

### 使用

***acmatcher.php***

```
<?

$str = "英雄赞歌".
       "烽烟滚滚唱英雄,四面青山侧耳听,侧耳听".
       "青天响雷敲金鼓,大海扬波作和声".
       "人民战士驱虎豹,舍生忘死保和平".
       "为什么战旗美如画,英雄的鲜血染红了她".
       "为什么大地春常在,英雄的生命开鲜花".
       "英雄猛跳出战壕,一道电光裂长空,裂长空".
       "地陷进去独身挡,天塌下来只手擎".
       "两脚熊熊趟烈火,浑身闪闪披彩虹".
       "为什么战旗美如画,英雄的鲜血染红了她".
       "为什么大地春常在,英雄的生命开鲜花".
       "一声呼叫炮声隆,倒海翻江天地崩,天地崩".
       "双手紧握爆破筒,怒目喷火热血涌".
       "敌人腐烂变泥土,勇士辉煌化金星".
       "为什么战旗美如画,雄的鲜血染红了她".
       "为什么大地春常在,英雄的生命开鲜花";

echo "当前进程号: ".getmypid();

//ACMatcher::build('字典名', '缓存时长', '函数名');
//ACMatcher::build('字典名', '缓存时长', array($object, 'method');

ACMatcher::build('hi', 120, function () {
    // ...... 返回数组
    return [
        '为什么',
        '花儿',
        '红',
        "美如画",
        "英雄",
        "画",
        [
            '敌人',
            '鲜花',
            'lists' => [ 1, 2, 3, 4, 5, 6, 7]
        ]
    ];
});

echo "<br><br>--------------------------------<br><br>";

$has = ACMatcher::match("hi", "一起盘他");
echo "字典命中结果: "; var_dump($has);

echo "<br><br>--------------------------------<br><br>";

echo "匹配结果:<br>";
$arr = ACMatcher::search("hi", $str);

echo "<pre>";var_dump($arr);echo "<pre>";

echo "<br><br>--------------------------------<br><br>";

echo "打印字典信息……<br><br>";

ACMatcher::print('hi');

echo "<br>--------------------------------<br><br>";

echo "当前字典hi缓存是否存在: "; var_dump(ACMatcher::has('hi'));

echo "<br>--------------------------------<br><br>";

echo "删除字典hi结果: "; var_dump(ACMatcher::destroy('hi'));
```

*** 输出 ***

```
当前进程号: 46020

--------------------------------

字典命中结果: bool(false)

--------------------------------

匹配结果:
array(6) {
  [0]=>
  array(5) {
    ["word"]=>
    string(9) "为什么"
    ["word_length"]=>
    int(3)
    ["index"]=>
    int(53)
    ["repeats"]=>
    int(6)
    ["values"]=>
    string(9) "为什么"
  }
  [1]=>
  array(5) {
    ["word"]=>
    string(6) "敌人"
    ["word_length"]=>
    int(2)
    ["index"]=>
    int(206)
    ["repeats"]=>
    int(1)
    ["values"]=>
    array(3) {
      [0]=>
      string(6) "敌人"
      [1]=>
      string(6) "鲜花"
      ["lists"]=>
      array(7) {
        [0]=>
        int(1)
        [1]=>
        int(2)
        [2]=>
        int(3)
        [3]=>
        int(4)
        [4]=>
        int(5)
        [5]=>
        int(6)
        [6]=>
        int(7)
      }
    }
  }
  [2]=>
  array(5) {
    ["word"]=>
    string(9) "美如画"
    ["word_length"]=>
    int(3)
    ["index"]=>
    int(58)
    ["repeats"]=>
    int(3)
    ["values"]=>
    string(9) "美如画"
  }
  [3]=>
  array(5) {
    ["word"]=>
    string(3) "红"
    ["word_length"]=>
    int(1)
    ["index"]=>
    int(68)
    ["repeats"]=>
    int(3)
    ["values"]=>
    string(3) "红"
  }
  [4]=>
  array(5) {
    ["word"]=>
    string(6) "鲜花"
    ["word_length"]=>
    int(2)
    ["index"]=>
    int(86)
    ["repeats"]=>
    int(3)
    ["values"]=>
    array(3) {
      [0]=>
      string(6) "敌人"
      [1]=>
      string(6) "鲜花"
      ["lists"]=>
      array(7) {
        [0]=>
        int(1)
        [1]=>
        int(2)
        [2]=>
        int(3)
        [3]=>
        int(4)
        [4]=>
        int(5)
        [5]=>
        int(6)
        [6]=>
        int(7)
      }
    }
  }
  [5]=>
  array(5) {
    ["word"]=>
    string(6) "英雄"
    ["word_length"]=>
    int(2)
    ["index"]=>
    int(0)
    ["repeats"]=>
    int(8)
    ["values"]=>
    string(6) "英雄"
  }
}


--------------------------------

打印字典信息……

ACMatcher V1.0.0

当前字典: hi

缓存信息:
缓存开始时间戳: 1550828949
缓存时长: 120 秒
缓存结束时间戳: 1550829069

字典信息:
1. 为什么
2. 花儿
3. 红
4. 美如画
5. 英雄
6. 画
7. 敌人
8. 鲜花

--------------------------------

当前字典hi缓存是否存在: bool(true)

--------------------------------

删除字典hi结果: bool(true)
```