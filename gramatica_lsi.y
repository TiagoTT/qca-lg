%{
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#include"yacc_util.h"
#include"circuit.h"
#include"stack.h"
#include"hash.h"

/* Estas variaveis sao definidas pelo lex. */
extern int lsi_lineno;
extern char* lsi_text;
extern FILE* lsi_in;/* ainda nao e utilizado*/

/* FILE* lsi_out = NULL; */
extern FILE* lsi_out;
circuit lsi_cir = NULL;

boolean lsi_input_def = no;
boolean lsi_output_def = no;

/* Funcoes. */
int lsi_error( char* error );
int lsi_lex(void);
%}

%token COMPILE DIRECTORY MODULE INPUTS OUTPUTS LEVEL DEFINE END 
%token MAJ AND NAND OR NOR INV
%union{
	int ival;	/* inteiro */
	char* sval;	/* string */
	Item* stack;	/* stack de objectos */
	function_type type;	/* enum { maj, and, or, inv } */
}
%token <sval> ID

//%type <ival>
%type <sval> CONNECT
%type <stack> FAN_IN ID_LIST
%type <type> GATE_TYPE

%%

CIRCUIT_LOGIC:
LOGIC	{
	/* Final production. */
	fprintf( lsi_out, "CIRCUIT_LOGIC <- LOGIC (Final production.)\n" );
}
;

LOGIC:
COMPILE ';' DIRECTORY ID ';' MODULE ID ';' MODULE_DEF END MODULE ';' END COMPILE ';' END ';'	{
	/* ?. */
	fprintf( lsi_out, "LOGIC <- ...\n" );
	free( $4 );
	free( $7 );
}
;

MODULE_DEF:
INPUT_DEF OUTPUT_DEF LEVEL ID ';' DEFINE WIRES GATES	{
	fprintf( lsi_out, "MODULE_DEF <- ...\n" );
	free( $4 );
}
;

INPUT_DEF:
INPUT_LIST_START ID_LIST ';'	{
	unsigned short int n_input, i;
	Item* it;

	fprintf( lsi_out, "INPUT_DEF <- ...\n" );
	/* End of the input list. */
	lsi_input_def = no;

	/* input counting */
	for( n_input = 0, it = $2 ; NULL != it ; it = it -> next, n_input++)
		;
	lsi_cir -> n_inputs = n_input;
	/* input array */
	if( NULL == ( lsi_cir -> inputs = 
			(gate*)malloc( (n_input)*sizeof(gate) ) ) ) {
		fprintf( stderr, "Err: malloc = NULL in input vector allocation.\n" );
		exit( -1 );
	}
	/* input array filling */
	for( i = 0 ; i < n_input ; i++ ) {
		lsi_cir -> inputs[ i ] = (gate)stack_pop( &($2) );
	}
}
;

INPUT_LIST_START:
INPUTS	{
	/* Beginning of the input list. */
	lsi_input_def = yes;
}
;

OUTPUT_DEF:
OUTPUT_LIST_START ID_LIST ';'	{
	unsigned short int n_output, i;
	Item* it;

	fprintf( lsi_out, "OUTPUT_DEF <- ...\n" );
	/* End of the output list. */
	lsi_output_def = no;
	
	/* output counting */
	for( n_output = 0, it = $2 ; NULL != it ; it = it -> next, n_output++)
		;
	lsi_cir -> n_outputs = n_output;
	/* output array */
	if( NULL == ( lsi_cir -> outputs = 
			(gate*)malloc( (n_output)*sizeof(gate) ) ) ) {
		fprintf( stderr, "Err: malloc = NULL in output vector allocation.\n" );
		exit( -1 );
	}
	/* output array filling */
	for( i = 0 ; i < n_output ; i++ ) {
		lsi_cir -> outputs[ i ] = (gate)stack_pop( &($2) );
	}
}
;

OUTPUT_LIST_START:
OUTPUTS	{
	/* Beginning of the output list. */
	lsi_output_def = yes;
}
;

ID_LIST:
ID	{
	gate g;
	/* gate insertion */
	if( NULL == ( g = find_gate( lsi_cir, $1 ) ) ) {
		/* this gate is new */
		insert_gate( lsi_cir, g = new_gate( 
				$1, (lsi_input_def)? p_input : inv , NULL, 0 ) );
		if( lsi_output_def ) {
			g -> p_output = yes;
			/* Insert this output in the stack. */
			$$ = NULL;	/* stack initialization ! */
			stack_push( &($$), (void*)g );
		}else if( lsi_input_def ) {
			/* Insert this input in the stack. */
			$$ = NULL;	/* stack initialization ! */
			stack_push( &($$), (void*)g );
		}
		/* keep $1 inside the gate */
	}else{
		/* this gate was already delared */
		fprintf( stderr, "Warn: Redeclaration of gate (%s), first declaration is considered here.\n", $1 );
		free( $1 );
	}
}
| ID_LIST ',' ID	{
	gate g;
	/* gate insertion */
	if( NULL == ( g = find_gate( lsi_cir, $3 ) ) ) {
		/* this gate is new */
		insert_gate( lsi_cir, g = new_gate( 
				$3, (lsi_input_def)? p_input : inv , NULL, 0 ) );
		if( lsi_output_def ) {
			g -> p_output = yes;
			/* Insert this output in the stack. */
			$$ = $1;
			stack_push( &($$), (void*)g );
		}else if( lsi_input_def ) {
			/* Insert this input in the stack. */
			$$ = $1;
			stack_push( &($$), (void*)g );
		}
		/* keep $1 inside the gate */
	}else{
		/* this gate was already delared */
		fprintf( stderr, "Warn: Redeclaration of gate (%s), first declaration is considered here.\n", $3 );
		free( $3 );
	}
}
;

