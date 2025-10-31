/**
 * @file enigma/ioc.h
 *
 * This file declares functions for cracking Enigma configurations
 * using the Index of Coincidence method.
 */
#ifndef ENIGMA_IOC_H
#define ENIGMA_IOC_H

#include "crack.h"

#define ENIGMA_IOC_ENGLISH     (1.73 / 26)
#define ENIGMA_IOC_ENGLISH_MIN (ENIGMA_IOC_ENGLISH - 0.25)
#define ENIGMA_IOC_ENGLISH_MAX (ENIGMA_IOC_ENGLISH + 0.25)

#define ENIGMA_IOC_GERMAN     (2.05 / 26)
#define ENIGMA_IOC_GERMAN_MIN (ENIGMA_IOC_GERMAN - 0.25)
#define ENIGMA_IOC_GERMAN_MAX (ENIGMA_IOC_GERMAN + 0.25)

float enigma_ioc_score(const enigma_crack_t*, const char*);

#endif
