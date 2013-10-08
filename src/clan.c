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
#include "tables.h"
#include "lookup.h"
#include "olc.h"



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
	int  clan;
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

	clan = clan_lookup(arg);

	if(clan == -1)
	{
		send_to_one(ch,"There is no such clan.");
		return;
	}

	if(clan == ch->clan)
	{
		send_to_one(ch,"How about you use YOUR clan channel?");
		return;
	}
	
	send_to_one(ch,"You [ %s ] clan '%s'",clan_table[clan].name,argument);

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
			is_same_clan(ch,d->character) &&
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
			send_to_one(d->character,"[ %s ] %s clans '%s'{x",clan_table[ch->clan].name,ch->name,
				argument);
		}
    }

    return;
}


/* -----------------------------------------------------------------------
The following snippet was written by Gary McNickle (dharvest) for
Rom 2.4 specific MUDs and is released into the public domain. My thanks to
the originators of Diku, and Rom, as well as to all those others who have
released code for this mud base.  Goes to show that the freeware idea can
actually work. ;)  In any case, all I ask is that you credit this code
properly, and perhaps drop me a line letting me know it's being used.

from: gary@dharvest.com
website: http://www.dharvest.com
or http://www.dharvest.com/resource.html (rom related)

Send any comments, flames, bug-reports, suggestions, requests, etc... 
to the above email address.
----------------------------------------------------------------------- */

/***************************************************************************
*       ROM 2.4 is copyright 1993-1995 Russ Taylor			   *
*       ROM has been brought to you by the ROM consortium		   *
*           Russ Taylor (rtaylor@pacinfo.com)				   *
*           Gabrielle Taylor (gtaylor@pacinfo.com)			   *
*           Brian Moore (rom@rom.efn.org)				   *
*       By using this code, you have agreed to follow the terms of the     *
*       ROM license, in the file Rom24/doc/rom.license                     *
***************************************************************************/

/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,	   *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *									   *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael	   *
 *  Chastain, Michael Quan, and Mitchell Tse.				   *
 *									   *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc	   *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.						   *
 *									   *
 *  Much time and thought has gone into this software and you are	   *
 *  benefitting.  We hope that you share your changes too.  What goes	   *
 *  around, comes around.						   *
 ***************************************************************************/


/**  file   : guild.c
  *  author : Gary McNickle (gary@dharvest.com)
  *  purpose: Meant to provide a guild system configurable without 
  *           recompiling. This is done via an ASCII file called "guild.dat"  
  *           that you can edit or create with any text editor 
  *           (vi/pico/jed/jaret/etc).  All gulid/clan related commands from 
  *           ROM 2.4 have been moved to this file in an effort to keep 
  *           related code located all in one spot.
  */







/** Function: guild_bit_name
  * Descr   : Returns the ascii name of a GUILD_ bit vector.
  * Returns : (char *)
  * Syntax  : (n/a- called by system)
  * Written : v1.0 3/98
  * Author  : Gary McNickle <gary@dharvest.com>
  */
char *clan_bit_name( int clan_flags )
{
  static char buf[512];
  
  buf[0] = '\0';
  if ( clan_flags & CLAN_INDEPENDENT	) strcat( buf, " independent"	);
  if ( clan_flags & CLAN_CHANGED	) strcat( buf, " changed"	);
  if ( clan_flags & CLAN_DELETED	) strcat( buf, " deleted"	);
 // if ( clan_flags & CLAN_WOLF		) strcat( buf, " wolfkin"	);
  if ( clan_flags & CLAN_IMMORTAL	) strcat( buf, " immortal"	);
  
  return ( buf[0] != '\0' ) ? buf+1 : "none";
}



/** Function: load_guilds
  * Descr   : Loads the information specific to each guild/clan from
  *           the ../data/guild.dat file.
  * Returns : (n/a)
  * Syntax  : (n/a)
  * Written : v1.0 1/98
  * Author  : Gary McNickle <gary@dharvest.com>
  */

