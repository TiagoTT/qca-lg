/*
 * Ficheiro: mesh.c
 * 
 * Definicao da malha de entrada.
 *
 * Input mesh definition.
 */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"errors.h"
#include"celulas.h"
#include"circuit.h"
#include"wire.h"

#include"mesh.h"


/*
 * This function iterates over each primary input.
 * If a given p_input has been copied, a set of wires is draw to ensure the
 * synchronised signal flow from the unique p_input of the QCA layout towards
 * the gates.
 *
 * Three more layers are specified here, two of them are "vias", and the other
 * is the crossover layer. The p_inputs of the layout will be in the crossover
 * layer, rooting the main distribution wires layed horizontally.
 *
 * The wires from the gates' inputs will rise in the vertical to meet the 
 * main distribution wires, and at their intersection, "vias" will be placed.
 *
 */
int input_mesh( circuit cir ) {
	/* Variables. */
	int i = 0, j = 0, clk_min = 0, y_max = 0, x_min = 0,
		L = 0, l = 0, n = 0, b = 0, extension = 0;
	clock_zone near, earliest;
	boolean rerun = no;
	gate g = NULL;
	cell c = new_cell( 0, 0, normal, 0, 0 );
	/* List of Main distribution wires and their positions. */
	int *y_base_array = NULL, *clk_count = NULL, *x_max = NULL;
	/* Array of copies. */
	gate** copies = NULL;
	/* Array of the clock zone borders of the main dist. wires. */
	clock_zone** borders = NULL;

	/* The Main distribution wires' position array. */
	y_base_array = (int*)malloc( sizeof(int) * cir->n_inputs );
	if( NULL == y_base_array ) {
		fprintf(stderr,"ERR: malloc() = NULL in input_mesh.\n");
		exit( MEM_ALLOC_ERROR );
	}
	/* Fill it with zeros. */
	memset( y_base_array, 0, sizeof(int) * cir->n_inputs );

	/* The Clock Count array. */
	clk_count = (int*)malloc( sizeof(int) * cir->n_inputs );
	if( NULL == clk_count ) {
		fprintf(stderr,"ERR: malloc() = NULL in input_mesh.\n");
		exit( MEM_ALLOC_ERROR );
	}
	/* Fill it with zeros. */
	memset( clk_count, 0, sizeof(int) * cir->n_inputs );

	/* The Right most position  array. */
	x_max = (int*)malloc( sizeof(int) * cir->n_inputs );
	if( NULL == x_max ) {
		fprintf(stderr,"ERR: malloc() = NULL in input_mesh.\n");
		exit( MEM_ALLOC_ERROR );
	}
	/* Fill it with zeros. */
	memset( x_max, 0, sizeof(int) * cir->n_inputs );

	/* The array of arrays of copies. */
	copies = (gate**)malloc( sizeof(gate*) * cir->n_inputs );
	if( NULL == copies ) {
		fprintf(stderr,"ERR: malloc() = NULL in input_mesh.\n");
		exit( MEM_ALLOC_ERROR );
	}
	/* Fill it with zeros(NULL). */
	memset( copies, 0, sizeof(gate*) * cir->n_inputs );

	/* The array of arrays of borders. */
	borders = (clock_zone**)malloc( sizeof(clock_zone*) * cir->n_inputs );
	if( NULL == borders ) {
		fprintf(stderr,"ERR: malloc() = NULL in input_mesh.\n");
		exit( MEM_ALLOC_ERROR );
	}
	/* Fill it with zeros(NULL). */
	memset( borders, 0, sizeof(clock_zone*) * cir->n_inputs );

	/* The following procedure is repeated for each primary input. */
	for( i = 0 ; i < cir -> n_inputs ; i++ ) {
		/* This gate is the original input. */
		g = cir -> inputs[ i ];

		/* The copies will be sorted by restriction:
		 * r(x,y,clk) = (x - x_min) + (y_max - y)
		 * 		+ ((clk - clk_min)*MAX_CLK_ZONE);
		 * (The constants will be ignored during comparisons.)
		 */

		/* Put the copies and the original itself in an array. */
		copies[ i ] = (gate*)malloc( sizeof(gate) * (g->n_copies + 1) );
		if( NULL == copies[ i ] ) {
			fprintf(stderr,"ERR: malloc() = NULL in input_mesh.\n");
			exit( MEM_ALLOC_ERROR );
		}
		for( j = 0 ; j < g -> n_copies ; j++ ) {
			copies[ i][ j ] = (gate)stack_pop( &(g->copy_stack) );
		}
		copies[ i ][ j ] = g; /* The first becomes the last. */

		/* Use qsort and copy_comparator() to sort the array. */
		qsort( copies[ i ], g -> n_copies + 1, sizeof(gate),
			copy_comparator );
		
		/* Find the limits of the set of copies.
		 * Min CLK, Max Y,  Min X.
		 * Main distribution wire Y pos >= Max Y. (y_main >= y_max)
		 */
		/* The earliest clk. */
		clk_min = find_clk_min( copies[i], g -> n_copies + 1 );
		/* The highest y in absolute value. */
		y_max = - find_y_min( copies[i], g -> n_copies + 1 );
		/* The left most x. */
		x_min = find_x_min( copies[i], g -> n_copies + 1 );
		/* The right most x. */
		x_max[ i ] = find_x_max( copies[i], g -> n_copies + 1 );

		/* Raise Y base to (at least) y_max. */
		raise_y_base( y_base_array, cir->n_inputs, i, y_max );

		do{
			rerun = no;
			/* Find the CLK_ZONE distribution that minimizes */
			/* Wire length. */
			l = copies[i][0]->place.y_final 
				- y_base_array[i]
				+ copies[i][0]->place.x_final
				- x_min;
			if( l <= MIN_CLK_ZONE ) {
				/* Raise y_base. Once is enough because 
				 * Y_BASE_STEP = MIN_CLK_ZONE */
				raise_y_base( y_base_array, cir->n_inputs,
					i, Y_BASE_STEP);
				/* Recalculate wire length */
				l = copies[i][0]->place.y_final 
					- y_base_array[i]
					+ copies[i][0]->place.x_final
					- x_min;
			}
			/* Number of clock zones. */
			b = CLK_ZONES( l, MAX_CLK_ZONE );
			/* What is longer. the path to the most restrictive
			 * or the main dist wire? */
			L = x_max[ i ] - x_min;
			if( l < L ) {
				extension = CLK_ZONES( (L-l), (l/b) );
			}else{
				extension = 0;
			}
			clk_count[ i ] = b + extension +1;
				/* One more border to finish the last zone. */
			/* Adjust the size of the borders array. */
			borders[ i ] = (clock_zone*)realloc( borders[ i ],
						sizeof( clock_zone ) *
						clk_count[ i ] );
			if( NULL == borders[ i ] ) {
				fprintf(stderr,
				"ERR: malloc() = NULL in input_mesh.\n");
				exit( MEM_ALLOC_ERROR );
			}
			/* Fill the borders array, as Wire() will draw. */
			borders[i][0].start = x_min;
			borders[i][0].clk = copies[i][0]->place.clk_out - b;
			for( j = 1 ; j < clk_count[ i ] ; j++ ) {
				borders[i][j].start = 
					borders[i][j-1].start + 
					l/b +
					((j >= (clk_count[ i ] - l%b))?
						1:0 ); 
				borders[i][j].clk = borders[i][j-1].clk + 1;
			}

			/* For each copy left, starting by the 
			 * 2nd most restrictive. */
			for( j = 1 ; j < g -> n_copies + 1 ; j++ ) {
				/* Find the nearest border (on the left). */
				near = border_find( borders[i], b,
						copies[i][j]->place.x_final);
				
				if( copies[ i ][ j ]->place.x_final >=
					near.start + MIN_CLK_ZONE ) {
					/* Wire length.
					 * (including extremities) */
					l = copies[i][j]->place.y_final 
						- y_base_array[i];
					/* First cell is below MDW. */
					/* Number of clock zones. */
					n = copies[i][j]->place.clk_out
						- (near.clk+1);
				} else {
					/* Wire length.
					 * (including extremities) */
					l = copies[i][j]->place.y_final 
						- y_base_array[i] 
						+ copies[i][j]->place.x_final
						- near.start
						+1 /* First cell. */;
					/* Number of clock zones. */
					n = copies[i][j]->place.clk_out
						- near.clk;
				}
				/* Try to find a CLK_ZONE distribution
				 * that fits, with minimal number of 
				 * CLK_ZONES (less delay).*/

				/* If there is success, apply the resulting
				 * restrictions and go for the next input copy.
				 * 
				 * Else raise y_main and start again from 
				 * the most restrictive case. The raising of
				 * y_main is done in a way to prevent
				 * collisions of the Main dist. wires of 
				 * different inputs.*/
				if( l >= n * MIN_CLK_ZONE
						&& l <= n * MAX_CLK_ZONE ) {
					/* Not too long, not too short. */
					/* Save result and move on. */
#if DEBUG
		fprintf( stderr, "Found a solution for node: %s => "\
				"Length = %d, ClockZones = %d\n",
				copies[i][j]->id, l, n );
#endif
					continue;
				} else {
					/* Wire too short.
					 * Raising y_base will make it grow. */
					rerun = yes;
					raise_y_base( y_base_array, 
						cir->n_inputs, i, Y_BASE_STEP );
#if DEBUG
		fprintf( stderr, "Unable to find a solution for "\
			"node: %s at (%d,%d) and Ybase = %d => "\
			"Length = %d, ClockZones = %d\n",
			copies[i][j]->id, copies[i][j]->place.x_final,
			copies[i][j]->place.y_final, y_base_array[i], l, n );
#endif
					break;
				}
			}
		}while( rerun );
	
		/* Draw the vertical wires from gate inputs to the main 
		 * distribution line.
		 * Given copies[i][*]->place and y_base_array[i]. */
			
		/* Find the nearest border (on the left). */
		near = border_find( borders[i], clk_count[i],
				copies[i][0]->place.x_final);		
		/* The most urgent input coy. */
		Wire( copies[ i ][ 0 ]->place.x_final,
			copies[ i ][ 0 ]->place.y_final,
			near.start,
			y_base_array[ i ],
			copies[ i ][ 0 ]->place.clk_out,
			copies[ i ][ 0 ]->place.clk_out - near.clk,
			yx, no,
			- (copies[i][0]->place.x_final -near.start) );

		/* The other input copies.  */
		for( j = 1 ; j < g -> n_copies + 1 ; j++ ) {
			/* Find the nearest border (on the left). */
			near = border_find( borders[i], clk_count[i],
					copies[i][j]->place.x_final);
			if( copies[ i ][ j ]->place.x_final >=
					near.start + MIN_CLK_ZONE ) {
				/* Just the vertical wire. */
				Wire( copies[ i ][ j ]->place.x_final,
				copies[ i ][ j ]->place.y_final,
				copies[i][j]->place.x_final,
				y_base_array[ i ] +1,
				copies[ i ][ j ]->place.clk_out,
				copies[ i ][ j ]->place.clk_out - (near.clk+1),
				yx, no, 0 );
				/* The cell below the MDW. */
				c -> x = copies[ i ][ j ] -> place.x_final
					* (CELL_X_DIM + CELL_SPACING);
				c -> y = y_base_array[ i ]
					* (CELL_Y_DIM + CELL_SPACING);
				c -> clk = near.clk+1;
				write_cell( NULL, c );
			} else {
				Wire( copies[ i ][ j ]->place.x_final,
				copies[ i ][ j ]->place.y_final,
				near.start,
				y_base_array[ i ],
				copies[ i ][ j ]->place.clk_out,
				copies[ i ][ j ]->place.clk_out - near.clk,
				yx, no,
				- (copies[i][j]->place.x_final - near.start) );
			}
#if DEBUG
		fprintf(stderr,"Wire from (%d,%d) to (%d,%d),"\
			" start clk is %d, split into %d clk zones,"\
			" supressing %d cells.\n",
			copies[ i ][ j ]->place.x_final,
			copies[ i ][ j ]->place.y_final,
			MAX_2( near.start,
				copies[i][j]->place.x_final - MIN_CLK_ZONE),
			y_base_array[ i ],
			copies[ i ][ j ]->place.clk_out,
			copies[ i ][ j ]->place.clk_out - near.clk,
			-MIN_2(copies[i][j]->place.x_final -near.start,
				MIN_CLK_ZONE ) );
#endif
		}
	}

	/* Once all input distribution meshes are defined, it is time to draw
	 * them layer by layer. There is one layer already open, 
	 * it has to be completed and closed, then the two "vias" layers will
	 * be described, at last the "crossover" layer will be opened and 
	 * the main dist. wires will be drawn with the respective 
	 * QCA input cells. */
	
	/* Close the Main Cell Layer. */
	printf("[#TYPE:QCADLayer]\n");

	/* Open via1, write the cells, close via1. */
	printf("[TYPE:QCADLayer]\ntype=1\nstatus=0\npszDescription=Via 1\n");
	for( i = 0 ; i < cir -> n_inputs ; i++ ) {
		/* This gate is the original input. */
		g = cir -> inputs[ i ];
		for( j = 0 ; j < g -> n_copies + 1 ; j++ ) {
			/* Find the nearest border (on the left). */
			near = border_find( borders[ i ], clk_count[ i ],
					copies[ i ][ j ] -> place.x_final);
			c -> x = copies[ i ][ j ] -> place.x_final
				* (CELL_X_DIM + CELL_SPACING);
			c -> y = y_base_array[ i ] *(CELL_Y_DIM + CELL_SPACING);
			c -> clk = near.clk;
			/*write_cell( NULL, c ); not working well */
		}
	}
	printf("[#TYPE:QCADLayer]\n");

	/* Open via2, write the cells, close via2. */
	printf("[TYPE:QCADLayer]\ntype=1\nstatus=0\npszDescription=Via 2\n");
	printf("[#TYPE:QCADLayer]\n");

	/* Draw the main dist. wires. */
	printf("[TYPE:QCADLayer]\ntype=1\nstatus=0\n"\
		"pszDescription=CrossOver\n");
	for( i = 0 ; i < cir -> n_inputs ; i++ ) {
		/* Draw each clk zone separately. */
		for( j = 0 ; j < clk_count[ i ] -1 ; j++ ) {
			Wire( borders[ i ][ j ].start,
				y_base_array[ i ],
				borders[ i ][ j+1 ].start
					-1 /* don't overlap */,
				y_base_array[ i ],
				borders[ i ][ j+1 ].clk,
				1, yx, no,
				(( (borders[i][j+1].start -1)> x_max[i] )?
					-( (borders[i][j+1].start -1)
					- x_max[i] )
					:0 )/* in the last one... */);
		}
#if DEBUG
		fprintf(stderr,"Wire from (%d,%d) to (%d,%d),"\
			" start clk is %d, split into %d clk zones,"\
			" supressing %d cells.\n",
			copies[ i ][ 0 ]->place.x_final,
			copies[ i ][ 0 ]->place.y_final,
			borders[ i ][ 0 ].start,
			y_base_array[ i ],
			copies[ i ][ 0 ]->place.clk_out,
			copies[ i ][ 0 ]->place.clk_out - borders[ i ][ 0 ].clk,
			copies[ i ][ 0 ]->place.y_final - y_base_array[ i ] );
#endif
	}

	/* Find the Main dist. wire  with lowest CLK_ZONE at it's beginning.
	 * Extend all other main wires, so that, they all start in the same
	 * CLK_ZONE. 
	 *
	 * This can be called Equalization of Input CLK_ZONES. */
	
	/* Find the earliest needed input. */
	for( i = 0, earliest = borders[i][0] ; i < cir -> n_inputs ; i++ ) {
		if( earliest.clk > borders[ i ][ 0 ].clk ) {
			earliest = borders[ i ][ 0 ];
		}
	}
	/* Round it to start at CLK ZERO. */
	earliest.clk &= ~(0x3);

	/* Extend the other wires as needed. */
	for( i = 0 ; i < cir -> n_inputs ; i++ ) {
		/* Draw as short as possible.
		 * Length = MIN_CLK_ZONE*( borders[i].clk - earliest.clk )
		 */
		Wire( borders[ i ][ 0 ].start,
			y_base_array[ i ],
			borders[ i ][ 0 ].start
				- MIN_CLK_ZONE *
				( borders[ i ][ 0 ].clk - earliest.clk +1),
			y_base_array[ i ],
			borders[ i ][ 0 ].clk,
			borders[ i ][ 0 ].clk - earliest.clk +1,
				/* +1 to make stop at earliest.clk */
			yx, no, 1 /* The first one is already 
			part of the main dis. wire. */ );

			c -> x = ( borders[ i ][ 0 ].start -1 /* at the left */
					- MIN_CLK_ZONE
					*( borders[i][0].clk - earliest.clk +1))
				* (CELL_X_DIM + CELL_SPACING);
			c -> y = y_base_array[ i ] *(CELL_Y_DIM + CELL_SPACING);
			c -> clk = earliest.clk;
			c -> label = cir -> inputs[ i ] -> id;
			c -> t = input;
			write_cell( NULL, c );
			c -> label = NULL;

	}
	printf("[#TYPE:QCADLayer]\n");

	/* TODO Values for statistical purpose must be acquired.
	 * For example, the number of CLK_ZONES from inputs to outputs.
	 */

	/* Clean Up. */
	free( y_base_array );
	free( clk_count );
	free( x_max );
	for( i = 0 ; i < cir -> n_inputs ; i++ ) {
		free( copies[ i ] );
		free( borders[ i ] );
	}
	free( copies );
	free( borders );
	kill_cell( c );

	return 0;
}