WIRES:
WIRE
| WIRES WIRE
;

WIRE:
ID '=' '(' ID ')' ';'	{
	/* Nao sei para que e' que isto serve, e' redundante. */
	fprintf( lsi_out, "WIRE <- ...\n" );
	free( $1 );
	free( $4 );
}
;

GATES:
GATE
| GATES GATE
;

GATE:
ID '(' CONNECT ')' '=' GATE_TYPE '(' FAN_IN ')' ';'	{
	unsigned short int n_input, i;
	Item* it;
	char* s;
	gate g, *inputs;

	fprintf( lsi_out, "GATE <- ...\n" );

	/* input counting */
	for( n_input = 0, it = $8 ; NULL != it ; it = it -> next, n_input++)
		;
	/* input array */
	if( NULL == ( inputs = (gate*)malloc( (n_input)*sizeof(gate) ) ) ) {
		fprintf( stderr, "Err: malloc = NULL in input vector allocation.\n" );
		exit( -1 );
	}
	/* input checking */
	for( i = 0 ; i < n_input ; i++ ) {
		s = (char*)stack_pop( &($8) );
		if( NULL == ( g = find_gate( lsi_cir, s ) ) ) {
			/* This is the first reference to this input node. We
			have to add it to the circuit, it's entry will be filled
			later. It is inconsistent (inv without input), I know*/
			inputs[ i ] = new_gate( s, inv, NULL, 0 );
			insert_gate( lsi_cir, inputs[ i ] );
			/* keep s inside the gate */
		}else{
			/* The refered node already exists in the circuit. */
			if( yes == g -> p_output ) {
				fprintf( stderr, "Warn: Primary output (%s) is used as input for (%s).\n", g -> id, $3 );
			}
			inputs[ i ] = g;
			free( s );
		}
	}

	/* gate insertion */
	if( NULL == ( g = find_gate( lsi_cir, $3 ) ) ) {
		/* this gate is new */
		insert_gate( lsi_cir, new_gate( $3, $6, inputs, n_input ) );
		/* keep $3 inside the gate */
	}else{
		/* this gate was already refered or delared */
		if( (NULL != g -> inputs) && (p_input != g -> t) ) {
			/* redeclaration */
			fprintf( stderr, "Warn: Redeclaration of gate (%s), first declaration is considered here.\n", $3);
			free( $3 );
			free( inputs );
		}else{
			/* gate was only refered before */
			g -> t = $6;
			g -> inputs = inputs;
			g -> n_inputs = n_input;
			free( $3 );
		}
	}
	free( $1 );
}
;

GATE_TYPE:
MAJ	{ $$ = maj; }
| AND	{ $$ = and; }
| NAND	{ $$ = nand; }
| OR	{ $$ = or; }
| NOR	{ $$ = nor; }
| INV	{ $$ = inv; }
;

FAN_IN:
CONNECT	{
	fprintf( lsi_out, "FAN_IN <- CONNECT(%s)\n", $1 );
	$$ = NULL; /* E' importante inicializar a stack! */
	stack_push( &($$), (void*)$1 );
}
| FAN_IN ',' CONNECT	{
	$$ = $1;
	fprintf( lsi_out, "FAN_IN <- FAN_IN , CONNECT(%s)\n", $3 );
	stack_push( &($$), (void*)$3 );
}
;

CONNECT:
ID '=' ID	{
	fprintf( lsi_out, "CONNECT <- %s (global id) = %s (gate id)\n", $1, $3 );
	$$ = $1;
	free( $3 );
}
;

%%

int lsi_error( char* msg ) {
	#define MAX_ERRORS 10
	static unsigned int numero_erro = 0;

	fprintf( stderr, "%3d: Erro(%d): \"%s\" %s\n", lsi_lineno, ++numero_erro, lsi_text, msg );
	if( lsi_nerrs >= MAX_ERRORS ) {
		fprintf( stderr, "O limite maximo de erros foi atingido. Bye!\n" );
		exit( 1 );
	}
	return 0;
}

/* Este main e' apenas para testar o parser. */
/*
int main( int argc, char* argv[], char* env[] ) {
	
	out = stdout;
	lsi_cir = cir_init();

	yyparse();
	
	cir_do( lsi_cir, print_gate );
	cir_kill( lsi_cir );

	exit(0);
}
*/
