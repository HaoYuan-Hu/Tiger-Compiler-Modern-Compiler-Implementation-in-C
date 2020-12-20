#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "util.h"
#include "errormsg.h"
#include "symbol.h"
#include "absyn.h"
#include "types.h"
#include "helper.h"
#include "env.h"
#include "semant.h"

/*Lab4: Your implementation of lab4*/


typedef void* Tr_exp;
struct expty 
{
	Tr_exp exp; 
	Ty_ty ty;
};

//In Lab4, the first argument exp should always be **NULL**.
struct expty expTy(Tr_exp exp, Ty_ty ty)
{
	struct expty e;

	e.exp = exp;
	e.ty = ty;

	return e;
}

// 跳过所有Name类型
// 为了应对a = b, b = c, c = d, d = int 这种情况
Ty_ty actual_ty(Ty_ty type)
{
	// 如果type不存在，为NULL，则
	if(!type)
	{
		return NULL;
	}
	if(Ty_name == type->kind)
	{
		return actual_ty(type->u.name.ty);
	}

	return type;
}

// 比较类型之间的差异
bool TyCompare(Ty_ty type1, Ty_ty type2)
{
	if(!(type1 && type2))
	{
		return FALSE;
	}
	Ty_ty actual1 = actual_ty(type1);
	Ty_ty actual2 = actual_ty(type2);
	if(actual1 != actual2)
	{
		// 检查空类型
		if(!(Ty_record == actual1->kind && Ty_nil == actual2->kind) && !(Ty_nil == actual1->kind && Ty_record == actual2->kind))
		{
			return FALSE;
		}
	}
	return TRUE;
}

// 把抽象语法树变成类型
Ty_tyList change_absfield_to_Tylist(S_table tenv, A_fieldList absField)
{
	if(!absField)
	{
		return NULL;
	}

	Ty_tyList types = Ty_TyList(NULL, NULL);
	Ty_tyList rest = types;
	A_fieldList fields;
	
	for(fields = absField; fields; fields = fields->tail)
	{
		Ty_ty ty = S_look(tenv, fields->head->typ);
		if(!ty)
		{
			EM_error(absField->head->pos, "undefined type %s", S_name(absField->head->typ));
		}
		types->tail = Ty_TyList(ty, NULL);
		types = types->tail;
	}
	return rest->tail;
}

// 把抽象语法树变成类型
Ty_fieldList change_absfield_to_Tyfield(S_table tenv, A_fieldList absField)
{
	if(NULL == absField)
	{
		return NULL;
	}
	Ty_fieldList tyfields = Ty_FieldList(NULL, NULL);
	Ty_fieldList res = tyfields;
	Ty_ty ty;
	for(; absField; absField = absField->tail)
	{
		ty = S_look(tenv, absField->head->typ);
		if(NULL == ty)
		{
			EM_error(absField->head->pos, "undefined type %s", S_name(absField->head->typ));
		}
		tyfields->tail = Ty_FieldList(Ty_Field(absField->head->name, ty), NULL);
		tyfields = tyfields->tail;
	}
	return res->tail;
}

struct expty transVar(S_table venv, S_table tenv, A_var v)
{
	fprintf(stderr, "transVar\n");
	switch(v->kind)
	{
		// 分析简单变量
		case A_simpleVar:
		{
			fprintf(stderr, "\tsimpleVar\n");
			E_enventry env = S_look(venv, v->u.simple);
			if(env && E_varEntry == env->kind)
			{
				return expTy(NULL, actual_ty(env->u.var.ty));
			}
			else
			{
				EM_error(v->pos, "undefined variable %s", S_name(v->u.simple));
				return expTy(NULL, Ty_Int());
			}
		}
		// 分析[子代码]
		case A_subscriptVar:
		{
			fprintf(stderr, "\tsubscriptVar\n");
			struct expty varExpty = transVar(venv, tenv, v->u.subscript.var);
			struct expty expExpty = transExp(venv, tenv, v->u.subscript.exp);
			if(Ty_array != varExpty.ty->kind)
			{
				EM_error(v->pos, "array type required");
				return expTy(NULL, Ty_Int());
			}
			if(Ty_int != expExpty.ty->kind)
			{
				EM_error(v->pos, "Subscript was not an integer");
				return expTy(NULL, Ty_Int());	
			}
			return expTy(NULL, actual_ty(varExpty.ty->u.array));
		}
		// 分析记录中的域
		case A_fieldVar:
		{
			fprintf(stderr, "\tfieldVar\n");
			struct expty varExpty = transVar(venv, tenv, v->u.field.var);
			if(Ty_record != varExpty.ty->kind)
			{
				EM_error(v->pos, "not a record type");
				return expTy(NULL, Ty_Int());
			}
			else
			{
				Ty_fieldList fields;
				for(fields = varExpty.ty->u.record; fields; fields = fields->tail)
				{
					if(v->u.field.sym == fields->head->name)
					{
						return expTy(NULL, actual_ty(fields->head->ty));
					}
				}
				EM_error(v->pos, "field %s doesn't exist", S_name(v->u.field.sym));
				return expTy(NULL, Ty_Int());
			}
		}
	}
}

