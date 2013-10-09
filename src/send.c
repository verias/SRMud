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
#include "olc.h"

void send_to_one (CHAR_DATA *ch, char *fmt, ...)
{
	char buf [MAX_STRING_LENGTH];
	va_list args;
	va_start (args, fmt);
	vsprintf (buf, fmt, args);
	va_end (args);
	
	send_to_char (buf, ch);
	send_to_char ("\n\r",ch);
}

void send_to_one2 (CHAR_DATA *ch, char *fmt, ...)
{
	char buf [MAX_STRING_LENGTH];
	va_list args;
	va_start (args, fmt);
	vsprintf (buf, fmt, args);
	va_end (args);
	
	send_to_char (buf, ch);
	send_to_char ("\n\r\n\r", ch);
}

void send_to_except (CHAR_DATA *ch, char *fmt, ...)
{
	DESCRIPTOR_DATA *d;//, *d_next;
	char buf[MSL];
	va_list args;
	va_start (args, fmt);
	vsprintf (buf, fmt, args);
	va_end(args);

	for( d = descriptor_list; d; d = d->next)
	{
		if( d->connected == CON_PLAYING &&
			d->character != ch )
		{
			send_to_char(buf,d->character);
		send_to_char("\n\r",d->character);
		}
	}
}

void send_to_all ( char *fmt, ...)
{
	DESCRIPTOR_DATA *d;//, *d_next;
	char buf[MSL];
	va_list args;
	va_start (args, fmt);
	vsprintf (buf, fmt, args);
	va_end(args);

	for( d = descriptor_list; d; d = d->next)
	{
        if ( d->connected == CON_PLAYING)
		{
		send_to_char(buf,d->character);
		send_to_char("\n\r",d->character);
		}
	}
}

/* How to make a string look drunk... by Apex (robink@htsa.hva.nl) */
/* Modified and enhanced for envy(2) by the Maniac from Mythran */
/* Further mods/upgrades for ROM 2.4 by Kohl Desenee */

char * makedrunk (char *string, CHAR_DATA * ch)
{
/* This structure defines all changes for a character */
  struct struckdrunk drunk[] =
  {
    {3, 10,
     {"a", "a", "a", "A", "aa", "ah", "Ah", "ao", "aw", "oa", "ahhhh"}},
    {8, 5,
     {"b", "b", "b", "B", "B", "vb"}},
    {3, 5,
     {"c", "c", "C", "cj", "sj", "zj"}},
    {5, 2,
     {"d", "d", "D"}},
    {3, 3,
     {"e", "e", "eh", "E"}},
    {4, 5,
     {"f", "f", "ff", "fff", "fFf", "F"}},
    {8, 2,
     {"g", "g", "G"}},
    {9, 6,
     {"h", "h", "hh", "hhh", "Hhh", "HhH", "H"}},
    {7, 6,
     {"i", "i", "Iii", "ii", "iI", "Ii", "I"}},
    {9, 5,
     {"j", "j", "jj", "Jj", "jJ", "J"}},
    {7, 2,
     {"k", "k", "K"}},
    {3, 2,
     {"l", "l", "L"}},
    {5, 8,
     {"m", "m", "mm", "mmm", "mmmm", "mmmmm", "MmM", "mM", "M"}},
    {6, 6,
     {"n", "n", "nn", "Nn", "nnn", "nNn", "N"}},
    {3, 6,
     {"o", "o", "ooo", "ao", "aOoo", "Ooo", "ooOo"}},
    {3, 2,
     {"p", "p", "P"}},
    {5, 5,
     {"q", "q", "Q", "ku", "ququ", "kukeleku"}},
    {4, 2,
     {"r", "r", "R"}},
    {2, 5,
     {"s", "ss", "zzZzssZ", "ZSssS", "sSzzsss", "sSss"}},
    {5, 2,
     {"t", "t", "T"}},
    {3, 6,
     {"u", "u", "uh", "Uh", "Uhuhhuh", "uhU", "uhhu"}},
    {4, 2,
     {"v", "v", "V"}},
    {4, 2,
     {"w", "w", "W"}},
    {5, 6,
     {"x", "x", "X", "ks", "iks", "kz", "xz"}},
    {3, 2,
     {"y", "y", "Y"}},
    {2, 9,
     {"z", "z", "ZzzZz", "Zzz", "Zsszzsz", "szz", "sZZz", "ZSz", "zZ", "Z"}}
  };

  char buf[1024];
  char temp;
  int pos = 0;
  int drunklevel;
  int randomnum;

  /* Check how drunk a person is... */
  if (IS_NPC(ch))
        drunklevel = 0;
  else
        drunklevel = ch->pcdata->condition[COND_DRUNK];

  if (drunklevel > 0)
    {
      do
        {
          temp = toupper (*string);
          if ((temp >= 'A') && (temp <= 'Z'))
            {
              if (drunklevel > drunk[temp - 'A'].min_drunk_level)
                {
                  randomnum = number_range (0, drunk[temp - 'A'].number_of_rep);
                  strcpy (&buf[pos], drunk[temp - 'A'].replacement[randomnum]);
                  pos += strlen (drunk[temp - 'A'].replacement[randomnum]);
                }
              else
                buf[pos++] = *string;
            }
          else
            {
              if ((temp >= '0') && (temp <= '9'))
                {
                  temp = '0' + number_range (0, 9);
                  buf[pos++] = temp;
                }
              else
                buf[pos++] = *string;
            }
        }
      while (*string++);
      buf[pos] = '\0';          /* Mark end of the string... */
      strcpy(string, buf);
      return(string);
    }
  return (string);
}

