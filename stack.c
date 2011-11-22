/*
 * Ficheiro: stack.c
 *
 * Modulo de manipulacao de uma pilha.
 * Stack manipulation module.
 */

#include<stdio.h>
#include<stdlib.h>

#include"errors.h"
#include"stack.h"

/*
 * Adiciona o novo elemento no topo da pilha.
 * Add the new element at the top of the stack.
 */
void stack_push( Item** base, void* dados ) {
	Item* novo;

	novo = (Item*)malloc( sizeof(Item) );
	if( NULL == novo ) {
		fprintf(stderr, "Memory allocation error at stack_push().\n");
		exit( MEM_ALLOC_ERROR );
	}
	novo -> dados = dados;
	novo -> next = *base;

	*base = novo;
	
	return;
}

/*
 * Remove o elemento que se encontra no topo da pilha, e devolve-o.
 * Removes the element at the top of the stack, and returns it.
 */
void* stack_pop( Item** base ) {
	Item* topo;
	void* dados;

	if( NULL == *base) {
		fprintf(stderr,"Atempt to POP from an empty stack.\n");
		return NULL;
	}
	
	topo = (*base);
	dados = topo -> dados;
	(*base) = topo -> next;
	free( topo );
	
	return dados;
}

/*
 * Encontra um elemento dada uma referencia, e devolve-o.
 * Finds an element given a reference, and returns it.
 */
void* stack_find( Item* base, int (*comparador)(), void* ref ) {
	Item* it;
	
	for( it = base ; NULL != it ; it = it -> next ) {
		if( !comparador( ref, it -> dados ) ) {
			return it -> dados;
		}
	}

	return NULL;
}

void stack_do( Item* base, void (*func)() ) {
	Item* it;
	
	for( it = base ; NULL != it ; it = it -> next ) {
		func( it -> dados );
	}
}

/*
 * Apresenta a pilha.
 * Shows the stack.
 */
void stack_show( Item* base, FILE* out ) {
	FILE* fp;
	Item* it;
	int i;

	if( NULL == out ) { 
		fp = stdout;
	}else{	
		fp = out;
	}
	
	fprintf( fp, "\tShowing stack, starting from top:");
	for( i = 0, it = base ; NULL != it ; i++, it = it -> next ) {
		fprintf( fp, " Stack[%d]->dados=%d;", i, (int)(it -> dados) );
	}
	fprintf( fp, "\n");
	
	return;
}

/*
 * Testes
 */
/*
int main() {
	Item* base = NULL;

	int comp( void* a, void* b ) {
		int dif;
		
		dif = (int)a - (int)b;
		if(dif)
			printf("a != b\n");
		else 
			printf("a == b\n");
		
		return dif;
	}
	
	stack_show(base,NULL);
	stack_push(&base,(void*)1);stack_show(base,NULL);
	stack_push(&base,(void*)3);stack_show(base,NULL);
	stack_find(base,comp,(void*)5);
	stack_push(&base,(void*)5);stack_show(base,NULL);
	stack_find(base,comp,(void*)1);
	stack_push(&base,(void*)7);stack_show(base,NULL);
	stack_pop(&base);stack_show(base,NULL);
	stack_pop(&base);stack_show(base,NULL);
	stack_pop(&base);stack_show(base,NULL);
	stack_pop(&base);stack_show(base,NULL);

	exit(0);
}
*/
/*
 * EOF
 */
