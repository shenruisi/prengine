# prengine
`Poor Router Engine` 一种用来帮助客户端实现更好的路由解决方案的rewrite引擎
## 设计
通过`prengine`，你可以使用部分`nginx`语法来描写配置文件，`prengine`将会解析输入的URI，并根
据输入URI进行匹配，应用条件表达式，最后输出替换后的结果和规则。剩余的你可以使用Objective-C,Swift,
或是Java将`prengine`在iOS或是Android平台进行封装。

## 示例

[示例代码](https://github.com/shenruisi/prengine/blob/master/sample/main.cpp)  [示例配置文件](https://github.com/shenruisi/prengine/blob/master/sample/sample.conf)

可以进入`sample/`目录 运行

g++ ../package/prengine_val.cpp ../package/cond_ast.cpp ../package/prengine.cpp  main.cpp

./a.out

查看结果。

## 安装
拷贝`package/`目录下的 `prengine.h`,`prengine.cpp`,`prengine_val.h`,`prengine_val.cpp`,`cond_ast.h`,`cond_ast.cpp`到目标目录即可。

## 使用
1.`include "prengine.h"`

2.调用`PRFILE* pr_creat(char *cnt)`来装载config文件

3.调用`PRREWRITE* pr_rewrite_matched_creat(PRFILE *f,const char *uri)` 传入`PRFILE`句柄和需要转化的`URI`,得到相应的`PRREWRITE`对象。

4.通过`const char *pr_getserver(PRREWRITE *)`,`const char *pr_getscheme(PRREWRITE *)`,`const char *pr_getoutval(PRREWRITE *)`,`const char *pr_getrule(PRREWRITE *)`
来获取`PRREWRITE`对象的不同属性。

5.调用`void pr_rewritefree(PRREWRITE *)`释放`PRREWRITE`对象。

## LICENSE
`prengine` is distributed under the BSD license.
