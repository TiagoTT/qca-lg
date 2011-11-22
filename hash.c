/*
 * Ficheiro: hash.c
 *
 * Modulo de manipulacao de uma tabela de dispersao.
 * Hash manipulation module.
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"errors.h"
#include"circuit.h"
#include"hash.h"

/*
 * Aloca memoria para a tabela, inicializa-a e devolve-a.
 * Allocates memory for the table, initializes and returns it.
 */
Item** hash_init(void) {
	Item** tabela;

	tabela = (Item**)malloc( HASH_SIZE * sizeof(Item*) );
	if( NULL == tabela ) {
		fprintf(stderr, "Memory allocation error at HASH_INIT.\n");
		exit( MEM_ALLOC_ERROR );
	}
	/* NULL = 0 */
	memset( tabela, 0, HASH_SIZE * sizeof(Item*) );

	return tabela;
}

/*
 * Insere um elemento na tabela de dispersao.
 * Inserts an element in the hash table.
 */
void hash_insert( Item** tabela, void* dados ) {
	stack_push( tabela + hash_function( dados ) , dados );
	return;
}

/*
 * Encontra um elemento dada uma referencia, e devolve-o.
 * Finds an element given a reference, and returns it. 
 */
void* hash_find( Item** tabela, int (*comparador)(), void* ref ) {
	return stack_find( tabela[ hash_function( ref ) ], comparador, ref );
}

/*
 * Elimina a tabela de dispersao, utilizando a funcao matador() para libertar
 * os recursos alocados para cada bloco de dados.
 * Deletes the hash table, using the given function matador() to free the 
 * allocated resources of each dados, (dados is the payload of an Item).
 */
void hash_kill( Item** tabela, void (*matador)() ) {
	int i;

	for( i = 0 ; i < HASH_SIZE ; i++ ) {
		while( NULL != tabela[ i ] ) {
			matador( stack_pop( tabela + i ) );
		}
	}
	free( tabela );

	return;	
}

/*
 * Aplica a funcao func a todos os elementos da tabela de dispersao.
 * Call func function over each element in the hash table.
 */
void hash_do( Item** tabela, void (*func)() ) {
	int i;

	for( i = 0 ; i < HASH_SIZE ; i++ ) {
		stack_do( tabela[ i ], func );
	}
	return;	
}

/*
 * Funcao de dispersao.
 * Hash function.
 */
unsigned int hash_function( void* dados ) {
	unsigned int val;
	char* str;
	
	str = ((gate)dados) -> id;
	for( val = 0 ; '\0' != *str ; str++ ) {
		val += *str;
	}
	
	return val % HASH_SIZE;
}

/*
 * Apresenta a pilha.
 * Shows the stack.
 */
void hash_show( Item** tabela, FILE* out ) {
	FILE* fp;
	int i;

	if( NULL == out ) { 
		fp = stdout;
	}else{	
		fp = out;
	}
	
	fprintf( fp, "Showing hash.\n");
	for( i = 0 ; i < HASH_SIZE ; i++ ) {
		fprintf( fp, "\tPrinting hash table[%d]\n", i );
		stack_show( tabela[ i ], fp );
	}
	fprintf( fp, "Done printing hash.\n");
	
	return;
}

/*
 * Testes.
 */
/*
int main() {
	Item** tabela;

	int comp( void* a, void* b ) {
		int dif;

		dif = (int)a - (int)b;
		if(dif)
			printf("a != b\n");
		else
			printf("a == b\n");

		return dif;
	}

	tabela = hash_init();

	hash_insert( tabela, (void*)3 ); hash_show( tabela, NULL );
	hash_find( tabela, comp, (void*)13 );
	hash_insert( tabela, (void*)1 ); hash_show( tabela, NULL );
	hash_insert( tabela, (void*)13 ); hash_show( tabela, NULL );
	hash_find( tabela, comp, (void*)13 );
	hash_insert( tabela, (void*)7 ); hash_show( tabela, NULL );
	hash_find( tabela, comp, (void*)3 );
	
	exit(0);
}
*/
/*
 * EOF
 */