void load_clans(void)
{
    FILE *fp;
    char buf[MAX_STRING_LENGTH];
    char *string;
    int count = 0;
    int i;
    bool fMatch = FALSE;

    for (i=0; i < MAX_CLAN; i++)
    {
      clan_table[i].name = "";
      clan_table[i].who_name = "";
      clan_table[i].room[0]= 0;
      clan_table[i].room[1]= 0;
      clan_table[i].room[2]= 0;
      clan_table[i].rank[0].rankname = "";
      clan_table[i].rank[0].skillname = "";
      clan_table[i].ml[0] = 0;
      clan_table[i].ml[1] = 0;
      clan_table[i].ml[2] = 0;
      clan_table[i].ml[3] = 0;
      clan_table[i].flags = 0;
    }
    
    sprintf(buf, "%sclan.dat", DATA_DIR);

    if ((fp = fopen(buf, "r")) == NULL)
    {
	log_string("Error: clan.dat file not found!");
	exit(1);
    }
    for (;;)
    {
	string = feof(fp) ? "End" : fread_word(fp);

	if (!str_cmp(string, "End"))
	    break;

	switch (UPPER(string[0]))
	  {
          case 'F':
              clan_table[count].flags  = fread_flag( fp );
              fMatch = TRUE;
              break;
              
	  case 'G':
	      count++;
	      clan_table[count].name = fread_string(fp);
	      fMatch = TRUE;
	      break;

	  case 'R':
	      if (!str_cmp(string, "Rooms"))
	      {
		  clan_table[count].room[0] = fread_number(fp);	/* hall   */
		  clan_table[count].room[1] = fread_number(fp);	/* morgue */
		  clan_table[count].room[2] = fread_number(fp);	/* temple */
		  fMatch = TRUE;
	      } else if (!str_cmp(string, "Rank"))
	      {
		  i = fread_number(fp);
		  clan_table[count].rank[i - 1].rankname = fread_string(fp);
		  fMatch = TRUE;
	      }
	      break;

	  case 'S':
	      i = fread_number(fp);
	      clan_table[count].rank[i - 1].skillname = fread_string(fp);
	      fMatch = TRUE;
	      break;

	  case 'M':
	      clan_table[count].ml[0] = fread_number(fp);
	      clan_table[count].ml[1] = fread_number(fp);
	      clan_table[count].ml[2] = fread_number(fp);
	      clan_table[count].ml[3] = fread_number(fp);
	      fMatch = TRUE;
	      break;

	  case 'W':
	      clan_table[count].who_name = fread_string(fp);
	      fMatch = TRUE;
	      break;

	  }			/* end of switch */

    }				/* end of while (!feof) */

    if (!fMatch)
    {
	bug("Fread_clans: no match.", 0);
	fread_to_eol(fp);
    }
    fclose(fp);
    return;
} /* end: load_guilds */



/** Function: is_leader
  * Descr   : Validates guild leadership of (ch).
  * Returns : True (yes, ch is a mortal leader) or False
  * Syntax  : (n/a)
  * Written : v1.1 3/98
  * Author  : Gary McNickle <gary@dharvest.com>
  */

bool is_leader(CHAR_DATA * ch)
{
    /* update by: Zak Johnson (jakj@usa.net) Thanks Zak! */
    return IS_SET(ch->act, PLR_MORTAL_LEADER) ? 1 : 0;
} /* end: is_leader */



/** Function: can_guild
  * Descr   : Validates ability of (ch) to guild other players into
  *         : a specific (or generic, if implementor) guild.
  * Returns : TRUE/FALSE
  * Syntax  : (n/a)
  * Written : v1.1 2/98
  * Author  : Gary McNickle <gary@dharvest.com>
  */

