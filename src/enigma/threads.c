#include "threads.h"

#include <stdlib.h>
#include <string.h>

const enigma_crack_config_t* global_cfg = NULL;
enigma_t* enigma_enigmas = NULL;
char* enigma_plaintexts = NULL;
pthread_t* enigma_threads = NULL;
int enigma_threadCount = 0;
int* enigma_freeThreads = NULL;
int* enigma_threadArgs = NULL;
pthread_mutex_t spawn_mutex = PTHREAD_MUTEX_INITIALIZER;

void *(*enigma_thread_main)(void*);

void enigma_crack_multithreaded(enigma_crack_config_t* config, void* (*thread_main)(void*)) {
    global_cfg = config;
    enigma_enigmas = malloc(global_cfg->maxThreads * sizeof(enigma_t));
    enigma_plaintexts = calloc(global_cfg->maxThreads * (global_cfg->ciphertextLen + 1), sizeof(char));
    enigma_threads = malloc(global_cfg->maxThreads * sizeof(pthread_t));
    enigma_freeThreads = calloc(global_cfg->maxThreads, sizeof(int));
    enigma_threadArgs = malloc(global_cfg->maxThreads * 2 * sizeof(int));
    memset(enigma_freeThreads, 1, global_cfg->maxThreads * sizeof(int));

    int flags = 0;
    if (!(global_cfg->flags & ENIGMA_PREDEFINED_ROTOR_SETTINGS)) {
        flags |= ENIGMA_FLAG_ROTORS;
    }
    if (!(global_cfg->flags & ENIGMA_PREDEFINED_ROTOR_POSITIONS)) {
        flags |= ENIGMA_FLAG_POSITIONS;
    }
    if (!(global_cfg->flags & ENIGMA_PREDEFINED_REFLECTOR)) {
        flags |= ENIGMA_FLAG_REFLECTOR;
    }
    if (!(global_cfg->flags & ENIGMA_PREDEFINED_PLUGBOARD_SETTINGS)) {
        flags |= ENIGMA_FLAG_PLUGBOARD;
    }

    config->flags = flags;

    memcpy(enigma_enigmas, &global_cfg->enigma, sizeof(enigma_t));

    enigma_thread_main = (void* (*)(void*))thread_main;

    thread_main((int[]){global_cfg->flags, 0});

    while (1) {
        int done = 0;
        for (int i = 0; i < global_cfg->maxThreads; i++) {
            done += enigma_freeThreads[i] ? 1 : 0;
        }

        if (done >= global_cfg->maxThreads) {
            break;
        }
    }

    free(enigma_freeThreads);
    free(enigma_threads);
    free(enigma_plaintexts);
    free(enigma_enigmas);
    free(enigma_threadArgs);
}

void enigma_spawn(int flags, int parent) {
    pthread_mutex_lock(&spawn_mutex);
    int threadNum = -1;

    while (threadNum == -1) {
        for (int i = 1; i < global_cfg->maxThreads; i++) {
            if (enigma_freeThreads[i]) {
                threadNum = i;
                break;
            }
        }
    }
    enigma_threadArgs[threadNum * 2] = flags;
    enigma_threadArgs[threadNum * 2 + 1] = threadNum;

    memcpy(&enigma_enigmas[threadNum], &enigma_enigmas[parent], sizeof(enigma_t));
    pthread_create(&enigma_threads[threadNum], NULL, enigma_thread_main, &enigma_threadArgs[threadNum * 2]);
    enigma_freeThreads[threadNum] = 0;
    pthread_mutex_unlock(&spawn_mutex);
}
