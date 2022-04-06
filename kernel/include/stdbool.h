#pragma once

#ifndef __cplusplus

#define bool	_Bool

#if defined __STDC_VERSION__ && __STDC_VERSION__ > 201710L

#define true	((_Bool)+1u)
#define false	((_Bool)+0u)

#else

#define true	1
#define false	0

#endif

#else

#define _Bool	bool

#endif

#define __bool_true_false_are_defined	1