/* RT code to display channel status */

void do_channels( CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];

    /* lists all channels and their status */
    send_to_char("   channel     status\n\r",ch);
    send_to_char("---------------------\n\r",ch);
 
    send_to_char("{dgossip{x         ",ch);
    if (!IS_SET(ch->comm,COMM_NOGOSSIP))
      send_to_char("ON\n\r",ch);
    else
      send_to_char("OFF\n\r",ch);

    send_to_char("{aauction{x        ",ch);
    if (!IS_SET(ch->comm,COMM_NOAUCTION))
      send_to_char("ON\n\r",ch);
    else
      send_to_char("OFF\n\r",ch);

    send_to_char("{emusic{x          ",ch);
    if (!IS_SET(ch->comm,COMM_NOMUSIC))
      send_to_char("ON\n\r",ch);
    else
      send_to_char("OFF\n\r",ch);

    send_to_char("{qQ{x/{fA{x            ",ch);
    if (!IS_SET(ch->comm,COMM_NOQUESTION))
      send_to_char("ON\n\r",ch);
    else
      send_to_char("OFF\n\r",ch);

    send_to_char("{hQuote{x          ",ch);
    if (!IS_SET(ch->comm,COMM_NOQUOTE))
	send_to_char("ON\n\r",ch);
    else
	send_to_char("OFF\n\r",ch);

    send_to_char("city           ",ch);
	if(!IS_SET(ch->comm, COMM_NOCITY))
		send_to_one(ch,"ON");
	else
		send_to_one(ch,"OFF");

	send_to_char("{tgrats{x          ",ch);
    if (!IS_SET(ch->comm,COMM_NOGRATS))
      send_to_char("ON\n\r",ch);
    else
      send_to_char("OFF\n\r",ch);

    if (is_granted_name(ch,"immtalk") || is_granted_name(ch,":"))
    {
      send_to_char("{igod channel{x    ",ch);
      if(!IS_SET(ch->comm,COMM_NOWIZ))
        send_to_char("ON\n\r",ch);
      else
        send_to_char("OFF\n\r",ch);
    }

	if (ch->level >= LEVEL_HERO)
	{
		send_to_char("Hero channel   ",ch);
		if(!IS_SET(ch->comm,COMM_NOHERO))
			send_to_one(ch,"ON");
		else
			send_to_one(ch,"OFF");
	}

    send_to_char("{tshouts{x         ",ch);
    if (!IS_SET(ch->comm,COMM_SHOUTSOFF))
      send_to_char("ON\n\r",ch);
    else
      send_to_char("OFF\n\r",ch);

    send_to_char("{ktells{x          ",ch);
    if (!IS_SET(ch->comm,COMM_DEAF))
	send_to_char("ON\n\r",ch);
    else
	send_to_char("OFF\n\r",ch);

    send_to_char("{tquiet mode{x     ",ch);
    if (IS_SET(ch->comm,COMM_QUIET))
      send_to_char("ON\n\r",ch);
    else
      send_to_char("OFF\n\r",ch);

    if (IS_SET(ch->comm,COMM_AFK))
	send_to_char("You are AFK.\n\r",ch);

    if (IS_SET(ch->comm,COMM_SNOOP_PROOF))
	send_to_char("You are immune to snooping.\n\r",ch);
   
    if (ch->lines != PAGELEN)
    {
	if (ch->lines)
	{
	    sprintf(buf,"You display %d lines of scroll.\n\r",ch->lines+2);
	    send_to_char(buf,ch);
 	}
	else
	    send_to_char("Scroll buffering is off.\n\r",ch);
    }

    if (ch->prompt != NULL)
    {
	sprintf(buf,"Your current prompt is: %s\n\r",ch->prompt);
	send_to_char(buf,ch);
    }

    if (IS_SET(ch->comm,COMM_NOSHOUT))
      send_to_char("You cannot shout.\n\r",ch);
  
    if (IS_SET(ch->comm,COMM_NOTELL))
      send_to_char("You cannot use tell.\n\r",ch);
 
    if (IS_SET(ch->comm,COMM_NOCHANNELS))
     send_to_char("You cannot use channels.\n\r",ch);

    if (IS_SET(ch->comm,COMM_NOEMOTE))
      send_to_char("You cannot show emotions.\n\r",ch);

}

//Keith's revised deaf mode

void do_deaf( CHAR_DATA *ch, char *argument)
{
	TOGGLE_BIT(ch->comm,COMM_DEAF);  
	send_to_one(ch, "Deaf mode toggled.");
}

//Keith's revised queit mode

void do_quiet ( CHAR_DATA *ch, char * argument)
{
	TOGGLE_BIT(ch->comm,COMM_QUIET);  
	send_to_one(ch, "Quiet mode toggled.");
}

/* afk command */

void do_afk ( CHAR_DATA *ch, char * argument)
{
	TOGGLE_BIT(ch->comm,COMM_AFK);  
	send_to_one(ch, "AFK mode toggled.");
}

