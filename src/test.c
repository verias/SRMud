#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "recycle.h"
#include "magic.h"
#include "tables.h"
#include "recycle.h"


//Local functions


void do_test(CHAR_DATA *ch, char *argument)
{

		char arg[MSL];

		argument = one_argument(argument, arg);

		if (IS_NPC(ch))
		{
			return;
		}

		if(arg[0] == '\0')
		{

			send_to_one(ch, "Syntax: test <function to test>");
			return;
		}


		return;
}

