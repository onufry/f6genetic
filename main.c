/* 
 * File:   main.c
 * Author: Łukasz Szymura
 *
 * Created on 17 listopad 2013, 08:42
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "random_Gauss.h"
#include "gen.h"

#define PLIK_OCENA "ocena.txt"
#define PLIK_SELEKCJA "selekcja.txt"
#define PLIK_KRZYZOWANIE "krzyzowanie.txt"
#define PLIK_MUTACJA "mutacja.txt"
#define PLIK_BEST "best.txt"


//uint32_t gen_size =500;
//double p_cross= 0.5;
//double p_mut=0.1;

/**
 * Funkcja porównująca dla qsort
 * @param a
 * @param b
 * @return 
 */
int rosnaco (const void * a, const void * b)
{
    const PARAMETRY_OSOBNIKA *a1 = a;
    const PARAMETRY_OSOBNIKA *b1 = b;
    if (a1->ocena > b1->ocena) return 1;
    else if (a1->ocena < b1->ocena) return -1;
  else return 0; 
}

/**
 * Funkcja porównująca dla qsort
 * @param a
 * @param b
 * @return 
 */
int malejaco (const void * a, const void * b)
{
    const PARAMETRY_OSOBNIKA *a1 = a;
    const PARAMETRY_OSOBNIKA *b1 = b;
    if (a1->ocena < b1->ocena) return 1;
    else if (a1->ocena > b1->ocena) return -1;
  else return 0; 
}


