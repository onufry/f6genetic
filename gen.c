/* 
 * File:   gen.c
 * Author: Łukasz Szymura
 *
 * Created on 17 listopad 2013, 14:46
 */

#include "gen.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>

static double a,b,c;
extern double p_cross;
extern double p_mut;
/**
 * Generowanie zmiennej losowej z rozkładu liniowego 
 * @return zmienna losowa (0,1) 
 */
double RozkladLiniowy() {
    static int n;
    static double d,x;
    d = (a + b + c) * 8192;
    x = fmod(d,4294967291.0);
    a = b; b = c; c= x;
 
    if (x < (float) 2147483648.0) {
        n = (int) x;
    } else {
        n = (int) (x - 4294967296.0);
    }
 
    return (n * 2.3283064365e-10 + 0.5); //zakres od 0 do 1
}

/**
 * Inicjalizacja rozkładu liniowego
 */
void rozklad_init(){
    
    a = time(NULL);
    b = time(NULL);
    c = time(NULL);
}



/**
 * Inicjalizacja populacji
 * @param population
 */
void pop_init(PARAMETRY_OSOBNIKA *population){
    for(int i=0; i<POP_S; i++){
        population[i] = osobnik_init();       
    }
}

/**
 * Inicjalizacja pojedyńczego osobnika
 * @return 
 */
PARAMETRY_OSOBNIKA osobnik_init(){
    PARAMETRY_OSOBNIKA nowy_osobnik;
     
        for(int j=0; j<DIM; j++){
            /*Losowanie fenotypu*/
            nowy_osobnik.fenotyp[j]=(65535*RozkladLiniowy());
            for(int k=0; k<16; k++){
                /* kodowanie binarne genotypu */
                nowy_osobnik.genotyp[k+j*16] = (nowy_osobnik.fenotyp[j]>>(15-k))&1;
            }
        }
    nowy_osobnik.ocena = 0;
    nowy_osobnik.pr_wyboru = 0;
    nowy_osobnik.dystr = 0;
    nowy_osobnik.krzyz = 0;

    return nowy_osobnik;
}

/**
 * Selekcja nowej populacji metodą ruletki
 * @param population
 * @param new_population
 */
int selection(PARAMETRY_OSOBNIKA *population, PARAMETRY_OSOBNIKA *new_population, double dop_pop){
    int cross_s = 0;
        /* Konstruowanie ruletki */
    for(int i=0; i<POP_S;i++){
        /* Prawdopodobieństwo wyboru chromosomu i (im większa ocena tym większe prawdopodobieństwo wyboru) */
        population[i].pr_wyboru = population[i].ocena / dop_pop;
        /* Dystrybuanta chromosomu i */
        for(int j=0; j<=i; j++){
            population[i].dystr += population[j].pr_wyboru;
        }
    }
    /* Losowanie osobnika metodą ruletki */
    for(int i=0; i<POP_S;i++){
        int wybrano = 0;
        int j = 0;
        double r = RozkladLiniowy();
        /* Metoda odwrotnej dystrybuanty */
        do{
            if(r<population[j].dystr)
            {
                memcpy(&new_population[i],&population[j],sizeof(population[j]));
                wybrano = 1;
            }
            else
            {
                j++;
                wybrano = 0;
            }
        }while(wybrano<1);
        
        /*oznaczenie osobnika do krzyżowania*/
        double k = RozkladLiniowy();
        if(k<p_cross){
            new_population[i].krzyz = 1;
            cross_s++;
        }
        else new_population[i].krzyz = 0;
        
       
    }

     return cross_s;
    
}


/**
 * krzyżowanie wylosowanych wcześniej osobników
 * @param population
 * @param populacja_potomna
 * @param k_s
 */
void crossing(PARAMETRY_OSOBNIKA *population, PARAMETRY_OSOBNIKA *populacja_potomna, int k_s){
    PARAMETRY_OSOBNIKA parent1, parent2;
    PARAMETRY_OSOBNIKA child1, child2;
    
    /* Indeksy osobników które nadają się do krzyżowania*/
    int pozycje[k_s];
    int j = 0;
    for(int i=0; i<POP_S; i++){
        memcpy(&populacja_potomna[i],&population[i],sizeof(population[i]));
        //populacja_potomna[i]=population[i];
        if(population[i].krzyz){
            pozycje[j]=i;
            j++;
            //printf("Ocena: %f - Pr: %f - Dyst: %f - K: %d\n", population[i].ocena, population[i].pr_wyboru, population[i].dystr, population[i].krzyz);
        }   
    }
    /* Punkt krzyżowania */
    int pk = 16*DIM*RozkladLiniowy();
        if(k_s%2==0){
            for(int i=0; i<k_s; i++){
                parent1 = population[pozycje[i]];
                parent2 = population[pozycje[i+1]];
                memcpy(child1.genotyp,parent1.genotyp,sizeof(child1.genotyp));
                memcpy(child2.genotyp,parent2.genotyp,sizeof(child2.genotyp));
                for(int j=pk+1;j<DIM*16;j++){
                    child1.genotyp[j]=parent2.genotyp[j];
                    child2.genotyp[j]=parent1.genotyp[j];
                }
                memcpy(populacja_potomna[i].genotyp,child1.genotyp,sizeof(child1.genotyp));
                memcpy(populacja_potomna[i+1].genotyp,child2.genotyp,sizeof(child2.genotyp));
                i++;
            }
        }
        else{
            for(int i=0; i<k_s-1; i++){
                parent1 = population[pozycje[i]];
                parent2 = population[pozycje[i+1]];
                memcpy(child1.genotyp,parent1.genotyp,sizeof(child1.genotyp));
                memcpy(child2.genotyp,parent2.genotyp,sizeof(child2.genotyp));
                for(int j=pk+1;j<DIM*16;j++){
                    child1.genotyp[j]=parent2.genotyp[j];
                    child2.genotyp[j]=parent1.genotyp[j];
                }
                memcpy(populacja_potomna[i].genotyp,child1.genotyp,sizeof(child1.genotyp));
                memcpy(populacja_potomna[i+1].genotyp,child2.genotyp,sizeof(child2.genotyp));
                i++;
            }
            
        } 
}

/**
 * Mutacja
 * @param population
 * @param new_population
 */
void mutation(PARAMETRY_OSOBNIKA *population, PARAMETRY_OSOBNIKA *new_population){
    for(int i=0; i<POP_S; i++){
        /* Kopiuję bazową populację */
        memcpy(&new_population[i],&population[i],sizeof(population[i]));
        /* Dla każdego genu chromosomu sprawdzam czy zachodzi mutacja */
        for(int j=0; j<DIM*16; j++){
            double r = RozkladLiniowy();
            if(r<p_mut){
                /* Jeżeli tak to zmieniam gen na przeciwny */
                new_population[i].genotyp[j]=!new_population[i].genotyp[j];
            }
        }
        
        /* Oceniam tu osobników w celu porównania ich z najlepszym wynikiem */
        double x = 0.0;
        double t;
        for(int j = 0; j<DIM; j++){
            uint16_t fenotyp = 0;
            for(int k=j*16;k<(j+1)*16;k++){
                fenotyp=(fenotyp<<1)+new_population[i].genotyp[k];
            }
            new_population[i].fenotyp[j]=fenotyp;
            t = (double)(fenotyp)/65535;
            t*=(double)200;
            t-=(double)100;
            new_population[i].x[j] = t;
            x = x+pow(t,2); 
        }
        new_population[i].ocena = 1-(0.5+(pow(sin(sqrt(x)),2)-0.5)/pow(1+0.001*x, 2));
    }
}