void do_replay (CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
    {
	send_to_char("You can't replay.\n\r",ch);
	return;
    }

    if (buf_string(ch->pcdata->buffer)[0] == '\0')
    {
	send_to_char("You have no tells to replay.\n\r",ch);
	return;
    }

    page_to_char(buf_string(ch->pcdata->buffer),ch);
    clear_buf(ch->pcdata->buffer);
}

//Keith's compressed channels
void do_auction( CHAR_DATA *ch, char *argument )
{
//    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0' )
    {
	TOGGLE_BIT(ch->comm,COMM_NOAUCTION);  
	send_to_one(ch, "{aAuction channel toggled.{x");
	return;
    }
    else  /* auction message sent, turn auction on if it is off */
    {
	if (IS_SET(ch->comm,COMM_QUIET)|| IS_SET(ch->comm,COMM_NOCHANNELS)
		|| IS_SET(ch->comm, COMM_NOAUCTION))
	{
	  send_to_char("You can't auction like that!\n\r",ch);
	  return;
	}

    }

    send_to_one( ch, "{aYou auction '{A%s{a'{x\n\r", argument );

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	CHAR_DATA *victim;

	victim = d->original ? d->original : d->character;

	if ( d->connected == CON_PLAYING &&
	     d->character != ch &&
	     !IS_SET(victim->comm,COMM_NOAUCTION) &&
	     !IS_SET(victim->comm,COMM_QUIET) )
	{
	    send_to_one(d->character,"{a%s auctions '{A%s{a'{x",  ch->name,argument);
 	}
    }
}

void do_hero( CHAR_DATA *ch, char *argument )
{
//    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

	if (ch->level < LEVEL_HERO)
	{
		send_to_one(ch, "You must be a hero to use this command.");
		return;
	}

    if (argument[0] == '\0' )
    {
	TOGGLE_BIT(ch->comm,COMM_NOHERO);  
	send_to_one(ch, "Hero channel toggled.");
	return;
    }
    else  
    {
	if (IS_SET(ch->comm,COMM_QUIET)|| IS_SET(ch->comm,COMM_NOCHANNELS)
		|| IS_SET(ch->comm, COMM_NOHERO))
	{
	  send_to_one(ch, "You can't herotalk like that!");
	  return;
	}

    }

    send_to_one( ch, "{B[{Y%s{B] %s{x", ch->name, argument );

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	CHAR_DATA *victim;

	victim = d->original ? d->original : d->character;

	if ( d->connected == CON_PLAYING &&
	     d->character != ch 
		 && d->character->level >= LEVEL_HERO
		 && !IS_SET(victim->comm,COMM_NOHERO) 
		 && !IS_SET(victim->comm,COMM_QUIET) )
	{
	    send_to_one(d->character,"{B[{Y%s{B] %s{x",  ch->name,argument);
 	}
    }
}

/* RT chat replaced with ROM gossip */
void do_gossip( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
 
    if (argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOGOSSIP))
      {
        send_to_char("Gossip channel is now ON.\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_NOGOSSIP);
      }
      else
      {
        send_to_char("Gossip channel is now OFF.\n\r",ch);
        SET_BIT(ch->comm,COMM_NOGOSSIP);
      }
    }
    else  /* gossip message sent, turn gossip on if it isn't already */
    {
        if (IS_SET(ch->comm,COMM_QUIET))
        {
          send_to_char("You must turn off quiet mode first.\n\r",ch);
          return;
        }
 
        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
          send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
          return;
 
       	}

      REMOVE_BIT(ch->comm,COMM_NOGOSSIP);

	  if(!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
		  argument = makedrunk(argument,ch);
      sprintf( buf, "{dYou gossip '{9%s{d'{x\n\r", argument );
      send_to_char( buf, ch );
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
        CHAR_DATA *victim;
 
        victim = d->original ? d->original : d->character;
 
        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
             !IS_SET(victim->comm,COMM_NOGOSSIP) &&
             !IS_SET(victim->comm,COMM_QUIET) )
        {
          act_new( "{d$n gossips '{9$t{d'{x", 
		   ch,argument, d->character, TO_VICT,POS_SLEEPING );
        }
      }
    }
}

void do_grats( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
 
    if (argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOGRATS))
      {
        send_to_char("Grats channel is now ON.\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_NOGRATS);
      }
      else
      {
        send_to_char("Grats channel is now OFF.\n\r",ch);
        SET_BIT(ch->comm,COMM_NOGRATS);
      }
    }
    else  /* grats message sent, turn grats on if it isn't already */
    {
        if (IS_SET(ch->comm,COMM_QUIET))
        {
          send_to_char("You must turn off quiet mode first.\n\r",ch);
          return;
        }
 
        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
          send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
          return;
 
        }
 
      REMOVE_BIT(ch->comm,COMM_NOGRATS);
 	  if(!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
		  argument = makedrunk(argument,ch);

      sprintf( buf, "{tYou grats '%s'{x\n\r", argument );
      send_to_char( buf, ch );
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
        CHAR_DATA *victim;
 
        victim = d->original ? d->original : d->character;
 
        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
             !IS_SET(victim->comm,COMM_NOGRATS) &&
             !IS_SET(victim->comm,COMM_QUIET) )
        {
          act_new( "{t$n grats '$t'{x",
                   ch,argument, d->character, TO_VICT,POS_SLEEPING );
        }
      }
    }
}