bool can_clan(CHAR_DATA * ch)
{
    /* ok if ch is a SUPREME or higher */
    if (ch->level >= SUPREME || ch->trust >= SUPREME)
	return TRUE;

    /* not ok if ch is not guilded or is not a mortal leader */
    if (ch->clan == 0 || !is_leader(ch))
	return FALSE;

    return clan_table[ch->clan].ml[0];
} /* end: can_guild */



/** Function: can_deguild
  * Descr   : Validates ability of (ch) to de-guild players from a 
  *         : particular guild, or generic (if ch=implementor)
  * Returns : TRUE/FALSE
  * Syntax  : (n/a)
  * Written : v1.1 2/98
  * Author  : Gary McNickle <gary@dharvest.com>
  */

bool can_declan(CHAR_DATA * ch)
{
    /* ok if ch is a SUPREME or higher */
    if (ch->level >= SUPREME || ch->trust >= SUPREME)
	return TRUE;

    /* not ok if ch is not guilded or is not a mortal leader */
    if (ch->clan == 0 || !is_leader(ch))
	return FALSE;

    return clan_table[ch->clan].ml[1];
} /* end: can_deguild */



/** Function: can_promote
  * Descr   : Validates ability of (ch) to promote players in their guilds
  * Returns : True/False
  * Syntax  : (n/a)
  * Written : v1.1 2/98
  * Author  : Gary McNickle <gary@dharvest.com>
  */

bool can_promote(CHAR_DATA * ch)
{
    /* ok if ch is a SUPREME or higher */
    if (ch->level >= SUPREME || ch->trust >= SUPREME)
	return TRUE;

    /* not ok if ch is not guilded or is not a mortal leader */
    if (ch->clan == 0 || !is_leader(ch))
	return FALSE;

    /* is a mortal leader, but do they have the right? */
    return clan_table[ch->clan].ml[2];
} /* end: can_promote */



/** Function: can_demote
  * Descr   : Validates ability of (ch) to demote a player within their guild.
  * Returns : True/False
  * Syntax  : (n/a)
  * Written : v1.1 2/98
  * Author  : Gary McNickle <gary@dharvest.com>
  */

bool can_demote(CHAR_DATA * ch)
{
    /* ok if ch is a SUPREME or higher */
    if (ch->level >= SUPREME || ch->trust >= SUPREME)
	return TRUE;

    /* not ok if ch is not guilded or is not a mortal leader */
    if (ch->clan == 0 || !is_leader(ch))
	return FALSE;

    return clan_table[ch->clan].ml[3];
} /* end: can_demote */




/** Function: is_clan
  * Descr   : Returns wether or not (ch) is a member of any guild/clan
  * Returns : True/False
  * Syntax  : (n/a)
  * Written : v1.0 ?
  * Author  : Rom Consortium. 
  */

bool is_clan(CHAR_DATA * ch)
{
    return ch->clan;
} /* end: is_clan */




/** Function: is_same_clan
  * Descr   : Returns wether or not (ch) and (victim) are members of
  *         : the same guild/clan
  * Returns : True/False
  * Syntax  : (n/a)
  * Written : v1.2 1/98
  * Author  : Rom Consortium. Updated by Gary McNickle <gary@dharvest.com>
  */

bool is_same_clan(CHAR_DATA * ch, CHAR_DATA * victim)
{
    if (IS_SET(clan_table[ch->clan].flags,CLAN_INDEPENDENT))
	return FALSE;
    else
	return (ch->clan == victim->clan);
} /* end: is_same_clan */



/** Function: clan_lookup
  * Descr   : Returns the numeric position of a clan name within the
  *         : clan_table 
  * Returns : (int)
  * Syntax  : (n/a)
  * Written : v1.0 
  * Author  : Rom Consortium
  */

int clan_lookup(const char *name)
{
    int clan;

    for (clan = 0; clan < MAX_CLAN; clan++)
    {
	if (!str_prefix(name, clan_table[clan].name))
	    return clan;
    }

    return 0;
} /* end: clan_lookup */