struct expty transExp(S_table venv, S_table tenv, A_exp a)
{
	fprintf(stderr, "transExp\n");
	switch(a->kind)
	{
	// 简单变量
	case A_varExp:
		fprintf(stderr, "\tvarExp\n");
		return transVar(venv, tenv, a->u.var);
	// 空
	case A_nilExp:
		fprintf(stderr, "\tnilExp\n");
		return expTy(NULL, Ty_Nil());
	// 整形
	case A_intExp:
		fprintf(stderr, "\tintExp\n");
		return expTy(NULL, Ty_Int());
	// 字符
	case A_stringExp:
		fprintf(stderr, "\tstringExp\n");
		return expTy(NULL, Ty_String());
	// 函数调用
	case A_callExp:
	{
		fprintf(stderr, "\tcallExp\n");
		E_enventry env = S_look(venv, a->u.call.func);
		if(NULL == env)
		{
			EM_error(a->pos, "undefined function %s", S_name(a->u.call.func));
			return expTy(NULL, Ty_Int());
		}
		if(E_funEntry != env->kind)
		{
			EM_error(a->pos, "variable %s is not a function", S_name(a->u.call.func));
			return expTy(NULL, Ty_Int());
		}
		Ty_tyList formals;
		A_expList args;
		for(formals = env->u.fun.formals, args = a->u.call.args; formals && args; formals = formals->tail, args = args->tail)
		{
			struct expty arg = transExp(venv, tenv, args->head);
			if(!TyCompare(formals->head, arg.ty))
			{
				EM_error(a->pos, "para type mismatch");
			}
		}
		if(formals)
		{
			EM_error(a->pos, "too few params in function %s", S_name(a->u.call.func));
		}
		if(args)
		{
			EM_error(a->pos, "too many params in function %s", S_name(a->u.call.func));
		}
		return expTy(NULL, env->u.fun.result);
	}
	// 计算操作
	case A_opExp:
	{
		fprintf(stderr, "\topExp\n");
		A_oper oper = a->u.op.oper;
		struct expty left = transExp(venv, tenv, a->u.op.left);
		struct expty right = transExp(venv, tenv, a->u.op.right);
		switch(oper)
		{
		case A_plusOp:
		case A_minusOp:
		case A_timesOp:
		case A_divideOp:
		{
			if(Ty_int != left.ty->kind)
			{
				EM_error(a->u.op.left->pos, "integer required");
			}
			if(Ty_int != right.ty->kind)
			{
				EM_error(a->u.op.right->pos, "integer required");
			}
			return expTy(NULL, Ty_Int());
		}
		case A_eqOp:
		case A_neqOp:
		{
			if(!TyCompare(left.ty, right.ty))
			{
				EM_error(a->u.op.right->pos, "same type required");
			}
			else if(Ty_void == left.ty->kind)
			{
				EM_error(a->u.op.left->pos, "expression had no value");
			}
			else if(Ty_void == right.ty->kind)
			{
				EM_error(a->u.op.right->pos, "expression had no value");
			}
			return expTy(NULL, Ty_Int());
		}
		case A_ltOp:
		case A_leOp:
		case A_gtOp:
		case A_geOp:
		{
			if(!TyCompare(left.ty, right.ty))
			{
				EM_error(a->u.op.right->pos, "same type required");
			}
			else if(Ty_int != left.ty->kind && Ty_string != left.ty->kind)
			{
				EM_error(a->u.op.left->pos, "string or integer required");
			}
			return expTy(NULL, Ty_Int());
		}
		}
	}
	// 记录表达式
	case A_recordExp:
	{
		fprintf(stderr, "\trecordExp\n");
		Ty_ty record = actual_ty(S_look(tenv, a->u.record.typ));
		if(!record)
		{
			EM_error(a->pos, "undefined type %s", S_name(a->u.record.typ));
			return expTy(NULL, Ty_Int());
		}
		if(Ty_record != record->kind)
		{
			EM_error(a->pos, "'%s' was not a record type", S_name(a->u.record.typ));
			return expTy(NULL, Ty_Int());
		}

		Ty_fieldList fields;
		A_efieldList actualFields;
		for(fields = record->u.record, actualFields = a->u.record.fields; fields && actualFields; fields = fields->tail, actualFields = actualFields->tail)
		{
			if(fields->head->name != actualFields->head->name)
			{
				EM_error(a->pos, "need member '%s' but '%s'", S_name(fields->head->name), S_name(actualFields->head->name));
			}
			else
			{
				struct expty actualField = transExp(venv, tenv, actualFields->head->exp);
				if(!TyCompare(fields->head->ty, actualField.ty))
				{
					EM_error(a->pos, "member '%s' type mismatch", S_name(fields->head->name));
				}
			}
		}
		if(fields)
		{
			EM_error(a->pos, "too few initializers for '%s'", S_name(a->u.record.typ));
		}
		else if(actualFields)
		{
			EM_error(a->pos, "too many initializers for '%s'", S_name(a->u.record.typ));
		}
		return expTy(NULL, record);
	}
	// 程序序列
	case A_seqExp:
	{
		fprintf(stderr, "\tseqExp\n");
		if(NULL == a->u.seq)
		{
			return expTy(NULL, Ty_Void());
		}
		A_expList expseq;
		for(expseq = a->u.seq; expseq; expseq = expseq->tail)
		{
			if(!expseq->tail)
			{
				return transExp(venv, tenv, expseq->head);
			}
			transExp(venv, tenv, expseq->head);
		}
	}
	// 赋值
	case A_assignExp:
	{
		fprintf(stderr, "\tassignExp\n");
		if(A_simpleVar == a->u.assign.var->kind)
		{
			E_enventry ifloop = S_look(venv, a->u.assign.var->u.simple);
			if(ifloop && ifloop->isloop)
			{
				EM_error(a->pos, "loop variable can't be assigned");
			}
		}
		struct expty varExpty = transVar(venv, tenv, a->u.assign.var);
		struct expty expExpty = transExp(venv, tenv, a->u.assign.exp);
		if(!TyCompare(varExpty.ty, expExpty.ty))
		{
			EM_error(a->pos, "unmatched assign exp");
		}
		return expTy(NULL, Ty_Void());
	}
	// 条件
	case A_ifExp:
	{
		fprintf(stderr, "\tifExp\n");
		struct expty testExpty = transExp(venv, tenv, a->u.iff.test);
		struct expty thenExpty = transExp(venv, tenv, a->u.iff.then);

		if(Ty_int != testExpty.ty->kind)
		{
			EM_error(a->pos, "if-exp was not an integer");
		}

		if(a->u.iff.elsee)
		{
			struct expty elseExpty = transExp(venv, tenv, a->u.iff.elsee);
			if(Ty_nil == elseExpty.ty->kind)
			{
				if(Ty_nil != thenExpty.ty->kind && Ty_record != thenExpty.ty->kind)
				{
					EM_error(a->pos, "if-then exp's body must produce no value");
				}
			}
			else if(!TyCompare(thenExpty.ty, elseExpty.ty))
			{
				EM_error(a->pos, "then exp and else exp type mismatch");
			}
			return expTy(NULL, thenExpty.ty);
		}
		else
		{
			if(Ty_void != thenExpty.ty->kind)
			{
				EM_error(a->pos, "if-then exp's body must produce no value");
			}
			return expTy(NULL, Ty_Void());
		}
	}
	// while循环
	case A_whileExp:
	{
		fprintf(stderr, "\twhileExp\n");
		struct expty testExpty = transExp(venv, tenv, a->u.whilee.test);
		struct expty bodyExpty = transExp(venv, tenv, a->u.whilee.body);
		if(Ty_int != testExpty.ty->kind)
		{
			EM_error(a->pos, "while-exp was not an integer");
		}
		if(Ty_void != bodyExpty.ty->kind)
		{
			EM_error(a->pos, "while body must produce no value");
		}
		return expTy(NULL, Ty_Void());
	}
	// for循环
	case A_forExp:
	{
		fprintf(stderr, "\tforExp\n");
		struct expty lowExpty = transExp(venv, tenv, a->u.forr.lo);
		struct expty highExpty = transExp(venv, tenv, a->u.forr.hi);
		if(Ty_int != lowExpty.ty->kind || Ty_int != highExpty.ty->kind)
		{
			EM_error(a->pos, "for exp's range type is not integer");
		}
		S_beginScope(venv);
		S_beginScope(tenv);
		E_enventry loop = E_VarEntry(Ty_Int());
		loop->isloop = 1;
		S_enter(venv, a->u.forr.var, loop);
		struct expty bodyExpty = transExp(venv, tenv, a->u.forr.body);
		S_endScope(tenv);
		S_endScope(venv);
		if(Ty_void != bodyExpty.ty->kind)
		{
			EM_error(a->pos, "body exp shouldn't return a value");
		}
		return expTy(NULL, Ty_Void());
	}
	// break操作
	case A_breakExp:
		fprintf(stderr, "\tbreakExp\n");
		return expTy(NULL, Ty_Void());
	// let操作
	case A_letExp:
	{
		fprintf(stderr, "\tletExp\n");
		struct expty bodyExpty;
		A_decList decs;
		S_beginScope(venv);
		S_beginScope(tenv);
		for(decs = a->u.let.decs; decs; decs = decs->tail)
		{
			transDec(venv, tenv, decs->head);
		}
		bodyExpty = transExp(venv, tenv, a->u.let.body);
		S_endScope(tenv);
		S_endScope(venv);
		return bodyExpty;
	}
	// 数组操作
	case A_arrayExp:
	{
		fprintf(stderr, "\tarrayExp\n");
		Ty_ty arrayty = actual_ty(S_look(tenv, a->u.array.typ));
		if(NULL == arrayty)
		{
			EM_error(a->pos, "undefined type %s", S_name(a->u.array.typ));
			return expTy(NULL, Ty_Int());
		}
		if(Ty_array != arrayty->kind)
		{
			EM_error(a->pos, "'%s' was not a array type", S_name(a->u.array.typ));
			return expTy(NULL, Ty_Int());
		}
		struct expty sizeExpty = transExp(venv, tenv, a->u.array.size);
		struct expty initExpty = transExp(venv, tenv, a->u.array.init);
		if(Ty_int != sizeExpty.ty->kind)
		{
			EM_error(a->pos, "array size was not an integer value");
		}
		if(!TyCompare(arrayty->u.array, initExpty.ty))
		{
			EM_error(a->pos, "array init type mismatch");
		}
		return expTy(NULL, arrayty);
	}
	}
}