void do_quote( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
 
    if (argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOQUOTE))
      {
        send_to_char("{hQuote channel is now ON.{x\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_NOQUOTE);
      }
      else
      {
        send_to_char("{hQuote channel is now OFF.{x\n\r",ch);
        SET_BIT(ch->comm,COMM_NOQUOTE);
      }
    }
    else  /* quote message sent, turn quote on if it isn't already */
    {
        if (IS_SET(ch->comm,COMM_QUIET))
        {
          send_to_char("You must turn off quiet mode first.\n\r",ch);
          return;
        }
 
        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
          send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
          return;
 
        }
 
      REMOVE_BIT(ch->comm,COMM_NOQUOTE);
 
	  if(!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
		  argument = makedrunk(argument,ch);

      sprintf( buf, "{hYou quote '{H%s{h'{x\n\r", argument );
      send_to_char( buf, ch );
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
        CHAR_DATA *victim;
 
        victim = d->original ? d->original : d->character;
 
        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
             !IS_SET(victim->comm,COMM_NOQUOTE) &&
             !IS_SET(victim->comm,COMM_QUIET) )
        {
          act_new( "{h$n quotes '{H$t{h'{x",
                   ch,argument, d->character, TO_VICT,POS_SLEEPING );
        }
      }
    }
}

/* RT question channel */
void do_question( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
 
    if (argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOQUESTION))
      {
        send_to_char("Q/A channel is now ON.\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_NOQUESTION);
      }
      else
      {
        send_to_char("Q/A channel is now OFF.\n\r",ch);
        SET_BIT(ch->comm,COMM_NOQUESTION);
      }
    }
    else  /* question sent, turn Q/A on if it isn't already */
    {
        if (IS_SET(ch->comm,COMM_QUIET))
        {
          send_to_char("You must turn off quiet mode first.\n\r",ch);
          return;
        }
 
        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
          send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
          return;
	}
 
        REMOVE_BIT(ch->comm,COMM_NOQUESTION);
 	  if(!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
		  argument = makedrunk(argument,ch);

      sprintf( buf, "{qYou question '{Q%s{q'{x\n\r", argument );
      send_to_char( buf, ch );
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
        CHAR_DATA *victim;
 
        victim = d->original ? d->original : d->character;
 
        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
             !IS_SET(victim->comm,COMM_NOQUESTION) &&
             !IS_SET(victim->comm,COMM_QUIET) )
        {
	  act_new("{q$n questions '{Q$t{q'{x\n\r",
	 	  ch,argument,d->character,TO_VICT,POS_SLEEPING);
        }
      }
    }
}

/* RT answer channel - uses same line as questions */
void do_answer( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
 
    if (argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOQUESTION))
      {
        send_to_char("Q/A channel is now ON.\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_NOQUESTION);
      }
      else
      {
        send_to_char("Q/A channel is now OFF.\n\r",ch);
        SET_BIT(ch->comm,COMM_NOQUESTION);
      }
    }
    else  /* answer sent, turn Q/A on if it isn't already */
    {
        if (IS_SET(ch->comm,COMM_QUIET))
        {
          send_to_char("You must turn off quiet mode first.\n\r",ch);
          return;
        }
 
        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
          send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
          return;
	}
 
        REMOVE_BIT(ch->comm,COMM_NOQUESTION);
	  if(!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
		  argument = makedrunk(argument,ch);
 
      sprintf( buf, "{fYou answer '{F%s{f'{x\n\r", argument );
      send_to_char( buf, ch );
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
        CHAR_DATA *victim;
 
        victim = d->original ? d->original : d->character;
 
        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
             !IS_SET(victim->comm,COMM_NOQUESTION) &&
             !IS_SET(victim->comm,COMM_QUIET) )
        {
	  act_new("{f$n answers '{F$t{f'{x\n\r",
		  ch,argument,d->character,TO_VICT,POS_SLEEPING);
        }
      }
    }
}

/* RT music channel */
void do_music( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
 
    if (argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOMUSIC))
      {
        send_to_char("Music channel is now ON.\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_NOMUSIC);
      }
      else
      {
        send_to_char("Music channel is now OFF.\n\r",ch);
        SET_BIT(ch->comm,COMM_NOMUSIC);
      }
    }
    else  /* music sent, turn music on if it isn't already */
    {
        if (IS_SET(ch->comm,COMM_QUIET))
        {
          send_to_char("You must turn off quiet mode first.\n\r",ch);
          return;
        }
 
        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
          send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
          return;
	}
 
        REMOVE_BIT(ch->comm,COMM_NOMUSIC);
 	  if(!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
		  argument = makedrunk(argument,ch);

      sprintf( buf, "{eYou MUSIC: '{E%s{e'{x\n\r", argument );
      send_to_char( buf, ch );
      sprintf( buf, "$n MUSIC: '%s'\n\r", argument );
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
        CHAR_DATA *victim;
 
        victim = d->original ? d->original : d->character;
 
        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
             !IS_SET(victim->comm,COMM_NOMUSIC) &&
             !IS_SET(victim->comm,COMM_QUIET) )
        {
	    act_new("{e$n MUSIC: '{E$t{e'{x\n\r",
		    ch,argument,d->character,TO_VICT,POS_SLEEPING);
        }
      }
    }
}


