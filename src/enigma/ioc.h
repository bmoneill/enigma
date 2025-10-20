/**
 * @file enigma/ioc.h
 *
 * This file declares functions for cracking Enigma configurations
 * using the Index of Coincidence method.
 */
#ifndef ENIGMA_IOC_H
#define ENIGMA_IOC_H

#include "crack.h"

// Average English index of coincidence is 1.73/26
static const float enigma_ioc_english_min = 0.05769; // 1.5/26
static const float enigma_ioc_english_max = 0.07692; // 2.0/26

// Average German index of coincidence is 2.05/26
static const float enigma_ioc_german_min = 0.06730; // 1.75/26
static const float enigma_ioc_german_max = 0.08654; // 2.25/26

float enigma_ioc_score(const enigma_crack_config_t*, const char*);

#endif
