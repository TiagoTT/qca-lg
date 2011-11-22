/*
 * Ficheiro: wire.h
 * 
 * Desenho de fios.
 *
 * Wire design.
 */
#include<stdio.h>
#include<stdlib.h>

#include"errors.h"
#include"celulas.h"
#include"circuit.h"

#include"wire.h"


/* 
 * Cria, Escreve e Destroi as Celulas QCA, formando um fio.
 *
 * Draws a QCA wire.
 */
int Wire( int x0, int y0, int x1, int y1,
		int clk_start, int clk_zones,
		WireForm form, boolean invert,
		int supression ) {
	int i=0, j=0, n=0, b=0, r=0, l=0, dx=0, dy=0, sup_init=0,sup_end=0;
	float extra_x = 0.0, extra_y = 0.0;
	cell c;

	/* Wire length. */
	l = ABS( x0-x1 ) + ABS( y0-y1 ) + 1;
	dx = ABS( x0-x1 );
	dy = ABS( y0-y1 );
	/* Clock zone considerations. */
	if( 0 < clk_zones ) {
		n = l / clk_zones;/* Minimum cells per clock zone. */
		r = l % clk_zones;/* Number of clock zones with 
					one extra cell. */
	} else {
		n = l;	/* Only one clock zone. */
		r = 0;	/* There is not any extra cell. */

	}
	b = n + ((r>0)?1:0);	/* Next clock zone border. */
	/* Supression considerations. */
	if( supression > 0 ) {
		sup_init = 0;
		sup_end = supression -1;
	} else {
		sup_init = l + supression;
		sup_end = l;
	}

	/* Create a QCA cell. */
	c = new_cell( 0, 0, normal, 0, clk_start );
	c -> x = x0 * (CELL_X_DIM + CELL_SPACING);
	c -> y = y0 * (CELL_Y_DIM + CELL_SPACING);

	/* Select how to draw the wire. */
	switch( form ) {
		case xy :
			/* Prepare the extra spacing needed to accommodate the
			 * half cell used to invert the signal. */
			if( yes == invert ){
				/* Try to put the half cell 
				 * in the X part first. */
				if( dx > 1 ){
					extra_x = (CELL_X_DIM/2)/(dx-1);
					dx --;
				/* If it doesn't fit in the X part, 
				 * try to put it in the Y part. */
				}else if( dy > 1 ){
					extra_y = (CELL_Y_DIM/2)/(dy);
					dy --;
				/* It doesn't fit anywhere.
				 * Complain about it! */
				}else{
					/* TODO */
				}
			}
			
			/* Star drawing the X part of the wire. */
			for( i = 0, j = 0 ; i < dx ; i++, j++, 
				c -> x += ( x0 < x1 )?
				(CELL_X_DIM + CELL_SPACING + extra_x) :
				-(CELL_X_DIM + CELL_SPACING + extra_x) ) {
				/* Clock adjustment. */
				if( b == j ) { /* Have we reached the border? */
					r --;	/* One less extra cell. */
					b += n + ( (r>0)?1:0 );
					c -> clk --; /* Next clk zone. */
				}
				if( j<sup_init || j>sup_end ) {
					write_cell( NULL, c );
				}
			}
			if( extra_x > 0 ) {
				/* The X part of the wire isn't complete yet. */
				c -> x -= ( x0 < x1 )?
					(CELL_X_DIM/4) :
					-(CELL_X_DIM/4) ;
				/* Clock adjustment. */
				if( b == j ) { /* Have we reached the border? */
					r --;	/* One less extra cell. */
					b += n + ( (r>0)?1:0 );
					c -> clk --; /* Next clk zone. */
				}
				if( j<sup_init || j>sup_end ) {
					write_half_cell( NULL, c, vertical );
				}
				i++;
				j++;
				c -> x += ( x0 < x1 )?
					(CELL_X_DIM*3/4 + CELL_SPACING) :
					-(CELL_X_DIM*3/4 + CELL_SPACING) ;
			}

			/* Continue with the Y part. */
			for( i = 0 ; i <= dy ; i++, j++, 
				c -> y += ( y0 < y1 )?
				(CELL_Y_DIM + CELL_SPACING + extra_y) :
				-(CELL_Y_DIM + CELL_SPACING + extra_y) ) {
				/* Clock adjustment. */
				if( b == j ) { /* Have we reached the border? */
					r --;
					b += n + ( (r>0)?1:0 );
					c -> clk --;
				}
				if( j<sup_init || j>sup_end ) {
					write_cell( NULL, c );
				}
			}
			if( extra_y > 0 ) {
				/* The Y part of the wire isn't complete yet. */
				c -> y -= ( y0 < y1 )?
					(CELL_Y_DIM/4) :
					-(CELL_Y_DIM/4) ;
				/* Clock adjustment. */
				if( b == j ) { /* Have we reached the border? */
					r --;	/* One less extra cell. */
					b += n + ( (r>0)?1:0 );
					c -> clk --; /* Next clk zone. */
				}
				if( j<sup_init || j>sup_end ) {
					write_half_cell( NULL, c, horizontal );
				}
				i++;
				j++;
				c -> y += ( y0 < y1 )?
					(CELL_Y_DIM*3/4 + CELL_SPACING) :
					-(CELL_Y_DIM*3/4 + CELL_SPACING) ;
			}
			break;
		case yx :	/* TODO The following code was not tested. */
			/* Prepare the extra spacing needed to accommodate the
			 * half cell used to invert the signal. */
			if( yes == invert ){
				/* Try to put the half cell 
				 * in the Y part first. */
				if( dy > 1 ){
					extra_y = (CELL_Y_DIM/2)/(dy-1);
					dy --;
				/* If it doesn't fit in the Y part, 
				 * try to put it in the X part. */
				}else if( dx > 1 ){
					extra_x = (CELL_X_DIM/2)/(dx);
					dx --;
				/* It doesn't fit anywhere.
				 * Complain about it! */
				}else{
					/* TODO */
				}
			}
			
			/* Star drawing the Y part of the wire. */
			for( i = 0, j = 0 ; i < dy ; i++, j++, 
				c -> y += ( y0 < y1 )?
				(CELL_Y_DIM + CELL_SPACING + extra_y) :
				-(CELL_Y_DIM + CELL_SPACING + extra_y) ) {
				/* Clock adjustment. */
				if( b == j ) { /* Have we reached the border? */
					r --;
					b += n + ( (r>0)?1:0 );
					c -> clk --;
				}
				if( j<sup_init || j>sup_end ) {
					write_cell( NULL, c );
				}
			}
			if( extra_y > 0 ) {
				/* The Y part of the wire isn't complete yet. */
				c -> y -= ( y0 < y1 )?
					(CELL_Y_DIM/4) :
					-(CELL_Y_DIM/4) ;
				/* Clock adjustment. */
				if( b == j ) { /* Have we reached the border? */
					r --;	/* One less extra cell. */
					b += n + ( (r>0)?1:0 );
					c -> clk --; /* Next clk zone. */
				}
				if( j<sup_init || j>sup_end ) {
					write_half_cell( NULL, c, horizontal );
				}
				i++;
				j++;
				c -> y += ( y0 < y1 )?
					(CELL_Y_DIM*3/4 + CELL_SPACING) :
					-(CELL_Y_DIM*3/4 + CELL_SPACING) ;
			}

			/* Continue with the X part. */
			for( i = 0 ; i <= dx ; i++, j++,
				c -> x += ( x0 < x1 )?
				(CELL_X_DIM + CELL_SPACING + extra_x) :
				-(CELL_X_DIM + CELL_SPACING + extra_x) ) {
				/* Clock adjustment. */
				if( b == j ) { /* Have we reached the border? */
					r --;
					b += n + ( (r>0)?1:0 );
					c -> clk --;
				}
				if( j<sup_init || j>sup_end ) {
					write_cell( NULL, c );
				}
			}
			if( extra_x > 0 ) {
				/* The X part of the wire isn't complete yet. */
				c -> x -= ( x0 < x1 )?
					(CELL_X_DIM/4) :
					-(CELL_X_DIM/4) ;
				/* Clock adjustment. */
				if( b == j ) { /* Have we reached the border? */
					r --;	/* One less extra cell. */
					b += n + ( (r>0)?1:0 );
					c -> clk --; /* Next clk zone. */
				}
				if( j<sup_init || j>sup_end ) {
					write_half_cell( NULL, c, vertical );
				}
				i++;
				j++;
				c -> x += ( x0 < x1 )?
					(CELL_X_DIM*3/4 + CELL_SPACING) :
					-(CELL_X_DIM*3/4 + CELL_SPACING) ;
			}
			break;
		default:
			/* Unknown form. */
			;
	}

	/* Clean up. */
	kill_cell( c );
	
	return 0;
}