int main(void) {
   
    
    /* Zmienne */
    char plik_wynik[20] = "wynik";
    char *txt = ".txt";
    double dop_pop = 0; /* suma ocen wszystkich osobników danej generacji */
    double ocena_aver = 0; /* Średnia ocena danej populacji */
    PARAMETRY_OSOBNIKA population[POP_S]; /* Populacja początkowa */
    PARAMETRY_OSOBNIKA population_temp[POP_S]; /* Populacja po selekcji */
    PARAMETRY_OSOBNIKA population_mut[POP_S]; /* Populacja po mutacji */
    PARAMETRY_OSOBNIKA population_new[POP_S]; /* Populacja po krzyżowaniu */
    PARAMETRY_OSOBNIKA best; /* najlepszy osobnik */
    int _gen_size = -1;
    double _p_cross = -1;
    double _p_mut = -1;
    
    /**
     * Wprowadzanie parametrów do algorytmu:
     * gen_size - liczba generacji
     * p_cross - prawdopodobieństwo krzyżowania
     * p_mut - prawdopodobieństwo mutacji
     */
    printf ("Podaj liczbę generacji: ");
    scanf ("%d", &_gen_size);
    if(_gen_size<1){
        printf ("Liczba generacji musi być większa od 0!\n");
        return -1;
    }
    else{
        gen_size = _gen_size;
    }
    
    printf ("Podaj prawdopodobieństwo krzyżowania (0-1): ");
    scanf ("%lf", &_p_cross);
    if(_p_cross<0||_p_cross>1){
        printf ("Prawdopodobieństwo musi być z przedziału od 0 do 1!\n");
        return -1;
    }
    else{
        p_cross = _p_cross;
    }
    
    printf ("Podaj prawdopodobieństwo mutowania (0-1): ");
    scanf ("%lf", &_p_mut);
    if(_p_mut<0||_p_mut>1){
        printf ("Prawdopodobieństwo musi być z przedziału od 0 do 1!\n");
        return -1;
    }
    else{
        p_mut = _p_mut;
    }
    
  
    for(int p=0; p<10;p++){
    
   /*Inicjalizacja parametrów do rozkładu liniowego*/
    rozklad_init();
    
    /**
     * Inicjalizacja populacji
     * @return 
     */
    pop_init(population);
    best.ocena = 0;
    char plik_wynik[20] = "wynik";
    char powt[100];
    sprintf (powt, "%d_Gen%d_Pk%.1f_Pm%.1f", p, gen_size, p_cross,p_mut);
    strcat(powt, txt);
    strcat(plik_wynik, powt);
    
    FILE *plik_danych_best;
    plik_danych_best = fopen(plik_wynik, "w");
    fprintf(plik_danych_best,"Lp  F6        x1       x2        Ocena     Średni    Najgorszy\n");


    for(int g=0; g<gen_size;g++){
        /**
         * Ocena osobników
         * @return 
         */       
        for(int i=0; i<POP_S; i++){
            double x = 0.0;
            double t;
            for(int j = 0; j<DIM; j++){
                uint16_t fenotyp = 0;
                for(int k=j*16;k<(j+1)*16;k++){
                    fenotyp=(fenotyp<<1)+population[i].genotyp[k];
                }
                population[i].fenotyp[j]=fenotyp;
                t = (double)(fenotyp)/65535;
                t*=(double)200;
                t-=(double)100;
                population[i].x[j] = t;
                x = x+pow(t,2);                     
            }
            population[i].ocena = 1-(0.5+(pow(sin(sqrt(x)),2)-0.5)/pow(1+0.001*x, 2));

            /* Dopasowanie populacji */
            dop_pop += population[i].ocena;
        }  
        qsort(population, POP_S, sizeof(PARAMETRY_OSOBNIKA), malejaco);

#ifdef PRINT
        FILE *plik_danych_ocena;
        plik_danych_ocena = fopen(PLIK_OCENA, "w");
        fprintf(plik_danych_ocena,"Lp Chromosom        Ocena    Fenotyp\n");
        for(int i=0;i<POP_S;i++){
            char chrom[DIM*16];
            for(int l=0;l<DIM*16;l++){
                sprintf(&chrom[l], "%d", population[i].genotyp[l]);
            }
            fprintf(plik_danych_ocena,"%d %s %f %d\n",i,chrom,population[i].ocena,population[i].fenotyp[0]);
        }
        fclose(plik_danych_ocena);
#endif

        /**
         * Selekcja metodą ruletki
         */
        int k_s = selection(population, population_temp, dop_pop);
        
#ifdef PRINT
        FILE *plik_danych_selekcja;
        plik_danych_selekcja = fopen(PLIK_SELEKCJA, "w");
        fprintf(plik_danych_selekcja,"Lp Chromosom        Ocena    Fenotyp\n");
        for(int i=0;i<POP_S;i++){
            char chrom[DIM*16];
            for(int l=0;l<DIM*16;l++){
                sprintf(&chrom[l], "%d", population_temp[i].genotyp[l]);
            }
            fprintf(plik_danych_selekcja,"%d %s %f %d\n",i,chrom,population_temp[i].ocena,population_temp[i].fenotyp[0]);
        }
        fclose(plik_danych_selekcja);
#endif
        /**
         * Krzyrzowanie
         * @return population potomna
         */
        crossing(population_temp,population_new, k_s);

#ifdef PRINT
        FILE *plik_danych_krzyzowanie;
        plik_danych_krzyzowanie = fopen(PLIK_KRZYZOWANIE, "w");
        fprintf(plik_danych_krzyzowanie,"Lp Chromosom        Ocena    Fenotyp\n");
        for(int i=0;i<POP_S;i++){
            char chrom[DIM*16];
            for(int l=0;l<DIM*16;l++){
                sprintf(&chrom[l], "%d", population_new[i].genotyp[l]);
            }
            fprintf(plik_danych_krzyzowanie,"%d %s %f %d\n",i,chrom,population_new[i].ocena,population_new[i].fenotyp[0]);
        }
        fclose(plik_danych_krzyzowanie);
#endif
        /**
         * Mutacja
         * @return 
         */
        mutation(population_new, population_mut);

#ifdef PRINT
        FILE *plik_danych_mutacja;
        plik_danych_mutacja = fopen(PLIK_MUTACJA, "w");
        fprintf(plik_danych_mutacja,"Lp Chromosom        Ocena    Fenotyp\n");
        for(int i=0;i<POP_S;i++){
            char chrom[DIM*16];
            for(int l=0;l<DIM*16;l++){
                sprintf(&chrom[l], "%d", population_mut[i].genotyp[l]);
            }
            fprintf(plik_danych_mutacja,"%d %s %f %d\n",i,chrom,population_mut[i].ocena,population_mut[i].fenotyp[0]);
        }
        fclose(plik_danych_mutacja);
#endif
        
        /**
         * Nadpisanie zmutowanej polulacji na populację początkową
         * @return 
         */
        memcpy(population,population_mut,sizeof(population_mut));
        
        
        /* Wybranie najlepszego osobnika */
        qsort(population, POP_S, sizeof(PARAMETRY_OSOBNIKA), malejaco);
        if(population[0].ocena>best.ocena) memcpy(&best,&population[0],sizeof(population[0]));
        
        //printf("Best: %.12f\n", (1-best.ocena));
        
        double _ocena=0;
        for(int i=0;i<POP_S;i++){
            _ocena += population[i].ocena;
        }
        
        ocena_aver = _ocena/(double)POP_S;
            
        char chrom[DIM*16];
        for(int l=0;l<DIM*16;l++){
                sprintf(&chrom[l], "%d", best.genotyp[l]);
            }
        fprintf(plik_danych_best,"%d %f %f %f %f %f %f\n", g,(1-best.ocena), best.x[0],best.x[1], best.ocena, ocena_aver, population[POP_S-1].ocena);
       
    }
    
        fclose(plik_danych_best);
    }
    
    return (EXIT_SUCCESS);
}

