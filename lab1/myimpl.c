#include "prog1.h"
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

typedef struct table *Table_;
typedef struct IntAndTable *IntAndTable_;

struct table 
{
	string id; 
	int value; 
	Table_ tail;
};
Table_ Table(string id, int value, struct table *tail) {
	Table_ t = checked_malloc(sizeof *t);
	t->id = id;
	t->value = value;
	t->tail = tail;
	return t;
}

struct IntAndTable
{
	int value;
	Table_ table;
};
IntAndTable_ IntAndTable(int in_value, Table_ in_table) {
	struct IntAndTable *a = checked_malloc(sizeof *a);
	a->value = in_value;
	a->table = in_table;
	return a;
}

int maxargsStm(A_stm stm);									//找Stm中print的最大参数
int maxargsExp(A_exp exp);									//找Exp中print的最大参数

Table_ interpStm(A_stm s, Table_ t);				//解释Stm的递归函数
struct IntAndTable interExp(A_exp s, Table_ t);		//解释Exp的递归函数

int lookup(Table_ t, string key);					//从符号表t中，找到key对应的值
Table_ update(Table_ t, string key, int new);		//在符号表t中，如果key存在，则把key对应的值设为new；如果不存在，则插入


int maxargs(A_stm stm)
{
	//TODO: put your code here.
	return maxargsStm(stm);
}

int maxargsStm(A_stm stm)
{
	int max = 0;

	switch (stm->kind) 
	{
		case A_compoundStm: 
		{
			int tmp1 = 0;
			int tmp2 = 0;
			tmp1 = maxargs(stm->u.compound.stm1);
			tmp2 = maxargs(stm->u.compound.stm2);
			
			if(tmp1 > tmp2){
				max = tmp1;
			}else{
				max = tmp2;
			}
			break;
		}
		case A_assignStm:
		{
			max = maxargsExp(stm->u.assign.exp);		
			break;
		}
		case A_printStm:
		{
			int length = 0;
			int tmp = 0;
			A_expList expp = stm->u.print.exps;

			//循环遍历explist
			while(expp->kind == A_pairExpList) {
				tmp = maxargsExp(expp->u.pair.head);
				if(tmp > max){
					max = tmp;
				}
				length++;
				expp = expp->u.pair.tail;
			}

			//处理尾节点
			if(expp->kind != A_lastExpList) {
				printf("explist ending error\n");
				exit(0);
			}
			tmp = maxargsExp(expp->u.pair.head);
			length++;
			if(tmp > max){
				max = tmp;
			}
			if(length > max){
				max = length;
			}
			break;
		}
		default:
			break;
	}

	return max;
}

int maxargsExp(A_exp exp)
{
	//TODO: put your code here.
	int max = 0;

	switch (exp->kind) 
	{
		case A_idExp: 
		{
			break;
		}
		case A_numExp:
		{
			break;
		}
		case A_opExp:
		{		
			int tmp1 = maxargsExp(exp->u.op.left);
			int tmp2 = maxargsExp(exp->u.op.right);
			if(tmp1 > tmp2){
				max = tmp1;
			}else{
				max = tmp2;
			}
			break;
		}
		case A_eseqExp:
		{
			int tmp1 = maxargsStm(exp->u.eseq.stm);
			int tmp2 = maxargsExp(exp->u.eseq.exp);
			if(tmp1 > tmp2){
				max = tmp1;
			}else{
				max = tmp2;
			}
			break;
		}
		default:
			break;
	}

	return max;
}





void interp(A_stm stm)
{
	//TODO: put your code here.
	Table_ t = NULL;
	interpStm(stm, t);
	return;
}

Table_ interpStm(A_stm s, Table_ t)
{
	// if(debug_mode == 1){
	// 	printf("debug stm: %d\n",s->kind);
	// }

	switch (s->kind) 
	{
		case A_compoundStm: 
		{
			t = interpStm(s->u.compound.stm1, t);
			t = interpStm(s->u.compound.stm2, t);
			break;
		}
		case A_assignStm:
		{
			string idtmp = s->u.assign.id;
			struct IntAndTable IAT = interExp(s->u.assign.exp, t);		//获得右子树的运算结果
			// if(debug_mode == 1){
			// 	printf("debug asvalue: %s, %d\n", idtmp, IAT.value);
			// }
			t = update(t, idtmp, IAT.value);
			break;
		}
		case A_printStm:
		{			
			A_expList expp = s->u.print.exps;

			//循环遍历explist
			while(expp->kind == A_pairExpList) {
				struct IntAndTable IAT = interExp(expp->u.pair.head, t);
				printf("%d ", IAT.value);
				expp = expp->u.pair.tail;
			}

			//处理尾节点
			if(expp->kind != A_lastExpList) {
				printf("explist ending error\n");
				exit(0);
			}
			struct IntAndTable IAT = interExp(expp->u.pair.head, t);
			printf("%d", IAT.value);
			printf("\n");
			break;
		}
		default:
			break;
	}

	return t;
}

struct IntAndTable interExp(A_exp s, Table_ t)
{
	IntAndTable_ IAT = IntAndTable(0, t);

	//看看种类
	A_exp expp = s;
	switch (expp->kind) 
	{
		case A_idExp: 
		{
			string idtmp = expp->u.id; 
			int valuetmp = lookup(t, idtmp);
			IAT->value = valuetmp;
			break;
		}
		case A_numExp:
		{
			int valuetmp = expp->u.num;
			IAT->value = valuetmp;
			break;
		}
		case A_opExp:
		{		
			int valuetmp = 0;

			if(expp->u.op.oper == A_plus) {
				valuetmp = interExp(expp->u.op.left, t).value + interExp(expp->u.op.right, t).value;
			}
			else if(expp->u.op.oper == A_minus) {
				valuetmp = interExp(expp->u.op.left, t).value - interExp(expp->u.op.right, t).value;
			}
			else if(expp->u.op.oper == A_times) {
				valuetmp = interExp(expp->u.op.left, t).value * interExp(expp->u.op.right, t).value;
			}
			else if(expp->u.op.oper == A_div) {
				valuetmp = interExp(expp->u.op.left, t).value / interExp(expp->u.op.right, t).value;
			}
			IAT->value = valuetmp;
			break;
		}
		case A_eseqExp:
		{
			t = interpStm(expp->u.eseq.stm, t);
			*IAT = interExp(expp->u.eseq.exp, t);
			break;
		}
		default:
			break;
	}

	return *IAT;
}



int lookup(Table_ t, string key)
{
	Table_ tmp = t;
	if(tmp == NULL) {
		printf("No thing in the table\n");
	}

	while(tmp != NULL) {
		if(tmp->id == key){
			return tmp->value;
		}
		tmp = tmp->tail;
	}

	while(tmp != NULL){
		printf("%s, %d \n", tmp->id, tmp->value);
	}
	printf("%s is not in the table\n", key);

	return -1;
}



Table_ update(Table_ t, string key, int new)
{
	Table_ tt = t;
	if(tt == NULL) {
		Table_ tmp = Table(key, new, NULL);
		tt = tmp;
		return tt;
	}
	else {
		while(tt->tail != NULL){
			if(tt->id == key){
				tt->value = new;
				return t;
			}
			tt = tt->tail;
		}

		if(tt->id == key) {
			tt->value = new;
		}
		else {
			Table_ tmp = Table(key, new, NULL);
			tt->tail = tmp;
		}
	}

	return t;
}
