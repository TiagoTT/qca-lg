/*
 * Ficheiro: celulas.c
 * 
 * Funcoes de manipulacao das estruturas de dados que suportam as celulas QCA.
 */
#include<stdio.h>
#include<stdlib.h>

#include"errors.h"
#include"celulas.h"

/*
 * Cria e devolve uma nova celula.
 *
 * Creates a new QCA cell and returns a pointer to it.
 */
cell new_cell( float x, float y, tipo t, float pol, int clk ) {
	cell c;

	c = (cell)malloc( sizeof( Cell ) );
	if( NULL == c ) {
		fprintf(stderr, "ERR: malloc() = NULL in new_cell().\n");
		exit( MEM_ALLOC_ERROR );
	}
/*	c -> x = ( 0 < x)? x : 0;	*/
/*	c -> y = ( 0 < y)? y : 0;	*/
	c -> x = x;
	c -> y = y;
	c -> t = t;
	c -> pol = (1 < pol)? 1 : ( (-1 > pol)? -1 : pol );
	c -> clk = clk;
	c -> label = NULL;

	return c;
}

/*
 * Liberta a memoria ocupada pela celula.
 *
 * Frees the resources allocated to this cell.
 */
void kill_cell( cell c ) {
	free( c );
}

/*
 * Imprime a informacao sobre a celula para um dado ficheiro.
 * Caso o ficheiro seja NULL, escreve-se para o stdout.
 * O formato de escrita TENTA ser compativel com o QCADesigner.
 */