/** Function: player_rank
  * Descr   : Returns the players (ch) rank as a string.
  * Returns : char*
  * Syntax  : (n/a)
  * Written : v1.0 1/98
  * Author  : Gary McNickle <gary@dharvest.com>
  */

char *player_rank(CHAR_DATA * ch)
{
    if (ch->clan == 0)
	return '\0';
    return clan_table[ch->clan].rank[ch->rank].rankname;
} /* end: player_rank */


/** Function: player_clan
  * Descr   : Returns the name of the players (ch) clan as a string
  * Returns : char*
  * Syntax  : (n/a)
  * Written : v1.0 1/98
  * Author  : Gary McNickle <gary@dharvest.com>
  */

char *player_clan(CHAR_DATA * ch)
{
    if (ch->clan == 0)
	return '\0';
    return clan_table[ch->clan].name;
} /* end: player_clan */


/** Function: do_promote
  * Descr   : Promotes (or demotes) a character within their own guild
  * Returns : List of ranks if given no argument
  * Syntax  : promote <who> <rank #>
  * Written : v1.5 3/98
  * Author  : Gary McNickle <gary@dharvest.com>
  */

void do_clanpmote(CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int cnt;
    int sn = 0;

    argument = one_argument(argument, arg1);

    if (!can_promote(ch))
    {
	send_to_char("huh?\n\r", ch);
	return;
    }

    if (arg1[0] == '\0' || argument[0] == '\0')
    {
	/*
	 * --------------------------------------------------------------
	 * Keep in mind that we are displaying the ranks as 1 - MAX_RANK,
	 * so, since the structure is actually 0 - MAX_RANK-1, we need to
	 * set "cnt" to cnt-1.
	 * -------------------------------------------------------------- 
	 */

	send_to_char("Syntax: promote <who> <rank #>\n\r", ch);
	send_to_char("where rank is one of the following:\n\r", ch);

	for (cnt = 0; cnt < MAX_RANK; cnt++)
	{
	  sprintf(buf, "%2d] %s\n\r", cnt + 1,
 	    is_clan(ch) ? clan_table[ch->clan].rank[cnt].rankname : "(None)");
	  send_to_char(buf, ch);
	}
	send_to_char("\n\r", ch);
	return;
    }				/* end syntax */

    if ((victim = get_char_world(ch, arg1)) == NULL)
    {
	send_to_char("They aren't playing.\n\r", ch);
	return;
    }

    if (!is_clan(victim))
    {
        send_to_char("They are not a member of any guilds!\n\r", ch);
        return;
    }

    if (!is_same_clan(ch, victim) &&
	(ch->level < SUPREME))
    {
	send_to_char("They are a member of a guild different than yours!\n\r", ch);
	return;
    }

    if (!str_prefix(argument, "leader") && ch != victim)
    {
	SET_BIT(victim->act, PLR_MORTAL_LEADER);
	send_to_char("They are now a mortal leader.\n\r", ch);
	send_to_char("You have just been promoted to a leader of your guild!\n\r", victim);
	return;
    }

    cnt = atoi(argument) - 1;
    if (cnt < 0 ||
	cnt > MAX_RANK -1 ||
	clan_table[victim->clan].rank[cnt].rankname == NULL)
    {
	send_to_char("That rank does not exist!", ch);
	return;
    }
    if (cnt > victim->rank && ((ch == victim) & (!IS_IMMORTAL(ch))))
    {
	send_to_char("Heh. I dont think so...", ch);
	return;
    }

  /** bug report by: Zak Jonhnson (zakj@usa.net)
    * we were checking ch->rank here..*sigh* Thanks Zak!
    */
    if (cnt > victim->rank)
    {
	int i;

	sprintf(buf, "You have been promoted to %s!\n\r",
		clan_table[victim->clan].rank[cnt].rankname);
	send_to_char(buf, victim);

	sprintf(buf, "%s has been promoted to %s!\n\r",
	      victim->name, clan_table[victim->clan].rank[cnt].rankname);
	send_to_char(buf, ch);

	for (i = victim->rank; i < cnt; i++)
	    if (clan_table[victim->clan].rank[i].skillname != NULL)
	    {
		sn = skill_lookup(clan_table[victim->clan].rank[i].skillname);
		if (sn < 0)
		{
		    sprintf(buf, "Bug: Add skill [%s] is not a valid skill",
			    clan_table[victim->clan].rank[cnt].skillname);
		    log_string(buf);
		} else if (!victim->pcdata->learned[sn])
		    victim->pcdata->learned[sn] = 20 + (victim->level / 4);
	    }
    } 
    
    else if (cnt < victim->rank)
    {
	if (IS_SET(victim->act, PLR_MORTAL_LEADER))
	    REMOVE_BIT(victim->act, PLR_MORTAL_LEADER);

	sprintf(buf, "You have been demoted to %s!\n\r",
		clan_table[victim->clan].rank[cnt].rankname);

	send_to_char(buf, victim);
	sprintf(buf, "%s has been demoted to %s!\n\r",
	      victim->name, clan_table[victim->clan].rank[cnt].rankname);

	send_to_char(buf, ch);
	/*
	 * ---------------------------------------------------------------
	 * Note: I dont think it would be fair here to take away any skills
	 * the victim may have earned at a higher rank. It makes no RP sense
	 * to do so and only hurts the player (loss of practices etc). Imms
	 * may want to keep an eye on this, as we dont want players jumping
	 * guilds just to gain new skills.
	 * -------------------------------------------------------------- 
	 */
    }				/* else no change */
    victim->rank = cnt;
    return;
} /* end: do_promote */