void do_immtalk( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if ( argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOWIZ))
      {
	send_to_char("Immortal channel is now ON\n\r",ch);
	REMOVE_BIT(ch->comm,COMM_NOWIZ);
      }
      else
      {
	send_to_char("Immortal channel is now OFF\n\r",ch);
	SET_BIT(ch->comm,COMM_NOWIZ);
      } 
      return;
    }

    REMOVE_BIT(ch->comm,COMM_NOWIZ);

    sprintf( buf, "{i[{I$n{i]: %s{x\n\r", argument );
    act_new("{i[{I$n{i]: $t{x\n\r",ch,argument,NULL,TO_CHAR,POS_DEAD);
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d->connected == CON_PLAYING && 
	        (is_granted_name(d->character,"immtalk") ||
            is_granted_name(d->character,":")) &&
             !IS_SET(d->character->comm,COMM_NOWIZ) )
	{
	    act_new("{i[{I$n{i]: $t{x\n\r",ch,argument,d->character,TO_VICT,POS_DEAD);
	}
    }

    return;
}



void do_say( CHAR_DATA *ch, char *argument )
{
//	char arg[MIL];
//	char arg2[MIL];
//	CHAR_DATA *victim;
	CHAR_DATA *to, *to_next;
//	DESCRIPTOR_DATA *d, *d_next;
//	int sn;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Say what?\n\r", ch );
	return;
    }
	  if(!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
		  argument = makedrunk(argument,ch);

	  crushcaps(argument);

	send_to_one(ch, "{6You say '{7%s{6'{x",argument );
	if(!IS_NPC(ch))
	{
		calc_rpxp(ch,argument);
	}

	for(to = ch->in_room->people; to; to = to_next)
	{
		to_next = to->next_in_room;
			if(to->in_room == ch->in_room && to != ch)
			{
				if(!IS_SET(ch->act,PLR_DEAD))
				send_to_one(to, "{6%s says '{7%s{6'{x", PERS(ch,to),argument );
				else
				send_to_one(to,"{6The spirit of %s says '{7%s{6'{x",PERS(ch,to),argument);
			}
	}
	
	if (!IS_NPC(ch) )
	{
		CHAR_DATA *mob, *mob_next;
		OBJ_DATA *obj, *obj_next;
	for(mob = ch->in_room->people; mob != NULL; mob = mob_next)
	{
		mob_next = mob->next_in_room;
		if (IS_NPC(mob) && HAS_TRIGGER_MOB( mob, TRIG_SPEECH )
			&&	mob->position == mob->pIndexData->default_pos)
			p_act_trigger( argument, mob, NULL, NULL, ch, NULL, NULL, TRIG_SPEECH);
	    for ( obj = mob->carrying; obj; obj = obj_next )
	    {
		obj_next = obj->next_content;
		if ( HAS_TRIGGER_OBJ( obj, TRIG_SPEECH ) )
		    p_act_trigger( argument, NULL, obj, NULL, ch, NULL, NULL, TRIG_SPEECH );
	    }
	}
	for ( obj = ch->in_room->contents; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( HAS_TRIGGER_OBJ( obj, TRIG_SPEECH ) )
		p_act_trigger( argument, NULL, obj, NULL, ch, NULL, NULL, TRIG_SPEECH );
	}
	
	if ( HAS_TRIGGER_ROOM( ch->in_room, TRIG_SPEECH ) )
	    p_act_trigger( argument, NULL, NULL, ch->in_room, ch, NULL, NULL, TRIG_SPEECH );
	}
    return;
}