/*
 * Prints a Majority/And/Or Gate with the especified dimensions (L,H),
 * centered at the specifed position (X,Y) and with the specified
 * clock for the output cell.
 *	 A    B    C
 * 	 #    #    #	/
 * 	...  ...  ...	|
 * 	 #    #    #	|
 * 	 ##..###..##	H
 * 	      #		|
 *	/---- L ---/	/
 *
 * Returns the inputs clock zone.
 */
int PrintGate( gate g, int* x_offset, int* y_offset ) {
	function_type t = inv;
	int x = 0, y = 0;
	cell c = NULL;
	int dLl = 0, dLr = 0, dH = 0;
	int L = 0, newL = 0, shortL = 0, short_zones = 0, 
		clk_zones = 0, clk = 0;
	boolean invert = no;
	
	/*
	 * Preprocess the gate parameters.
	 */
	t = g -> t;
	clk = g -> place.clk_out;
	/* H = dH + 3; ( H >= 6 ) */
	dH = MIN_CLK_ZONE;

	/* Two special cases. */
	if( inv == t ) {
		/* Ignore INVs as separate gates. 
		 * They will be put in wires. */
		return clk;
	} else if ( p_input == t) {
		/* Just print the input cell. (by now... TODO) */
		
		/* Calculate and save the coordinates. (always needed) */
		g -> place.x_final =
		x = (g -> place.x + (*x_offset)) * 3;

		g -> place.y_final =
		y = - ( (g -> place.y + (*y_offset)) * (2*MIN_CLK_ZONE) 
			+ g -> place.y_offset -1 /* one cell lower */ );
		
		/* Writing this simgle cell may be avoid if it is included 
		 * in the input wires mesh. */
		/*
		c = new_cell( x * (CELL_X_DIM + CELL_SPACING),
			(y+1) * (CELL_Y_DIM + CELL_SPACING),
			normal, 0, clk );
		c -> label = g -> id;
		
		write_cell( NULL, c );
		c -> label = NULL;
		kill_cell( c );
		*/

		return clk;
	}

	/* L = dLl + dLr + 3; ( L >= 7 ) */
	dLl = (g->place.x - g->inputs[ 0 ]->place.x) * 3 - 1;
	if( dLl < 2 ) {
		dLl = 2;
	}
	dLr = (g->inputs[ g->n_inputs -1 ]->place.x - g->place.x) * 3 - 1;
	if( dLr < 2 ) {
		dLr = 2;
	}

	/* Adjust input wire length to follow layout rule of Maximum 
	 * and Minimum wire length within the same clock zone. */
	
	/* Find the longest input wire length. */
	newL = L = MAX_2( dLl, dLr ) + dH + 1;
	
	/* Find the shortest input wire length. */
	if( maj == t ) {
		/* The central input wire is the shortest. */
		shortL = dH;
	} else {
		/* The "other" is the shortest. */
		shortL = MIN_2( dLl, dLr ) + dH + 1;
	}

	/* Minimum clock zones in the longest wire. */
	clk_zones = CLK_ZONES( newL, MAX_CLK_ZONE );
	/* Maximum clock zones in the shortest wire. */
	short_zones = shortL / MIN_CLK_ZONE;
	
	/* Iteratively adjust wire lengths. */
	while( clk_zones > short_zones ) {
		
		/* Adjust lengths. */
		newL += MIN_CLK_ZONE * ( clk_zones - short_zones );
		shortL += MIN_CLK_ZONE * ( clk_zones - short_zones );

		/* Recalculate the number of clock zones. */
		/* Minimum clock zones in the longest wire. */
		clk_zones = CLK_ZONES( newL, MAX_CLK_ZONE );
		/* Maximum clock zones in the shortest wire. */
		short_zones = shortL / MIN_CLK_ZONE;
	}

	/* Effectively adjust the wire length. */
	dH += newL - L;

	/* TODO Check how much dH can be reduce in this case.
	 * AND REDUCE IT. */

	/* Determine where the central cell will be placed. */
	x = (g -> place.x + (*x_offset)) * 3;
	y = - ( (g -> place.y + (*y_offset)) * (2*MIN_CLK_ZONE) 
			+ g -> place.y_offset );
	/* Export the Y offset. */
	*y_offset = newL - L;
	
	/* Central cell. */
	c = new_cell( x * (CELL_X_DIM + CELL_SPACING),
			y * (CELL_Y_DIM + CELL_SPACING),
			normal, 0, clk );
	write_cell( NULL, c );
	/* Left cell (input A). */
	c -> x = (x-1) * (CELL_X_DIM + CELL_SPACING);
	write_cell( NULL, c );
	/* Right cell (input C). */
	c -> x = (x+1) * (CELL_X_DIM + CELL_SPACING);
	write_cell( NULL, c );
	/* Top cell (input B). */
	c -> x = (x) * (CELL_X_DIM + CELL_SPACING);
	c -> y = (y-1) * (CELL_Y_DIM + CELL_SPACING);
	write_cell( NULL, c );
	/* Bottom cell (output). */
	c -> x = x * (CELL_X_DIM + CELL_SPACING);
	c -> y = (y+1) * (CELL_Y_DIM + CELL_SPACING);
	if( yes == g -> p_output ) {
		c -> t = output;
	}
	c -> label = g -> id;
	write_cell( NULL, c );
	c -> label = NULL;
	c -> t = normal;

	/* The cells preceding the majority gate must be in the 
	 * preceding clock zone. 
	 * This applies to all the following cells.
	 * The modulo 4 conversion is done only in the writing function. */
	c -> clk = clk - 1;

	/* 
	 * Left corner (input A). 
	 */
	switch( g -> inputs[ 0 ] -> t ) {
		case inv:
		case nand:
		case nor:
			/* Inverting gates. */
			invert = yes;
			break;
		default:
			invert = no;
	}
	Wire( x-2, y, x-1-dLl, y-1-dH, clk-1, clk_zones, xy, invert, 0 );

	/*
	 * Right corner (input C).
	 */ 
	switch( g -> inputs[ g->n_inputs -1 ] -> t ) {
		case inv:
		case nand:
		case nor:
			/* Inverting gates. */
			invert = yes;
			break;
		default:
			invert = no;
	}
	Wire( x+2, y, x+1+dLr, y-1-dH, clk-1, clk_zones, xy, invert, 0 );

	/*
	 * The central input.
	 */
	switch( t ) {
		case maj:
			/* Input B wire. */
			switch( g -> inputs[ 1 ] -> t ) {
				case inv:
				case nand:
				case nor:
					/* Inverting gates. */
					invert = yes;
					break;
				default:
					invert = no;
			}

			Wire( x, y-2, x, y-1-dH, clk-1, clk_zones, xy,
				invert, 0 );
			break;
		case and:
		case nand:
			/* Fixed polarization cell, with '0' logic value. */
			c -> x = (x) * (CELL_X_DIM + CELL_SPACING);
			c -> y = (y-2) * (CELL_Y_DIM + CELL_SPACING);
			c -> t = constante;
			c -> pol = -1.0;
			c -> label = "Zero";
			write_cell( NULL, c );
			c -> label = NULL;
			break;
		case or:
		case nor:
			/* Fixed polarization cell, with '1' logic value. */
			c -> x = (x) * (CELL_X_DIM + CELL_SPACING);
			c -> y = (y-2) * (CELL_Y_DIM + CELL_SPACING);
			c -> t = constante;
			c -> pol = +1.0;
			c -> label = "One";
			write_cell( NULL, c );
			c -> label = NULL;
			break;
		default:
			/* None. */
			break;
	}
	
	/* At this momment c->t may be "normal" or "constante". */
	kill_cell( c );

	/* Save the expected clock zone at the inputs. */
	return clk - 1 - clk_zones;
}

/*
 * EOF
 */