clock_zone border_find( clock_zone* array, unsigned int n_elements, int x ) {
	unsigned int i = 0;

	for( i = 1 ; i < n_elements ; i++ ) {
		if( x < array[ i ].start ) {
			return array[ i - 1 ];
		}
	}
	/* This is for the right most copy. */
	return array[ n_elements -1 ];
}

int find_clk_min( gate* array, unsigned int n_elements ) {
	unsigned int i = 0;
	int min;

	min = array[ 0 ] -> place.clk_out;
	for( i = 0 ; i < n_elements ; i++ ) {
		if( min > array[ i ] -> place.clk_out ) {
			min = array[ i ] -> place.clk_out;
		}
	}
	return min;
}

int find_y_min( gate* array, unsigned int n_elements ) {
	unsigned int i = 0;
	int min;
	
	min = array[ 0 ] -> place.y_final;
	for( i = 0 ; i < n_elements ; i++ ) {
		if( min > array[ i ] -> place.y_final ) {
			min = array[ i ] -> place.y_final;
		}
	}
	return min;
}

int find_x_min( gate* array, unsigned int n_elements ) {
	unsigned int i = 0;
	int min;

	min = array[ 0 ] -> place.x_final;
	for( i = 0 ; i < n_elements ; i++ ) {
		if( min > array[ i ] -> place.x_final ) {
			min = array[ i ] -> place.x_final;
		}
	}
	return min;
}

