#include "optimizer.h"

void Optimize_AST(Expression* ast)
{
    for(unsigned int i = 0; i < ast->e.Program.expressions.size; ++i)
    {
        Expression expr = ast->e.Program.expressions.content[i];
        switch(expr.type)
        {
            case EXPR_Program:
            {
                if(!expr.e.Program.expressions.size)
                {
                    ExpressionArray_Drop(&ast->e.Program.expressions, i);
                    --i;
                }
                else if(expr.e.Program.expressions.size == 1)
                {
                    Expression cpy = ast->e.Program.expressions.content[0];
                    if(cpy.type == EXPR_Program)
                    {
                        Optimize_AST(&cpy);
                    }
                    ExpressionArray_Drop(&ast->e.Program.expressions, i);
                    ExpressionArray_Insert(&ast->e.Program.expressions, cpy, i);
                }
                else
                {
                    Optimize_AST(&ast->e.Program.expressions.content[i]);
                }
            }
            break;
            case EXPR_Conditional:
            {
                Expression cpy = (Expression){};
                if(expr.e.Conditional.program && expr.e.Conditional.program->type == EXPR_Program)
                {
                    cpy = *expr.e.Conditional.program;
                    Optimize_AST(&cpy);
                    *expr.e.Conditional.program = cpy;

                    if(!expr.e.Conditional.program)
                    {
                        ExpressionArray_Drop(&ast->e.Program.expressions, i);
                        --i;
                    }
                }
                if(expr.e.Conditional.expr)
                {
                    if(expr.e.Conditional.expr->type == EXPR_Program)
                    {
                        cpy = *expr.e.Conditional.expr;
                        Optimize_AST(&cpy);
                        *expr.e.Conditional.expr = cpy;
                    }
                    if(expr.e.Conditional.expr->type == EXPR_Literal)
                    {
                        if(expr.e.Conditional.expr->e.Literal.type == LITERAL_Num)
                        {
                            if(!expr.e.Conditional.expr->e.Literal.value.num)
                            {
                                Expression next = ast->e.Program.expressions.content[i+1];
                                ExpressionArray_Drop(&ast->e.Program.expressions, i);
                                --i;
                                if(next.type == EXPR_Conditional && next.e.Conditional.type == CONDITIONAL_Else)
                                {
                                    Expression cpy = *next.e.Conditional.program;
                                    if(cpy.type == EXPR_Program)
                                    {
                                        Optimize_AST(&cpy);
                                    }
                                    ExpressionArray_Drop(&ast->e.Program.expressions, i+1);
                                    ExpressionArray_Insert(&ast->e.Program.expressions, cpy, i+1);
                                }
                            }
                            else
                            {
                                if(ast->e.Program.expressions.content[i+1].type == EXPR_Conditional && ast->e.Program.expressions.content[i+1].e.Conditional.type == CONDITIONAL_Else)
                                {
                                    ExpressionArray_Drop(&ast->e.Program.expressions, i+1);
                                }

                                Expression cpy = *expr.e.Conditional.program;
                                if(cpy.type == EXPR_Program)
                                {
                                    Optimize_AST(&cpy);
                                }
                                ExpressionArray_Drop(&ast->e.Program.expressions, i);
                                ExpressionArray_Insert(&ast->e.Program.expressions, cpy, i);
                            }
                        }
                    }
                }
            }
            break;
            case EXPR_DefVar:
                if(expr.e.DefVar.expr->type == EXPR_Program)
                {
                    Expression cpy = *expr.e.DefVar.expr;
                    Optimize_AST(&cpy);
                    *expr.e.DefVar.expr = cpy;
                }
            break;
            case EXPR_SetVar:
                if(expr.e.SetVar.expr->type == EXPR_Program)
                {
                    Expression cpy = *expr.e.SetVar.expr;
                    Optimize_AST(&cpy);
                    *expr.e.SetVar.expr = cpy;
                }
            break;
            case EXPR_DefFunction:
                if(expr.e.DefFunction.program->type == EXPR_Program)
                {
                    Expression cpy = *expr.e.DefFunction.program;
                    Optimize_AST(&cpy);
                    *expr.e.DefFunction.program = cpy;
                }
            break;

            default:
            break;
        }
    }
}