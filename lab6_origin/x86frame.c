#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "symbol.h"
#include "temp.h"
#include "table.h"
#include "tree.h"
#include "frame.h"

/*Lab5: Your implementation here.*/

/*========= 一些全局变量，可描述目标机的具体特征 ===========*/

// 字长为 8 字节
const int wordSize = 8;

// 函数传参时，前 6 个参数默认为寄存器
const int registerNum = 6;

/*========================================================*/

/*=====================具体的函数实现======================*/

// 传入：Frame 对应的汇编中的 label，形式参数的存储形式（在寄存器/内存中）
// 功能：新建一个 Frame
// 返回：新建的 Frame
F_frame F_newFrame(Temp_label name, U_boolList formals)
{



}

Temp_label F_name(F_frame f)
{
	return f->name;
}

F_accessList F_formals(F_frame f)
{
	return f->formals; 
}

F_access F_allocLocal(F_frame f, bool escape)
{
	// 判断要分配本地变量的 frame 是否为空
	if(f == NULL){
		printf("F_allocLocal: empty frame");
		return NULL;
	}

	// 增加 Frame 的大小
	f->size += wordSize;

	// 如果为 true，则说明为逃逸变量，要分配在栈上
	if(escape){
		return 
	}





}


F_frag F_StringFrag(Temp_label label, string str) {   
	    return NULL;                                      
}                                                     
                                                      
F_frag F_ProcFrag(T_stm body, F_frame frame) {        
	    return NULL;                                      
}                                                     
                                                      
F_fragList F_FragList(F_frag head, F_fragList tail) { 
	    return NULL;                                      
}                       