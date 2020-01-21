
#include "arbrebin.h"
#include "bfile.h"
#include "fap.h"
#include "huffman_code.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

typedef struct {
    int tab[256];
} TableOcc_t;

struct code_char HuffmanCode[256];

int EstFeuille(Arbre a);
void Coder_symb(Arbre huff,int code[], int lg);
void Affiche_arbreCode();

void ConstruireTableOcc(FILE *fichier, TableOcc_t *TableOcc) {
    int c;
    for (int i = 0; i < 256; i++){
        TableOcc->tab[i]= 0;
    }
    while (( c = fgetc(fichier))!= EOF) {
        TableOcc->tab[c]++;
    }
    printf("\n**** ConstruireTableOcc done ****\n");
   
    for (int i = 0; i < 256; i++) {
            if (TableOcc->tab[i]!= 0)
                printf("Occurences du caractere %c (code %d) : %d\n", i, i, TableOcc->tab[i]);
    }
}

fap InitHuffman(TableOcc_t *TableOcc) {
    Arbre a;
    fap f;
    f = creer_fap_vide();
    
    for (int i = 0; i < 256; i++) {
        if (TableOcc->tab[i]!= 0){
            a = NouveauNoeud(ArbreVide(),i,ArbreVide());
            f = inserer(f, a, TableOcc->tab[i]);
        }
    }
    printf("\n**** InitHuffman done ****\n");
    return f;
}

Arbre ConstruireArbre(fap file) {
    Arbre x, y, z = NULL;
    int prx, pry,prz;
    if (!est_fap_vide(file)){
        x = ArbreVide();
        y = ArbreVide();
        z = ArbreVide();

        while(!est_fap_vide(file)){
            file = extraire(file, &x, &prx);
            file = extraire(file, &y, &pry);
            z = NouveauNoeud(x,'$', y);
            file =inserer(file, z ,prx+pry);

            file = extraire(file, &z, &prz);
            if (!est_fap_vide(file)){
                file =inserer(file, z, prz);
            }
        }
    }


    printf("\n**** ConstruireArbre done ****\n");
    return z;
}


void ConstruireCode(Arbre huff) {
	int code[256],lg=0;
    if (!EstVide(huff)){
        Coder_symb(huff,code,lg);
    }

	printf("\n**** ConstruireCode done ****\n");
}

void Coder_symb(Arbre huff,int code[], int l){
		if(EstFeuille(huff)){
				HuffmanCode[Etiq(huff)].lg = l;
				memcpy(HuffmanCode[Etiq(huff)].code,code,(sizeof(int)*l));
		}else{
					if (!EstVide(FilsGauche(huff))){
						code[l] = 0;
						Coder_symb(FilsGauche(huff),code,l+1);
					}
					if (!EstVide(FilsDroit(huff))){
						code[l] = 1;
						Coder_symb(FilsDroit(huff),code,l+1);
					}
		}
}

void Affiche_arbreCode(){
        for (int i = 0; i < 256; ++i){
        if (HuffmanCode[i].lg!= 0){
            printf("Symbole:%c\tASCII:%d\tlongeur:%d\t",i,i,HuffmanCode[i].lg);
            for (int j = 0; j < HuffmanCode[i].lg; ++j){
                printf("%d",HuffmanCode[i].code[j]);
            }
            printf("\n");
        }
    }
}
void Encoder(FILE *fic_in, FILE *fic_out, Arbre ArbreHuffman) {
        int c,err=0;
        BFILE *f;
        EcrireArbre(fic_out,ArbreHuffman);
        f = bstart(fic_out,"w");
        assert(f!=NULL);
        while((c = getc(fic_in)) != EOF){
        	for (int i = 0; i < HuffmanCode[c].lg && err != -1; ++i){
        		err=bitwrite(f,HuffmanCode[c].code[i]);
        		printf("%d",HuffmanCode[c].code[i]);
        	}
        		printf("\t");
        }
        bstop(f);
        printf("\n**** Encodage done ****\n");
}

int EstFeuille(Arbre a){
	if (EstVide(FilsGauche(a))&&EstVide(FilsDroit(a)))
		return 1;
	return 0;
}


int main(int argc, char *argv[]) {
    clock_t debut, fin; 
    double duree;
    TableOcc_t TableOcc;
    FILE *fichier;
    FILE *fichier_encode;

    debut = clock();

    fichier = fopen(argv[1], "r");
    /* Construire la table d'occurences */
    ConstruireTableOcc(fichier, &TableOcc);
    fclose(fichier);

    /* Initialiser la FAP */
    fap file = InitHuffman(&TableOcc);

    /* Construire l'arbre d'Huffman */
    Arbre ArbreHuffman = ConstruireArbre(file);
 
    AfficherArbre(ArbreHuffman);

    /* Construire la table de codage */
    ConstruireCode(ArbreHuffman);
    Affiche_arbreCode();

    
    /* Encodage */
    fichier = fopen(argv[1], "r");
    fichier_encode = fopen(argv[2], "w");

    Encoder(fichier, fichier_encode, ArbreHuffman);
    
 
    fclose(fichier_encode);
    fclose(fichier);
 
    LibererArbre(ArbreHuffman);
    fin = clock(); 
    duree = (double)(fin - debut) / CLOCKS_PER_SEC;  
    printf( "Temps d'execution: %f seconds\n", duree);
    return 0;
}