void write_cell( FILE* output, cell c ) {
	FILE* fp;
	float charge[ N_DOT ], spin = 0.0, potential = 0.0;
	int i;
	
	if( NULL == output ) {
		fp = stdout;
	}else{
		fp = output;
	}
	
	switch( c->t ) {
		case constante:
			/* Calculate each dot charge in a constant 
			 * polarization cell. */
			charge[ 0 ] = charge[ 2 ] = 
				DOT_CHARGE_HALF * ( 1 + c-> pol );
			charge[ 1 ] = charge[ 3 ] =
				DOT_CHARGE_HALF * ( 1 - c-> pol );
			break;
		default:
			/* The dots in other cells will be half charged. */
			for( i = 0 ; i < N_DOT ; i++ ) {
				charge[ i ] = DOT_CHARGE_HALF;
			}
			break;
	}
/*	fprintf( fp, "Posicao X: %f\nPosicao Y: %f\nTipo: %d\nPol.: %f\n",
			c -> x, c -> y, (int)c -> t, c -> pol );
*/	
	fprintf( fp, "[TYPE:QCADCell]\n"\
			"[TYPE:QCADDesignObject]\nx=%f\ny=%f\nbSelected=%s\n"\
			"%s"\
			"bounding_box.xWorld=%f\nbounding_box.yWorld=%f\n"\
			"bounding_box.cxWorld=%f\nbounding_box.cyWorld=%f\n"\
			"[#TYPE:QCADDesignObject]\n",
			c->x, c->y, SELECTED, GetColorString( c->t, c->clk ),
			c->x - (CELL_X_DIM/2), c->y - (CELL_Y_DIM/2),
			CELL_X_DIM, CELL_Y_DIM );

	fprintf( fp, "cell_options.cxCell=%f\n", CELL_X_DIM );
	fprintf( fp, "cell_options.cyCell=%f\n", CELL_Y_DIM );
	fprintf( fp, "cell_options.dot_diameter=%f\n", DOT_DIAMETER);
	fprintf( fp, "cell_options.clock=%d\n", ( c->clk & 0x3 ) );
	fprintf( fp, "cell_options.mode=QCAD_CELL_MODE_NORMAL\n" );
	fprintf(fp, "cell_function=%s\n", TipoToString( c -> t ) );
	fprintf(fp, "number_of_dots=%d\n", N_DOT );


	/* Descreve os 4 quantum dots da celula. */
	/* Top Right. */
	fprintf( fp,"[TYPE:CELL_DOT]\nx=%f\ny=%f\ndiameter=%f\ncharge=%e\n"\
			"spin=%f\npotential=%f\n[#TYPE:CELL_DOT]\n",
	      		c->x + (CELL_X_DIM/4), c->y - (CELL_Y_DIM/4),
	     		DOT_DIAMETER, charge[ TR ], spin, potential );
	/* Bottom Right. */
	fprintf( fp,"[TYPE:CELL_DOT]\nx=%f\ny=%f\ndiameter=%f\ncharge=%e\n"\
			"spin=%f\npotential=%f\n[#TYPE:CELL_DOT]\n",
	      		c->x + (CELL_X_DIM/4), c->y + (CELL_Y_DIM/4),
	     		DOT_DIAMETER, charge[ BR ], spin, potential );
	/* Bottom Left. */
	fprintf( fp,"[TYPE:CELL_DOT]\nx=%f\ny=%f\ndiameter=%f\ncharge=%e\n"\
			"spin=%f\npotential=%f\n[#TYPE:CELL_DOT]\n",
	      		c->x - (CELL_X_DIM/4), c->y + (CELL_Y_DIM/4),
	     		DOT_DIAMETER, charge[ BL ], spin, potential );
	/* Top Left. */
	fprintf( fp,"[TYPE:CELL_DOT]\nx=%f\ny=%f\ndiameter=%f\ncharge=%e\n"\
			"spin=%f\npotential=%f\n[#TYPE:CELL_DOT]\n",
	      		c->x - (CELL_X_DIM/4), c->y - (CELL_Y_DIM/4),
	     		DOT_DIAMETER, charge[ TL ], spin, potential );
	
	/* If the cell has a label, add it. */
	if( NULL != c->label ) {
		fprintf( fp, "[TYPE:QCADLabel]\n[TYPE:QCADStretchyObject]\n"\
			"[TYPE:QCADDesignObject]\nx=%f\ny=%f\nbSelected=%s\n"\
			"%s"\
			"bounding_box.xWorld=%f\nbounding_box.yWorld=%f\n"\
			"bounding_box.cxWorld=%f\nbounding_box.cyWorld=%f\n"\
			"[#TYPE:QCADDesignObject]\n"\
			"[#TYPE:QCADStretchyObject]\n"\
			"psz=%s\n[#TYPE:QCADLabel]\n",
			c->x + CELL_X_DIM, c->y + CELL_Y_DIM, SELECTED,
			GetColorString( c->t, c->clk ),
			c->x + (CELL_X_DIM/2), c->y + (CELL_Y_DIM/2),
			CELL_X_DIM, CELL_Y_DIM, c->label );
	}

	/* Cell end. */
	fprintf( fp,"[#TYPE:QCADCell]\n" );
}


/*
 * Imprime a informacao sobre metade da celula para um dado ficheiro.
 * Caso o ficheiro seja NULL, escreve-se para o stdout.
 * O formato de escrita TENTA ser compativel com o QCADesigner.
 */
