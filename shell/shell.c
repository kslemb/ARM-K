#include "../main.h"

#ifdef USE_NEWLIB_PRINTF
#include <stdio.h>
#endif

#ifdef USE_INTERNAL_PRINTF
#include "../generic/console.h"
#endif

static const char * const	Shell_Version = "1.0";

void shell (void)
{
	printf("ARM-K builtin shell (version %s)\n\r", Shell_Version);

}
