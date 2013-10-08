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
#include "olc.h"
#include "interp.h"
#include "recycle.h"
#include "lookup.h"
#include "tables.h"


void do_allclan( CHAR_DATA *ch, char *argument )
{
	if(IS_NPC(ch))
		return;

	if(!IS_SET(ch->comm,COMM_ALLCLAN))
	{
		SET_BIT(ch->comm,COMM_ALLCLAN);
		send_to_one(ch,"All clan channels will now be heard.");
		return;
	}
	else
	{
		REMOVE_BIT(ch->comm,COMM_ALLCLAN);
		send_to_one(ch,"You cease listening to all clans.");
		return;
	}
	return;
}

void do_iclan( CHAR_DATA *ch, char *argument )
{
	char arg[MSL];
//	char arg2[MSL];
	ORGANIZATION  *clan;
	DESCRIPTOR_DATA *d;

	argument = one_argument(argument,arg);
//	argument = one_argument(argument,ar2);

	if(IS_NPC(ch))
		return;

	if( argument[0] == '\0' || arg[0] == '\0')
		//|| arg2[0] == '\0')
	{
		send_to_one(ch,"Syntax: iclan <clan> <message>");
		return;
	}

	clan = org_lookup(arg);

	if(clan == 0)
	{
		send_to_one(ch,"There is no such clan.");
		return;
	}

	if(clan == ch->clan)
	{
		send_to_one(ch,"How about you use YOUR clan channel?");
		return;
	}
	
	send_to_one(ch,"You [ %s ] clan '%s'",clan->name,argument);

	for(d = descriptor_list; d; d = d->next)
	{
		if(d->connected == CON_PLAYING
			&& d->character != ch
			&& d->character->clan == clan
			&& !IS_SET(d->character->comm,COMM_NOCLAN)
			&& !IS_SET(d->character->comm,COMM_QUIET))
		{
			send_to_one(d->character,"%s clans '%s'",ch->name,argument);
		}
	}
	return;
}


/* clan channels */
void do_clantalk( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if (!is_clan(ch))
    {
	send_to_char("You aren't in a clan.\n\r",ch);
	return;
    }
    if ( argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOCLAN))
      {
        send_to_char("Clan channel is now ON\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_NOCLAN);
      }
      else
      {
        send_to_char("Clan channel is now OFF\n\r",ch);
        SET_BIT(ch->comm,COMM_NOCLAN);
      }
      return;
    }

        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
         send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
          return;
        }

        REMOVE_BIT(ch->comm,COMM_NOCLAN);
	  if(!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
		  argument = makedrunk(argument,ch);

      sprintf( buf, "You clan '%s'{x\n\r", argument );
      send_to_char( buf, ch );
      sprintf( buf, "$n clans '%s'{x", argument );
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
	     is_same_clan(ch,d->character) &&
             !IS_SET(d->character->comm,COMM_NOCLAN) &&
	     !IS_SET(d->character->comm,COMM_QUIET) )
        {
            act_new("$n clans '$t'{x",ch,argument,d->character,TO_VICT,POS_DEAD);
        }
		if(IS_SET(d->character->comm,COMM_ALLCLAN) &&
			d->connected == CON_PLAYING &&
             d->character != ch
			 && !is_same_clan(d->character, ch))
		{
			send_to_one(d->character,"[ %s ] %s clans '%s'{x",ch->clan->name,ch->name,
				argument);
		}
    }

    return;
}