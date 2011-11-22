/*
 *	Ficheiro: yacc_util.c
 *	
 *	Modulo que contem funcoes utilizados no yacc
 *      	  para traduzir, concatenar strings. 
 */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"errors.h"
#include"yacc_util.h"

/*
 * Concatena duas strings e devolve a string resultante.
 */
char* cola( char* s1, char* s2 ) {
	char* res;

	res = (char*)malloc( sizeof(char)*( strlen(s1)+strlen(s2)+1 ) );
	if( NULL == res ) {
		fprintf(stderr,"Erro ao alocar memoria em cola().\n");
		exit( MEM_ALLOC_ERROR );
	}

	strcpy( res, s1 );
	strcat( res, s2 );

	return res;
}

/*
 * Imprime um dado numero de tabs para um ficheiro.
 */
void identa( FILE* fp, unsigned char ident ) {
	while( ident-- )
		fputc( '\t', fp );
}

void processa_argumentos( int argc, char* argv[], boolean* source_file, char** spl,
				boolean* c_file, char** c_code,
				boolean* compile, char** executable,
				boolean* debug ) {
	int i;
	for( i = 1 ; i < argc ; i++ ){
		if( (!strcmp( "-s", argv[ i ] )) && (i<(argc-1)) ) {
			*source_file = yes;
			*spl = argv[ i + 1 ];
			i++;
		}else if( (!strcmp( "-c", argv[ i ] )) && (i<(argc-1)) ) {
			*c_file = yes;
			*c_code = argv[ i + 1 ];
			i++;
		}else if( !strcmp( "-d", argv[ i ] ) ) {
			*debug = yes;
		}else if( !strcmp( "-e", argv[ i ] ) ) {
			*compile = yes;
			if( i<(argc-1) ) {
				if( strcmp("-s",argv[i+1])
						&& strcmp("-c",argv[i+1]) 
						&& strcmp("-d",argv[i+1]) ) {
					*executable = argv[i+1];
					i++;
				}
			}
		}else {
			fprintf( stderr, "Opcao desconhecida: %s\n", argv[i] );
		}
	}
	return;	

}

char* troca_extensao( char* original, char* ext_orig, char* ext_nova ) {
	char* novo;
	int len_orig, len_e_o, len_e_n;

	len_orig = strlen( original );
	len_e_o = strlen( ext_orig );
	len_e_n = strlen( ext_nova );

	/* Primeiro procuramos a extensao original no texto original SSE COUBER LA. */
	if( len_orig > len_e_o ) {
		if( !strncmp( original+(len_orig-len_e_o) , ext_orig, len_e_o ) ) {
			novo = (char*)malloc(sizeof(char)*(len_orig-len_e_o+len_e_n+1));
			strncpy( novo, original, len_orig-len_e_o );
			novo[ len_orig-len_e_o ] = '\0';
			strcat( novo, ext_nova );
			return novo;
		}
	}

	/* Simplesmente concatenamos a nova extensao. */
	novo = (char*)malloc(sizeof(char)*(len_orig+len_e_n+1));
	strcpy( novo, original );
	strcat( novo, ext_nova );

	return novo;
}
/*
int main() {
	printf( "%s\n", troca_extensao( "nome", "extensao", "muito_grande_extensao_x") );
}
*/

/*
 * EOF
 */
