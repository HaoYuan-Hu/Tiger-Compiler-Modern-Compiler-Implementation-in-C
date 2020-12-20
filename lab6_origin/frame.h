
/*Lab5: This header file is not complete. Please finish it with more definition.*/

#ifndef FRAME_H
#define FRAME_H

#include "tree.h"
#include "assem.h"

/*=============================================*/
/*================ 1、F_frame =================*/
/*=============================================*/

typedef struct F_frame_ *F_frame;

// 描述 1
// 描述：帧栈描述符，一个 frame 在一次函数调用后创建，保存该函数分配存储所需要的信息
struct F_frame_ {
	Temp_label name;			// frame 的名称
	F_accessList formals;		// frame 中的形式参数
	int size;					// 这个 frame 的大小
};

// 函数 1
// 传入：Frame 对应的汇编中的 label，形式参数的存储形式（在寄存器/内存中）
// 功能：新建一个 Frame
// 返回：新建的 Frame
F_frame F_newFrame(Temp_label name, U_boolList formals);

// 函数 2
// 传入：想获得具体 label 名称的 Frame 
// 功能：获得某个 Frame 的 label
// 返回：该 Frame 的 label
Temp_label F_name(F_frame f);

// 函数 3
// 传入：想获得具体形式参数的 Frame 
// 功能：获得某个 Frame 的 形式参数
// 返回：该 Frame 的 形式参数
F_accessList F_formals(F_frame f);

// 函数 4
// 传入：需要创建本地变量的 Frame，以及该本地变量的存储形式 
// 功能：创建一个本地变量
// 返回：该本地变量的 F_access
F_access F_allocLocal(F_frame f, bool escape);

/*=============================================*/
/*================= 2、F_access ===============*/
/*=============================================*/

typedef struct F_access_ *F_access;

// 描述 2
// 描述：一个 F_access 相当于一个形式参数/局部变量，存储在栈/寄存器中
struct F_access_ {
	enum {inFrame, inReg} kind;		// 变量存储在寄存器上还是栈上
	union {
		int offset; 				// inFrame 相对于该视角下的帧的位置
		Temp_temp reg; 				// inReg 一个临时的虚寄存器
	} u;
};

// 函数 5
// 传入：要创建的变量在栈上的 offset 处 
// 功能：在栈上对应的 offset 处创建一个变量
// 返回：该变量的 F_access
static F_access InFrame(int offset);

// 函数 6
// 传入：要创建的变量在 reg 寄存器上
// 功能：在 reg 寄存器中创建一个变量
// 返回：该变量的 F_access
static F_access InReg(Temp_temp reg);

/*=============================================*/
/*============== 3、F_accessList ==============*/
/*=============================================*/

typedef struct F_accessList_ *F_accessList;

// 描述 3
// 描述：F_access 串成的 List，用来描述多个变量
struct F_accessList_ {
	F_access head; 
	F_accessList tail;
};

// 函数 7
// 传入：F_access 作为 List 头部，剩余的 F_accessList 作为 List 尾部
// 功能：F_accessList 的构造函数
// 返回：构造出的 F_accessList
F_accessList F_AccessList(F_access head, F_accessList tail);

/*=============================================*/
/*================= 4、F_frag =================*/
/*=============================================*/

typedef struct F_frag_ *F_frag;

// 描述 4 
// 描述：片段 frag 有两种形式
//   第一种是 字符串 形式，例如：
// 	   a:  hhy 是个大 sb
//   第二种是 帧栈描述符（包含有关局部变量和参数的存储信息） + 已翻译好的函数体
struct F_frag_ {
	enum {F_stringFrag, F_procFrag} kind;					// 是 字符串片段 还是 程序片段
	union {
		struct {Temp_label label; string str;} stringg;		// 字符串片段的 label 和实际存储内容 str
		struct {T_stm body; F_frame frame;} proc;			// 程序片段的 帧栈描述符frame 和 已翻译好的函数体 body
	} u;
};

