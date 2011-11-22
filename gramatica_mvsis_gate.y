%{
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#include"yacc_util.h"
#include"circuit.h"
#include"stack.h"
#include"hash.h"

/* Variaveis globais. */
extern int mvsis_gate_lineno;
extern char* mvsis_gate_text;
extern FILE* mvsis_gate_in;/* ainda nao e utilizado*/

/* FILE* mvsis_gate_out = NULL; */
extern FILE* mvsis_gate_out;
circuit mvsis_gate_cir = NULL;

boolean mvsis_gate_input_def = no;
boolean mvsis_gate_output_def = no;

/* Funcoes. */
int mvsis_gate_error( char* error );
int mvsis_gate_lex(void);

%}

%token MODEL INPUTS OUTPUTS DEFAULT_INPUT_ARRIVAL GATE END
%token MAJ AND NAND OR NOR INV
%union{
	int ival;	/* inteiro */
	char* sval;	/* string */
	Item* stack;	/* stack de objectos */
	function_type type;	/* enum { maj, and, or, inv } */
}
%token <sval> ID INTEIRO

//%type <ival>
%type <sval> CONNECT
%type <stack> FAN_IN ID_LIST
%type <type> GATE_TYPE

%%

CIRCUIT_LOGIC:
LOGIC	{
	/* Final production. */
	fprintf( mvsis_gate_out, "CIRCUIT_LOGIC <- LOGIC (Final production.)\n" );
}
;

LOGIC:
MODEL ID INPUT_DEF OUTPUT_DEF DEFAULT_INPUT_ARRIVAL INTEIRO INTEIRO GATES END	{
	/* ?. */
	fprintf( mvsis_gate_out, "LOGIC <- ...\n" );
	free( $2 );
	free( $6 );
	free( $7 );
}
;

INPUT_DEF:
INPUT_LIST_START ID_LIST	{
	unsigned short int n_input, i;
	Item* it;

	fprintf( mvsis_gate_out, "INPUT_DEF <- ...\n" );
	/* End of the input list. */
	mvsis_gate_input_def = no;

	/* input counting */
	for( n_input = 0, it = $2 ; NULL != it ; it = it -> next, n_input++)
		;
	mvsis_gate_cir -> n_inputs = n_input;
	/* input array */
	if( NULL == ( mvsis_gate_cir -> inputs = 
			(gate*)malloc( (n_input)*sizeof(gate) ) ) ) {
		fprintf( stderr, "Err: malloc = NULL in input vector allocation.\n" );
		exit( -1 );
	}
	/* input array filling */
	for( i = 0 ; i < n_input ; i++ ) {
		mvsis_gate_cir -> inputs[ i ] = (gate)stack_pop( &($2) );
	}
}
;

INPUT_LIST_START:
INPUTS	{
	/* Beginning of the input list. */
	mvsis_gate_input_def = yes;
}
;

OUTPUT_DEF:
OUTPUT_LIST_START ID_LIST	{
	unsigned short int n_output, i;
	Item* it;

	fprintf( mvsis_gate_out, "OUTPUT_DEF <- ...\n" );
	/* End of the output list. */
	mvsis_gate_output_def = no;
	
	/* output counting */
	for( n_output = 0, it = $2 ; NULL != it ; it = it -> next, n_output++)
		;
	mvsis_gate_cir -> n_outputs = n_output;
	/* output array */
	if( NULL == ( mvsis_gate_cir -> outputs = 
			(gate*)malloc( (n_output)*sizeof(gate) ) ) ) {
		fprintf( stderr, "Err: malloc = NULL in input vector allocation.\n" );
		exit( -1 );
	}
	/* output array filling */
	for( i = 0 ; i < n_output ; i++ ) {
		mvsis_gate_cir -> outputs[ i ] = (gate)stack_pop( &($2) );
	}
}
;

OUTPUT_LIST_START:
OUTPUTS	{
	/* Beginning of the output list. */
	mvsis_gate_output_def = yes;
}
;