void do_sayto( CHAR_DATA *ch, char *argument )
{
	char arg[MIL];
//	char arg2[MIL];
	CHAR_DATA *victim;
	CHAR_DATA *to, *to_next;
//	DESCRIPTOR_DATA *d, *d_next;
//	int sn;

	argument = one_argument(argument,arg);

	if ( argument[0] == '\0' )
    {
	send_to_char( "Say what, to whom?\n\r", ch );
	return;
    }

	if((victim = get_char_world(ch,arg)) == NULL ||
		victim->in_room != ch->in_room )
	{
		send_to_one(ch, "They are not here.");
		return;
	}
	else
	{
	  if(!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
		  argument = makedrunk(argument,ch);

	  crushcaps(argument);

	send_to_one(ch, "{6You say to %s, '{7%s{6'{x",PERS(victim,ch),argument );
	send_to_one(victim, "{6%s says to you, '{7%s{6'{x",PERS(ch,victim),argument);
	if(!IS_NPC(ch))
	{
		calc_rpxp(ch,argument);
	}

	for(to = ch->in_room->people; to; to = to_next)
	{
		to_next = to->next_in_room;
			if((to->in_room == ch->in_room) && (to != ch) && (to != victim))
			{
				if(!IS_SET(ch->act,PLR_DEAD))
				send_to_one(to, "{6%s says to %s, '{7%s{6'{x", PERS(ch,to),PERS(victim,to),argument );
				else
				send_to_one(to,"{6The spirit of %s says to %s, '{7%s{6'{x",PERS(ch,to),PERS(victim,to),argument);
			}
	}
	}
	if (!IS_NPC(ch) )
	{
		CHAR_DATA *mob, *mob_next;
		OBJ_DATA *obj, *obj_next;
	for(mob = ch->in_room->people; mob != NULL; mob = mob_next)
	{
		mob_next = mob->next_in_room;
		if (IS_NPC(mob) && HAS_TRIGGER_MOB( mob, TRIG_SPEECH )
			&&	mob->position == mob->pIndexData->default_pos)
			p_act_trigger( argument, mob, NULL, NULL, ch, NULL, NULL, TRIG_SPEECH);
	    for ( obj = mob->carrying; obj; obj = obj_next )
	    {
		obj_next = obj->next_content;
		if ( HAS_TRIGGER_OBJ( obj, TRIG_SPEECH ) )
		    p_act_trigger( argument, NULL, obj, NULL, ch, NULL, NULL, TRIG_SPEECH );
	    }
	}
	for ( obj = ch->in_room->contents; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( HAS_TRIGGER_OBJ( obj, TRIG_SPEECH ) )
		p_act_trigger( argument, NULL, obj, NULL, ch, NULL, NULL, TRIG_SPEECH );
	}
	
	if ( HAS_TRIGGER_ROOM( ch->in_room, TRIG_SPEECH ) )
	    p_act_trigger( argument, NULL, NULL, ch->in_room, ch, NULL, NULL, TRIG_SPEECH );
	}
    return;
}


void do_shout( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0' )
    {
      	if (IS_SET(ch->comm,COMM_SHOUTSOFF))
      	{
            send_to_char("You can hear shouts again.\n\r",ch);
            REMOVE_BIT(ch->comm,COMM_SHOUTSOFF);
      	}
      	else
      	{
            send_to_char("You will no longer hear shouts.\n\r",ch);
            SET_BIT(ch->comm,COMM_SHOUTSOFF);
      	}
      	return;
    }

    if ( IS_SET(ch->comm, COMM_NOSHOUT) )
    {
        send_to_char( "You can't shout.\n\r", ch );
        return;
    }
 
    REMOVE_BIT(ch->comm,COMM_SHOUTSOFF);

    WAIT_STATE( ch, 12 );

	if(!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
		  argument = makedrunk(argument,ch);


    act( "{tYou shout '$T'{x", ch, NULL, argument, TO_CHAR );
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	CHAR_DATA *victim;

	victim = d->original ? d->original : d->character;

	if ( d->connected == CON_PLAYING &&
	     d->character != ch &&
	     !IS_SET(victim->comm, COMM_SHOUTSOFF) &&
	     !IS_SET(victim->comm, COMM_QUIET) ) 
	{
	    act("{t$n shouts '$t'{x",ch,argument,d->character,TO_VICT);
	}
    }

    return;
}



void do_tell( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH],buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    if ( IS_SET(ch->comm, COMM_NOTELL) || IS_SET(ch->comm,COMM_DEAF))
    {
	send_to_char( "Your message didn't get through.\n\r", ch );
	return;
    }

    if ( IS_SET(ch->comm, COMM_QUIET) )
    {
	send_to_char( "You must turn off quiet mode first.\n\r", ch);
	return;
    }

    if (IS_SET(ch->comm,COMM_DEAF))
    {
	send_to_char("You must turn off deaf mode first.\n\r",ch);
	return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Tell whom what?\n\r", ch );
	return;
    }

    /*
     * Can tell to PC's anywhere, but NPC's only in same room.
     * -- Furey
     */
    if ( ( victim = get_char_world( ch, arg ) ) == NULL
    || ( IS_NPC(victim) && victim->in_room != ch->in_room ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim->desc == NULL && !IS_NPC(victim))
    {
	act("$N seems to have misplaced $S link...try again later.",
	    ch,NULL,victim,TO_CHAR);
        sprintf(buf,"{k%s tells you '{K%s{k'{x\n\r",PERS(ch,victim),argument);
        buf[0] = UPPER(buf[0]);
        add_buf(victim->pcdata->buffer,buf);
	return;
    }

    if ( !(IS_IMMORTAL(ch) && ch->level > LEVEL_IMMORTAL) && !IS_AWAKE(victim) )
    {
	act( "$E can't hear you.", ch, 0, victim, TO_CHAR );
	return;
    }
  
    if ((IS_SET(victim->comm,COMM_QUIET) || IS_SET(victim->comm,COMM_DEAF))
    && !IS_IMMORTAL(ch))
    {
	act( "$E is not receiving tells.", ch, 0, victim, TO_CHAR );
  	return;
    }

    if (IS_SET(victim->comm,COMM_AFK))
    {
	if (IS_NPC(victim))
	{
	    act("$E is AFK, and not receiving tells.",ch,NULL,victim,TO_CHAR);
	    return;
	}

	act("$E is AFK, but your tell will go through when $E returns.",
	    ch,NULL,victim,TO_CHAR);
	sprintf(buf,"{k%s tells you '{K%s{k'{x\n\r",PERS(ch,victim),argument);
	buf[0] = UPPER(buf[0]);
	add_buf(victim->pcdata->buffer,buf);
	return;
    }

    act( "{kYou tell $N '{K$t{k'{x", ch, argument, victim, TO_CHAR );
    act_new("{k$n tells you '{K$t{k'{x",ch,argument,victim,TO_VICT,POS_DEAD);
    victim->reply	= ch;

	if (!IS_NPC(ch) && IS_NPC(victim)&& HAS_TRIGGER_MOB(victim,TRIG_SPEECH) )
		p_act_trigger( argument, victim, NULL, NULL, ch, NULL, NULL, TRIG_SPEECH);

    return;
}



void do_reply( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];

    if ( IS_SET(ch->comm, COMM_NOTELL) )
    {
	send_to_char( "Your message didn't get through.\n\r", ch );
	return;
    }

    if ( ( victim = ch->reply ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim->desc == NULL && !IS_NPC(victim))
    {
        act("$N seems to have misplaced $S link...try again later.",
            ch,NULL,victim,TO_CHAR);
        sprintf(buf,"{k%s tells you '{K%s{k'{x\n\r",PERS(ch,victim),argument);
        buf[0] = UPPER(buf[0]);
        add_buf(victim->pcdata->buffer,buf);
        return;
    }

    if ( !IS_IMMORTAL(ch) && !IS_AWAKE(victim) )
    {
	act( "$E can't hear you.", ch, 0, victim, TO_CHAR );
	return;
    }

    if ((IS_SET(victim->comm,COMM_QUIET) || IS_SET(victim->comm,COMM_DEAF))
    &&  !IS_IMMORTAL(ch) && !IS_IMMORTAL(victim))
    {
        act_new( "$E is not receiving tells.", ch, 0, victim, TO_CHAR,POS_DEAD);
        return;
    }

    if (!IS_IMMORTAL(victim) && !IS_AWAKE(ch))
    {
	send_to_char( "In your dreams, or what?\n\r", ch );
	return;
    }

    if (IS_SET(victim->comm,COMM_AFK))
    {
        if (IS_NPC(victim))
        {
            act_new("$E is AFK, and not receiving tells.",
		ch,NULL,victim,TO_CHAR,POS_DEAD);
            return;
        }
 
        act_new("$E is AFK, but your tell will go through when $E returns.",
            ch,NULL,victim,TO_CHAR,POS_DEAD);
        sprintf(buf,"{k%s tells you '{K%s{k'{x\n\r",PERS(ch,victim),argument);
	buf[0] = UPPER(buf[0]);
        add_buf(victim->pcdata->buffer,buf);
        return;
    }

    act_new("{kYou tell $N '{K$t{k'{x",ch,argument,victim,TO_CHAR,POS_DEAD);
    act_new("{k$n tells you '{K$t{k'{x",ch,argument,victim,TO_VICT,POS_DEAD);
    victim->reply	= ch;

    return;
}



void do_yell( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;

    if ( IS_SET(ch->comm, COMM_NOSHOUT) )
    {
        send_to_char( "You can't yell.\n\r", ch );
        return;
    }
 
    if ( argument[0] == '\0' )
    {
	send_to_char( "Yell what?\n\r", ch );
	return;
    }

	  if(!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
		  argument = makedrunk(argument,ch);

    act("You yell '$t'",ch,argument,NULL,TO_CHAR);
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d->connected == CON_PLAYING
	&&   d->character != ch
	&&   d->character->in_room != NULL
	&&   d->character->in_room->area == ch->in_room->area 
        &&   !IS_SET(d->character->comm,COMM_QUIET) )
	{
	    act("$n yells '$t'",ch,argument,d->character,TO_VICT);
	}
    }

    return;
}


