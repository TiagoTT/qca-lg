/*
 * Ficheiro: main.c
 * 
 * Programa.
 */
#include<stdio.h>
#include<stdlib.h>

#include"errors.h"

#include"circuit.h"
#include"hash.h"
#include"stack.h"

#include"celulas.h"
#include"layout.h"

extern FILE* lsi_out;
extern FILE* mvsis_gate_out;

extern circuit lsi_cir;
extern circuit mvsis_gate_cir;

int main( int argc, char* argv[], char* env[] ) {
	FILE* out = NULL;
	circuit cir = NULL;

	/* Read the circuit's logic description.
	 * Leitura da descricao logica do circuito. 
	 */
/*	out = stdout;	*/
	out = fopen( "/dev/null", "w" );
	if( NULL == out ) {
		fprintf( stderr, "Error opening file.\n" );
		exit( FILE_OPEN_ERROR );
	} else {
		lsi_out = mvsis_gate_out = out;
	}
	
	/* Set Up an empty circuit. */
	cir = lsi_cir = mvsis_gate_cir =new_cir();

	/* Chose the parser to fill the circuit. */
	if( 1 < argc ) {
		mvsis_gate_parse();
	} else {
		lsi_parse();
	}
/*	cir_do( cir, print_gate );	*/

	/* Translate to Majority logic.
	 * Traducao para logica Maioritaria.
	 */
	
	/* Map the circuit's Majority (or AND/OR) logic into QCA.
	 * Mapeamento da logica Maioritaria para QCA.
	 */
	

	/* Gate replication to make layout easier.
	 * Replicacao de logica para facilitar o layout.
	 */
	Expand( cir );

	/*
	 * Atribuicao de posicoes pre-layout.
	 * Pre-layout positions attribution.
	 */
/*	CircuitPlace( cir ); */

	/* Write the QCADesigner compatible desciption of the circuit.
	 * Escrita da descricao do circuito no formato compativel com o 
	 * QCADesigner.
	 */
	Layout( cir );

	/*
	 * Clean Up.
	 */
	kill_cir( cir );

	fclose( out );
	
	exit( NO_ERROR );
}

/*
 * EOF
 */
