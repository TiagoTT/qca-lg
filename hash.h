/*
 * Ficheiro: hash.h
 *
 * Modulo de manipulacao de uma tabela de dispersao.
 * Hash manipulation module.
 */

#ifndef _HASH_H_
#define _HASH_H_

#include"stack.h"

#define HASH_SIZE 100

/* Funcoes publicas. */
Item** hash_init(void);
void hash_insert( Item** tabela, void* dados );
void* hash_find( Item** tabela, int (*comparador)(), void* ref );
void hash_kill( Item** tabela, void (*matador)() );
void hash_do( Item** tabela, void (*func)() );

/* Funcoes privadas. */
unsigned int hash_function( void* dados );
void hash_show( Item** tabela, FILE* out );

#endif

/*
 * EOF
 */