void do_emote( CHAR_DATA *ch, char *argument )
{
    if ( !IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE) )
    {
        send_to_char( "You can't show your emotions.\n\r", ch );
        return;
    }
 
    if ( argument[0] == '\0' )
    {
        send_to_char( "Emote what?\n\r", ch );
        return;
    }
 
	MOBtrigger = FALSE;
    act( "$n $T", ch, NULL, argument, TO_ROOM );
    act( "$n $T", ch, NULL, argument, TO_CHAR );
	MOBtrigger = TRUE;
	if(!IS_NPC(ch))
	{
		calc_rpxp(ch,argument);
	}
    return;
}


void do_pmote( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *vch;
    char *letter,*name;
    char last[MAX_INPUT_LENGTH], temp[MAX_STRING_LENGTH];
    int matches = 0;

    if ( !IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE) )
    {
        send_to_char( "You can't show your emotions.\n\r", ch );
        return;
    }
 
    if ( argument[0] == '\0' )
    {
        send_to_char( "Emote what?\n\r", ch );
        return;
    }
 
    act( "$n $t", ch, argument, NULL, TO_CHAR );

    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
	if (vch->desc == NULL || vch == ch)
	    continue;

	if ((letter = strstr(argument,vch->name)) == NULL)
	{
		MOBtrigger = FALSE;
	    act("$N $t",vch,argument,ch,TO_CHAR);
		MOBtrigger = TRUE;
	    continue;
	}

	strcpy(temp,argument);
	temp[strlen(argument) - strlen(letter)] = '\0';
   	last[0] = '\0';
 	name = vch->name;
	
	for (; *letter != '\0'; letter++)
	{ 
	    if (*letter == '\'' && matches == strlen(vch->name))
	    {
		strcat(temp,"r");
		continue;
	    }

	    if (*letter == 's' && matches == strlen(vch->name))
	    {
		matches = 0;
		continue;
	    }
	    
 	    if (matches == strlen(vch->name))
	    {
		matches = 0;
	    }

	    if (*letter == *name)
	    {
		matches++;
		name++;
		if (matches == strlen(vch->name))
		{
		    strcat(temp,"you");
		    last[0] = '\0';
		    name = vch->name;
		    continue;
		}
		strncat(last,letter,1);
		continue;
	    }

	    matches = 0;
	    strcat(temp,last);
	    strncat(temp,letter,1);
	    last[0] = '\0';
	    name = vch->name;
	}

	MOBtrigger = FALSE;
	act("$N $t",vch,temp,ch,TO_CHAR);
	MOBtrigger = TRUE;
    }
	
    return;
}

