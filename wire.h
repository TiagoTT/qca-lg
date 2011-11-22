/*
 * Ficheiro: wire.h
 * 
 * Desenho de fios.
 *
 * Wire design.
 */
#ifndef _WIRE_H_
#define _WIRE_H_

/* Constants. */
#define MIN_CLK_ZONE	3
#define MAX_CLK_ZONE	10

/* Data structures. */
typedef enum OrientationEnum{ right, top, left, bottom }orientation;
typedef enum WireFormEnum{ xy, yx }WireForm;

/* Functions. */
int Wire( int x0, int y0, int x1, int y1, 
	int clk_start, int clk_zones,
	WireForm form, boolean invert,
	int supression );

int PrintGate( gate g, int* x_offset, int* y_offset );

/* Macros. */
#define CLK_ZONES( L, MAX ) \
	( ((L)%(MAX))? ((L)/(MAX)+1) : ((L)/(MAX)) )

#endif
/*
 * EOF
 */
