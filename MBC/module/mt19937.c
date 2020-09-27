#include "common.h"

/* Period parameters */  
#define N 624u
#define M 397u
#define UMASK 0x80000000u		/* most significant w-r bits */
#define LMASK 0x7FFFFFFFu		/* least significant r bits */
#define MATRIX_A 0x9908B0DFu	/* constant vector a */
#define MIXBITS(u,v) (((u) & UMASK) | ((v) & LMASK))
#define TWIST(u,v) ((MIXBITS(u, v) >> 1) ^ (((v) & 1) ? MATRIX_A : 0))

static unsigned int init, next, state[N]; /* the array for the state vector  */

/* initializes state[N] with a seed */
void init_rand(unsigned int seed)
{
	unsigned int i, j;

	for (*state = (j = seed), i = 1; i < N; ++i)
		state[i] = (j = ((j ^ (j >> 30)) + i) * 1812433253u);

	next = init = i;
}

/* generates a random number on [0,0xffffffff]-interval */
unsigned int gen_rand(void)
{
	unsigned int i, j;

    if (next >= N) { /* generate N words at one time */
		if (init == 0)
			init_rand(5489u);

		for (i = 0; i < N - M; ++i) {
			j = (state[i] & UMASK) | (state[i + 1u] & LMASK);
			state[i] = state[i + M] ^ (j >> 1) ^ ((j & 1) ? MATRIX_A : 0);
		}

		for (; i < N - 1u; ++i) {
			j = (state[i] & UMASK) | (state[i + 1u] & LMASK);
			state[i] = state[i + (M - N)] ^ (j >> 1) ^ ((j & 1) ? MATRIX_A : 0);
		}

		j = (state[i] & UMASK) | (*state & LMASK);
		state[i] = state[i + (M - N)] ^ (j >> 1) ^ ((j & 1) ? MATRIX_A : 0);
		next = 0;
    }

	i = state[next++];
	i ^= (i >> 11);
	i ^= (i << 7) & 0x9D2C5680u;
	i ^= (i << 15) & 0xEFC60000u;
	i ^= (i >> 18);

	return i;
}