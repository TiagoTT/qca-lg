/*
 * Ficheiro: circuit.c
 *
 * Codigo das funcoes de manipulacao das tabelas de identificadores.
 * 
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"errors.h"
#include"circuit.h"
#include"fifo.h"

/*
 * Cria uma nova porta logica e retorna um ponteiro para ela.
 * 
 * Creates a new logic gate and returns a pointer to it.
 */
gate new_gate( char* id, function_type t, gate* inputs, int n_inputs ) {
	gate g;

	g = (gate)malloc( sizeof( Gate ) );
	if( NULL == g ) {
		fprintf(stderr, "ERR: malloc() = NULL in new_gate().\n");
		exit( MEM_ALLOC_ERROR );
	}

	g -> id = id;
	g -> t = t;
	g -> inputs = inputs;
	g -> n_inputs = n_inputs;
	g -> p_output = no;
	g -> visited = no;
	g -> queued = no;
	g -> place.x = g -> place.y 
		= g -> place.n = g -> place.l 
		= g -> place.clk_out = g -> place.y_offset
		= g -> place.x_final = g -> place.y_final = 0;
	g -> n_copies = 0;
	g -> copy_stack = NULL;
	g -> copy_of = NULL;

	return g;
}

/*
 * Liberta os recursos alocados para esta porta logica.
 * 
 * Frees the resources allocated to this gate.
 */
void kill_gate( void* dados ) {
	free( ((gate)dados) -> id );		/* char* id */
	free( ((gate)dados) -> inputs );	/* gate* inputs */
	while( NULL != ((gate)dados) -> copy_stack  ) {
		stack_pop( &(((gate)dados) -> copy_stack) );
	}
	free( ((gate)dados) );
}

/*
 * Resets the visited field of a gate to "no".
 */
void reset_visited_gate( void* dados ) {
	((gate)dados) -> visited = no;
	((gate)dados) -> queued = no;
}

/*
 * Cria um novo circuito (uma tabela de dispersao).
 * 
 * Allocates resources to a new circuit (represented in a hash table)
 */
circuit new_cir( ) {
	circuit c;

	c = (circuit)malloc( sizeof( Circuit ) );
	if( NULL == c ) {
		fprintf(stderr, "ERR: malloc() = NULL in cir_init().\n");
		exit( MEM_ALLOC_ERROR );
	}

	c -> gates = hash_init();
	c -> n_outputs = 0;
	c -> outputs = NULL;
	c -> n_inputs = 0;
	c -> inputs = NULL;
	c -> x_max = 0;
	c -> y_max = 0;

	return c;
}

/*
 * Destroi o circuito.
 * 
 * Frees the resources allocated for the circuit.
 */
void kill_cir( circuit cir ) {
	hash_kill( cir -> gates, kill_gate );
	if( NULL != cir -> outputs )
		free( cir -> outputs );
	if( NULL != cir -> inputs )
		free( cir -> inputs );
	free( cir );
}

/*
 * Duplicates the sub-circuit rooted at a specific gate.
 * Returns the root gate of the copied tree.
 */
gate sub_cir_duplicate( circuit cir, gate root ) {
	int j;
	char number[ LONG_INT_STRING ];
	gate orig, copy, *inputs;
	
	/* Copy the gate, creating a new inputs array, 
	 * and changing the ID of the copy. */
	
	/* Check if the given root is the original, if not get the original.
	 * The original is needed for copy control ONLY. */
	if( NULL != root -> copy_of ) {
		orig = root -> copy_of;
	}else{
		orig = root;
	}

	/* The original gate's ID will NOT be changed 
	 * to avoid the name collision of second, third, forth... copies, 
	 * the copies name have a numeric suffix. */
	snprintf( number, LONG_INT_STRING, "%lu", ++ (orig -> n_copies) );

	inputs = ( gate* )malloc( root->n_inputs * sizeof(gate*)  );
	if( NULL == inputs ) {
		fprintf(stderr,"ERR: malloc() = NULL in sub_cir_duplicate.\n");
		exit( MEM_ALLOC_ERROR );
	}
	copy = new_gate( cola( orig -> id, number ),
			root -> t, inputs, root -> n_inputs );
	copy -> p_output = root -> p_output;
	copy -> visited = root -> visited;
	copy -> queued = root -> queued;
	copy -> place = root -> place;

	/* This stack will contain all the copies of the gate. */
	stack_push( &(orig -> copy_stack), (void*)copy );
	/* Record in the copy who is the original. */
	copy -> copy_of = orig;

	/* Insert the copy into the circuit. */
	insert_gate( cir, copy );

	/* Iterate over this gate's inputs. */
	for( j = 0 ; j < root -> n_inputs ; j++ ) {
		/* Recursively call sub_cir_duplicate(). */
		copy->inputs[ j ] = sub_cir_duplicate( cir, root->inputs[ j ] );
	}

	return copy;
}

