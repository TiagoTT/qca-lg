#include<stdio.h>

typedef struct {
	unsigned char h:1;
	unsigned char g:1;
	unsigned char f:1;
	unsigned char e:1;
	unsigned char d:1;
	unsigned char c:1;
	unsigned char b:1;
	unsigned char a:1;
}inputs;

typedef union {
	unsigned char byte;
	inputs var;
}it;

#define MAJ(a,b,c) (((a)&(b))|((a)&(c))|((b)&(c)))
#define FUNC(a,b,c,d,e,f,g,h) (MAJ( ~(MAJ(f,g,h)), h, MAJ(f,g,~h) ))

int main() {
	it i;
	
	printf("Tabela da Verdade\nabcdefgh|int|out\n");
	for( i.byte = 0 ; i.byte < 8 ; i.byte ++ ) {
		printf("%d%d%d%d%d%d%d%d|%3d|%d\n",
			i.var.a, i.var.b, i.var.c, i.var.d,
			i.var.e, i.var.f, i.var.g, i.var.h,
			i.byte,
			FUNC( i.var.a, i.var.b, i.var.c, i.var.d,
				i.var.e, i.var.f, i.var.g, i.var.h));
	}

	return 0;
}