void write_half_cell( FILE* output, cell c, part p ) {
	FILE* fp;
	float charge[ N_DOT ], spin = 0.0, potential = 0.0;
	int i;
	
	if( NULL == output ) {
		fp = stdout;
	}else{
		fp = output;
	}
	
	switch( c->t ) {
		case constante:
			/* Calculate each dot charge in a constant 
			 * polarization cell. */
			charge[ 0 ] = charge[ 2 ] = 
				DOT_CHARGE_HALF * ( 1 + c-> pol );
			charge[ 1 ] = charge[ 3 ] =
				DOT_CHARGE_HALF * ( 1 - c-> pol );
			break;
		default:
			/* The dots in other cells will be half charged. */
			for( i = 0 ; i < N_DOT ; i++ ) {
				charge[ i ] = DOT_CHARGE_HALF;
			}
			break;
	}
	
	fprintf( fp, "[TYPE:QCADCell]\n"\
			"[TYPE:QCADDesignObject]\nx=%f\ny=%f\nbSelected=%s\n"\
			"%s"\
			"bounding_box.xWorld=%f\nbounding_box.yWorld=%f\n"\
			"bounding_box.cxWorld=%f\nbounding_box.cyWorld=%f\n"\
			"[#TYPE:QCADDesignObject]\n",
			c->x, c->y, SELECTED, GetColorString( c->t, c->clk ),
			c->x - (CELL_X_DIM/( ( vertical == p )? 4 : 2 ) ),
			c->y - (CELL_Y_DIM/( ( horizontal == p )? 4 : 2 ) ),
			( ( vertical == p )? CELL_X_DIM/2 : CELL_X_DIM ),
			( ( horizontal == p )? CELL_Y_DIM/2 : CELL_Y_DIM ) );


	fprintf( fp, "cell_options.cxCell=%f\n", ( ( vertical == p )?
					CELL_X_DIM/2 : CELL_X_DIM ) );
	fprintf( fp, "cell_options.cyCell=%f\n",( ( horizontal == p )?
					CELL_Y_DIM/2 : CELL_Y_DIM ) );
	fprintf( fp, "cell_options.dot_diameter=%f\n", DOT_DIAMETER);
	fprintf( fp, "cell_options.clock=%d\n", ( c->clk & 0x3 ) );
	fprintf( fp, "cell_options.mode=QCAD_CELL_MODE_NORMAL\n" );
	fprintf(fp, "cell_function=%s\n", TipoToString( c -> t ) );
	fprintf(fp, "number_of_dots=%d\n", N_DOT/2 );


	/* Descreve os 2 quantum dots da celula. */
	if( vertical == p ) {
		/* Top. */
		fprintf( fp,
		"[TYPE:CELL_DOT]\nx=%f\ny=%f\ndiameter=%f\ncharge=%e\n"\
				"spin=%f\npotential=%f\n[#TYPE:CELL_DOT]\n",
				c->x, c->y - (CELL_Y_DIM/4),
				DOT_DIAMETER, charge[ TR ], spin, potential );
		/* Bottom. */
		fprintf( fp,
		"[TYPE:CELL_DOT]\nx=%f\ny=%f\ndiameter=%f\ncharge=%e\n"\
				"spin=%f\npotential=%f\n[#TYPE:CELL_DOT]\n",
				c->x, c->y + (CELL_Y_DIM/4),
				DOT_DIAMETER, charge[ BR ], spin, potential );
	}else if ( horizontal == p ) {
		/* Left. */
		fprintf( fp,
		"[TYPE:CELL_DOT]\nx=%f\ny=%f\ndiameter=%f\ncharge=%e\n"\
				"spin=%f\npotential=%f\n[#TYPE:CELL_DOT]\n",
				c->x - (CELL_X_DIM/4), c->y,
				DOT_DIAMETER, charge[ BL ], spin, potential );
		/* Right. */
		fprintf( fp,
		"[TYPE:CELL_DOT]\nx=%f\ny=%f\ndiameter=%f\ncharge=%e\n"\
				"spin=%f\npotential=%f\n[#TYPE:CELL_DOT]\n",
				c->x + (CELL_Y_DIM/4), c->y,
				DOT_DIAMETER, charge[ BR ], spin, potential );
	}else{
		/* Complain. TODO */
	}

	/* Cell end. */
	fprintf( fp,"[#TYPE:QCADCell]\n" );
}

const char* TipoToString( tipo t ) {
	switch( t ) {
		case 0 : return NORMAL_STRING;
		case 1 : return CONSTANTE_STRING;
		case 2 : return INPUT_STRING;
		case 3 : return OUTPUT_STRING;
		default : return UNKNOWN_STRING;
	}
	return UNKNOWN_STRING;
}

const char* GetColorString( tipo t, int clk ) {
	switch( t ) {
		case 0 :
			 switch( clk & 0x3 /* Only the two LSB. */ ) {
				 case 0 : return GREEN;
				 case 1 : return PURPLE;
				 case 2 : return TURQUOISE;
				 case 3 : return WHITE;
				 default : return WRONG;
			 }
		case 1 : return BROWN;
		case 2 : return BLUE;
		case 3 : return YELLOW;
		default : return WRONG;
	}
	return WRONG;
}

/*
 * EOF
 */
