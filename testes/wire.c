#include<stdio.h>
#include<stdlib.h>

#define MAX_2(a,b) ( (a>b)? (a) : (b) )
#define MAX_3(a,b,c) ( (a>b)? MAX_2(a,c) : MAX_2(b,c) )
#define CLK_ZONES( L, MAX ) \
        ( (L%MAX)? (L/MAX+1) : (L/MAX) )

int main( int argc, char* argv[] ) {
	unsigned int L=0, newL=0, Max=0, Min=0, clks=0, i=0;

	if( 4 == argc ) {
		L = atoi( argv[1] );
		Max = atoi( argv[2] );
		Min = atoi( argv[3] );
	} else {
		printf("Passa 3 inteiros na linha de comandos.\n");
		return -1;
	}

	newL = L;
	do{
		clks = CLK_ZONES( newL, Max );
		newL = Min * ( clks -1 ) + L;
		i++;
	}while( CLK_ZONES( newL, Max ) != clks );

	printf("Dados: L=%u MAX=%u Min=%u\n"\
		"Resultado: newL=%u clks=%u (%u iteracoes)\n",
			L, Max, Min, newL, clks, i );
	return 0;
}