int find_x_max( gate* array, unsigned int n_elements ) {
	unsigned int i = 0;
	int max;

	max = array[ 0 ] -> place.x_final;
	for( i = 0 ; i < n_elements ; i++ ) {
		if( max < array[ i ] -> place.x_final ) {
			max = array[ i ] -> place.x_final;
		}
	}
	return max;
}

/*
 * Given an array of y_base values, this function raises the value of a given
 * position (y) by a given amount (delta), rounding the result to a fixed step,
 * ensuring that every elements of the array are unique.
 *
 * Depending on collisions or round up the resulting value may be 
 * larger than expected. This is the intended behaviour.
 */
int raise_y_base( int* y_base_array, unsigned int n_elements, 
		unsigned int y, unsigned int delta ) {

	int y_base = 0;
	
	/* Unchecked. */
	/* Remember y values grow downwards, 
	 * so geometric raise (pull up) means numerical subtraction. */
	y_base = y_base_array[ y ];	/* Get the current value. */
	y_base -= delta;		/* Raise it by delta. */
	y_base = -Y_BASE_INIT( -y_base );	/* Round it UP. */
	
	/* Raise it even a bit more to avoid collisions if needed. */
	while( 0 <= find_in_array( y_base_array, n_elements, y_base ) ) {
		/* Every time we collide, we have to pull it up another step. */
		y_base -= Y_BASE_STEP;
	}

	/* Save the resulting value and return it. */
	y_base_array[ y ] = y_base;
	return y_base;
}

