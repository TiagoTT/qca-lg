/*
 * Ficheiro: layout.c
 * 
 * Modulo de manipulacao do Layout QCA.
 *
 * QCA Layout manipulation module.
 */
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include"fifo.h"
#include"errors.h"
#include"celulas.h"
#include"circuit.h"
#include"wire.h"
#include"mesh.h"

#include"layout.h"

/*
 * Realiza a expancao do circuito, tal que, nenhum porta tenha mais que 1 saida.
 * Cada vez que se encontra uma porta reutilizada, ela e' copiada para um local
 * mais favoravel.
 *
 * Expands the circuit, so that, no gate has fanout > 1.
 * Every time a subcircuit is reused, it is copied to an apropriate location.
 */
void Expand( circuit cir ) {
	int i, j;
	fifo q = fifo_init();
	gate g, dup;

	/* 
	 * First pass over the logic network.
	 */
	
	/* Starting exploring from each primary output. */
	for( i = 0 ; i < cir -> n_outputs ; i++ ) {
		
		/* Run Breadth First Search to visit every gate on this
		 * output's dependency tree. */

		if( yes == cir -> outputs[ i ] -> visited ) {
			/* This output is used as other gate's input, so it must
			 * be copied with all it's dependencies. */
			dup = sub_cir_duplicate( cir, cir -> outputs[ i ] );
			/* The original will no longer be an output. */
			cir -> outputs[ i ] -> p_output = no;
			/* The duplicate will replace the original. */
			cir -> outputs[ i ] = dup;
			/* There is no need to go deeper into this sub-graph. */
			continue;
		}

		cir -> outputs[ i ] -> visited = yes;
		fifo_append( q, cir -> outputs[ i ] );
		
		while( 0 < q -> size ) {
			
			/* Get the first gate from the queue. */
			g = fifo_first( q );
			/* Remove the first gate from the queue. */
			fifo_rest( q );

			/* Iterate over this gate's inputs. */
			for( j = 0 ; j < g -> n_inputs ; j++ ) {
				
				/* Duplicate sub-graph if needed. */
				if( yes == g->inputs[ j ]->visited ) {
					dup = sub_cir_duplicate( cir, 
							g->inputs[j] );
					g->inputs[ j ] = dup;
				} else {
					/* Add to the queue. */
					g->inputs[ j ]->visited = yes;
					fifo_append( q, g->inputs[ j ] );
				}
			}
		}
#if DEBUG
		fprintf(stderr,
			"DEBUG: After Logic Expansion/Replication of %s.\n",
			cir -> outputs[ i ] -> id );
		cir_do( cir, print_gate ); 	/* DEBUG */
#endif
	}
	/* It is necessary to clean the visited/queued marks. */
	cir_do( cir, reset_visited_gate );
	
	/* Clean up. */
	fifo_kill( q );
	
	return;
}



