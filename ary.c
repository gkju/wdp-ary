#include <math.h>
#include <stdbool.h>
#include "ary.h"
// na dzielenia trzeba sprawdzic czy min/max nie sa zero, jesli tak to ustawic na dokladnie 0.0
// jesli mianownik to dokladnie 0 to zwrocic 0 chyba ze b tez zero

// szukajac min/max do mnozenia potrzeba wziac cos niezerowego

// jak dzielimy przez cos to jesli licznik jest niezerowy to jesli mianownik nie zawiera i dodatnich i ujemnych to latwo; 
// co jak mnozymy/dzielimy przez antyprzedzial 

// wynik dodawania to po prostu bedzie max wart a / min wart b itd chyba ze ktoras zero
// dzielac potrzebuje najmniejsza dodatnia, najwieksza ujemna poza naj/najm dod/uj

double inf = HUGE_VAL;
double neg_inf = -HUGE_VAL;

bool is_zero(double x) {
    return fabs(x) < 1e-10;
}

bool is_equiv(double x, double y) {
    return is_zero(x - y);
}

// czy x >= y
bool is_geq(double x, double y) {
    return x >= y || is_equiv(x, y);
}

// czy x <= y
bool is_leq(double x, double y) {
    return x <= y || is_equiv(x, y);
}

bool is_gt(double x, double y) {
    return x > y && !is_equiv(x, y);
}

bool is_lt(double x, double y) {
    return x < y && !is_equiv(x, y);
}

// konwencyjnie przedzial jest pusty jesli min/max to nan
bool is_empty(wartosc x) {
    return isnan(x.min) || isnan(x.max);
}

bool dokladnie_zero(wartosc x) {
    x = normalizuj_przedzial(x);
    if(x.anty_przedzial) {
        return false;
    }
    return is_zero(x.min) && is_zero(x.max);
}

bool zawiera_zero(wartosc x) {
    if(x.anty_przedzial) {
        return is_geq(x.min, 0.0) || is_leq(x.max, 0.0);
    }
    return is_leq(x.min, 0.0) && is_geq(x.max, 0.0);
}

// p podane w %
wartosc wartosc_dokladnosc(double x, double p) {
    p /= 100;
    return (wartosc) {
        .anty_przedzial = false,
        .min = x * (1.0 - p),
        .max = x * (1.0 + p)
    };
}

wartosc wartosc_od_do(double x, double y) {
    return (wartosc) {
        .anty_przedzial = false,
        .min = x,
        .max = y
    };
}

wartosc wartosc_dokladna(double x) {
    return (wartosc) {
        .anty_przedzial = false,
        .min = x,
        .max = x
    };
}

// jesli antyprzedzial opisuje tak naprawde przedzial pusty to zmieniamy go w normalny przedzial
wartosc normalizuj_przedzial(wartosc x) {
    if(x.anty_przedzial && x.max == inf && x.min == neg_inf) {
        x.anty_przedzial = false;
        x.min = x.max = NAN;
    }
    return x;
}

// ta funkcja zwraca "lewa" czesc antyprzedzialu
wartosc l_antyprzedzial(wartosc x) {
    return (wartosc) {
        .anty_przedzial = false,
        .min = neg_inf,
        .max = x.min
    };
}

wartosc r_antyprzedzial(wartosc x) {
    return (wartosc) {
        .anty_przedzial = false,
        .min = x.max,
        .max = inf
    };
}

double in_wartosc(wartosc x, wartosc y) {
    x = normalizuj_przedzial(x);
    y = normalizuj_przedzial(y);
    
    if(x.anty_przedzial && y.anty_przedzial) {
        return true;
    }

    if(x.anty_przedzial) {
        wartosc lewa_czesc = l_antyprzedzial(x);
        wartosc prawa_czesc = r_antyprzedzial(x);
        return in_wartosc(lewa_czesc, y) || in_wartosc(prawa_czesc, y);
    } else if(y.anty_przedzial) {
        wartosc lewa_czesc = l_antyprzedzial(y);
        wartosc prawa_czesc = r_antyprzedzial(y);
        return in_wartosc(x, lewa_czesc) || in_wartosc(x, prawa_czesc);
    }
}

// z wykresu funkcji 1/x wiemy, ze ekstrema a/b z a != 0, a stale, b parametr, sa +- w 4 mozliwych pktach
double najmniejsza_dodatnia(wartosc x) {

}

double najwieksza_ujemna(wartosc x) {

}