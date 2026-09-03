/*-----------------------------------------------------------------------------

	mother.h

	Mother - 나만의 수학 솔루션.

	Mother 마스터 헤더 파일.

	2000.8.2 KJTINC

-----------------------------------------------------------------------------*/

#ifndef __MOTHER_H__
#define __MOTHER_H__

#include "typedef.h"
#include <math.h>

#define PERCENTAGE						100

double Proportion(double target, double target_max, double base);
double Percentage(double target, double target_max);

#endif