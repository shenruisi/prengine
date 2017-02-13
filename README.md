# prengine
[![Contact me](https://img.shields.io/badge/contact-me-orange.svg)](mailto:tigris.shin@gmail.com)
[![Build Status](https://travis-ci.org/shenruisi/prengine.svg?branch=master)](https://travis-ci.org/shenruisi/prengine)
[![GitHub license](https://img.shields.io/badge/license-BSD-blue.svg?style=flat)](https://github.com/shenruisi/prengine/blob/master/LICENSE)

`Poor Router Engine` 一种用来帮助客户端实现更好的路由解决方案的rewrite引擎，可以用于客户端页面路由，模块解耦。

## 设计
通过`prengine`，你可以使用部分`nginx`语法来描写配置文件，`prengine`将会解析输入的URI，并根
据输入URI进行匹配，应用条件表达式，最后输出替换后的结果和规则。剩余的你可以使用Objective-C,Swift,
或是Java将`prengine`在iOS或是Android平台上进行封装。

## 示例

[示例代码](https://github.com/shenruisi/prengine/blob/master/sample/main.cpp)  [示例配置文件](https://github.com/shenruisi/prengine/blob/master/sample/sample.conf)

可以进入`sample/`目录 运行

g++ ../package/prengine_val.cpp ../package/cond_ast.cpp ../package/prengine.cpp  main.cpp

./a.out

查看结果。

## 安装

* 直接安装

拷贝`package/`目录下的 `prengine.h`,`prengine.cpp`,`prengine_val.h`,`prengine_val.cpp`,`cond_ast.h`,`cond_ast.cpp`到目标目录即可。

* 获取.a文件

进入`package/`目录 运行make 在`bin/`目录下获取.a文件。

* 获取ios平台.a文件

进入`package/`目录 运行make platform=ios 在`bin/`目录下获取.a文件。

## 使用
1.`include "prengine.h"`

2.调用`PRFILE* pr_creat(char *cnt, VAL_HANDlER valhandler)`来装载config文件

3.调用`PRREWRITE* pr_rewrite_matched_creat(PRFILE *f,const char *uri)` 传入`PRFILE`句柄和需要转化的`URI`,得到相应的`PRREWRITE`对象。

4.通过`const char *pr_getserver(PRREWRITE *)`,`const char *pr_getscheme(PRREWRITE *)`,`const char *pr_getoutval(PRREWRITE *)`,`const char *pr_getrule(PRREWRITE *)`
来获取`PRREWRITE`对象的不同属性。

5.调用`void pr_rewritefree(PRREWRITE *)`释放`PRREWRITE`对象。

## 变量注入
```c++
pr_id your_handler(const char *vname){
  if(strcmp(vname, "test") == 0){
      return pr_yes();
  }
  return pr_undefined();
}

pr_creat(f /*PRFILE句柄*/,your_handler);
```
在config文件将可以使用`$test`变量。
