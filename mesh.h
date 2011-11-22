/*
 * Ficheiro: mesh.h
 * 
 * Definicao da malha de entrada.
 *
 * Input mesh definition.
 */
#ifndef _MESH_H_
#define _MESH_H_

/* Constants. */
/* To ensure the minimal distance of the main distribution wires. */
#define Y_BASE_STEP 3

/* Data structures. */
typedef struct Clock_Zone_Str{
	int clk;	/* Numerical value of the clock zone's clock. */
	int start;	/* Point of start of this clock zone along the wire. */
}clock_zone;

/* Array of copies. */

/* Functions. */
int input_mesh( circuit cir );

clock_zone border_find( clock_zone* array, unsigned int n_elements, int x );

int raise_y_base( int* y_base_array, unsigned int n_elements,
                unsigned int y, unsigned int delta );
int find_in_array( int* array, unsigned int n_elements, int value );
int find_clk_min( gate* array, unsigned int n_elements );
int find_y_min( gate* array, unsigned int n_elements );
int find_x_min( gate* array, unsigned int n_elements );
int find_x_max( gate* array, unsigned int n_elements );

int copy_comparator( void* a, void* b );
/* static int copy_comparator( const void* a, const void* b ); */

/* Macros. */
#define Y_BASE_INIT(y) \
	( ( ((y)%(Y_BASE_STEP)) )? \
		( (((y)/(Y_BASE_STEP))+1)*(Y_BASE_STEP) ) \
		: (y) )

#endif
/*
 * EOF
 */
