%{
/* Analise e classificacao do input. */
#include<stdio.h>
#include<stdlib.h>

#include"stack.h"
#include"circuit.h"
#include"yacc_util.h"
#include"lsi.tab.h"

extern FILE* lsi_out;

%}

%option yylineno

id	[_a-zA-Z][_a-zA-Z0-9]*
inteiro	[0-9]+
espaco	[\ \t\n\v]

%%

COMPILE	{
	/* COMPILE directive. */
	fprintf( lsi_out, "%3d: COMPILE: \"%s\"\n", yylineno, yytext);
	return COMPILE;
}

DIRECTORY	{
	/* DIRECTORY identification ?. */
	fprintf( lsi_out, "%3d: DIRECTORY: \"%s\"\n", yylineno, yytext);
	return DIRECTORY;
}

MODULE	{
	/* MODULE identification. */
	fprintf( lsi_out, "%3d: MODULE: \"%s\"\n", yylineno, yytext);
	return MODULE;
}

INPUTS	{ 
	/* INPUTS list start. */
	fprintf( lsi_out, "%3d: INPUTS: \"%s\"\n", yylineno, yytext);
	return INPUTS;
}

OUTPUTS	{ 
	/* OUTPUTS list start. */
	fprintf( lsi_out, "%3d: OUTPUTS: \"%s\"\n", yylineno, yytext);
	return OUTPUTS;
}

LEVEL	{ 
	/* LEVEL directive. */
	fprintf( lsi_out, "%3d: LEVEL: \"%s\"\n", yylineno, yytext);
	return LEVEL;
}

DEFINE	{ 
	/* DEFINE directive. */
	fprintf( lsi_out, "%3d: DEFINE: \"%s\"\n", yylineno, yytext);
	return DEFINE;
}

END	{
	/* END directive. */
	fprintf( lsi_out, "%3d: END: \"%s\"\n", yylineno, yytext);
	return END;
}

MAJ3	{ 
	/* MAJ3. */
	fprintf( lsi_out, "%3d: MAJ3: \"%s\"\n", yylineno, yytext);
	return MAJ;
}

AND2	{ 
	/* AND2. */
	fprintf( lsi_out, "%3d: AND2: \"%s\"\n", yylineno, yytext);
	return AND;
}

NAND2	{ 
	/* NAND2. */
	fprintf( lsi_out, "%3d: NAND2: \"%s\"\n", yylineno, yytext);
	return NAND;
}

OR2	{ 
	/* OR2. */
	fprintf( lsi_out, "%3d: OR2: \"%s\"\n", yylineno, yytext);
	return OR;
}

NOR2	{ 
	/* NOR2. */
	fprintf( lsi_out, "%3d: NOR2: \"%s\"\n", yylineno, yytext);
	return NOR;
}

INV	{ 
	/* Inversor. */
	fprintf( lsi_out, "%3d: INV: \"%s\"\n", yylineno, yytext);
	return INV;
}

{id}	{
	/* Identificador de variavel. */
	fprintf( lsi_out, "%3d: Identificador: \"%s\"\n", yylineno, yytext);
	lsi_lval.sval = ( char* )malloc( sizeof( char ) * ( strlen( yytext ) + 1 ) );
	strcpy( lsi_lval.sval, yytext );
	return ID;
}

[\(\)\=\,\;]	{
	/* Caracter especial. */
	fprintf( lsi_out, "%3d: Caracter especial: \"%c\"\n", yylineno, *yytext);
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
	fprintf( lsi_out, "%3d: Erro: palavra desconhecida: \"%s\"\n", yylineno, yytext);
}

.	{ 
	/* Erro: caracter desconhecido. */
	fprintf( lsi_out, "%3d: Erro: caracter desconhecido: \"%s\"\n", yylineno, yytext);
}

%%