ID_LIST:
ID	{
	gate g;
	/* gate insertion */
	if( NULL == ( g = find_gate( mvsis_gate_cir, $1 ) ) ) {
		/* this gate is new */
		insert_gate( mvsis_gate_cir, g = new_gate( 
				$1, (mvsis_gate_input_def)? p_input : inv , NULL, 0 ) );
		if( mvsis_gate_output_def ) {
			g -> p_output = yes;
			/* Insert this output in the stack. */
			$$ = NULL;	/* stack initialization ! */
			stack_push( &($$), (void*)g );
		}else if( mvsis_gate_input_def ) {
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
| ID_LIST ID	{
	gate g;
	/* gate insertion */
	if( NULL == ( g = find_gate( mvsis_gate_cir, $2 ) ) ) {
		/* this gate is new */
		insert_gate( mvsis_gate_cir, g = new_gate( 
				$2, (mvsis_gate_input_def)? p_input : inv , NULL, 0 ) );
		if( mvsis_gate_output_def ) {
			g -> p_output = yes;
			/* Insert this output in the stack. */
			$$ = $1;
			stack_push( &($$), (void*)g );
		}else if( mvsis_gate_input_def ) {
			/* Insert this input in the stack. */
			$$ = $1;
			stack_push( &($$), (void*)g );
		}
		/* keep $1 inside the gate */
	}else{
		/* this gate was already delared */
		fprintf( stderr, "Warn: Redeclaration of gate (%s), first declaration is considered here.\n", $2 );
		free( $2 );
	}
}
;

GATES:
GATE_DEF
| GATES GATE_DEF
;

GATE_DEF:
GATE GATE_TYPE FAN_IN CONNECT	{
	unsigned short int n_input, i;
	Item* it;
	char* s;
	gate g, *inputs;

	fprintf( mvsis_gate_out, "GATE <- ...\n" );

	/* input counting */
	for( n_input = 0, it = $3 ; NULL != it ; it = it -> next, n_input++)
		;
	/* input array */
	if( NULL == ( inputs = (gate*)malloc( (n_input)*sizeof(gate) ) ) ) {
		fprintf( stderr, "Err: malloc = NULL in input vector allocation.\n" );
		exit( -1 );
	}
	/* input checking */
	for( i = 0 ; i < n_input ; i++ ) {
		s = (char*)stack_pop( &($3) );
		if( NULL == ( g = find_gate( mvsis_gate_cir, s ) ) ) {
			/* This is the first reference to this input node. We
			have to add it to the circuit, it's entry will be filled
			later. It is inconsistent (inv without input), I know*/
			inputs[ i ] = new_gate( s, inv, NULL, 0 );
			insert_gate( mvsis_gate_cir, inputs[ i ] );
			/* keep s inside the gate */
		}else{
			/* The refered node already exists in the circuit. */
			if( yes == g -> p_output ) {
				fprintf( stderr, "Warn: Primary output (%s) is used as input for (%s).\n", g -> id, $4 );
			}
			inputs[ i ] = g;
			free( s );
		}
	}

	/* gate insertion */
	if( NULL == ( g = find_gate( mvsis_gate_cir, $4 ) ) ) {
		/* this gate is new */
		insert_gate( mvsis_gate_cir, new_gate( $4, $2, inputs, n_input ) );
		/* keep $3 inside the gate */
	}else{
		/* this gate was already refered or delared */
		if( (NULL != g -> inputs) && (p_input != g -> t) ) {
			/* redeclaration */
			fprintf( stderr, "Warn: Redeclaration of gate (%s), first declaration is considered here.\n", $4);
			free( $4 );
			free( inputs );
		}else{
			/* gate was only refered before */
			g -> t = $2;
			g -> inputs = inputs;
			g -> n_inputs = n_input;
			free( $4 );
		}
	}

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
	fprintf( mvsis_gate_out, "FAN_IN <- CONNECT(%s)\n", $1 );
	$$ = NULL; /* E' importante inicializar a stack! */
	stack_push( &($$), (void*)$1 );
}
| FAN_IN CONNECT	{
	$$ = $1;
	fprintf( mvsis_gate_out, "FAN_IN <- FAN_IN CONNECT(%s)\n", $2 );
	stack_push( &($$), (void*)$2 );
}
;

CONNECT:
ID '=' ID	{
	fprintf( mvsis_gate_out, "CONNECT <- %s (gate id) = %s (global id)\n", $1, $3 );
	$$ = $3;
	free( $1 );
}
;

%%

int mvsis_gate_error( char* msg ) {
	#define MAX_ERRORS 10
	static unsigned int numero_erro = 0;

	fprintf( stderr, "%3d: Erro(%d): \"%s\" %s\n", mvsis_gate_lineno, ++numero_erro, mvsis_gate_text, msg );
	if( mvsis_gate_nerrs >= MAX_ERRORS ) {
		fprintf( stderr, "O limite maximo de erros foi atingido. Bye!\n" );
		exit( 1 );
	}
	return 0;
}

/* Este main e' apenas para testar o parser. */

#if PARSER_TEST

int main( int argc, char* argv[], char* env[] ) {
	
	out = stdout;
	mvsis_gate_cir = cir_init();

	mvsis_gate_parse();
	
	cir_do( mvsis_gate_cir, print_gate );
	cir_kill( mvsis_gate_cir );

	exit(0);
}

#endif