/*
 * Determines the distance of each gate to the farthest input it depends on.
 * Starts from root and explores towards the inputs.
 */
int cir_graph_level( gate root ) {
	
	switch( root -> t ) {
		case p_input:
			root -> place.l = 0; /* It was already... */
			return 1;
			break;
		case inv:
			/* The INVs are not considered. */
			if( 0 == root -> n_inputs )
				/* TODO Arranjar isto. Quando sao declaradas
				 * saidas que nao se usam, ficam portas INV
				 * sem entradas e rebenta aqui. E quem sabe
				 * onde mais. IMPORTANTE. */
				return 0;
			root -> place.l = cir_graph_level( root->inputs[0] );
			return root -> place.l;
			break;
		default:
			switch( root -> n_inputs ) {
				case 2:
					root -> place.l = MAX_2(
					cir_graph_level( root->inputs[0] ),
					cir_graph_level( root->inputs[1] ) );
					break;
				case 3:
					root -> place.l = MAX_3(
					cir_graph_level( root->inputs[0] ),
					cir_graph_level( root->inputs[1] ),
					cir_graph_level( root->inputs[2] ) );
					break;
			}
	}
	return root -> place.l + 1;
}

/*
 * Insere uma porta no circuito. Se a pora ja existir devolve um apontador 
 * para a mesma, caso contrario devolve NULL.
 * 
 * Inserts a gate in the circuit (the circuit is supported by a hash table).
 * If the gate is already in the circuit a pointer to it is returned, else NULL
 * is returned.
 */
gate insert_gate( circuit cir, gate g ) {
	gate previous;
	
	if( NULL == cir ) {
		fprintf( stderr, "Err: Trying to add a gate" \
				"to an uninitialized circuit.\n");
		exit( CIRCUIT_ERROR );
	}
	if( NULL == cir -> gates ) {
		fprintf(stderr,"Err: Trying to add a gate to an uninitialized circuit.\n");
		exit( CIRCUIT_ERROR );
	}
	
	if( NULL != ( previous = hash_find( cir -> gates,
					id_match, (void*)g ) ) ) {
		return previous;
	}

	hash_insert( cir -> gates, (void*)g );
	return NULL;
}


/*
 * Procura uma porta pelo nome. Devolve um ponteiro para 
 * essa porta caso a encontre, caso contrario devolve NULL.
 * 
 * Finds a gate by it's name. Returns a pointer to it if found, 
 * else returns NULL.
 */
gate find_gate( circuit cir, char* name ) {
	gate id;
	gate ref;

	/* Apenas interessa definir o nome. */
	ref = new_gate( name, inv /* tanto faz */, NULL, 0 );
	
	if( NULL != cir && NULL != cir -> gates ) {
		if( NULL != ( id = hash_find( cir -> gates, 
						id_match, (void*)ref ) ) ) {
			free( ref );
			return id;
		}
	}
	/* Nao se faz free do name, pois quem faz a pesquisa nao 
	 * ia achar piada se a sua string desaparecesse. 
	 */
	free( ref );
		
	return NULL;
}

/*
 * Funcao que avalia a correspondencia entre um nome e um identificador.
 * Serve para encontrar um identificador pelo nome.
 * Devolve zero caso tenha sucesso.
 */
int id_match( void* ref, void* dados ) {
	return strcmp( ( (gate)ref) -> id,
			( (gate)dados) -> id );
}

/*
 * Aplica a funcao func a todos os elementos da tabela de dispersao.
 * 
 * Calls function func over all the gates.
 */
void cir_do( circuit cir, void (*func)() ) {

	if( NULL == cir || NULL == cir -> gates ) {
		fprintf(stderr,"Err: Trying to operate over an empty circuit in cir_do().\n");
		exit( CIRCUIT_ERROR );
	}

	hash_do( cir -> gates, func );
}

/* 
 * All the code below this point is for test purpose.
 */

/*
 * Imprime informacao sobre uma porta logica para o stdout.
 * 
 * Prints the gate info to the stdout.
 */
void print_gate( void* dados ) {
	int i;
	
	fprintf(stderr, "Gate( id=%s, type=%d",
			((gate)dados) -> id,
			((gate)dados) -> t );
	
	if( NULL != ((gate)dados) -> inputs ) {
		for( i = 0 ; i < ((gate)dados) -> n_inputs ; i++ ) {
			fprintf(stderr,
				", %s",((gate)dados) -> inputs [i] -> id );
		}
	}else{
		fprintf(stderr, ", (no inputs)" );
	}
	fprintf(stderr, ", place = (x=%d,y=%d,n=%d,l=%d)", 
			((gate)dados) -> place.x,
			((gate)dados) -> place.y, ((gate)dados) -> place.n,
			((gate)dados) -> place.l );
	fprintf(stderr, " );%s\n", (((gate)dados) -> p_output)? 
			" <primary output>":""  );

	return;
}



/*
 * EOF
 */
