/* 
 * File:   gen.h
 * Author: Łukasz Szymura
 *
 * Created on 17 listopad 2013, 14:46
 */
#include <stdint.h>

#ifndef GEN_H
#define	GEN_H


#define POP_S 100 /* rozmiar populacji */
#define DIM 2 /* wymiar funkcji F6 */
//#define PRINT


typedef struct  parametry_osobnika_s{
	uint16_t fenotyp[DIM];
	uint16_t genotyp[DIM*16];
	double ocena; //funkcja przystosowania
        double pr_wyboru; /* Prawdopodobieństwo wyboru */
        double dystr; /* Dystrybuanta */
        int krzyz; /* 1 - nadaje się do krzyzowania, 0 - nie nadaje się*/
        double x[DIM];
} PARAMETRY_OSOBNIKA;



/* Prototypy */
PARAMETRY_OSOBNIKA osobnik_init();
void rozklad_init();
double RozkladLiniowy();
int selection(PARAMETRY_OSOBNIKA *populacja, PARAMETRY_OSOBNIKA *nowa_populacja, double dop_pop);
void crossing(PARAMETRY_OSOBNIKA *populacja, PARAMETRY_OSOBNIKA *populacja_potomna, int k_s);
void pop_init(PARAMETRY_OSOBNIKA *populacja);
void mutation (PARAMETRY_OSOBNIKA *populacja, PARAMETRY_OSOBNIKA *new_populacja);

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* GEN_H */

