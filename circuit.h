/*
 * Ficheiro: circuit.h
 *
 * Estruturas de dados que suportam as tabelas de identificadores.
 * Cabecalhos das funcoes de manipulacao das tabelas de identificadores.
 * 
 */

#ifndef _CIRCUIT_H_
#define _CIRCUIT_H_

#include"yacc_util.h"
#include"stack.h"
#include"hash.h"

/* Constants. */
#define LONG_INT_STRING 11
/* (unsigned long long int)0xffffffff => "4294967295" + '\0' */

/* Logic function types. */
typedef enum {
	p_input,/* Primary input. */
	maj,	/* Majority gate. */
	and,	/* Logic and gate. */
	nand,	/* Logic nand gate. */
	or,	/* Logic or gate. */
	nor,	/* Logic nor gate. */
	inv	/* Logic inverter. */
}function_type;

/* Placement information. */
typedef struct {
	int x;
	int y;	/* This value has strong relation to the graph level of
		 * the gate. */
	unsigned int n;	/* Numbering within it's level (along X). */
	unsigned int l;	/* Graph level (distance to the farthest input).*/
	int clk_out;	/* Output clock zone. */
	int y_offset;	/* Y offset in cell units. */
	int x_final;	/* The coordinates were it is drawn. */
	int y_final;
}placement;

/* Logic gate data structure. */
typedef struct Gate_Str {
	char* id;		/* Nome do simbolo. */
	function_type t;	/* Conteudo do simbolo. */
	struct Gate_Str** inputs;	/* Vector de entradas (fanin). */
	unsigned short int n_inputs;	/* Numero de entradas. */
	boolean p_output;	/* Is it a primary output? */
	boolean visited;	/* Visited flag for graph algorithms. */
	boolean queued;		/* Queued flag for graph algorithms. */
	placement place;	/* Where the gate will be placed. */
	unsigned long int n_copies;	/* Number of copies. */
	Item* copy_stack;	/* List of all it's copies, in a stack. */
	struct Gate_Str* copy_of;	/* Pointer to the original if it is a
					 * copy, NULL if it is the original. */
}Gate;
typedef Gate* gate;

/* Logic circuit data structure. */
typedef struct Circuit_Str {
	Item** gates;		/* A hash structure for the gates. */
	unsigned short int n_outputs;	/* The number of outputs. */
	gate* outputs;		/* Output vector. */
	unsigned short int n_inputs;	/* The number of inputs. */
	gate* inputs;		/* Input vector. */
	int x_max;		/* Maximum width of all the logic networks. */
	int y_max;		/* Maximum depth of all the logic networks. */
				/* This may become an array, with an element 
				 * for each output's sub-network. */
}Circuit;
typedef Circuit* circuit;

/* Prototipos de funcoes. */
gate new_gate( char* id, function_type t, gate* inputs, int n_inputs );
void kill_gate( void* dados );
void reset_visited_gate( void* dados );

circuit new_cir( );
void kill_cir( circuit cir );

gate sub_cir_duplicate( circuit cir, gate root );
int cir_graph_level( gate root );
gate insert_gate( circuit cir, gate g );
gate find_gate( circuit cir, char* id );
void cir_do( circuit cir, void (*func)() );
int id_match( void* ref, void* dados );

void print_gate( void* dados );
/* void print_gate( FILE* fp, void* dados ); */

circuit cir_split( circuit cir );

/* Some usefull MACROS. */
#define ABS(a)		( ((a)<0)? -(a) : (a) )
#define MAX_2(a,b)	( ((a)>(b))? (a) : (b) )
#define MIN_2(a,b)	( ((a)<(b))? (a) : (b) )
#define MAX_3(a,b,c)	( ((a)>(b))? MAX_2((a),(c)) : MAX_2((b),(c)) )

#endif
