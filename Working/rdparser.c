/*
 ============================================================================
 Name        : rdparser.c
 Author      : @saafo
 Version     : v0.1 Pre Alpha
 Copyright   : Copyright (c) 2019 SaffordHuang All rights reserved.
 Description : A Parser
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRING_MAX 100
enum yytokentype {
	NUMBER = 258,ID,ASSIGN,STRING,CMP,SCAN,PRINT,RETURN,WHILE,IF,ELSE,INT,STR,VOID
};


extern int yylex();
extern int yylval;
extern char* yypstr;//记得free!!!
extern char* yytext;


int factor();
int expr();
int term();

int tok;


void advance()
{
	tok = yylex();
	printf("tok: %s\n", yytext);
}

//--------------------------
// principles:
//1. advance()写在函数内部，return 1时要注意需不需要advance()
//2. 回归旧的设计模式：恢复函数参数，print只在函数开始匹配东西后输出，并exit(-1),若函数未匹配则return 0;
//3. 新发现：if(func())失败了不用printf和exit
//--------------------------
//记录一下：本来最开始采用参数控制是否要输出并退出，但后面以为这样很麻烦，
//只需要匹配第一个模块是否能匹配就好了，但直到statement()的时候发现第一个参数
//也不能确定是否就一定匹配该函数，所以新建了nonparameter分支保留了当时的痕迹
//再次开始带参数的工作
//--------------------------
// return value:
// 0:failed
// 1:success
// parameter(allow print error):
// 0:false
// 1:true(forced)
//--------------------------


// program
//     : external_declaration
//     | program external_declaration
//     ;
// to clear left recursion:

int program(int t)
{
	if(external_declaration(t))
	{
		while(external_declaration(0)){}//似乎存在潜在?隐患：ext_decl+ext_decl+type+decl时，yylex无法退回
		return 1;//各list可能也有此隐患，记得处理
	}
	else if(t == 1)
	{
		printf("Expected correct external_declaration.\n");
		exit(-1);
	}
	return 0;
}

// external_declaration
//     : type declarator decl_or_stmt
//     ;

int external_declaration(int t)
{
	if(type(t))
	{
		// advance();
		if(declarator(t))
		{
			// advance();
			if(decl_or_stmt(t))
			{
				// advance();
				return 1;
			}
		}
	}
	if(t == 1)
	{
		printf("Expected correct external_declaration.\n");
		exit(-1);
	}
	return 0;
}

// decl_or_stmt
//     : '{' statement_list '}'
//     | '{' '}'
//     | ',' declarator_list ';'
//     | ';'
//     ;

int decl_or_stmt(int t)
{
	if(tok == '{')
	{
		advance();
		if(tok == '}')
		{
			advance();
			return 1;
		}
		else if(statement_list(t))
		{
			// advance();
			if(tok == '}')
			{
				advance();
				return 1;
			}
			else if(t == 1)
			{
				printf("Expected a '}'.\n");
				exit(-1);
			}
		}
		else if(t == 1)
		{
			printf("Expected a '}' or statement_list.\n");
			exit(-1);
		}
	}
	else if(tok == ',')
	{
		advance();
		if(declarator_list(t))
		{
			// advance();
			if(tok == ';')
			{
				advance();
				return 1;
			}
			else if(t == 1)
			{
				printf("Expected a ';'.\n");
				exit(-1);
			}
		}
		else if(t == 1)
		{
			printf("Expected correct declarator_list.\n");
			exit(-1);
		}
	}
	else if(tok == ';')
	{
		advance();
		return 1;
	}
	return 0;
}

// declarator_list
//     : declarator
//     | declarator_list ',' declarator
//     ;

int declarator_list(int t)
{
	if(declarator(t))
	{
		while (tok == ',')
		{
			advance();
			if(!declarator(t) && t == 1)
			{
				printf("Expected a declarator.\n");
				exit(-1);
			}
		}
		return 1;//没少advance
	}
	return 0;
}

// intstr_list
//     : initializer
//     | intstr_list ',' initializer
//     ;

int intstr_list(int t)
{
	if(initializer(t))
	{
		while (tok == ',')
		{
			advance();
			if(!initializer(t) && t == 1)
			{
				printf("Expected an initializer.\n");
				exit(-1);
			}
		}
		return 1;//没少advance
	}
	return 0;
}

// initializer
//     : NUMBER
//     | STRING
//     ;

int initializer(int t)
{
	if(tok == NUMBER)
	{
		advance();
		return 1;
	}
	else if(tok == STRING)
	{
		advance();
		return 1;
	}
	else if(t == 1)
	{
		printf("Expected a number or a string.\n");
		exit(-1);
	}
	return 0;
}

// declarator
//     : ID
//     | ID '=' expr
//     | ID '(' parameter_list ')'
//     | ID '(' ')'
//     | ID '[' expr ']'
//     | ID '[' ']'
//     | ID '[' expr ']' '=' '{' intstr_list '}'
//     | ID '[' ']' '=' '{' intstr_list '}'
//     ;

int declarator(int t)
{
	if(tok == ID)
	{
		advance();
		if(tok == '=')
		{
			advance();
			if(expr(t))
				return 1;
			else if (t == 1)
			{
				printf("Expected correct expr.\n");
				exit(-1);
			}
		}
		else if(tok == '(')
		{
			advance();
			if(tok == ')')
			{
				advance();
				return 1;
			}
			else if(parameter_list(t))
			{
				if(tok == ')')
				{
					advance();
					return 1;
				}
				else if(t == 1)
				{
					printf("Expected a ')'.\n");
					exit(-1);
				}
			}
			else if(t == 1)
			{
				printf("Expected a ')' or parameter_list.\n");
				exit(-1);
			}
		}
		else if(tok == '[')
		{
			advance();
			if(tok == ']')
			{
				advance();
				if(tok == '=')
				{
					advance();
					if(tok == '{')
					{
						advance();
						if(intstr_list(t))
						{
							if(tok == '}')
							{
								advance();
								return 1;
							}
							else if(t == 1)
							{
								printf("Expected a '}'.\n");
								exit(-1);
							}
						}
						else if(t == 1)
						{
							printf("Expected correct intstr_list.\n");
							exit(-1);
						}
					}
					else if(t == 1)
					{
						printf("Expected '{' intstr_list '}'.\n");
						exit(-1);
					}
				}
				else
					return 1;//没少advance
			}
			else if(expr(t))
			{
				if(tok == ']')
				{
					advance();
					if(tok == '=')
					{
						if(tok == '{')
						{
							advance();
							if(intstr_list(t))
							{
								if(tok == '}')
								{
									advance();
									return 1;
								}
								else if(t == 1)
								{
									printf("Expected a '}'.\n");
									exit(-1);
								}
							}
						}
					}
					else
						return 1;//没少advance
				}
			}
			// else
			// 	if(t == 1)
			// 		printf("Expected correct declarator expression.\n");
		}
		// else
		// printf("Expected correct declarator expression.\n");exit(-1);
	}
	if(t == 1)
	{
		printf("Expected correct declarator expression.\n");
		exit(-1);
	}
	return 0;
}

// parameter_list
//         : parameter
//         | parameter_list ',' parameter
//         ;

int parameter_list(int t)
{
	if(parameter(t))
	{
		while(tok == ',')
		{
			advance();
			if(!parameter(t) && t == 1)
			{
				printf("Expected a parameter.\n");
				exit(-1);
			}
		}
		return 1;//没少advance
	}

	return 0;
}

// parameter
//         : type ID
//         ;

int parameter(int t)
{
	if(type(t))
	{
		if(tok == ID)
		{
			advance();
			return 1;
		}
		else if(t == 1)
		{
			printf("Expected correct parameter expression.\n");
			exit(-1);
		}
	}
	return 0;
}

// type
//         : INT
//         | STR
//         | VOID
//         ;

int type(int t)
{
	if(tok == INT)
	{
		advance();
		return 1;
	}
	else if(tok == STR)
	{
		advance();
		return 1;
	}
	else if(tok == VOID)
	{
		advance();
		return 1;
	}
	else if(t == 1)
	{
		printf("Expected correct type.\n");
		exit(-1);
	}
	return 0;
}

// statement
//     : type declarator_list ';'
//     | '{' statement_list '}'
//     | expr_statement
//     | IF '(' expr ')' statement
//     | IF '(' expr ')' statement ELSE statement
//     | WHILE '(' expr ')' statement
//     | RETURN ';'
//     | RETURN expr ';'
//     | PRINT ';
//     | PRINT expr_list ';'
//     | SCAN id_list ';'
//     ;

int statement(int t)
{
	if(type(0))
	{
		if(declarator_list(t))
		{
			if(tok == ';')
			{
				advance();
				return 1;
			}
			else if(t == 1)
			{
				printf("Expected a ';'.\n");
				exit(-1);
			}
		}
		// else if(t == 1)
		// {
		// 	printf("Expected correct declarator_list.\n");
		// 	exit(-1);
		// }
	}
	else if(tok == '{')
	{
		advance();
		if(statement_list(t))
		{
			if(tok == '}')
			{
				advance();
				return 1;
			}
			else if(t == 1)
			{
				printf("Expected a '}'.\n");
				exit(-1);
			}
		}
		// else
	}
	else if(expression_statement(0))
		return 1;
	else if(tok == IF)
	{
		advance();
		if(tok == '(')
		{
			advance();
			if(expr(t))
			{
				if(tok == ')')
				{
					advance();
					if(statement(t))
					{
						if(tok == ELSE)
						{
							advance();
							if(statement(t))
								return 1;
						}
						else
							return 1;
					}
				}
				else if(t == 1)
				{
					printf("Expected a ')'.\n");
					exit(-1);
				}
			}
		}
		else if(t == 1)
		{
			printf("Expected a '('.\n");
			exit(-1);
		}
	}
	else if(tok == WHILE)
	{
		advance();
		if(tok == '(')
		{
			advance();
			if(expr(t))
			{
				if(tok == ')')
				{
					advance();
					if(statement(t))
						return 1;
				}
				else if(t == 1)
				{
					printf("Expected a ')'.\n");
					exit(-1);
				}
			}
		}
		else if(t == 1)
		{
			printf("Expected a '('.\n");
			exit(-1);
		}
	}
	else if(tok == RETURN)
	{
		advance();
		if(tok == ';')
		{
			advance();
			return 1;
		}
		else if(expr(t))
		{
			if(tok == ';')
			{
				advance();
				return 1;
			}
			else if(t == 1)
			{
				printf("Expected a ';'.\n");
				exit(-1);
			}
		}
	}
	else if(tok == PRINT)
	{
		//CONTINUE YOUR WORK HERE.
	}
}

//-----------------------------------------------
//exp: factor
//	| exp '+' factor
//	| exp '-' factor
//	;

int expr()
{
	int l = factor();
	while(tok == '+' || tok == '-')
	{
		int oper = tok;
		advance();
		int r = factor();
		if( oper == '+')
			l += r;
		else
			l -= r;
	}
	return l;
}

//factor: term
//	| factor '*' term
//	| factor '/' term
//	;

int factor()
{
	int l = term();
	while(tok == '*' || tok == '/')
	{
		int oper = tok;
		advance();
		int r = term();
		if( oper == '*')
			l *= r;
		else
			l /= r;
	}
	return l;
}


//term: NUMBER
//	| '-' term
//	;

int term()
{
	if(tok == NUMBER)
	{
		advance();
		return yylval;
	}
	else if(tok == '-')
	{
		advance();
		int k = term();
		return -k;
	}
	else if(tok == 'q')
		exit(0);
	return -1;
}


typedef struct _ast ast;
typedef struct _ast *past;
struct _ast{
	int ivalue;
	char* nodeType;
	past left;
	past right;
};

past newAstNode()
{
	past node = malloc(sizeof(ast));
	if(node == NULL)
	{
		printf("run out of memory.\n");
		exit(0);
	}
	memset(node, 0, sizeof(ast));
	return node;
}

past newNum(int value)
{
	past var = newAstNode();
	var->nodeType = "intValue";
	var->ivalue = value;
	return var;
}
past newExpr(int oper, past left,past right)
{
	past var = newAstNode();
	var->nodeType = "expr";
	var->ivalue = oper;
	var->left = left;
	var->right = right;
	return var;
}

past astTerm()
{
	if(tok == NUMBER)
	{
		past n = newNum(yylval);
		advance();
		return n;
	}
	else if(tok == '-')
	{
		advance();
		past k = astTerm();
		past n = newExpr('-', NULL, k);
		return n;
	}
	else if(tok == 'q')
		exit(0);
	return NULL;
}


past astFactor()
{
	past l = astTerm();
	while(tok == '*' || tok == '/')
	{
		int oper = tok;
		advance();
		past r = astTerm();
		l = newExpr(oper, l, r);
	}
	return l;
}

past astExpr()
{
	past l = astFactor();
	while(tok == '+' || tok == '-')
	{
		int oper = tok;
		advance();
		past r = astFactor();
		l = newExpr(oper, l, r);
	}
	return l;
}

void showAst(past node, int nest)
{
	if(node == NULL)
		return;

	int i = 0;
	for(i = 0; i < nest; i ++)
		printf("  ");
	if(strcmp(node->nodeType, "intValue") == 0)
		printf("%s %d\n", node->nodeType, node->ivalue);
	else if(strcmp(node->nodeType, "expr") == 0)
		printf("%s '%c'\n", node->nodeType, (char)node->ivalue);
	showAst(node->left, nest+1);
	showAst(node->right, nest+1);

}

int main(int argc, char **argv)
{
	while(1)
	{
		printf("input expression, 'q' to exit>");
		advance();
		int r = expr();
		printf("result: %d\n", r);

		//past rr = astExpr();
		//showAst(rr, 0);
	}
	return 0;
}
