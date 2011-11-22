%{
/* Analise e classificacao do input. */
#include<stdio.h>
#include<stdlib.h>

#include"stack.h"
#include"circuit.h"
#include"yacc_util.h"
#include"mvsis_gate.tab.h"

extern FILE* mvsis_gate_out;

%}

%option yylineno

id	[_a-zA-Z][_a-zA-Z0-9]*
inteiro	[0-9]+
espaco	[\ \t\n\v\\]

%%

\.model	{
	/* MODEL directive. */
	fprintf( mvsis_gate_out, "%3d: MODEL: \"%s\"\n", yylineno, yytext);
	return MODEL;
}

\.inputs	{ 
	/* INPUTS list start. */
	fprintf( mvsis_gate_out, "%3d: INPUTS: \"%s\"\n", yylineno, yytext);
	return INPUTS;
}

\.outputs	{ 
	/* OUTPUTS list start. */
	fprintf( mvsis_gate_out, "%3d: OUTPUTS: \"%s\"\n", yylineno, yytext);
	return OUTPUTS;
}


\.default_input_arrival	{ 
	/* DEFINE directive. */
	fprintf( mvsis_gate_out, "%3d: DEFAULT_INPUT_ARRIVAL: \"%s\"\n", yylineno, yytext);
	return DEFAULT_INPUT_ARRIVAL;
}

\.end	{
	/* END directive. */
	fprintf( mvsis_gate_out, "%3d: END: \"%s\"\n", yylineno, yytext);
	return END;
}

\.gate	{
	/* GATE directive. */
	fprintf( mvsis_gate_out, "%3d: GATE: \"%s\"\n", yylineno, yytext);
	return GATE;
}

MAJ3	{ 
	/* MAJ3. */
	fprintf( mvsis_gate_out, "%3d: MAJ3: \"%s\"\n", yylineno, yytext);
	return MAJ;
}

AND2	{ 
	/* AND2. */
	fprintf( mvsis_gate_out, "%3d: AND2: \"%s\"\n", yylineno, yytext);
	return AND;
}

NAND2	{ 
	/* NAND2. */
	fprintf( mvsis_gate_out, "%3d: NAND2: \"%s\"\n", yylineno, yytext);
	return NAND;
}

OR2	{ 
	/* OR2. */
	fprintf( mvsis_gate_out, "%3d: OR2: \"%s\"\n", yylineno, yytext);
	return OR;
}

NOR2	{ 
	/* NOR2. */
	fprintf( mvsis_gate_out, "%3d: NOR2: \"%s\"\n", yylineno, yytext);
	return NOR;
}

INV	{ 
	/* Inversor. */
	fprintf( mvsis_gate_out, "%3d: INV: \"%s\"\n", yylineno, yytext);
	return INV;
}

{id}	{
	/* Identificador de variavel. */
	fprintf( mvsis_gate_out, "%3d: Identificador: \"%s\"\n", yylineno, yytext);
	mvsis_gate_lval.sval = ( char* )malloc( sizeof( char ) * ( strlen( yytext ) + 1 ) );
	strcpy( mvsis_gate_lval.sval, yytext );
	return ID;
}

{inteiro}	{
	/* Numero inteiro. */
	fprintf( mvsis_gate_out, "%3d: Inteiro: \"%s\"\n", yylineno, yytext);
	/* mvsis_gate.ival = atoi( yytext ); */
	mvsis_gate_lval.sval = ( char* )malloc( sizeof( char ) * ( strlen( yytext ) + 1 ) );
	strcpy( mvsis_gate_lval.sval, yytext );
	return INTEIRO;
}

\=	{
	/* Caracter especial. */
	fprintf( mvsis_gate_out, "%3d: Caracter especial: \"%c\"\n", yylineno, *yytext);
	return *yytext;
}

{espaco}	{
	/* Espaco, tab (horiz. ou vert.) , nova linha */
	;
}

"/*"	{
	/* Ignora comentarios. */
	char m,n;
	m = input();
	n = input();
	while( /* printf("[m,n] = [%c,%c]\n",m,n), */ ( m != '*' ) || ( n != '/') ) {
		m = n;
		n = input();
	}
}

[.]+	{ 
	/* Erro: palavra desconhecida. */
	fprintf( mvsis_gate_out, "%3d: Erro: palavra desconhecida: \"%s\"\n", yylineno, yytext);
}

.	{ 
	/* Erro: caracter desconhecido. */
	fprintf( mvsis_gate_out, "%3d: Erro: caracter desconhecido: \"%s\"\n", yylineno, yytext);
}

%%


