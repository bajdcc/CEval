# 四则运算器（C++）

[知乎专栏](https://zhuanlan.zhihu.com/p/29589948) 
[知乎专栏](https://zhuanlan.zhihu.com/p/29632312)

[Dev-Cpp精简版代码](https://github.com/bajdcc/learnstl/tree/master/projects/calc)

看到[https://github.com/JosanSun/DesignPattern](https://github.com/JosanSun/DesignPattern)，有了些灵感，因为最近把jMiniLang玩得差不多了，有点腻了。

重温一下设计模式，这也不是重新造轮子，不必用高大上的LR分析去弄。

回故一下曾经做的：

- LR分析（Java）一条龙工程——Parser、IR、VM、UI，维护最久的：https://github.com/bajdcc/jMiniLang
- 偷懒还没开始写的LISP解释器（C++）：https://github.com/bajdcc/CLisp  已经做好的（ANTLR）在https://github.com/bajdcc/MyScript
- 简单C语言编译器+虚拟机（C++，正则表达式）：https://github.com/bajdcc/CParser
- 管道：https://github.com/bajdcc/CppShell
- 仿Prolog（Java & ANTLR）：https://github.com/bajdcc/jProlog
- 函数式语言（C#）：https://github.com/bajdcc/SimpleConsole
- 数字转人民币（Java）——设计模式装逼典范，过度设计的反面教材，找机会我把它翻译成C++，绝对会亮瞎你：https://github.com/bajdcc/translate

没想到做过了这么多东西，写了这么多轮子，所以说对Parser这块可以说比较熟练了（排除编译优化哈），对计算器呢，肯定不能用已经实现过的方法，应该要有创新点。

[数字转人民币](https://github.com/bajdcc/translate)这个工程我感觉很有价值（你想过用编译指令去Parse一个简单的数字吗？），想办法把它融到本项目中去（感觉用用扩展）。

## 思路

实现的部分功能：

- 【词法分析】atod/字符串转义/操作符查表
- 【语法分析】二元运算
- 【语法分析】【重点讲解】**非递归**生成AST并求值

还欠缺：

- ~~括号的识别~~
- 调用的识别

### 一、解析（Parser & Lexer）

肯定要用LL人肉分析啦，这是**基本功**嘛，，

这里会重温：

1. 词法分析
   1. 【已完成，通过测试】**字符串转整型/浮点**（不用正则表达式）
   2. 【已完成，通过测试】**字符串转变量名/操作符/字符串**（不用正则表达式）
   3. 识别操作符（包括内置操作符和**可自定义扩展操作符**）
   4. 赋值语句（简单点就是ID=Exp）
2. 语法分析
   1. LL分析
   2. 生成语法树

### 二、求值（Eval）

一般而言，求个表达式的值，用算符优先文法也能做，不过太老套了。

LL分析表达式最大问题是优先级问题，如果我写了文法，然后去掉左递归，当然也可以，不过太没人性了，这方面用yaac和ANTLR比较好，它们帮你做。

比如对于“3+4\*5”，识别到乘号前，“3+4”肯定不能搞成一棵树，如果我硬要做，也行，只是到时候到乘号时就要忍痛割爱了（[函数式编程](https://github.com/bajdcc/SimpleConsole)这个项目就用这个坑爹方法）。我不做，那行，把“3+4”顺序放到栈里，也就是算符文法。看来没别的路走了。

然而我还是要做，就是要生成树，到时候我改还不成么。碰到乘号后，我就把“4”这个结点替换成树，也就是说，算符文法是碰到没有更高优先级就归约，我不管，**我就是要立即马上归约，这是本项目要尝试的地方**，管它高不高效。

还有个问题是归约后立马求值还是整体一起求值，其实，前者是自底向上，后者是自顶向下。我就是倾向于前者——**立马求值**，第一，出错后立马爆炸，免得解析后面多余的内容，第二，直接在归约时计算数值类型，该整型换浮点就换掉。

**PS：实践中可行，见代码。**

### 三、扩展（Extension）

就是函数呗，如abs等函数。引入变量名后，好处还是比较大的。当然这又会有一个坑爹问题——函数对参数的类型要求。参照上文，我的想法是——类型不同，如果不能自动转换，那立马爆炸。

扩展的第一个内容我想应该是[人民币翻译方案](https://github.com/bajdcc/translate)，我把多余的设计去掉，只剩下指令部分。一般LR分析看到一个函数调用，肯定先把参数给归约了，然后再到函数。而LL的话，我一看到变量名，发现是函数，就调用了相应函数的Parser方法，将控制权交给它。

为什么？因为不用对参数归约了呀，让函数去做。人民币翻译的代码里面，它就是Lexer和Parser搅在一起，哈哈，真是神设计，不知道当初怎么想的。

**PS：等待实现中。**

## 计划

~~废话少说，牛B少吹，先把它实现了再说。~~我已经实现了！

先把解析搞定，然后搞人民币的指令部分。

1. **【已完成】** 人民币解析（Java移植到C++）
2. **【已完成】** 完成四则运算解析

## 【模块】人民币翻译（C++）

本模块代码：2k Lines。

### 一、顶层设计

三个阶段：词法过滤、正则分组、语法分析。

设计原因：便于匹配顶层调用（面向调用的），结合常见的设计方式。

#### 阶段一：RefString 系列代码（词法分析阶段的流过滤器）

实现基于字符串的装饰器模式，类似于LINQ。

示例代码在ModRmbLexer结尾处（建造者模式）：

```C++
shared_ptr<IRefStringIterator> RmbLexer::decorator(string text)
{
    return make_shared<RefString>(text)
        ->iterator() // 启用流
        ->skipHead('0') // 启用向前看，跳过数字前置零
        ->findFirst('.') // 找到小数点
        ->take(2) // 只取小数点后两位
        ->lookAhead(); // 启用向前看
}
```

经过筛选后，"000905000234.0678"就变成了"905000234.06"。

而RefString设计之初就是保存**原字符串指针和起始终止位置**，并可以启用流功能， 这个想法是我自己实现的。

#### 阶段二：ModRmbStyle 系列代码（词法分析阶段的正则匹配状态机）

ModRmbStyle 生成了一系列指令，用于字符串的分组捕获及错误处理（解释器模式）。该阶段将数字拆分成**整数、小数点、小数**这三个部分，结果还是字符串，可以看做是正则捕获阶段。

指令集的设计以及分阶段的指令集也是我自己的想法和实现。

示例代码在ModRmbStyle，这里不贴了。

#### 阶段三：ModRmbParser 系列代码（生成AST）

ModRmbParser 生成AST，用访问者模式就可以遍历生成最终的人民币大写表示。

LL分析不说了，由于第二阶段将字符串拆分成三组，这样就分别对整数、小数点、小数三部分进行LL分析，生成AST。

语法分析阶段顶层调用代码：

```c++
lexer->match(); // 词法分析+正则捕获
group = lexer->getGroup(); // 获取匹配后的分组
normalizeGroup(3); // 填充缺失的分组
root = make_shared<StoreableNode>(); // AST的根
root->addNode(parseNode(INTEGER, group.at(INTEGER)), true); // 添加整数部分子树
root->addNode(parseNode(DOT, group.at(DOT)), true); // 添加小数点
root->addNode(parseNode(DECIMAL, group.at(DECIMAL)), true); // 添加小数部分子树
```

### 二、小结

具体的过程说明、设计模式使用、设计的原因我还没来得及想好，打算写到专栏上去。

这样折腾的好处：

1. 锻炼/复习一下设计模式
2. 便于扩展
3. 每个项目都是自己所做的工艺品，追求极致和完美，当然是很有满足感喽

原本是打算再写个RomanStyle将数字转成罗马数字的，我将转换过程独立成四个阶段：**词法过滤、正则匹配、生成AST、遍历AST生成结果**。后面会尝试完成RomanStyle，以证明这个坑爹的设计是可取的（逃

呵呵，一个小模块就有2k行代码了，那CEval这个项目最终也不会很简单就是了。