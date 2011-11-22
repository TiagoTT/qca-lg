/*
 * Ficheiro: celulas.h
 * 
 * Definicao das estruturas de dados que suportam as celulas QCA.
 * Cabecalhos de funcoes.
 */

#ifndef _CELULAS_H_
#define _CELULAS_H_

/* Parametros. Deveriam ser lidos de um ficheiro. */
#define CELL_X_DIM	18.0
#define CELL_Y_DIM	18.0
#define CELL_SPACING	2.0
#define N_DOT	4
#define DOT_DIAMETER	5.0
#define DOT_CHARGE_FULL	1.602176e-19
#define DOT_CHARGE_HALF	8.010882e-20
#define DOT_CHARGE_NULL	0.000000e+00
#define SELECTED	"TRUE" /* "FALSE" */

/* Predefinitions of cell's parameters. */
#define NORMAL_STRING	"QCAD_CELL_NORMAL"
#define CONSTANTE_STRING	"QCAD_CELL_FIXED"
#define INPUT_STRING	"QCAD_CELL_INPUT"
#define OUTPUT_STRING	"QCAD_CELL_OUTPUT"
#define UNKNOWN_STRING	"QCAD_CELL_UNKNOWN"

#define GREEN	"clr.red=0\nclr.green=65535\nclr.blue=0\n"
#define PURPLE	"clr.red=65535\nclr.green=0\nclr.blue=65535\n"
#define TURQUOISE "clr.red=0\nclr.green=65535\nclr.blue=65535\n"
#define WHITE	"clr.red=65535\nclr.green=65535\nclr.blue=65535\n"
#define BROWN	"clr.red=65535\nclr.green=32768\nclr.blue=0\n"
#define BLUE	"clr.red=0\nclr.green=0\nclr.blue=65535\n"
#define YELLOW	"clr.red=65535\nclr.green=65535\nclr.blue=0\n"
#define WRONG	"clr.red=30000\nclr.green=30000\nclr.blue=0\n"

#define TR 0	/* Top Right */
#define BR 1	/* Bottom Right */
#define BL 2	/* Bottom Left */
#define TL 3	/* Top Left */


/* Estruturas. */
typedef enum TipoEnum{ normal, constante, input, output }tipo;
typedef enum PartEnum{ horizontal, vertical }part;

typedef struct CellStr {
	float x;	/* X Position. */
	float y;	/* Y Position. */
	tipo t;		/* Cell type. */
	float pol;	/* Initial polarization. */
	int clk;	/* Clock zone. */
	char* label;	/* Cell label (optional). */
	/* incompleto */
}Cell;

typedef Cell* cell;

/* Funcoes. */
cell new_cell( float x, float y, tipo t, float pol, int clk );
void kill_cell( cell c );
void write_cell( FILE* output, cell c );
void write_half_cell( FILE* output, cell c, part p );
const char* TipoToString( tipo t );
const char* GetColorString( tipo t, int clk );

#endif

/*
 * EOF
 */
