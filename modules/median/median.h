/*
 * median.h
 *
 *  Created on: Oct 5, 2022
 *      Author: grigory
 */

#ifndef MEDIAN_MEDIAN_H_
#define MEDIAN_MEDIAN_H_

/* Includes */

#include "stm32f4xx.h"

/* Public Preprocessor defines */

#define MEDIAN_BUFFER_LENGTH	9

/* Public functions (prototypes) */

uint32_t median_get_median(uint32_t newElement);

#endif /* MEDIAN_MEDIAN_H_ */
