#ifndef ENIGMA_THREADS_H
#define ENIGMA_THREADS_H

#include "crack.h"

#include <pthread.h>

#define ENIGMA_FLAG_REFLECTOR 1
#define ENIGMA_FLAG_ROTORS 2
#define ENIGMA_FLAG_POSITIONS 4
#define ENIGMA_FLAG_PLUGBOARD 8

void enigma_crack_multithreaded(enigma_crack_config_t*, void*(*thread_main)(void*));
void enigma_spawn(int, int);

extern const enigma_crack_config_t* global_cfg;
extern enigma_t* enigma_enigmas;
extern char* enigma_plaintexts;
extern pthread_t* enigma_threads;
extern int enigma_threadCount;
extern int* enigma_freeThreads;
extern int* enigma_threadArgs;

#endif
