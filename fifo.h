/*
 * Ficheiro: fifo.h
 *
 * Estruturas, definicoes e prototipos da implementacao da fila FIFO.
 * 
 */

#ifndef _FIFO_H_
#define _FIFO_H_

/*
 * Estruturas de dados.
 */

/* Elemento da lista. */
typedef struct ListElementStr{
	void* data;
	struct ListElementStr* next;
}ListElement;

/* Suporte da lista. */
typedef struct ListStr {
	ListElement* head;	/* inicio da lista */
	ListElement* tail;	/* fim da lista */
	unsigned int size;	/* numero de elementos da lista */
}List;
typedef List* fifo;

/* Funcoes usadas directamente no SPL. */
fifo fifo_init();
void fifo_kill( fifo q );
unsigned int fifo_length( fifo q );
void* fifo_first( fifo q );
fifo fifo_rest( fifo q );
fifo fifo_append( fifo q, void* data );

#endif

/*
 * EOF
 */
