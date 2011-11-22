/*
 * Ficheiro: fifo.c
 *
 * Linked list implementation of a FIFO queue of unlimited length.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include"errors.h"
#include "fifo.h"

/* 
 * Creates and returns a new fifo queue.
 */
fifo fifo_init() {
	fifo l;

	l = (fifo)malloc( sizeof(List) );
	if( NULL == l ) {
		fprintf(stderr, "ERROR: malloc() = NULL in fifo_init()\n");
		exit( MEM_ALLOC_ERROR );
	}

	l -> head = NULL;
	l -> tail = NULL;
	l -> size = 0;

	return l;
}

/* 
 * Frees the resources allocated for this fifo queue.
 */
void fifo_kill( fifo q ) {
	ListElement* it;
	ListElement* next;
	
	for( it = q -> head ; NULL != it ; it = next ) {
		next = it -> next;
		free( it );
	}
	free( q );
}

/*
 * Returns the status of the queue.
 */
unsigned int fifo_length( fifo q ) {
	return q -> size;
}

/*
 * Returns the first element in the queue, which is the oldest.
 */
void* fifo_first( fifo q ) {
	if( NULL == q -> head ) {
		fprintf(stderr,"WARNING: Trying to get first from an empty list.\n");
		return NULL;
	}
	return q -> head -> data;
}

/*
 * Removes the first element os the list and returns the new list.
 * The new list is the rest of the old one when the first element is removed.
 */
fifo fifo_rest( fifo q ) {
	ListElement* first;
	
	if( NULL == q -> head ) {
		fprintf(stderr,"WARNING: Trying to remove first from an empty list.\n");
		return q;
	}
	first = q -> head;
	
	if( NULL == first -> next ) {
		q -> head = q -> tail = NULL; /* the list is now empty */
	}else{
		q -> head = first -> next;
	}
	free( first );
	q -> size --;
	
	return q;
}

/*
 * Appends an element to the end of the queue.
 */
fifo fifo_append( fifo q, void* data ) {
	ListElement* last;
	
	last = (ListElement*)malloc( sizeof(ListElement) );
	if( NULL == last ) {
		fprintf(stderr, "ERROR: malloc() = NULL in append\n");
		exit( MEM_ALLOC_ERROR );
	}
	last -> data = data;
	last -> next = NULL;
	
	if( NULL != q -> tail ) {
		q -> tail -> next = last;
	}else{
		q -> head = last; /* list was empty */
	}
	
	q -> tail = last;
	q -> size ++;
	
	return q;
}

/*
 * EOF
 */
