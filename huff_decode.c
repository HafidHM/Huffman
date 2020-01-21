#include <stdio.h>
#include "arbrebin.h"
#include "assert.h"
#include "bfile.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>

int EstFeuille(Arbre a){
	if (EstVide(FilsGauche(a))&&EstVide(FilsDroit(a)))
		return 1;
	return 0;
}

void Decoder(FILE *fichier_encode, Arbre ArbreHuffman) {
        int bit;
        BFILE *f;
        AfficherArbre(ArbreHuffman);
       Arbre huff = ArbreVide();
       huff=ArbreHuffman;
	 		f = bstart(fichier_encode,"r");
	 		printf("\nText decode:\n");
	 		while((bit = bitread(f))!= -1){
	 				   				assert(bit!=-1);
						 			switch(bit){
						 				case 0:
						 						if (EstFeuille(FilsGauche(huff))){
						 							printf("%c",Etiq(FilsGauche(huff)));
						 							huff=ArbreHuffman;
						 						}else{
													huff = FilsGauche(huff);
												}
						 					break;
						 				case 1:
												if (EstFeuille(FilsDroit(huff))){
						 							printf("%c",Etiq(FilsDroit(huff)));
						 							huff=ArbreHuffman;
						 						}else{
													huff = FilsDroit(huff);
												}
						 					break;
						 			} 
	 		} 		
		bstop(f); 
		printf("\n**** Decodage done ****\n");
}


int main(int argc, char *argv[]) {
	clock_t debut, fin; 
    double duree;

	debut = clock();
    FILE *fichier_encode = fopen(argv[1], "r");

    Arbre ArbreHuffman = LireArbre(fichier_encode);

    Decoder(fichier_encode, ArbreHuffman);
    fclose(fichier_encode);
    LibererArbre(ArbreHuffman);

    fin = clock(); 
    duree = (double)(fin - debut) / CLOCKS_PER_SEC;  
    printf( "Temps d'execution: %f seconds\n", duree);

    return 0;
}