/*
 * Write to one char.
 */
void send_to_char_bw( const char *txt, CHAR_DATA *ch )
{
    if ( txt != NULL && ch->desc != NULL )
        write_to_buffer( ch->desc, txt, strlen(txt) );
    return;
}

/*
 * Write to one char, new colour version, by Lope.
 */
void send_to_char( const char *txt, CHAR_DATA *ch )
{
    const	char 	*point;
    		char 	*point2;
    		char 	buf[ MAX_STRING_LENGTH*4 ];
		int	skip = 0;

    buf[0] = '\0';
    point2 = buf;
    if( txt && ch->desc )
	{
	    if( IS_SET( ch->act, PLR_COLOUR ) )
	    {
		for( point = txt ; *point ; point++ )
	        {
		    if( *point == '{' )
		    {
			point++;
			skip = colour( *point, ch, point2 );
			while( skip-- > 0 )
			    ++point2;
			continue;
		    }
		    *point2 = *point;
		    *++point2 = '\0';
		}			
		*point2 = '\0';
        	write_to_buffer( ch->desc, buf, point2 - buf );
	    }
	    else
	    {
		for( point = txt ; *point ; point++ )
	        {
		    if( *point == '{' )
		    {
			point++;
			continue;
		    }
		    *point2 = *point;
		    *++point2 = '\0';
		}
		*point2 = '\0';
        	write_to_buffer( ch->desc, buf, point2 - buf );
	    }
	}
    return;
}


void send_to_desc( const char *txt, DESCRIPTOR_DATA *d )
{
    const	char 	*point;
    		char 	*point2;
    		char 	buf[ MAX_STRING_LENGTH*4 ];
		int	skip = 0;

    buf[0] = '\0';
    point2 = buf;
    if( txt && d )
	{
	    if( d->ansi == TRUE )
	    {
		for( point = txt ; *point ; point++ )
	        {
		    if( *point == '{' )
		    {
			point++;
			skip = colour( *point, NULL, point2 );
			while( skip-- > 0 )
			    ++point2;
			continue;
		    }
		    *point2 = *point;
		    *++point2 = '\0';
		}			
		*point2 = '\0';
        	write_to_buffer( d, buf, point2 - buf );
	    }
	    else
	    {
		for( point = txt ; *point ; point++ )
	        {
		    if( *point == '{' )
		    {
			point++;
			continue;
		    }
		    *point2 = *point;
		    *++point2 = '\0';
		}
		*point2 = '\0';
        	write_to_buffer( d, buf, point2 - buf );
	    }
	}
    return;
}

/*
 * Send a page to one char.
 */
void page_to_char_bw( const char *txt, CHAR_DATA *ch )
{
    if ( txt == NULL || ch->desc == NULL)
	return;

    if (ch->lines == 0 )
    {
	send_to_char_bw(txt,ch);
	return;
    }
	
#if defined(macintosh)
	send_to_char_bw(txt,ch);
#else
    ch->desc->showstr_head = alloc_mem(strlen(txt) + 1);
    strcpy(ch->desc->showstr_head,txt);
    ch->desc->showstr_point = ch->desc->showstr_head;
    show_string(ch->desc,"");
#endif
}

int strlen_color( char *argument )
{
    char        *str;
    int         length;

    if ( argument==NULL || argument[0]=='\0' )
        return 0;

    length=0;
    str=argument;

    while ( *str != '\0' )
    {
        if ( *str != '{' )
        {
            str++;
            length++;
            continue;
        }

        if (*(++str) == '{')
	    length++;

        str++;
    }

    return length;
}


/*
 * Page to one char, new colour version, by Lope.
 */
void page_to_char( const char *txt, CHAR_DATA *ch )
{
    const	char	*point;
    		char	*point2;
    		char	buf[ MAX_STRING_LENGTH * 4 ];
		int	skip = 0;

#if defined(macintosh)
		send_to_char( txt, ch );
#else
    buf[0] = '\0';
    point2 = buf;
    if( txt && ch->desc )
	{
	    if( IS_SET( ch->act, PLR_COLOUR ) )
	    {
		for( point = txt ; *point ; point++ )
	        {
		    if( *point == '{' )
		    {
			point++;
			skip = colour( *point, ch, point2 );
			while( skip-- > 0 )
			    ++point2;
			continue;
		    }
		    *point2 = *point;
		    *++point2 = '\0';
		}			
		*point2 = '\0';
		ch->desc->showstr_head  = alloc_mem( strlen( buf ) + 1 );
		strcpy( ch->desc->showstr_head, buf );
		ch->desc->showstr_point = ch->desc->showstr_head;
		show_string( ch->desc, "" );
	    }
	    else
	    {
		for( point = txt ; *point ; point++ )
	        {
		    if( *point == '{' )
		    {
			point++;
			continue;
		    }
		    *point2 = *point;
		    *++point2 = '\0';
		}
		*point2 = '\0';
		ch->desc->showstr_head  = alloc_mem( strlen_color( buf ) + 1 );
		strcpy( ch->desc->showstr_head, buf );
		ch->desc->showstr_point = ch->desc->showstr_head;
		show_string( ch->desc, "" );
	    }
	}
#endif
    return;
}
