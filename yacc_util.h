/*
 *	Ficheiro: yacc_util.h
 *	
 *	Modulo que contem funcoes utilizados no yacc
 *      	  para traduzir, concatenar strings. 
 */

#ifndef _YACC_UTIL_H_
#define _YACC_UTIL_H_

/* Tipo de dados boolean (true ou false) */
typedef enum { no, yes } boolean;
/* VERY IMPORTANT: no=0, yes=1. DO NOT CHANGE THE ORDER OF THE ENUM ELEMENTS. */

/*
typedef enum{ 
	condicao, 
	pre_instrucao, 
	instrucao, 
	declaracao, 
	pos_declaracao,
	bloco 
}tipo_erro;
*/

/* Funcoes locais deste modulo. */
char* cola( char* s1, char* s2 );
void processa_argumentos( int argc, char* argv[], boolean* source_file, char** spl, boolean* c_file, char** c_code, boolean* compile, char** executable, boolean* debug );
char* troca_extensao( char* original, char* ext_orig, char* ext_nova );


#endif
