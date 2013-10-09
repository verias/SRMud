/*
 *  RPXP for pesky and evil player deletion. 
 *  Written by Verias of Shattered Reality(HeavyMud <rom derived>) (c) Keith Wood 
 *.
 */

/* standard includes */
#if defined(macintosh)
#include <types.h>
#include <time.h>
#else
#include <sys/types.h>
#include <time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"


int countwords(char *str)
{
	int n = 0;
    for(str=strtok(str, " -.!,;"); str; str=strtok(NULL, " -.!,;"))
        n++;
    return n;
}

int calc_rpxp(CHAR_DATA *ch, char *str)
{
	int award = 0;
	int amt;
	int cap = 500;
	
	amt = countwords(str);
	if (amt < 5 )
	{
		award = 1;
	}
	else if (amt >= cap )
	{
		award = cap;
	}
	else
		award = (30 + (amt));// + people in room, will math soon


	ch->pcdata->rpxpbonus += award;
	return award;
}