void transDec(S_table venv, S_table tenv, A_dec d)
{
	fprintf(stderr, "transDec\n");
	switch(d->kind)
	{
	case A_functionDec:
	{
		fprintf(stderr, "\tfunctionDec\n");
		A_fundecList funs;
		A_fundec fun;
		for(funs = d->u.function; funs;funs = funs->tail)
		{
			fun = funs->head;
			if(S_look(venv, fun->name))
			{
				EM_error(fun->pos, "two functions have the same name");
				continue;
			}
			Ty_ty result;
			if(fun->result)
			{
				result = S_look(tenv, fun->result);
				if(!result)
				{
					EM_error(fun->pos, "undefined type %s in function result type", S_name(fun->result));
					result = Ty_Int();
				}
			}
			else
			{
				result = Ty_Void();
			}
			Ty_tyList params = change_absfield_to_Tylist(tenv, fun->params);
			S_enter(venv, fun->name, E_FunEntry(params, result));
		}
		for(funs = d->u.function; funs; funs = funs->tail)
		{
			fun = funs->head;
			S_beginScope(venv);
			A_fieldList fields;
			for(fields = fun->params; fields; fields = fields->tail)
			{
				S_enter(venv, fields->head->name, E_VarEntry(S_look(tenv, fields->head->typ)));
			}
			struct expty bodyExpty = transExp(venv, tenv, fun->body);
			E_enventry res = S_look(venv, fun->name);
			if(Ty_void == res->u.fun.result->kind && Ty_void != bodyExpty.ty->kind)
			{
				EM_error(fun->pos, "procedure returns value");
			}
			else if(!TyCompare(res->u.fun.result, bodyExpty.ty))
			{
				EM_error(fun->pos, "wrong return type");
			}
			S_endScope(venv);
		}
		break;
	}
	case A_varDec:
	{
		fprintf(stderr, "\tvarDec\n");
		struct expty initExpty = transExp(venv, tenv, d->u.var.init);
		if(d->u.var.typ)
		{
			Ty_ty type = S_look(tenv, d->u.var.typ);
			if(!type)
			{
				EM_error(d->pos, "undefined type %s", S_name(d->u.var.typ));
				return;
			}
			else
			{
				if(!TyCompare(type, initExpty.ty))
				{
					EM_error(d->pos, "type mismatch");
				}
				S_enter(venv, d->u.var.var, E_VarEntry(type));
				return;
			}
		}
		if(Ty_void == initExpty.ty->kind)
		{
			EM_error(d->pos, "can not init with void type");
		}
		if(Ty_nil == initExpty.ty->kind)
		{
			EM_error(d->pos, "init should not be nil without type specified");
		}
		S_enter(venv, d->u.var.var, E_VarEntry(initExpty.ty));
		return;
	}
	case A_typeDec:
	{
		fprintf(stderr, "\ttypeDec\n");
		A_nametyList types;
		for(types = d->u.type; types; types = types->tail)
		{
			if(S_look(tenv, types->head->name))
			{
				EM_error(d->pos, "two types have the same name");
				continue;
			}
			S_enter(tenv, types->head->name, Ty_Name(types->head->name, NULL));
		}
		for(types = d->u.type; types; types = types->tail)
		{
			Ty_ty type = S_look(tenv, types->head->name);
			if(!type)
			{
				continue;
			}
			type->u.name.ty = transTy(tenv, types->head->ty);
		}
		// 检查是否成环
		for(types = d->u.type; types; types = types->tail)
		{
			Ty_ty type = S_look(tenv, types->head->name);
			if(Ty_name != type->kind)
			{
				continue;
			}
			Ty_ty currentType;
			Ty_ty usedTypes[100];
			int typeIndex = 0;
			for(currentType = type->u.name.ty, typeIndex = 0; (Ty_name == currentType->kind)&&(currentType->u.name.ty); currentType = currentType->u.name.ty, typeIndex++)
			{
				for(int i = 0; i < typeIndex; i++)
				{
					if(currentType == usedTypes[i])
					{
						EM_error(d->pos, "illegal type cycle");
						return;
					}
				}
				usedTypes[typeIndex] = currentType;
			}
		}
	}
	}
}

Ty_ty transTy(S_table tenv, A_ty a)
{
	fprintf(stderr, "transTy\n");
	Ty_ty ty;
	switch(a->kind)
	{
		case A_nameTy:
			fprintf(stderr, "\tnameTy\n");
			ty = S_look(tenv, a->u.name);
			if(NULL == ty)
			{
				EM_error(a->pos, "undefined type %s", S_name(a->u.name));
				return Ty_Int();
			}
			return ty;

		case A_recordTy:
			fprintf(stderr, "\trecordTy\n");
			return Ty_Record(change_absfield_to_Tyfield(tenv, a->u.record));
		case A_arrayTy:
			fprintf(stderr, "\tarrayTy\n");
			ty = S_look(tenv, a->u.array);
			if(NULL == ty)
			{
				EM_error(a->pos, "undefined type %s", S_name(a->u.array));
				return Ty_Array(Ty_Int());
			}
			return Ty_Array(S_look(tenv, a->u.array));
	}
}

void SEM_transProg(A_exp exp)
{
	S_table tenv = E_base_tenv();
	S_table venv = E_base_venv();
	transExp(venv, tenv, exp);
}
