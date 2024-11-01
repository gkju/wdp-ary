#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include "ary.h"

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

// jesli antyprzedzial opisuje tak naprawde przedzial pusty to zmieniamy go w normalny przedzial
wartosc normalizuj_przedzial(wartosc x) {
    if(x.anty_przedzial && isinf(x.max) && isinf(x.min) && x.min < 0) {
        x.anty_przedzial = false;
        x.min = x.max = NAN;
    } else if(x.anty_przedzial && isinf(x.min) && x.min < 0) {
        x.anty_przedzial = false;
        x.min = x.max;
        x.max = inf;
    } else if(x.anty_przedzial && isinf(x.max)) {
        x.anty_przedzial = false;
        x.max = x.min;
        x.min = neg_inf;
    }
    return x;
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

void swap_double(double *a, double *b) {
    double tmp = *a;
    *a = *b;
    *b = tmp;
}

// p podane w %
wartosc wartosc_dokladnosc(double x, double p) {
    wartosc to_return = (wartosc) {
        .anty_przedzial = false,
        .min = (x * (100.0 - p)) / 100.0,
        .max = (x * (100.0 + p)) / 100.0
    };

    if(x < 0) {
        swap_double(&to_return.min, &to_return.max);
    }

    return to_return;
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

bool in_wartosc(wartosc x, double y) {
    x = normalizuj_przedzial(x);

    if(is_empty(x)) {
        return false;
    }
    
    if(x.anty_przedzial) {
        return is_leq(y, x.min) || is_geq(y, x.max);
    }

    return is_geq(y, x.min) && is_leq(y, x.max);
}

/*
bool in_wartosc(wartosc x, wartosc y) {
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
        return in_wartosc(y, x);
    }
}
*/

double min_wartosc(wartosc x) {
    x = normalizuj_przedzial(x);
    if(x.anty_przedzial) {
        return neg_inf;
    }

    return x.min;
}

double max_wartosc(wartosc x) {
    x = normalizuj_przedzial(x);
    if(x.anty_przedzial) {
        return inf;
    }

    return x.max;
}

double sr_wartosc(wartosc x) {
    x = normalizuj_przedzial(x);
    if(is_empty(x)) {
        return NAN;
    }

    if(x.anty_przedzial) {
        return NAN;
    }

    return (x.min + x.max) / 2.0;
}

// Zwraca odwrotność przedziału x
wartosc odwrotnosc(wartosc x) {
    if(dokladnie_zero(x)) {
        return (wartosc) {
            .anty_przedzial = false,
            .min = NAN,
            .max = NAN
        };
    }

    if(x.anty_przedzial) {
        // jesli nie zawiera 0 to jasne, ze z wykresu 1/x bedzie to jakis przedzial (bez 0, ale nie ma to znaczenia bo modyfikatory domykaja)
        x.anty_przedzial = false;
        if(zawiera_zero(x)) {
            double min = 1.0 / x.min;
            double max = 1.0 / x.max;

            wartosc to_return = (wartosc) {
                .anty_przedzial = false,
                .min = min,
                .max = max
            };

            // jesli antyprzedzial byl [0, cos] to ma zawierac wszystkie <= 1/cos
            if(is_zero(x.min)) {
                to_return.min = neg_inf;
            } else if(is_zero(x.max)) {
                to_return.max = inf;
            }

            to_return = normalizuj_przedzial(to_return);
            return to_return;
        }
        x.anty_przedzial = true;
        return (wartosc) {
            .anty_przedzial = true,
            .min = 1.0 / x.max,
            .max = 1.0 / x.min
        };
    }

    if(zawiera_zero(x)) {
        // zasadniczo to samo co wyzej, ale warto wyroznic co jak lewy/prawy koniec to 0
        double min = 1.0 / x.min;
        double max = 1.0 / x.max;

        wartosc to_return = (wartosc) {
            .anty_przedzial = true,
            .min = min,
            .max = max
        };

        // jesli przedzial byl [0, cos] to ma nie zawierac ujemnych
        if(is_zero(x.min)) {
            to_return.min = neg_inf;
        } else if(is_zero(x.max)) {
            to_return.max = inf;
        }

        to_return = normalizuj_przedzial(to_return);
        return to_return;
    }

    return (wartosc) {
        .anty_przedzial = false,
        .min = 1.0 / x.max,
        .max = 1.0 / x.min
    };
}

wartosc plus(wartosc a, wartosc b) {
    a = normalizuj_przedzial(a);
    b = normalizuj_przedzial(b);
    if(is_empty(a) || is_empty(b)) {
        return (wartosc) {
            .anty_przedzial = false,
            .min = NAN,
            .max = NAN
        };
    }

    if(a.anty_przedzial && b.anty_przedzial) {
        return (wartosc) {
            .anty_przedzial = false,
            .min = neg_inf,
            .max = inf
        };
    }

    if(a.anty_przedzial) {
        // moze nam wyjsc cale R, ifujemy to zeby konce sie przypadkiem nie zamienily miejscami
        if(is_geq(a.min + b.max, a.max + b.min)) {
            return (wartosc) {
                .anty_przedzial = false,
                .min = neg_inf,
                .max = inf
            };
        }

        return (wartosc) {
            .anty_przedzial = true,
            .min = a.min + b.max,
            .max = a.max + b.min
        };
    } else if(b.anty_przedzial) {
        return plus(b, a);
    }

    return (wartosc) {
        .anty_przedzial = false,
        .min = a.min + b.min,
        .max = a.max + b.max
    };
}

wartosc przeciwnienstwo(wartosc x) {
    x = normalizuj_przedzial(x);
    double oldmin = x.min;
    double oldmax = x.max;
    x.min = -oldmax;
    x.max = -oldmin;
    
    return x;
}

wartosc minus(wartosc a, wartosc b) {
    return plus(a, przeciwnienstwo(b));
}

wartosc razy(wartosc a, wartosc b) {
    a = normalizuj_przedzial(a);
    b = normalizuj_przedzial(b);
    if(is_empty(a) || is_empty(b)) {
        return (wartosc) {
            .anty_przedzial = false,
            .min = NAN,
            .max = NAN
        };
    }

    if(dokladnie_zero(a) || dokladnie_zero(b)) {
        return wartosc_dokladna(0.0);
    }

    if(a.anty_przedzial && b.anty_przedzial) {
        // jesli otoczenie lewo/prawostronne zera jest w ktorymkolwiek to mozna z tego otoczenia brac dowolnie male elementy a drugiego dowolnie duze na modul dowolnego znaku wiec dostajemy cale R
        if(zawiera_zero(a) || zawiera_zero(b)) {
            return (wartosc) {
                .anty_przedzial = false,
                .min = neg_inf,
                .max = inf
            };
        }

        // nasze antyprzedzialy nie zawieraja 0 - nie zawieraja jakiegos przedzialu od < 0 do > 0 - wiec latwo zauwazyc z jakie elementy beda zawierac
        double min = fmax(a.min * b.max, a.max * b.min);
        double max = fmin(a.min * b.min, a.max * b.max);
        return (wartosc) {
            .anty_przedzial = true,
            .min = min,
            .max = max
        };
    }

    if(a.anty_przedzial) {
        if(zawiera_zero(b)) {
            // tak jak poprzednio, samo otoczenie zera + antyprzedzial daja nam cale R
            return (wartosc) {
                .anty_przedzial = false,
                .min = neg_inf,
                .max = inf
            };
        }

        if(is_geq(b.min, 0)) {
            // sprawdzamy jak duze liczby mozemy uzyskac biorac liczby z lewej czesci a * rzeczy z b
            double ekstrema_lewe = fmax(a.min * b.min, a.min * b.max);
            // sprawdzamy jak male liczby mozemy uzyskac biorac liczby z prawej czesci a * rzeczy z b
            double ekstrema_prawe = fmin(a.max * b.min, a.max * b.max);

            // jesli np pomnozymy antyprzedzial [1, 2] przez przedzial [-1, 3] to dostaniemy cale R
            if(ekstrema_prawe <= ekstrema_lewe) {
                return (wartosc) {
                    .anty_przedzial = false,
                    .min = neg_inf,
                    .max = inf
                };
            }

            return (wartosc) {
                .anty_przedzial = true,
                .min = ekstrema_lewe,
                .max = ekstrema_prawe
            };
        } else {
            return przeciwnienstwo(razy(a, przeciwnienstwo(b)));
        }
    } else if(b.anty_przedzial) {
        return razy(b, a);
    }

    // w przypadku przedzialow zaleznie od tego czy zawieraja elementy dodatnie ujemne czy i takie i takie kazda mozliwosc nalezy rozwazyc
    double x1 = a.min * b.min, x2 = a.min * b.max, x3 = a.max * b.min, x4 = a.max * b.max;
    double min = fmin(x1, fmin(x2, fmin(x3 ,x4)));
    double max = fmax(x1, fmax(x2, fmax(x3, x4)));
    return (wartosc) {
        .anty_przedzial = false,
        .min = min,
        .max = max
    };
}

wartosc podzielic(wartosc a, wartosc b) {
    return razy(a, odwrotnosc(b));
}

int main() {
    return 0;
}