## lab6 我的工作

#### 一、编译执行顺序

**testcases**

助教提供的 tiger 测试代码

**tiger.lex**

用 Lex 工具，按照 tiger.lex 的规则，对 tiger 程序进行词法分析，找到匹配的单词并生成 token 。

**tiger.y**

用 Yacc 工具，按照 tiger.y 的规则，对 token 进行语法分析，生成抽象语法树 absyn 。





#### 二、文件列表

**所有相关文件（共53个，去除 Makefile 和 shell 脚本）**

1、absyn.h		absyn.c

2、assem.h		assem.c

3、canon.h		canon.c

4、codegen.h		codegen.c

5、color.h		color.c

6、env.h		env.c

7、errormsg.h		errormsg.c

8、escape.h		escape.c

9、flowgraph.h		flowgraph.c

10、frame.h		x86frame.c

11、graph.h		graph.c

12、liveness.h		liveness.c

13、parse.h		parse.c

14、prabsyn.h		prabsyn.c

15、printtree.h		printtree.c

16、regalloc.h		regalloc.c

17、semant.h		semant.c

18、symbol.h		symbol.c

19、table.h		table.c

20、temp.h		temp.c

21、translate.h		translate.c

22、tree.h		tree.c

23、types.h		types.c

24、util.h		util.c

25、tiger.lex		tiger.y

26、helper.h

27、runtime.c

28、main.c



**我要实现的文件**

| 编号 | 文件名      | 简述 | 掌握情况 | 实现情况 |
| ---- | ----------- | ---- | -------- | -------- |
| 1    | tiger.lex   |      |          |          |
| 2    | tiger.y     |      |          |          |
| 3    | canon.h     |      |          |          |
| 4    | canon.c     |      |          |          |
| 5    | codegen.h   |      |          |          |
| 6    | codegen.c   |      |          |          |
| 7    | color.h     |      |          |          |
| 8    | color.c     |      |          |          |
| 9    | env.h       |      |          |          |
| 10   | env.c       |      |          |          |
| 11   | escape.h    |      |          |          |
| 12   | escape.c    |      |          |          |
| 13   | flowgraph.h |      |          |          |
| 14   | flowgraph.c |      |          |          |
| 15   | frame.h     |      |          |          |
| 16   | x86frame.c  |      |          |          |
| 17   | liveness.h  |      |          |          |
| 18   | liveness.c  |      |          |          |
| 19   | regalloc.h  |      |          |          |
| 20   | regalloc.c  |      |          |          |
| 21   | semant.h    |      |          |          |
| 22   | semant.c    |      |          |          |
| 23   | translate.h |      |          |          |
| 24   | translate.c |      |          |          |



#### 三、约定

1、注释前带 " ？"，表示不知道这个函数有什么用，但实际用到了；

2、注释前带 "..."，表示这个函数没有用到，但出于完整性考虑，把它加上了；

3、注释前带 "_"，表示这个函数的注释有待补充，因为它依赖于其他模块；