// 函数 8
// 传入：字符串片段的 label 和实际要存储的字符串内容 str
// 功能：构造出一个字符串片段 F_stringFrag
// 返回：构造出的 F_stringFrag
F_frag F_StringFrag(Temp_label label, string str);

// 函数 9
// 传入：该程序片段的帧栈描述符 frame 和已翻译好的函数体 body
// 功能：F_procFrag 的构造函数
// 返回：构造出的 F_procFrag
F_frag F_ProcFrag(T_stm body, F_frame frame);

/*=============================================*/
/*=============== 5、F_fragList ===============*/
/*=============================================*/

typedef struct F_fragList_ *F_fragList;

// 描述 5
// 描述：F_frag 串成的 List，用来描述多个片段
struct F_fragList_ {
	F_frag head; 
	F_fragList tail;
};

// 函数 10
// 传入：F_frag 作为 List 头部，剩余的 F_fragList 作为 List 尾部
// 功能：F_fragList 的构造函数
// 返回：构造出的 F_fragList
F_fragList F_FragList(F_frag head, F_fragList tail);

/*=============================================*/
/*================ 6、辅助功能 =================*/
/*=============================================*/

// 描述 6
// 描述：一个 Frame 中的字大小，本 lab 中为 8
extern const int F_wordSize;

// 描述 7
// 描述：一个 Frame 中的字大小，本 lab 中为 8
extern const int F_formalRegNum;

// 描述 8
// 描述：一个 Frame 中的字大小，本 lab 中为 8
extern const int F_regNum;

// 描述 9
// 描述：我们目前生成的临时变量，都是t1、t2、t3......
//   但是有些临时变量是 “内建变量” ，是由 Frame 模块创建的表示特定寄存器的临时变量
//   如 FP，RV，SP......
//   我们需要把这些 “内建变量” 通过 F_tempMap 映射为本来的名字，将非特殊临时变量映射为 NULL
//   即给出每一个预着色临时变量的 “颜色”
Temp_map F_tempMap;

// 函数 11
// 传入：void
// 功能：返回一个保存函数返回值 returned value 的单元
// 返回：寄存器 rax（用于保存返回值）
Temp_temp F_RV(void);

// 函数 12
// 传入：void
// 功能：返回一个保存帧指针 frame pointer 的单元
// 返回：寄存器 rbp（用于保存帧指针）
Temp_temp F_FP(void);

// 函数 13
// 传入：void
// 功能：返回一个保存栈指针 stack pointer 的单元
// 返回：寄存器 rsp（用于保存栈指针）
Temp_temp F_SP(void);

// 函数 14
// 传入：要转换为 Tree 表达式的 acc，该 F_access 所在的栈帧地址 framePtr
// 功能：Translate 调用 函数 F_Exp，将一个 F_access 转换成 Tree 表达式 
// 返回：转换完的 Tree 表达式 T_exp
T_exp F_Exp(F_access acc, T_exp framePtr);

// 函数 15
// 传入：外部函数的名字 s，调用该函数时要传递的参数 args
// 功能：生成 调用运行时系统的函数 对应的汇编指令
// 返回：转换完的 Tree 表达式 T_exp
T_exp F_externalCall(string s, T_expList args);

// 函数 16
// 传入：机器所有寄存器的名字组成的一张表，它们将作为寄存器分配的“颜色”
// 功能：判断所有的寄存器 register 是否已经创建，
//   如果已经创建，则返回所有 register 构成的 Temp_tempList
//   如果没有创建，则创建所有 register 构成的 Temp_tempList 之后，并返回
// 返回：创建好的，包含所有 register 的 Temp_tempList
Temp_tempList F_registers(void);