void Layout( circuit cir ) {
	int i, j, clk = 0, y = 0, y_max = 0, y_offset = 0;
	fifo q = fifo_init();
	gate g;
	boolean updated = no;

	/* Abrir desenho. (ad hoc)
	 * Design start. (ad hoc)
	 */
	printf("[VERSION]\nqcadesigner_version=2.000000\n[#VERSION]\n"\
			"[TYPE:DESIGN]\n[TYPE:QCADLayer]\ntype=1\nstatus=0\n"\
			"pszDescription=Main Cell Layer\n");

	/* 
	 * First pass over the logic network.
	 * Iterative method to determine the PLACE of each logic gate in the
	 * layout. The coordinates defined here are not cell coordinates.
	 * Find the maximum area needed to draw the QCA Layout.
	 * */

	/* 
	 * The first pass over the logic circuit is to 
	 * find the Y position.
	 */

	/* Iterate over each primary output. (HUGE cicle) */
	for( i = 0 ; i < cir -> n_outputs ; i++ ) {

		/* Run Breadth First Search to visit every gate on this
		 * output's dependency tree. */
		fifo_append( q, cir -> outputs[ i ] );

		while( 0 < q -> size ) {

			/* Get the first gate from the queue. */
			g = fifo_first( q );
			/* Remove the first gate from the queue. */
			g -> queued = no;
			fifo_rest( q );

			/* Inverter gates are ignored (replaced by their inputs)
			 * EXCEPT if they are also primary outputs. */
			while( inv == g -> t && no == g -> p_output ) {
				g -> inputs[ 0 ] -> place = g -> place;
				g = g -> inputs[ 0 ];
			}

			/* Iterate over this gate's inputs. */
			for( j = 0 ; j < g -> n_inputs ; j++, updated = no ) {

				/* Update y position if needed. */
				y = g->place.y + 1;
				if( g->inputs[ j ]->place.y < y ) {
					g->inputs[ j ]->place.y = y;
					updated = yes;
					/* Update y_max. */
					y_max = ( y > y_max)? y : y_max ;
				}

				/* Add to the queue if needed. */
				if( no == g->inputs[ j ]->queued
						&& yes == updated ) {
					g->inputs[ j ]->visited = yes;
					fifo_append( q, g->inputs[ j ] );
					g->inputs[ j ]->queued = yes;
				}
			}
		}
		/* It is necessary to clean the visited/queued marks. */
#if DEBUG
		fprintf(stderr, "DEBUG: After y calculation.\n" );
		cir_do( cir, print_gate );	/* DEBUG */
#endif
		cir_do( cir, reset_visited_gate );

		/*
		 * The first and HALF pass over the graph is to determine
		 * the distance of each gate to the farthest 
		 * input it depends on.
		 */
		cir_graph_level( cir -> outputs[ i ] );


		/* 
		 * The second pass over the logic circuit is to 
		 * find the n value for each gate in a certain level.
		 * The n value represents the relative position of 
		 * the gate in it's level of the circuit graph.
		 * The value N is stored in the N field for convinience
		 * and will be used to compute the real x value after this pass.
		 */


		/* Run Breadth First Search to visit every gate on this
		 * output's dependency tree. */
		cir -> outputs[ i ] -> place.n = 0;
		cir -> outputs[ i ] -> visited = yes;
		fifo_append( q, cir -> outputs[ i ] );

		while( 0 < q -> size ) {

			/* Get the first gate from the queue. */
			g = fifo_first( q );
			/* Remove the first gate from the queue. */
			fifo_rest( q );
			
			/* Inverter gates are ignored (replaced by their inputs)
			 * EXCEPT if they are also primary outputs. */
			while( inv == g -> t && no == g -> p_output ) {
				g -> inputs[ 0 ] -> place = g -> place;
				g = g -> inputs[ 0 ];
			}

			/* Update n value of the inputs.
			 * This is done explicitly with care, this code 
			 * is dependant of the execution order. */

			/* TODO change this to type check. */
			if( 0 < g -> n_inputs ) {
				g -> inputs [ g -> n_inputs / 2 ] -> place.n =
					g -> place.n * 3 + 1;	/* middle */
				g -> inputs [ 0 ] -> place.n =
					g -> place.n * 3;	/* left */
				g -> inputs [ g -> n_inputs - 1 ] -> place.n =
					g -> place.n * 3 + 2;	/* right */
			}
			/* Iterate over this gate's inputs. */
			for( j = 0 ; j < g -> n_inputs ; j++ ) {

				/* Add to the queue if needed. */
				if( no == g->inputs[ j ]->visited ) {
					g->inputs[ j ]->visited = yes;
					fifo_append( q, g->inputs[ j ] );
				}
			}
		}
		/* It is necessary to clean the visited/queued marks. */
#if DEBUG
		fprintf(stderr, "DEBUG: After n calculation.\n" );
		cir_do( cir, print_gate );	/* DEBUG */
#endif
		cir_do( cir, reset_visited_gate );

		/* In this third pass over the circuit the value n stored
		 * in the n field will now be used to calculate real X value.
		 */


		/* Run Breadth First Search to visit every gate on this
		 * output's dependency tree. */
		cir -> outputs[ i ] -> visited = yes;
		fifo_append( q, cir -> outputs[ i ] );

		while( 0 < q -> size ) {

			/* Get the first gate from the queue. */
			g = fifo_first( q );
			/* Remove the first gate from the queue. */
			fifo_rest( q );

			/* TODO - Consider the sub-tree expansion number.
			 * Or the g->place.l = Graph level 
			 * (distance to the farthest input).*/
			g -> place.x = ( 1 + 2 * g->place.n ) 
				* ( pow( 3, y_max - g->place.y ) + 1 ) / 2
				- g->place.n;

			/* Inverter gates are ignored (replaced by their inputs)
			 * EXCEPT if they are also primary outputs. */
			while( inv == g -> t && no == g -> p_output ) {
				g -> inputs[ 0 ] -> place = g -> place;
				g = g -> inputs[ 0 ];
			}

			/* Iterate over this gate's inputs. */
			for( j = 0 ; j < g -> n_inputs ; j++ ) {

				/* Add to the queue if needed. */
				if( no == g->inputs[ j ]->visited ) {
					g->inputs[ j ]->visited = yes;
					fifo_append( q, g->inputs[ j ] );
				}
			}
		}
#if DEBUG
		fprintf(stderr, "DEBUG: After x calculation.\n" );
		cir_do( cir, print_gate ); 	/* DEBUG */
#endif
		/* It is necessary to clean the visited/queued marks. */
		cir_do( cir, reset_visited_gate );

		/* Second pass over the logic network.
		 * Start drawing from the outputs toward the inputs.
		 * DRAWS the circuit resulting from the previous step, 
		 * adding the needed interconnections (when it is simple), 
		 * or warn about the need of manual desing of such interconnections.
		 */
		/* CircuitDraw( cir ); */


		/* Run Breadth First Search to visit every gate on this
		 * output's dependency tree. */
		cir -> outputs[ i ] -> visited = yes;
		fifo_append( q, cir -> outputs[ i ] );

		while( 0 < q -> size ) {

			/* Get the first gate from the queue. */
			g = fifo_first( q );
			/* Remove the first gate from the queue. */
			fifo_rest( q );
			
			/* Inverter gates are ignored (replaced by their inputs)
			 * EXCEPT if they are also primary outputs. */
			while( inv == g -> t && no == g -> p_output ) {
				g -> inputs[ 0 ] -> place = g -> place;
				g = g -> inputs[ 0 ];
			}

			/* This is used to retrieve the offset that will be
			 * added (to this gate's inputs) after it is drawn.
			 * If y_offset is set to a value, it will change the 
			 * level of the gate in the graph. */
			y_offset = 0;
			clk = PrintGate( g, &(cir->x_max), &y_offset );

			/* Iterate over this gate's inputs. */
			for( j = 0 ; j < g -> n_inputs ; j++ ) {

				/* Add to the queue if needed. */
				if( no == g->inputs[ j ]->visited ) {
					g->inputs[ j ]->visited = yes;
					/* The input's shall have their 
					 * output cell in this clk zone. */
					g->inputs[ j ]->place.clk_out = clk;
					/* The y offset (in cells) is 
					 * acumulated. */
					g->inputs[ j ]->place.y_offset = 
						g->place.y_offset + y_offset;
					fifo_append( q, g->inputs[ j ] );
				}
			}
		}
		/* It is necessary to clean the visited/queued marks. */
		cir_do( cir, reset_visited_gate );

		/* Save acumulate x_max and update y_max. */
		cir -> x_max += pow( 3, y_max );
		if( cir -> y_max < y_max ) {
			cir -> y_max = y_max;
		}
		/* Reset subcircuit y_max, clk and y_offset. */
		y_max = clk = y_offset = 0;
	}
	
	/* Ligacao de cada uma das entradas primarias 'as suas copias.
	 * Connection of each primary input to it's copies. 
	 */
	input_mesh( cir );


	/* Clean up. */
	fifo_kill( q );

	/* Fechar desenho. (ad hoc)
	 * Design end. (ad hoc)
	 */
	printf("[#TYPE:QCADLayer]\n[#TYPE:DESIGN]\n");

	return;
}

/*
 * EOF
 */