/*
 * Given an array and it's dimension, this function searches for a given value.
 * If the value is present, it's index is returned, else (-1) is returned.
 */
int find_in_array( int* array, unsigned int n_elements, int value ) {
	unsigned int i = 0;

	for( i = 0 ; i < n_elements ; i++ ) {
		if( value == array[ i ] ) {
			/* We found it. */
			return i;
		}
	}
	/* Not found. */
	return (-1);
}

/*
 * The comparator for the sort operation.
 * The function qsort from stdlib.h will be used.
 *
 * The sort will be ascending, so the first element will be the minimum.
 * If a has more precedence than b, a must be rated as smaller than b, by 
 * returning a value <0.
 */
int copy_comparator( void* a, void* b ) {
/* static int copy_comparator( const void* a, const void* b ) { */
	gate ga, gb;
	int pa = 0, pb = 0;

	/* a and b are pointers to gates. */
	ga = *(gate*)a;
	gb = *(gate*)b;

	pa = (ga -> place.x_final)
		+ (ga -> place.y_final)
		- (ga -> place.clk_out) * MAX_CLK_ZONE;
	pb = (gb -> place.x_final)
		+ (gb -> place.y_final)
		- (gb -> place.clk_out) * MAX_CLK_ZONE;

/*
	pa = (ga -> place.y_final) + (ga -> place.clk_out) * MAX_CLK_ZONE;
	pb = (gb -> place.y_final) + (gb -> place.clk_out) * MAX_CLK_ZONE;
*/

/*	
	pa = (ga -> place.x_final) + (ga -> place.y_final);
	pb = (gb -> place.x_final) + (gb -> place.y_final);
*/

	return pb - pa; /* pa > pb -> return <0 */
}

/*
 * EOF
 */