// 函数 17
// 传入：void
// 功能：机器所有 callee saved 寄存器的名字组成的一张表
//   如果已经创建，则返回所有 callee saved register 构成的 Temp_tempList
//   如果没有创建，则创建所有 callee saved register 构成的 Temp_tempList 之后，并返回
// 返回：callee saved 寄存器所组成的 Temp_tempList
Temp_tempList F_calleeSavedReg(void);

// 函数 18
// 传入：void
// 功能：机器所有 caller saved 寄存器的名字组成的一张表
//   如果已经创建，则返回所有 caller saved register 构成的 Temp_tempList
//   如果没有创建，则创建所有 caller saved register 构成的 Temp_tempList 之后，并返回
// 返回：caller saved 寄存器所组成的 Temp_tempList
Temp_tempList F_callerSavedReg(void);

// 函数 19
// 传入：void
// 功能：机器所有 传参寄存器 的名字组成的一张表
//   如果已经创建，则返回所有 传参寄存器 构成的 Temp_tempList
//   如果没有创建，则创建所有 传参寄存器 构成的 Temp_tempList 之后，并返回
// 返回：传参寄存器 所组成的 Temp_tempList
Temp_tempList F_paramReg(void);

// ？函数 20
// 传入：void
// 功能：新建一个所有寄存器事先构成的 Temp_map
//   如果已经创建，则返回所有 register 构成的 Temp_tempMap
//   如果没有创建，则创建所有 register 构成的 Temp_tempMap 之后，并返回
// 返回：创建好的 Temp_Map
Temp_map F_regTempMap();

// ...函数 21
// 传入：void
// 功能：负责处理 X86 指令集下的乘运算，这个 lab 并不会用到
// 返回：处理 MUL 指令时所使用的 Temp_tempList
Temp_tempList F_X86MUL(void);

// ...函数 22
// 传入：void
// 功能：负责处理 X86 指令集下的除运算，这个 lab 并不会用到
// 返回：处理 DIV 指令时所使用的 Temp_tempList
Temp_tempList F_X86DIV(void);

// 函数 23
// 传入：该程序片段的帧栈描述符 frame 和已翻译好的函数体 stm
// 功能：负责翻译函数体的部分功能，处理第 4 条、第 5 条、第 8 条
// 	 第 4 条：入口处理代码，将逃逸参数（包括静态链）保存至栈帧，将非逃逸参数存储到新的临时寄存器
//   第 5 条：入口处理代码，保存在此函数内用到的 callee saved 寄存器、返回地址寄存器的存储指令
//   第 8 条：出口处理代码，恢复 callee saved 寄存器
//   Translate 在翻译过程体 5~7 条时，对每个过程体都会调用这个函数
//   简而言之，procEntryExit1 为每一个 callee saved 、返回地址寄存器建立新的临时变量，
//   在函数入口处，把这些寄存器传送到新临时变量单元中，在函数出口处再传送回来
// 返回：转换完的 Tree 表达式 T_stm 
T_stm F_procEntryExit1(F_frame frame, T_stm stm);

// 函数 24
// 传入：要添加 sink 指令的 AS_instrList
// 功能：在末尾添加一个 sink 指令，用以告诉寄存器分配器，某些寄存器在过程的出口是活跃的
//   换言之，扫描过程体并将实参信息记录到 frame 类型的某个新成员中
// 返回：添加完 sink 指令之后的 AS_instrList
AS_instrList F_procEntryExit2(AS_instrList body);

// 函数 25
// 传入：要生成入口处理、出口处理的汇编语言代码 body，该函数对应的 frame
// 功能：生成过程入口处理和出口处理的汇编语言代码，计算通过帧栈传递实参需要的空间大小
//   一旦知道了帧栈大小，有关过程的入口、栈指针的调整、过程出口和汇编语言代码可以整合到一起
// 返回：更新好的 frame 和增加了入口、出口处理的 body
AS_proc F_procEntryExit3(F_frame frame, AS_instrList body);

// _函数 26
// 传入：
// 功能：
// 返回：
Temp_labelList F_preDefineFuncs();

#endif