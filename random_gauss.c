/* 
 * File:   random_Gauss.c
 * Author: Łukasz
 *
 * Created on 17 listopad 2013, 08:42
 */


#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "random_gauss.h"

//static double a,b,c;


/*
double RozkladLiniowy() {
    static int n;
    static double d,x;
    d = (a + b + c) * 8192;
    x = fmod(d,4294967291.0);
    a = b; b = c; c= x;
    printf("a w rozklad: %f \n", a);
 
    if (x < (float) 2147483648.0) {
        n = (int) x;
    } else {
        n = (int) (x - 4294967296.0);
    }
 
    return (n * 2.3283064365e-10 + 0.5); //zakres od 0 do 1
}
*/



/**
 * 
 * 
 * @return 
 */
double GeneratorGaussa() {
 
    bool ok = false;
    double limit = sqrt(2/2.718281828);
    double X;
 
    do {
        // generowanie liczby o rozkladnie rownomiernym z przedzialu (0,1)
        double U = RozkladLiniowy();
	    
        // generowanie liczby o rozkladnie rownomiernym z przedzialu (-sqrt(2/e),sqrt(2/e))
        double V = 2 * limit * RozkladLiniowy() - limit;
        X = V / U;
 
        if (X*X <= 2*(3-U*(4+U))) {
 
            ok = true;
 
        } else if (X*X <= 2/U - 2*U) {
 
            if (X*X <= -4*log(U)) {
               ok = true;
            }
 
        }
    } while (!ok);
 
    return X;
}

