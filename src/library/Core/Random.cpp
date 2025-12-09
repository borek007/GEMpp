#include "Random.h"
#include <unistd.h>

void Random::seedRand(int seed) {
    if(seed < 0)
        srand(time(NULL) + GETPID() * 1000);
    else
        srand(seed);
    rand(); // The first value is not so random...
}

double Random::randDouble() {
    return (double)rand()/(double)RAND_MAX;
}

double Random::randDouble(double lower, double upper) {
    return randDouble()*(upper - lower) + lower;
}

int Random::randInt(int lower, int upper) {
    return lower + (int) ((upper + 1 - lower)*(rand()/(RAND_MAX + (double)lower)));
}