/** Function: do_guild
  * Descr   : Adds a character to the membership of a guild, or optionally,
  *         : removes them from a guild.
  * Returns : (n/a)
  * Syntax  : guild <who> <clan name>
  * Written : v1.3 3/98
  * Author  : Gary McNickle <gary@dharvest.com>
  */

void do_clanadd(CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int clan;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (!can_clan(ch))
    {
	send_to_char("huh?\n\r", ch);
	return;
    }
    if (arg1[0] == '\0' || arg2[0] == '\0')
    {
	send_to_char("Syntax: guild <char> <cln name>\n\r", ch);
	return;
    }
    if ((victim = get_char_world(ch, arg1)) == NULL)
    {
	send_to_char("They aren't playing.\n\r", ch);
	return;
    }

    /** thanks to Zanthras for the bug fix here...*/
    if (is_clan(victim) && !is_same_clan(ch, victim) &&
	((ch->level < SUPREME) & (ch->trust < SUPREME)))
    {
	send_to_char("They are a member of a clan other than your own.\n\r", ch);
	return;
    }

    if (!str_prefix(arg2, "none"))
    {
	send_to_char("They are no longer a member of any clan.\n\r", ch);
	send_to_char("You are no longer a member of any clan!\n\r", victim);
	victim->clan = 0;
	victim->rank = 0;

	/* add by: Zak Jonhson (zakj@usa.net) */
	if (IS_SET(victim->act, PLR_MORTAL_LEADER))
	    REMOVE_BIT(victim->act, PLR_MORTAL_LEADER);

	return;
    }
    if ((clan = clan_lookup(arg2)) == 0)
    {
	send_to_char("No such clan exists.\n\r", ch);
	return;
    }
    sprintf(buf, "They are now a %s of the %s.\n\r",
	    clan_table[clan].rank[0].rankname, clan_table[clan].name);
    send_to_char(buf, ch);

    sprintf(buf, "You are now a %s of the %s.\n\r",
	    clan_table[clan].rank[0].rankname, clan_table[clan].name);
    send_to_char(buf, victim);

    victim->clan = clan;
    victim->rank = 0;		/* lowest, default */
} /* end: do_guild */

