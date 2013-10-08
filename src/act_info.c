/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/***************************************************************************
*	ROM 2.4 is copyright 1993-1998 Russ Taylor			   *
*	ROM has been brought to you by the ROM consortium		   *
*	    Russ Taylor (rtaylor@hypercube.org)				   *
*	    Gabrielle Taylor (gtaylor@hypercube.org)			   *
*	    Brian Moore (zump@rom.org)					   *
*	By using this code, you have agreed to follow the terms of the	   *
*	ROM license, in the file Rom24/doc/rom.license			   *
***************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"

char *	const	where_name	[] =
{
    "<used as light>     ",
    "<worn on finger>    ",
    "<worn on finger>    ",
    "<worn around neck>  ",
    "<worn around neck>  ",
    "<worn on torso>     ",
    "<worn on head>      ",
	"<worn on shoulder>  ",
    "<worn on legs>      ",
    "<worn on feet>      ",
    "<worn on hands>     ",
    "<worn on arms>      ",
    "<worn as shield>    ",
    "<worn about body>   ",
    "<worn about waist>  ",
    "<worn around wrist> ",
    "<worn around wrist> ",
    "<wielded>           ",
	"<dual wielded>      ",
    "<held>              ",
    "<floating nearby>   ",
	"{W({Rlodged in an leg{W){x ",
	"{W({Rlodged in a arm{W){x  ",
	"{W({Rlodged in a rib{W){x  ",

};

bool is_command( char *arg )
{
int cmd;

for (cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
  if ( UPPER(arg[0]) == UPPER(cmd_table[cmd].name[0])
  && is_exact_name( cmd_table[cmd].name, arg ) )
    return TRUE;

return FALSE;
}

/* for  keeping track of the player count */
int max_on = 0;

/*
 * Local functions.
 */
char *	format_obj_to_char	args( ( OBJ_DATA *obj, CHAR_DATA *ch,
				    bool fShort ) );
void	show_list_to_char	args( ( OBJ_DATA *list, CHAR_DATA *ch,
				    bool fShort, bool fShowNothing ) );
void	show_char_to_char_0	args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void	show_char_to_char_1	args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void	show_char_to_char	args( ( CHAR_DATA *list, CHAR_DATA *ch ) );
bool	check_blind		args( ( CHAR_DATA *ch ) );
void look_sky	args(( CHAR_DATA *ch));

char *format_obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch, bool fShort )
{
    static char buf[MAX_STRING_LENGTH];

    buf[0] = '\0';

    if ((fShort && (obj->short_descr == NULL || obj->short_descr[0] == '\0'))
    ||  (obj->description == NULL || obj->description[0] == '\0'))
	return buf;

    if ( IS_OBJ_STAT(obj, ITEM_INVIS)     )   strcat( buf, "({wInvis{x) "     );
    if ( IS_AFFECTED(ch, AFF_DETECT_EVIL)
         && IS_OBJ_STAT(obj, ITEM_EVIL)   )   strcat( buf, "({RRed Aura{x) "  );
    if (IS_AFFECTED(ch, AFF_DETECT_GOOD)
    &&  IS_OBJ_STAT(obj,ITEM_BLESS))	      strcat(buf,"({bBlPBue Au{bra{X) "	);
    if ( IS_AFFECTED(ch, AFF_DETECT_MAGIC)
         && IS_OBJ_STAT(obj, ITEM_MAGIC)  )   strcat( buf, "({mM{Ma{wg{Wi{wc{Ma{ml{x) "   );
    if ( IS_OBJ_STAT(obj, ITEM_GLOW)      )   strcat( buf, "({gG{Glo{Ywi{Gn{gg{x) "   );
    if ( IS_OBJ_STAT(obj, ITEM_HUM)       )   strcat( buf, "({YH{yu{wm{Wm{wi{yn{Yg{x) "   );

    if ( fShort )
    {
	if ( obj->short_descr != NULL )
	    strcat( buf, obj->short_descr );
    }
    else
    {
	if ( obj->description != NULL)
	    strcat( buf, obj->description );
    }

		if (strlen(buf)<=0)
    strcat(buf,"This object has no description. Please inform the IMP.");
	return buf;

    return buf;
}



/*
 * Show a list to a character.
 * Can coalesce duplicated items.
 */
void show_list_to_char( OBJ_DATA *list, CHAR_DATA *ch, bool fShort, bool fShowNothing )
{
    char buf[MAX_STRING_LENGTH];
    BUFFER *output;
    char **prgpstrShow;
    int *prgnShow;
    char *pstrShow;
    OBJ_DATA *obj;
    int nShow;
    int iShow;
    int count;
    bool fCombine;

    if ( ch->desc == NULL )
	return;

    /*
     * Alloc space for output lines.
     */
    output = new_buf();

    count = 0;
    for ( obj = list; obj != NULL; obj = obj->next_content )
	count++;
    prgpstrShow	= alloc_mem( count * sizeof(char *) );
    prgnShow    = alloc_mem( count * sizeof(int)    );
    nShow	= 0;

    /*
     * Format the list of objects.
     */
    for ( obj = list; obj != NULL; obj = obj->next_content )
    { 
	if ( obj->wear_loc == WEAR_NONE && can_see_obj( ch, obj )) 
	{
	    pstrShow = format_obj_to_char( obj, ch, fShort );

	    fCombine = FALSE;

	    if ( IS_NPC(ch) || IS_SET(ch->comm, COMM_COMBINE) )
	    {
		/*
		 * Look for duplicates, case sensitive.
		 * Matches tend to be near end so run loop backwords.
		 */
		for ( iShow = nShow - 1; iShow >= 0; iShow-- )
		{
		    if ( !strcmp( prgpstrShow[iShow], pstrShow ) )
		    {
			prgnShow[iShow]++;
			fCombine = TRUE;
			break;
		    }
		}
	    }

	    /*
	     * Couldn't combine, or didn't want to.
	     */
	    if ( !fCombine )
	    {
		prgpstrShow [nShow] = str_dup( pstrShow );
		prgnShow    [nShow] = 1;
		nShow++;
	    }
	}
    }

    /*
     * Output the formatted list.
     */
    for ( iShow = 0; iShow < nShow; iShow++ )
    {
	if (prgpstrShow[iShow][0] == '\0')
	{
	    free_string(prgpstrShow[iShow]);
	    continue;
	}

	if ( IS_NPC(ch) || IS_SET(ch->comm, COMM_COMBINE) )
	{
	    if ( prgnShow[iShow] != 1 )
	    {
		sprintf( buf, "(%2d) ", prgnShow[iShow] );
		add_buf(output,buf);
	    }
	    else
	    {
		add_buf(output,"     ");
	    }
	}
	add_buf(output,prgpstrShow[iShow]);
	add_buf(output,"\n\r");
	free_string( prgpstrShow[iShow] );
    }

    if ( fShowNothing && nShow == 0 )
    {
	if ( IS_NPC(ch) || IS_SET(ch->comm, COMM_COMBINE) )
	    send_to_char( "     ", ch );
	send_to_char( "Nothing.\n\r", ch );
    }
    page_to_char(buf_string(output),ch);

    /*
     * Clean up.
     */
    free_buf(output);
    free_mem( prgpstrShow, count * sizeof(char *) );
    free_mem( prgnShow,    count * sizeof(int)    );

    return;
}



void show_char_to_char_0( CHAR_DATA *victim, CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH],message[MAX_STRING_LENGTH];

    buf[0] = '\0';

	if ( RIDDEN(victim) )
	{
	if ( ch == RIDDEN(victim) )	    strcat( buf, "(Your mount) " );
	else
		strcat(buf, "(Ridden) ");
	}
    if ( IS_SET(victim->comm,COMM_AFK	  )   ) strcat( buf, "[AFK] "	     );
    if ( IS_AFFECTED(victim, AFF_INVISIBLE)   ) strcat( buf, "({wInvis{x) "      );
    if ( victim->invis_level >= LEVEL_HERO    ) strcat( buf, "(Wizi) "	     );
    if ( IS_AFFECTED(victim, AFF_HIDE)        ) strcat( buf, "({yHide{x) "       );
    if ( IS_AFFECTED(victim, AFF_CHARM)       ) strcat( buf, "({mCharmed{x) "    );
    if ( IS_AFFECTED(victim, AFF_PASS_DOOR)   ) strcat( buf, "({CTranslucent{x) ");
    if ( IS_AFFECTED(victim, AFF_FAERIE_FIRE) ) strcat( buf, "({MPink Aura{x) "  );
    if ( IS_EVIL(victim)
    &&   IS_AFFECTED(ch, AFF_DETECT_EVIL)     ) strcat( buf, "({RRed Aura{x) "   );
    if ( IS_GOOD(victim)
    &&   IS_AFFECTED(ch, AFF_DETECT_GOOD)     ) strcat( buf, "({yGo{Yld{we{Wn {wA{Yur{ya{x) ");
    if ( IS_AFFECTED(victim, AFF_SANCTUARY)   ) strcat( buf, "({WWhite Aura{x) " );
    if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_KILLER ) )
						strcat( buf, "({RKILLER{x) "     );
    if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_THIEF  ) )
						strcat( buf, "({yTHIEF{x) "      );
	if ( IS_SET(victim->act,PLR_DEAD)&& !IS_NPC(victim) )		strcat( buf, "The spirit of "	);
	if( RIDDEN(victim) && ch != RIDDEN(victim))
		return;
    else if ( victim->position == victim->start_pos && victim->long_descr[0] != '\0')
    {
	strcat( buf, victim->long_descr );
	send_to_char( buf, ch );
	return;
    }

    strcat( buf, PERS( victim, ch ) );
//    if ( !IS_NPC(victim) && !IS_SET(ch->comm, COMM_BRIEF) 
  //  &&   victim->position == POS_STANDING && ch->on == NULL )
//	strcat( buf, victim->pcdata->title );

    switch ( victim->position )
    {
    case POS_DEAD:     strcat( buf, " is DEAD!!" );              break;
    case POS_MORTAL:   strcat( buf, " is mortally wounded." );   break;
    case POS_INCAP:    strcat( buf, " is incapacitated." );      break;
    case POS_STUNNED:  strcat( buf, " is lying here stunned." ); break;
    case POS_SLEEPING: 
	if (victim->on != NULL)
	{
	    if (IS_SET(victim->on->value[2],SLEEP_AT))
  	    {
		sprintf(message," is sleeping at %s.",
		    victim->on->short_descr);
		strcat(buf,message);
	    }
	    else if (IS_SET(victim->on->value[2],SLEEP_ON))
	    {
		sprintf(message," is sleeping on %s.",
		    victim->on->short_descr); 
		strcat(buf,message);
	    }
	    else
	    {
		sprintf(message, " is sleeping in %s.",
		    victim->on->short_descr);
		strcat(buf,message);
	    }
	}
	else 
	    strcat(buf," is sleeping here.");
	break;
    case POS_RESTING:  
        if (victim->on != NULL)
	{
            if (IS_SET(victim->on->value[2],REST_AT))
            {
                sprintf(message," is resting at %s.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
            else if (IS_SET(victim->on->value[2],REST_ON))
            {
                sprintf(message," is resting on %s.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
            else 
            {
                sprintf(message, " is resting in %s.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
	}
        else
	    strcat( buf, " is resting here." );       
	break;
    case POS_SITTING:  
        if (victim->on != NULL)
        {
            if (IS_SET(victim->on->value[2],SIT_AT))
            {
                sprintf(message," is sitting at %s.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
            else if (IS_SET(victim->on->value[2],SIT_ON))
            {
                sprintf(message," is sitting on %s.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
            else
            {
                sprintf(message, " is sitting in %s.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
        }
        else
	    strcat(buf, " is sitting here.");
	break;
    case POS_STANDING: 
	if (victim->on != NULL)
	{
	    if (IS_SET(victim->on->value[2],STAND_AT))
	    {
		sprintf(message," is standing at %s.",
		    victim->on->short_descr);
		strcat(buf,message);
	    }
	    else if (IS_SET(victim->on->value[2],STAND_ON))
	    {
		sprintf(message," is standing on %s.",
		   victim->on->short_descr);
		strcat(buf,message);
	    }
	    else
	    {
		sprintf(message," is standing in %s.",
		    victim->on->short_descr);
		strcat(buf,message);
	    }
	}
	else if ( MOUNTED(victim) )
	{
	    strcat( buf, " is here, riding " );
	    strcat( buf, MOUNTED(victim)->short_descr );
	    strcat( buf, ".");
	}
	else if (IS_SET(victim->act,PLR_DEAD))
	{
		strcat( buf, " is floating here.");
	}
	else
//	    strcat( buf, " is here." );               
	{
	    strcat( buf, " is here." );
	} 
	break;
    case POS_FIGHTING:
	strcat( buf, " is here, fighting " );
	if ( victim->fighting == NULL )
	    strcat( buf, "thin air??" );
	else if ( victim->fighting == ch )
	    strcat( buf, "YOU!" );
	else if ( victim->in_room == victim->fighting->in_room )
	{
	    strcat( buf, PERS( victim->fighting, ch ) );
	    strcat( buf, "." );
	}
	else
	    strcat( buf, "someone who left??" );
	break;
    }
 //   strcat( buf, PERS( victim, ch ) );
    strcat( buf, "\n\r" );
    buf[0] = UPPER(buf[0]);
    send_to_char( buf, ch );
    return;
}



void show_char_to_char_1( CHAR_DATA *victim, CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    int iWear;
    int percent;
    bool found;

    if ( can_see( victim, ch ) )
    {
	if (ch == victim)
	    act( "$n looks at $mself.",ch,NULL,NULL,TO_ROOM);
	else
	{
	    act( "$n looks at you.", ch, NULL, victim, TO_VICT    );
	    act( "$n looks at $N.",  ch, NULL, victim, TO_NOTVICT );
	}
    }

    if ( victim->description[0] != '\0' )
    {
	send_to_char( victim->description, ch );
    }
    else
    {
	act( "You see nothing special about $M.", ch, NULL, victim, TO_CHAR );
    }

    if ( MOUNTED(victim) )
    {
	sprintf( buf, "%s is riding %s.\n\r", victim->name, MOUNTED(victim)->short_descr);
	send_to_char( buf, ch);
    }
    if ( RIDDEN(victim) )
    {
        sprintf( buf, "%s is being ridden by %s.\n\r", victim->short_descr, RIDDEN(victim)->name );
        send_to_char( buf, ch);
    }

    if ( victim->max_hit > 0 )
	percent = ( 100 * victim->hit ) / victim->max_hit;
    else
	percent = -1;

    strcpy( buf, PERS(victim, ch) );

    if (percent >= 100) 
	strcat( buf, " is in excellent condition.\n\r");
    else if (percent >= 90) 
	strcat( buf, " has a few scratches.\n\r");
    else if (percent >= 75) 
	strcat( buf," has some small wounds and bruises.\n\r");
    else if (percent >=  50) 
	strcat( buf, " has quite a few wounds.\n\r");
    else if (percent >= 30)
	strcat( buf, " has some big nasty wounds and scratches.\n\r");
    else if (percent >= 15)
	strcat ( buf, " looks pretty hurt.\n\r");
    else if (percent >= 0 )
	strcat (buf, " is in awful condition.\n\r");
    else
	strcat(buf, " is bleeding to death.\n\r");

    buf[0] = UPPER(buf[0]);
    send_to_char( buf, ch );

    found = FALSE;
    for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
    {
	if ( ( obj = get_eq_char( victim, iWear ) ) != NULL
	&&   can_see_obj( ch, obj ) )
	{
	    if ( !found )
	    {
		send_to_char( "\n\r", ch );
		act( "$N is using:", ch, NULL, victim, TO_CHAR );
		found = TRUE;
	    }
	    send_to_char(where_name[iWear], ch );
	    send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
	    send_to_char( "\n\r", ch );
	}
    }

    if ( victim != ch
    &&   !IS_NPC(ch)
    &&   number_percent( ) < get_skill(ch,gsn_peek))
    {
	send_to_char( "\n\rYou peek at the inventory:\n\r", ch );
	check_improve(ch,gsn_peek,TRUE,4);
	show_list_to_char( victim->carrying, ch, TRUE, TRUE );
    }

    return;
}



void show_char_to_char( CHAR_DATA *list, CHAR_DATA *ch )
{
    CHAR_DATA *rch;

    for ( rch = list; rch != NULL; rch = rch->next_in_room )
    {
	if ( rch == ch 
	    || ( RIDDEN(rch) && rch->in_room == RIDDEN(rch)->in_room && RIDDEN(rch) != ch ) )
	    continue;

	if ( get_trust(ch) < rch->invis_level)
	    continue;

	if ( can_see( ch, rch ) )
	{
	    show_char_to_char_0( rch, ch );
            if( MOUNTED(rch) && (rch->in_room == MOUNTED(rch)->in_room) )
                show_char_to_char_0( MOUNTED(rch), ch );
	}
	else if ( room_is_dark( ch->in_room )
	&&        IS_AFFECTED(rch, AFF_INFRARED ) )
	{
	    send_to_char( "You see glowing red eyes watching YOU!\n\r", ch );
	}
    }

    return;
} 



bool check_blind( CHAR_DATA *ch )
{

    if (!IS_NPC(ch) && IS_SET(ch->act,PLR_HOLYLIGHT))
	return TRUE;

    if ( IS_AFFECTED(ch, AFF_BLIND) )
    { 
	send_to_char( "You can't see a thing!\n\r", ch ); 
	return FALSE; 
    }

    return TRUE;
}

/* changes your scroll */
void do_scroll(CHAR_DATA *ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char buf[100];
    int lines;

    one_argument(argument,arg);
    
    if (arg[0] == '\0')
    {
	if (ch->lines == 0)
	    send_to_char("You do not page long messages.\n\r",ch);
	else
	{
	    sprintf(buf,"You currently display %d lines per page.\n\r",
		    ch->lines + 2);
	    send_to_char(buf,ch);
	}
	return;
    }

    if (!is_number(arg))
    {
	send_to_char("You must provide a number.\n\r",ch);
	return;
    }

    lines = atoi(arg);

    if (lines == 0)
    {
        send_to_char("Paging disabled.\n\r",ch);
        ch->lines = 0;
        return;
    }

    if (lines < 10 || lines > 100)
    {
	send_to_char("You must provide a reasonable number.\n\r",ch);
	return;
    }

    sprintf(buf,"Scroll set to %d lines.\n\r",lines);
    send_to_char(buf,ch);
    ch->lines = lines - 2;
}

/* RT does socials */
void do_socials(CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    int iSocial;
    int col;
     
    col = 0;
   
    for (iSocial = 0; social_table[iSocial].name[0] != '\0'; iSocial++)
    {
	sprintf(buf,"%-12s",social_table[iSocial].name);
	send_to_char(buf,ch);
	if (++col % 6 == 0)
	    send_to_char("\n\r",ch);
    }

    if ( col % 6 != 0)
	send_to_char("\n\r",ch);
    return;
}


 
/* RT Commands to replace news, motd, imotd, etc from ROM */

void do_motd(CHAR_DATA *ch, char *argument)
{
    do_function(ch, &do_help, "motd");
}

void do_imotd(CHAR_DATA *ch, char *argument)
{  
    do_function(ch, &do_help, "imotd");
}

void do_rules(CHAR_DATA *ch, char *argument)
{
    do_function(ch, &do_help, "rules");
}

void do_story(CHAR_DATA *ch, char *argument)
{
    do_function(ch, &do_help, "story");
}


/* RT this following section holds all the auto commands from ROM, as well as
   replacements for config */

void do_autolist(CHAR_DATA *ch, char *argument)
{
    /* lists most player flags */
    if (IS_NPC(ch))
      return;

    send_to_char("   action     status\n\r",ch);
    send_to_char("---------------------\n\r",ch);
 
    send_to_char("autoassist     ",ch);
    if (IS_SET(ch->act,PLR_AUTOASSIST))
        send_to_char("ON\n\r",ch);
    else
        send_to_char("OFF\n\r",ch); 

    send_to_char("autoexit       ",ch);
    if (IS_SET(ch->act,PLR_AUTOEXIT))
        send_to_char("ON\n\r",ch);
    else
        send_to_char("OFF\n\r",ch);

    send_to_char("autogold       ",ch);
    if (IS_SET(ch->act,PLR_AUTOGOLD))
        send_to_char("ON\n\r",ch);
    else
        send_to_char("OFF\n\r",ch);

    send_to_char("autoloot       ",ch);
    if (IS_SET(ch->act,PLR_AUTOLOOT))
        send_to_char("ON\n\r",ch);
    else
        send_to_char("OFF\n\r",ch);

    send_to_char("autosac        ",ch);
    if (IS_SET(ch->act,PLR_AUTOSAC))
        send_to_char("ON\n\r",ch);
    else
        send_to_char("OFF\n\r",ch);

    send_to_char("autosplit      ",ch);
    if (IS_SET(ch->act,PLR_AUTOSPLIT))
        send_to_char("ON\n\r",ch);
    else
        send_to_char("OFF\n\r",ch);

    send_to_char("compact mode   ",ch);
    if (IS_SET(ch->comm,COMM_COMPACT))
        send_to_char("ON\n\r",ch);
    else
        send_to_char("OFF\n\r",ch);

    send_to_char("prompt         ",ch);
    if (IS_SET(ch->comm,COMM_PROMPT))
	send_to_char("ON\n\r",ch);
    else
	send_to_char("OFF\n\r",ch);

    send_to_char("combine items  ",ch);
    if (IS_SET(ch->comm,COMM_COMBINE))
	send_to_char("ON\n\r",ch);
    else
	send_to_char("OFF\n\r",ch);

    if (!IS_SET(ch->act,PLR_CANLOOT))
	send_to_char("Your corpse is safe from thieves.\n\r",ch);
    else 
        send_to_char("Your corpse may be looted.\n\r",ch);

    if (IS_SET(ch->act,PLR_NOSUMMON))
	send_to_char("You cannot be summoned.\n\r",ch);
    else
	send_to_char("You can be summoned.\n\r",ch);
   
    if (IS_SET(ch->act,PLR_NOFOLLOW))
	send_to_char("You do not welcome followers.\n\r",ch);
    else
	send_to_char("You accept followers.\n\r",ch);
}

void do_autoassist(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;
    
    if (IS_SET(ch->act,PLR_AUTOASSIST))
    {
      send_to_char("Autoassist removed.\n\r",ch);
      REMOVE_BIT(ch->act,PLR_AUTOASSIST);
    }
    else
    {
      send_to_char("You will now assist when needed.\n\r",ch);
      SET_BIT(ch->act,PLR_AUTOASSIST);
    }
}

void do_introduce(CHAR_DATA *ch, char *argument)
{
	CHAR_DATA *victim;
	char buf[MIL];

	if (IS_NPC(ch))
		return;

	if (argument[0] == '\0' )
	{
		send_to_one(ch,"Introduce yourself to whom?");
		return;
	}

	if ((victim = get_char_room(ch, NULL, argument)) == NULL)
	{
		send_to_one(ch,"They aren't here.");
		return;
	}

	if (IS_NPC(victim))
	{
		send_to_one(ch,"Sorry, but I don't think the mob will remember you.");
		return;
	}

	if (victim == ch)
	{
		send_to_one(ch,"Do you really think that's necessary?");
		return;
	}

	if(is_known(victim,ch))
	{
		send_to_one(ch,"But they already know you!.");
		return;
	}

	sprintf(buf, "%s",		ch->sex == 0 ? "him" : 
		ch->sex == 1 ? "her" :
	ch->sex == 3  ? "it" : "it");

	send_to_one(ch,"You introduce yourself to %s.", victim->name);
	send_to_one(victim,"%s introduces %sself to you.",ch->name,buf);
	act("$n introduces $mself to $N.",ch,NULL,victim,TO_NOTVICT);

//	free_string(victim->known->name);
	known_add(victim,ch->name);

	return;
}


void do_withdraw( CHAR_DATA *ch, char *argument)
{
	char arg1[MIL];
	char arg2[MIL];
	char arg3[MIL];
	long amt;
	long change;
	long diff;
	//int clan;

	argument = one_argument(argument,arg1);

	if(IS_NPC(ch))
		return;

	if(!IS_SET(ch->in_room->room_flags, ROOM_BANK))
	{
		send_to_one(ch,"You must be in a bank to make a withdrawl.");
		return;
	}

	if(arg1[0] == '\0')
	{
		send_to_one(ch,"Withdraw <amt> <gold/silver> (clan name)<optional>");
		return;
	}
	
	amt = atoi(arg1);


	argument = one_argument(argument,arg2);
	argument = one_argument(argument,arg3);

	if(arg2[0] == '\0')
	{
		send_to_one(ch,"Gold or silver?");
		return;
	}
	

	if(!str_prefix(arg2,"gold"))
	{
		if(amt == 0 || amt > ch->pcdata->bankgold )
		{
			send_to_one(ch,"You cannot withdraw that.");
			return;
		}

		change =  95 * amt / 100 ;
		diff = amt - change;
		if(change < 1 )
		{
		send_to_one(ch,"You withdraw {Y1 gold{x.");
		ch->gold++;
		ch->pcdata->bankgold--;
		}
		else
		{
		send_to_one(ch,"The bank charges you {Y%ld gold{x for your withdrawl.", diff);
		send_to_one(ch,"You withdraw {Y%ld gold{x.",change);
		ch->gold += change;
		ch->pcdata->bankgold -= amt;
		}

		send_to_one(ch,"Your account now contains {Y%ld gold{x and {W%ld silver.", 
			ch->pcdata->bankgold, ch->pcdata->banksilver);
		return;
	}
	else if(!str_prefix(arg2,"silver"))
	{
		if(amt == 0 || amt > ch->pcdata->banksilver )
		{
			send_to_one(ch,"You cannot withdraw that.");
			return;
		}
		change = 95 * amt / 100;
		diff = amt - change;
		if(change < 1 )
		{
		send_to_one(ch, "You withdraw {W1 silver{x.");
		ch->silver++;
		ch->pcdata->banksilver--;
		}
		else
		{
		send_to_one(ch,"The bank charges you {W%ld silver{x for your withdrawl.", diff);
		send_to_one(ch, "You withdraw {W%ld silver{x.",change);
		ch->silver += change;
		ch->pcdata->banksilver -= amt;
		}
		send_to_one(ch,"Your account now contains {Y%ld gold{x and {W%ld silver.", 
			ch->pcdata->bankgold, ch->pcdata->banksilver);
		return;
	}

	else
	{
		send_to_one(ch,"Withdraw <amt> <gold/silver> (clan name)<optional>");
		return;
	}
	return;
}

void do_balance( CHAR_DATA *ch, char *argument)
{
	char arg[MIL];
//	int clan;

	argument = one_argument(argument,arg);

	if(IS_NPC(ch))
		return;


	if(!IS_SET(ch->in_room->room_flags, ROOM_BANK))
	{
		send_to_one(ch,"You can only check account balances in a bank.");
		return;
	}
	
	if(arg[0] == '\0')
	{
	if(ch->pcdata->bankgold != 0 || ch->pcdata->banksilver != 0)
	{
		send_to_one(ch,"Your bank account contains {Y%ld gold{x and {W%ld silver{x.",
			ch->pcdata->bankgold, ch->pcdata->banksilver);
		return;
	}
	else
	{
		send_to_one(ch,"Your bank account is empty.");
		return;
	}
	}


	return;
}

	
void do_deposit( CHAR_DATA *ch, char *argument)
{
	char arg1[MIL];
	char arg2[MIL];
	char arg3[MIL];
	long amt;
//	int clan, city;

	argument = one_argument(argument,arg1);

	if(IS_NPC(ch))
		return;

	if(!IS_SET(ch->in_room->room_flags, ROOM_BANK))
	{
		send_to_one(ch,"You must be in a bank to make a deposit.");
		return;
	}

	if(arg1[0] == '\0')
	{
		send_to_one(ch,"Deposit <amt> <gold/silver> (clan/city name)<optional>");
		return;
	}
	
	amt = atoi(arg1);


	argument = one_argument(argument,arg2);

	argument = one_argument(argument, arg3);

	if(arg2[0] == '\0')
	{
		send_to_one(ch,"Gold or silver?");
		return;
	}


	if(!str_prefix(arg2,"gold"))
	{
		if(amt == 0 || amt > ch->gold)
		{
			send_to_one(ch,"You cannot deposit that.");
			return;
		}

		send_to_one(ch,"You deposit {Y%ld gold{x.",amt);
		ch->gold -= amt;
		ch->pcdata->bankgold += amt;
		send_to_one(ch,"Your account now contains {Y%ld gold{x and {W%ld silver.", 
			ch->pcdata->bankgold, ch->pcdata->banksilver);
		return;
	}
	else if(!str_prefix(arg2,"silver"))
	{
		if(amt == 0 || amt > ch->silver)
		{
			send_to_one(ch,"You cannot deposit that.");
			return;
		}

		send_to_one(ch, "You deposit {W%ld silver{x.",amt);
		ch->silver -= amt;
		ch->pcdata->banksilver += amt;
		send_to_one(ch,"Your account now contains {Y%ld gold{x and {W%ld silver.", 
			ch->pcdata->bankgold, ch->pcdata->banksilver);
		return;
	}

	
	return;
}


bool is_known( CHAR_DATA *ch, CHAR_DATA *victim)
{
KNOWN_DATA *known;

if (ch->desc == NULL) return FALSE;

if (ch->known == NULL) return FALSE;

if (ch->desc->original != NULL) ch = ch->desc->original;

for (known=ch->known; known != NULL; known=known->next)
{
  if (is_name(known->name,victim->name)
	  || IS_IMMORTAL(ch))
    return TRUE;
}

return FALSE;
}

void known_add(CHAR_DATA *ch, char *name)
{
KNOWN_DATA *known;

if (ch->desc->original != NULL) ch=ch->desc->original;

known = alloc_mem(sizeof(*known));
known->name = str_dup(name);

known->next = ch->known;
ch->known = known;

return;
}

void do_autoexit(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;
 
    if (IS_SET(ch->act,PLR_AUTOEXIT))
    {
      send_to_char("Exits will no longer be displayed.\n\r",ch);
      REMOVE_BIT(ch->act,PLR_AUTOEXIT);
    }
    else
    {
      send_to_char("Exits will now be displayed.\n\r",ch);
      SET_BIT(ch->act,PLR_AUTOEXIT);
    }
}

void do_autogold(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;
 
    if (IS_SET(ch->act,PLR_AUTOGOLD))
    {
      send_to_char("Autogold removed.\n\r",ch);
      REMOVE_BIT(ch->act,PLR_AUTOGOLD);
    }
    else
    {
      send_to_char("Automatic gold looting set.\n\r",ch);
      SET_BIT(ch->act,PLR_AUTOGOLD);
    }
}

void do_autoloot(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;
 
    if (IS_SET(ch->act,PLR_AUTOLOOT))
    {
      send_to_char("Autolooting removed.\n\r",ch);
      REMOVE_BIT(ch->act,PLR_AUTOLOOT);
    }
    else
    {
      send_to_char("Automatic corpse looting set.\n\r",ch);
      SET_BIT(ch->act,PLR_AUTOLOOT);
    }
}

void do_autosac(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;
 
    if (IS_SET(ch->act,PLR_AUTOSAC))
    {
      send_to_char("Autosacrificing removed.\n\r",ch);
      REMOVE_BIT(ch->act,PLR_AUTOSAC);
    }
    else
    {
      send_to_char("Automatic corpse sacrificing set.\n\r",ch);
      SET_BIT(ch->act,PLR_AUTOSAC);
    }
}

void do_autosplit(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;
 
    if (IS_SET(ch->act,PLR_AUTOSPLIT))
    {
      send_to_char("Autosplitting removed.\n\r",ch);
      REMOVE_BIT(ch->act,PLR_AUTOSPLIT);
    }
    else
    {
      send_to_char("Automatic gold splitting set.\n\r",ch);
      SET_BIT(ch->act,PLR_AUTOSPLIT);
    }
}

void do_brief(CHAR_DATA *ch, char *argument)
{
    if (IS_SET(ch->comm,COMM_BRIEF))
    {
      send_to_char("Full descriptions activated.\n\r",ch);
      REMOVE_BIT(ch->comm,COMM_BRIEF);
    }
    else
    {
      send_to_char("Short descriptions activated.\n\r",ch);
      SET_BIT(ch->comm,COMM_BRIEF);
    }
}

void do_compact(CHAR_DATA *ch, char *argument)
{
    if (IS_SET(ch->comm,COMM_COMPACT))
    {
      send_to_char("Compact mode removed.\n\r",ch);
      REMOVE_BIT(ch->comm,COMM_COMPACT);
    }
    else
    {
      send_to_char("Compact mode set.\n\r",ch);
      SET_BIT(ch->comm,COMM_COMPACT);
    }
}

void do_show(CHAR_DATA *ch, char *argument)
{
    if (IS_SET(ch->comm,COMM_SHOW_AFFECTS))
    {
      send_to_char("Affects will no longer be shown in score.\n\r",ch);
      REMOVE_BIT(ch->comm,COMM_SHOW_AFFECTS);
    }
    else
    {
      send_to_char("Affects will now be shown in score.\n\r",ch);
      SET_BIT(ch->comm,COMM_SHOW_AFFECTS);
    }
}

void do_prompt(CHAR_DATA *ch, char *argument)
{
   char buf[MAX_STRING_LENGTH];
 
   if ( argument[0] == '\0' )
   {
	if (IS_SET(ch->comm,COMM_PROMPT))
   	{
      	    send_to_char("You will no longer see prompts.\n\r",ch);
      	    REMOVE_BIT(ch->comm,COMM_PROMPT);
    	}
    	else
    	{
      	    send_to_char("You will now see prompts.\n\r",ch);
      	    SET_BIT(ch->comm,COMM_PROMPT);
    	}
       return;
   }
 
   if( !strcmp( argument, "all" ) )
      strcpy( buf, "<{C%r {Y%e{x>%c<{R%h{x/{C%Hhp{x {R%m{x/{C%Mm {M%vmv{x>%c<{Y%Xexp %g{yg {W%s{ws{x>%c>> ");
   else
   {
      if ( strlen(argument) > 100 )
         argument[100] = '\0';
      strcpy( buf, argument );
      smash_tilde( buf );
      if (str_suffix("%c",buf))
	strcat(buf," ");
	
   }
 
   free_string( ch->prompt );
   ch->prompt = str_dup( buf );
   sprintf(buf,"Prompt set to %s\n\r",ch->prompt );
   send_to_char(buf,ch);
   return;
}

void do_combine(CHAR_DATA *ch, char *argument)
{
    if (IS_SET(ch->comm,COMM_COMBINE))
    {
      send_to_char("Long inventory selected.\n\r",ch);
      REMOVE_BIT(ch->comm,COMM_COMBINE);
    }
    else
    {
      send_to_char("Combined inventory selected.\n\r",ch);
      SET_BIT(ch->comm,COMM_COMBINE);
    }
}

void do_noloot(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;
 
    if (IS_SET(ch->act,PLR_CANLOOT))
    {
      send_to_char("Your corpse is now safe from thieves.\n\r",ch);
      REMOVE_BIT(ch->act,PLR_CANLOOT);
    }
    else
    {
      send_to_char("Your corpse may now be looted.\n\r",ch);
      SET_BIT(ch->act,PLR_CANLOOT);
    }
}

void do_nofollow(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch)|| IS_AFFECTED(ch, AFF_CHARM))
      return;
 
    if (IS_SET(ch->act,PLR_NOFOLLOW))
    {
      send_to_char("You now accept followers.\n\r",ch);
      REMOVE_BIT(ch->act,PLR_NOFOLLOW);
    }
    else
    {
      send_to_char("You no longer accept followers.\n\r",ch);
      SET_BIT(ch->act,PLR_NOFOLLOW);
      die_follower( ch );
    }
}

void do_nosummon(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
    {
      if (IS_SET(ch->imm_flags,IMM_SUMMON))
      {
	send_to_char("You are no longer immune to summon.\n\r",ch);
	REMOVE_BIT(ch->imm_flags,IMM_SUMMON);
      }
      else
      {
	send_to_char("You are now immune to summoning.\n\r",ch);
	SET_BIT(ch->imm_flags,IMM_SUMMON);
      }
    }
    else
    {
      if (IS_SET(ch->act,PLR_NOSUMMON))
      {
        send_to_char("You are no longer immune to summon.\n\r",ch);
        REMOVE_BIT(ch->act,PLR_NOSUMMON);
      }
      else
      {
        send_to_char("You are now immune to summoning.\n\r",ch);
        SET_BIT(ch->act,PLR_NOSUMMON);
      }
    }
}

void do_look( CHAR_DATA *ch, char *argument )
{
    char buf  [MAX_STRING_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    EXIT_DATA *pexit;
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    char *pdesc;
    int door;
    int number,count;

    if ( ch->desc == NULL )
	return;

    if ( ch->position < POS_SLEEPING )
    {
	send_to_char( "You can't see anything but stars!\n\r", ch );
	return;
    }

    if ( ch->position == POS_SLEEPING )
    {
	send_to_char( "You can't see anything, you're sleeping!\n\r", ch );
	return;
    }

    if ( !check_blind( ch ) )
	return;

    if ( !IS_NPC(ch)
    &&   !IS_SET(ch->act, PLR_HOLYLIGHT)
    &&   room_is_dark( ch->in_room ) )
    {
	send_to_char( "It is pitch black ... \n\r", ch );
	show_char_to_char( ch->in_room->people, ch );
	return;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    number = number_argument(arg1,arg3);
    count = 0;

    if ( arg1[0] == '\0' || !str_cmp( arg1, "auto" ) )
    {
	/* 'look' or 'look auto' */
		sprintf( buf, "{s%s", ch->in_room->name );
		send_to_char(buf, ch);

	if ((IS_IMMORTAL(ch) && (IS_NPC(ch) || IS_SET(ch->act,PLR_HOLYLIGHT)))
		|| IS_BUILDER(ch, ch->in_room->area))
	{
	    sprintf(buf," {r[{RRoom %d{r]",ch->in_room->vnum);
	    send_to_char(buf,ch);
	}

	send_to_char( "{x\n\r", ch );

	if ( arg1[0] == '\0'
	|| ( !IS_NPC(ch) && !IS_SET(ch->comm, COMM_BRIEF) ) )
	{
		if (ch->desc && !ch->desc->run_buf)
		{
		if(IS_OUTSIDE(ch))
		show_weather(ch);
		//send_to_char( "{S  ",ch);
	    send_to_one(ch,"{S %s{x", ch->in_room->description );
//		send_to_char(buf, ch);
		}
	}

        if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_AUTOEXIT) )
	{
	    send_to_char("\n\r",ch);
            do_function(ch, &do_exits, "auto" );
	}

		send_to_char("{O",ch);
	show_list_to_char( ch->in_room->contents, ch, FALSE, FALSE );
	send_to_char("{x",ch);
	show_char_to_char( ch->in_room->people,   ch );
	return;
    }
	if ( !str_cmp( arg1, "sky" ) )
	{
		if ( !IS_OUTSIDE(ch) )
		{
	    send_to_char( "You can't see the sky indoors.\n\r", ch );
	    return;
	}
	else
	{
	    look_sky(ch);
		return;
	  }
	}

    if ( !str_cmp( arg1, "i" ) || !str_cmp(arg1, "in")  || !str_cmp(arg1,"on"))
    {
	/* 'look in' */
	if ( arg2[0] == '\0' )
	{
	    send_to_char( "Look in what?\n\r", ch );
	    return;
	}

	if ( ( obj = get_obj_here( ch,NULL, arg2 ) ) == NULL )
	{
	    send_to_char( "You do not see that here.\n\r", ch );
	    return;
	}

	switch ( obj->item_type )
	{
	default:
	    send_to_char( "That is not a container.\n\r", ch );
	    break;

	case ITEM_TOKEN:
		if (obj->value[4] != 1)
		{
			send_to_one(ch,"That is not a container.");
			break;
		}

		if(obj->value[2] > 0)
		{
			send_to_one(ch,"The current jackpot is %d gold.",obj->value[2]);
			break;
		}
		else
			send_to_one(ch,"There is no jackpot at this time.");
			break;

	case ITEM_DRINK_CON:
	    if ( obj->value[1] <= 0 )
	    {
		send_to_char( "It is empty.\n\r", ch );
		break;
	    }

	    sprintf( buf, "It's %sfilled with  a %s liquid.\n\r",
		obj->value[1] <     obj->value[0] / 4
		    ? "less than half-" :
		obj->value[1] < 3 * obj->value[0] / 4
		    ? "about half-"     : "more than half-",
		liq_table[obj->value[2]].liq_color
		);

	    send_to_char( buf, ch );
	    break;

case ITEM_QUIVER:
		if ( obj->value[0] <= 0 )
		{
		send_to_char( "{WThe quiver is out of arrows.{x\n\r", ch );
		break;
		}
		
		if (obj->value[0] == 1 )
		{
		send_to_char( "{WThe quiver has 1 arrow remaining in it.{x\n\r", ch );
		break;
		}
		
		if (obj->value[0] > 1 )
		{
		sprintf( buf, "{WThe quiver has %d arrows in it.{x\n\r", obj->value[0]);
		}
		send_to_char( buf, ch);
		break;

	case ITEM_CONTAINER:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	    if ( IS_SET(obj->value[1], CONT_CLOSED) )
	    {
		send_to_char( "It is closed.\n\r", ch );
		break;
	    }

	    act( "$p holds:", ch, obj, NULL, TO_CHAR );
	    show_list_to_char( obj->contains, ch, TRUE, TRUE );
	    break;
	}
	return;
    }

    if ( ( victim = get_char_room( ch, NULL, arg1 ) ) != NULL )
    {
	show_char_to_char_1( victim, ch );
	return;
    }

    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
	if ( can_see_obj( ch, obj ) )
	{  /* player can see object */
	    pdesc = get_extra_descr( arg3, obj->extra_descr );
	    if ( pdesc != NULL )
		{
	    	if (++count == number)
	    	{
		    send_to_char( pdesc, ch );
		    return;
	    	}
	    	else continue;
		}

 	    pdesc = get_extra_descr( arg3, obj->pIndexData->extra_descr );
 	    if ( pdesc != NULL )
		{
 	    	if (++count == number)
 	    	{	
		    send_to_char( pdesc, ch );
		    return;
	     	}
		else continue;
		}

	    if ( is_name( arg3, obj->name ) )
	    	if (++count == number)
	    	{
	    	    send_to_char( obj->description, ch );
	    	    send_to_char( "\n\r",ch);
		    return;
		  }
	  }
    }

    for ( obj = ch->in_room->contents; obj != NULL; obj = obj->next_content )
    {
	if ( can_see_obj( ch, obj ) )
	{
	    pdesc = get_extra_descr( arg3, obj->extra_descr );
	    if ( pdesc != NULL )
	    	if (++count == number)
	    	{
		    send_to_char( pdesc, ch );
		    return;
	    	}

	    pdesc = get_extra_descr( arg3, obj->pIndexData->extra_descr );
	    if ( pdesc != NULL )
	    	if (++count == number)
	    	{
		    send_to_char( pdesc, ch );
		    return;
	    	}

	    if ( is_name( arg3, obj->name ) )
		if (++count == number)
		{
		    send_to_char( obj->description, ch );
		    send_to_char("\n\r",ch);
		    return;
		}
	}
    }

    pdesc = get_extra_descr(arg3,ch->in_room->extra_descr);
    if (pdesc != NULL)
    {
	if (++count == number)
	{
	    send_to_char(pdesc,ch);
	    return;
	}
    }
    
    if (count > 0 && count != number)
    {
    	if (count == 1)
    	    sprintf(buf,"You only see one %s here.\n\r",arg3);
    	else
    	    sprintf(buf,"You only see %d of those here.\n\r",count);
    	
    	send_to_char(buf,ch);
    	return;
    }

         if ( !str_cmp( arg1, "n" ) || !str_cmp( arg1, "north" ) ) door = 0;
    else if ( !str_cmp( arg1, "e" ) || !str_cmp( arg1, "east"  ) ) door = 1;
    else if ( !str_cmp( arg1, "s" ) || !str_cmp( arg1, "south" ) ) door = 2;
    else if ( !str_cmp( arg1, "w" ) || !str_cmp( arg1, "west"  ) ) door = 3;
    else if ( !str_cmp( arg1, "u" ) || !str_cmp( arg1, "up"    ) ) door = 4;
    else if ( !str_cmp( arg1, "d" ) || !str_cmp( arg1, "down"  ) ) door = 5;
    else if ( !str_cmp( arg1, "ne" ) || !str_cmp( arg1, "northeast") ) door = 6;
	else if ( !str_cmp( arg1, "nw" ) || !str_cmp(arg1, "northwest" ))  door = 7;
	else if ( !str_cmp( arg1, "se" ) || !str_cmp(arg1, "southeast" ))  door = 8;
	else if (!str_cmp( arg1, "sw" ) || !str_cmp(arg1, "southwest" )) door = 9;
	else
    {
	send_to_char( "You do not see that here.\n\r", ch );
	return;
    }

    /* 'look direction' */
    if ( ( pexit = ch->in_room->exit[door] ) == NULL )
    {
	send_to_char( "Nothing special there.\n\r", ch );
	return;
    }

    if ( pexit->description != NULL && pexit->description[0] != '\0' )
	send_to_char( pexit->description, ch );
    else
	send_to_char( "Nothing special there.\n\r", ch );

    if ( pexit->keyword    != NULL
    &&   pexit->keyword[0] != '\0'
    &&   pexit->keyword[0] != ' ' )
    {
	if ( IS_SET(pexit->exit_info, EX_CLOSED) )
	{
	    act( "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
	}
	else if ( IS_SET(pexit->exit_info, EX_ISDOOR) )
	{
	    act( "The $d is open.",   ch, NULL, pexit->keyword, TO_CHAR );
	}
    }

    return;
}

/* RT added back for the hell of it */
void do_read (CHAR_DATA *ch, char *argument )
{
    do_function(ch, &do_look, argument);
}

void do_examine( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Examine what?\n\r", ch );
	return;
    }

    do_function(ch, &do_look, arg );

    if ( ( obj = get_obj_here( ch,NULL, arg ) ) != NULL )
    {
	switch ( obj->item_type )
	{
	default:
	    break;
	
	case ITEM_JUKEBOX:
	    do_function(ch, &do_play, "list");
	    break;

	case ITEM_MONEY:
	    if (obj->value[0] == 0)
	    {
	        if (obj->value[1] == 0)
		    sprintf(buf,"Odd...there's no coins in the pile.\n\r");
		else if (obj->value[1] == 1)
		    sprintf(buf,"Wow. One gold coin.\n\r");
		else
		    sprintf(buf,"There are %d gold coins in the pile.\n\r",
			obj->value[1]);
	    }
	    else if (obj->value[1] == 0)
	    {
		if (obj->value[0] == 1)
		    sprintf(buf,"Wow. One silver coin.\n\r");
		else
		    sprintf(buf,"There are %d silver coins in the pile.\n\r",
			obj->value[0]);
	    }
	    else
		sprintf(buf,
		    "There are %d gold and %d silver coins in the pile.\n\r",
		    obj->value[1],obj->value[0]);
	    send_to_char(buf,ch);
	    break;

	case ITEM_DRINK_CON:
	case ITEM_CONTAINER:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	    sprintf(buf,"in %s",argument);
	    do_function(ch, &do_look, buf );
	}
    }

    return;
}



/*
 * Thanks to Zrin for auto-exit part.
 */
void do_exits( CHAR_DATA *ch, char *argument )
{
    extern char * const dir_name[];
    char buf[MAX_STRING_LENGTH];
    EXIT_DATA *pexit;
    bool found;
    bool fAuto;
    int door;

    fAuto  = !str_cmp( argument, "auto" );

    if ( !check_blind( ch ) )
	return;

    if (fAuto)
	sprintf(buf,"[{oExits:");
    else if (IS_IMMORTAL(ch))
	sprintf(buf,"Obvious exits from room %d:\n\r",ch->in_room->vnum);
    else
	sprintf(buf,"Obvious exits:\n\r");

    found = FALSE;
    for ( door = 0; door <= 9; door++ )
    {
	if ( ( pexit = ch->in_room->exit[door] ) != NULL
	&&   pexit->u1.to_room != NULL
	&&   can_see_room(ch,pexit->u1.to_room) )
//	&&   !IS_SET(pexit->exit_info, EX_CLOSED)) 
	{
	    found = TRUE;
	    if ( fAuto )
	    {
			if(IS_SET(pexit->exit_info,EX_CLOSED) && IS_AFFECTED(ch,AFF_INFRARED))
			{
				strcat(buf, " (");
				strcat(buf, dir_name[door]);
				strcat(buf, ")");
			}
			else
			{
			strcat( buf, " " );
			strcat( buf, dir_name[door] );
			}
	    }
	    else
	    {
		sprintf( buf + strlen(buf), "%-5s - %s",
		    capitalize( dir_name[door] ),
		    room_is_dark( pexit->u1.to_room )
			?  "Too dark to tell"
			: pexit->u1.to_room->name
		    );
		if (IS_IMMORTAL(ch))
		    sprintf(buf + strlen(buf), 
			" (room %d)\n\r",pexit->u1.to_room->vnum);
		else
		    sprintf(buf + strlen(buf), "\n\r");
	    }
	}
    }

    if ( !found )
	strcat( buf, fAuto ? " none" : "None.\n\r" );

    if ( fAuto )
	strcat( buf, "{x]\n\r" );

    send_to_char( buf, ch );
    return;
}

void do_worth( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    if (IS_NPC(ch))
    {
	sprintf(buf,"You have %ld gold and %ld silver.\n\r",
	    ch->gold,ch->silver);
	send_to_char(buf,ch);
	return;
    }

    sprintf(buf, 
    "You have %ld gold, %ld silver, and %d experience (%d exp to level).\n\r",
	ch->gold, ch->silver,ch->exp,
	(ch->level + 1) * exp_per_level(ch,ch->pcdata->points) - ch->exp);

    send_to_char(buf,ch);

    return;
}

/* This is NEW score listing for The Mage's Lair */
void do_score( CHAR_DATA *ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  int i;

  if (IS_NPC(ch))
    {
      send_to_char("Use the OLD score function\n\r",ch);
      return;
    }
    
  send_to_char("{Y------------------------------------------------------------------{x\n\r",ch);
  printf_to_char(ch,
		 "{cYou are {W%s%s{c, level {W%d{c, {W%d {cyears old ({W%d{c hours).\n\r",
		 ch->name,
		 IS_NPC(ch) ? "" : ch->pcdata->title,
		 ch->level, get_age(ch),
		 ( ch->played + (int) (current_time - ch->logon) ) / 3600);

    
  if ( get_trust( ch ) != ch->level )
    {
      printf_to_char(ch, "{cYou are trusted at level {W%d{c.\n\r",
		     get_trust( ch ) );
    }

  printf_to_char(ch, "{cRace: {W%s  {cSex: {W%s  {cClass: {W%s\n\r",
		 race_table[ch->race].name,
		 sex_table[ch->sex].name,
		 IS_NPC(ch) ? "mobile" : class_table[ch->class].name);
 /* if (ch->clan) { 
    printf_to_char(ch, "{cClan:{x %s  {cRank:{x %s\n\r",
		   clan_table[ch->clan].who_name,
		   clan_rank_table[ch->pcdata->clan_rank].title_of_rank[ch->sex]);
  }*/
  if(is_clan(ch))

  printf_to_char(ch, "You are a %s of the %s\n\r", player_rank(ch), player_clan(ch));
  printf_to_char(ch,
		 "{cStr: {W%d(%d)  {cInt: {W%d(%d)  {cWis: {W%d(%d)  {cDex: {W%d(%d)  {cCon: {W%d(%d)\n\r",
		 ch->perm_stat[STAT_STR],
		 get_curr_stat(ch,STAT_STR),
		 ch->perm_stat[STAT_INT],
		 get_curr_stat(ch,STAT_INT),
		 ch->perm_stat[STAT_WIS],
		 get_curr_stat(ch,STAT_WIS),
		 ch->perm_stat[STAT_DEX],
		 get_curr_stat(ch,STAT_DEX),
		 ch->perm_stat[STAT_CON],
		 get_curr_stat(ch,STAT_CON) );
	
  if ( ch->level >= 15 )
    {
      printf_to_char(ch,
		     "{cToHit: {W%-9d                            {cToDam: {W%d\n\r",
		     GET_HITROLL(ch), GET_DAMROLL(ch) );
    }
  printf_to_char(ch,
		 "{cHP:   {W%5d/%-5d                           {cPracs: {W%d \n\r",
		 ch->hit,  ch->max_hit,
		 ch->practice);

  printf_to_char(ch,
		 "{cMANA: {W%5d/%-5d                           {cTrains: {W%d \n\r",
		 ch->mana, ch->max_mana,
		 ch->train);
  printf_to_char(ch,
		 "{cMOVE: {W%5d/%-5d                           {cItems: {W%d/%d \n\r",
		 ch->move, ch->max_move,
		 ch->carry_number, can_carry_n(ch));
  printf_to_char(ch,
		 "{cGlory: {W%-9d                            {cWeight: {W%d/%d \n\r",
		 ch->pcdata->quest_curr,
		 get_carry_weight(ch) / 10, can_carry_w(ch) /10 );

  /*
   * This section (exp, gold, balance and shares) was written by
   * The Maniac from Mythran Mud
   */
  if (IS_NPC(ch))             /* NPC's have no bank balance and shares */
    {                           /* and don't level !!       -- Maniac -- */
      printf_to_char(ch,
		     "You have scored %d exp, and have %ld gold coins\n\r",
		     ch->exp,  ch->gold );
    }

  if (!IS_NPC(ch))            /* PC's do level and can have bank accounts */
    {                           /* HERO's don't level anymore               */
      printf_to_char(ch,
		     "{cExp: {W%-6d       {cTNL: {W%-7d       {cWimpy: {W%d\n\r",
		     ch->exp,IS_HERO(ch) ? (1) : (ch->level + 1) *
		     exp_per_level(ch,ch->pcdata->points) - ch->exp,
		     ch->wimpy);

      printf_to_char(ch,
		     "{YGold: {W%-6d      {cSilver: {W%-6d     {cBank: {Y%-7d g  {w %d s{x\n\r",
			 ch->gold, ch->silver, ch->pcdata->bankgold, ch->pcdata->banksilver); 
/*      if ( ch->pcdata->shares ) 
        {
	  printf_to_char(ch,"You have %d gold invested in %d shares (%d each).\n\r",
			 (ch->pcdata->shares * share_value),
			 ch->pcdata->shares, share_value);
        }*/
    }

  send_to_char("{cCONDITION(S): ",ch);
  if ( !IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK]   > 10 )
    send_to_char( "{WDRUNK ",   ch );
  if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] ==  0 )
    send_to_char( "{WTHIRSTY ", ch );
  if ( !IS_NPC(ch) && ch->pcdata->condition[COND_HUNGER]   ==  0 )
    send_to_char( "{WHUNGRY{x",  ch );
  if ( !IS_NPC(ch) && ch->pcdata->condition[COND_TIRED] == 0 )
	  send_to_char("{WDEAD TIRED{x", ch);
  send_to_char(".\n\r",ch);

  send_to_char("{cPOSITION: ",ch);
  switch ( ch->position )
    {
    case POS_DEAD:     
      send_to_char( "{RDEAD{x!!\n\r",ch );
      break;
    case POS_MORTAL:
      send_to_char( "{WMortally wounded{x.\n\r",ch );
      break;
    case POS_INCAP:
      send_to_char( "{WIncapacitated{x.\n\r",ch );
      break;
    case POS_STUNNED:
      send_to_char( "{Wstunned{x.\n\r",ch );
      break;
    case POS_SLEEPING:
      send_to_char( "{WSleeping{x.\n\r",ch );
      break;
    case POS_RESTING:
      send_to_char( "{WResting{x.\n\r",ch );
      break;
    case POS_SITTING:
      send_to_char( "{WSitting{x.\n\r",ch );
      break;
    case POS_STANDING:
      send_to_char( "{WStanding{x.\n\r",ch );
      break;
    case POS_FIGHTING:
      send_to_char( "{WFighting{x.\n\r",ch );
      break;
    }
  /* print AC values */
  if (ch->level >= 25)
    {	
      printf_to_char(ch,"{cArmor: pierce: {W%d  {cbash: {W%d  {cslash: {W%d  {cmagic: {W%d\n\r",
		     GET_AC(ch,AC_PIERCE),
		     GET_AC(ch,AC_BASH),
		     GET_AC(ch,AC_SLASH),
		     GET_AC(ch,AC_EXOTIC));
    } else {
      for (i = 0; i < 4; i++)
	{
	  char * temp;

	  switch(i)
	    {
	    case(AC_PIERCE):	temp = "piercing";	break;
	    case(AC_BASH):	temp = "bashing";	break;
	    case(AC_SLASH):	temp = "slashing";	break;
	    case(AC_EXOTIC):	temp = "magic";		break;
	    default:		temp = "error";		break;
	    }
	
	  send_to_char("You are ", ch);

	  if      (GET_AC(ch,i) >=  101 ) 
	    sprintf(buf,"hopelessly vulnerable to %s.\n\r",temp);
	  else if (GET_AC(ch,i) >= 80) 
	    sprintf(buf,"defenseless against %s.\n\r",temp);
	  else if (GET_AC(ch,i) >= 60)
	    sprintf(buf,"barely protected from %s.\n\r",temp);
	  else if (GET_AC(ch,i) >= 40)
	    sprintf(buf,"slightly armored against %s.\n\r",temp);
	  else if (GET_AC(ch,i) >= 20)
	    sprintf(buf,"somewhat armored against %s.\n\r",temp);
	  else if (GET_AC(ch,i) >= 0)
	    sprintf(buf,"armored against %s.\n\r",temp);
	  else if (GET_AC(ch,i) >= -20)
	    sprintf(buf,"well-armored against %s.\n\r",temp);
	  else if (GET_AC(ch,i) >= -40)
	    sprintf(buf,"very well-armored against %s.\n\r",temp);
	  else if (GET_AC(ch,i) >= -60)
	    sprintf(buf,"heavily armored against %s.\n\r",temp);
	  else if (GET_AC(ch,i) >= -80)
	    sprintf(buf,"superbly armored against %s.\n\r",temp);
	  else if (GET_AC(ch,i) >= -100)
	    sprintf(buf,"almost invulnerable to %s.\n\r",temp);
	  else
	    sprintf(buf,"divinely armored against %s.\n\r",temp);

	  send_to_char(buf,ch);
	}
    }

  /* RT wizinvis and holy light */
  if ( IS_IMMORTAL(ch))
    {
      send_to_char("{cHoly Light: ",ch);
      if (IS_SET(ch->act,PLR_HOLYLIGHT))
	send_to_char("{WON",ch);
      else
	send_to_char("{WOFF",ch);
 
      if (ch->invis_level)
	{
	  sprintf(buf, " {c Invisible: level {W%d",ch->invis_level);
	  send_to_char(buf,ch);
	}

      if (ch->incog_level)
	{
	  sprintf(buf,"  {cIncognito: level {W%d",ch->incog_level);
	  send_to_char(buf,ch);
	}
      send_to_char("\n\r",ch);
    }

    
  if ( ch->level >= 10 )
    {
      printf_to_char(ch, "{cAlignment: {W%d.  ", ch->alignment );
    }

  send_to_char( "You are ", ch );
  if ( ch->alignment >  900 ) send_to_char( "angelic.\n\r", ch );
  else if ( ch->alignment >  700 ) send_to_char( "saintly.\n\r", ch );
  else if ( ch->alignment >  350 ) send_to_char( "good.\n\r",    ch );
  else if ( ch->alignment >  100 ) send_to_char( "kind.\n\r",    ch );
  else if ( ch->alignment > -100 ) send_to_char( "neutral.\n\r", ch );
  else if ( ch->alignment > -350 ) send_to_char( "mean.\n\r",    ch );
  else if ( ch->alignment > -700 ) send_to_char( "evil.\n\r",    ch );
  else if ( ch->alignment > -900 ) send_to_char( "demonic.\n\r", ch );
  else                             send_to_char( "satanic.\n\r", ch );
  send_to_char("{Y------------------------------------------------------------------{x\n\r",ch);

  if (IS_SET(ch->comm,COMM_SHOW_AFFECTS)) {
    do_function(ch,&do_affects,"");
    send_to_char("{Y------------------------------------------------------------------{x\n\r",ch);
  }
}

void do_oldscore( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int i;
	int percent;
	    char *suf;
    int day;

    day     = ch->pcdata->birthday;

         if ( day > 4 && day <  20 ) suf = "th";
    else if ( day % 10 ==  1       ) suf = "st";
    else if ( day % 10 ==  2       ) suf = "nd";
    else if ( day % 10 ==  3       ) suf = "rd";
    else                             suf = "th";

    sprintf( buf,
	"You are %s%s, level %d, %d years old (%d hours).\n\r",
	ch->name,
	IS_NPC(ch) ? "" : ch->pcdata->title,
	ch->level, get_age(ch),
        ( ch->played + (int) (current_time - ch->logon) ) / 3600);
    send_to_char( buf, ch );

	send_to_one(ch, "You were born on the %d%s of %s %d years after the fall of the Great Evil.",
		ch->pcdata->birthday, suf, month_name[ch->pcdata->birthmonth],
		ch->pcdata->birthyear);
	if(ch->pcdata->organization != NULL)
	{
		send_to_one(ch,"You are a member of %s and hold the position of %s",
			ch->pcdata->organization->name,
			ch->pcdata->organization->rank[ch->pcdata->rank]);
	}

    if ( get_trust( ch ) != ch->level )
    {
	sprintf( buf, "You are trusted at level %d.\n\r",
	    get_trust( ch ) );
	send_to_char( buf, ch );
    }

    sprintf(buf, "Race: %s  Sex: %s  Class: %s\n\r",
	race_table[ch->race].name,
	sex_table[ch->sex].name,
 	IS_NPC(ch) ? "mobile" : class_table[ch->class].name);
    send_to_char(buf,ch);
	

    sprintf( buf,
	"You have %ld/%ld hit, %ld/%ld mana, %ld/%ld movement.\n\r",
	ch->hit,  ch->max_hit,
	ch->mana, ch->max_mana,
	ch->move, ch->max_move);
    send_to_char( buf, ch );

    sprintf( buf,
	"You have %d practices and %d training sessions.\n\r",
	ch->practice, ch->train);
    send_to_char( buf, ch );

    sprintf( buf,
	"You are carrying %d/%d items with weight %ld/%d pounds.\n\r",
	ch->carry_number, can_carry_n(ch),
	get_carry_weight(ch) / 10, can_carry_w(ch) /10 );
    send_to_char( buf, ch );

    sprintf( buf,
	"Str: %d(%d)  Int: %d(%d)  Wis: %d(%d)  Dex: %d(%d)  Con: %d(%d)\n\r",
	ch->perm_stat[STAT_STR],
	get_curr_stat(ch,STAT_STR),
	ch->perm_stat[STAT_INT],
	get_curr_stat(ch,STAT_INT),
	ch->perm_stat[STAT_WIS],
	get_curr_stat(ch,STAT_WIS),
	ch->perm_stat[STAT_DEX],
	get_curr_stat(ch,STAT_DEX),
	ch->perm_stat[STAT_CON],
	get_curr_stat(ch,STAT_CON) );
    send_to_char( buf, ch );

    sprintf( buf,
	"You have scored %d exp, and have %ld gold and %ld silver coins.\n\r",
	ch->exp,  ch->gold, ch->silver );
    send_to_char( buf, ch );

	if(IS_VAMP(ch))
		send_to_one(ch, "You have %d blood.", ch->blood);

	send_to_one(ch,"You have %d quest points.",ch->pcdata->questpoints);

    /* RT shows exp to level */
    if (!IS_NPC(ch) && ch->level < LEVEL_HERO)
    {
	 sprintf (buf, 
	"You are %d exp away from the next level.\n\r",
	((ch->level + 1) * exp_per_level(ch,ch->pcdata->points) - ch->exp));
      send_to_char( buf, ch );
     }

    sprintf( buf, "Wimpy set to %d hit points.\n\r", ch->wimpy );
    send_to_char( buf, ch );

    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK]   > 10 )
	send_to_char( "You are drunk.\n\r",   ch );
    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] ==  0 )
	send_to_char( "You are thirsty.\n\r", ch );
    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_HUNGER]   ==  0 )
	send_to_char( "You are hungry.\n\r",  ch );

    if ( ch->pcdata->condition[COND_TIRED] > 0 )
	percent = ( 100 * ch->pcdata->condition[COND_TIRED] / 48 );
    else
	percent = -1;

    if ( percent >= 90 )
	send_to_char( "You are well rested.\n\r", ch );
    else if ( percent >= 75 )
	send_to_char( "You feel rested.\n\r", ch );
    else if ( percent >= 50 )
	send_to_char( "You are a bit tired.\n\r", ch );
    else if ( percent >= 25 )
	send_to_char( "You feel fatigued.\n\r", ch );
    else if ( percent >= 7 )
	send_to_char( "You are very tired.\n\r", ch );
    else
	send_to_char( "You are asleep on your feet!\n\r", ch );


    switch ( ch->position )
    {
    case POS_DEAD:     
	send_to_char( "You are DEAD!!\n\r",		ch );
	break;
    case POS_MORTAL:
	send_to_char( "You are mortally wounded.\n\r",	ch );
	break;
    case POS_INCAP:
	send_to_char( "You are incapacitated.\n\r",	ch );
	break;
    case POS_STUNNED:
	send_to_char( "You are stunned.\n\r",		ch );
	break;
    case POS_SLEEPING:
	send_to_char( "You are sleeping.\n\r",		ch );
	break;
    case POS_RESTING:
	send_to_char( "You are resting.\n\r",		ch );
	break;
    case POS_SITTING:
	send_to_char( "You are sitting.\n\r",		ch );
	break;
    case POS_STANDING:
//	send_to_char( "You are standing.\n\r",		ch );
        if(MOUNTED(ch))
	{
	    sprintf( buf, "You are riding on %s.\n\r", MOUNTED(ch)->short_descr );
	    send_to_char( buf, ch);
	}
	else
	{
	    send_to_char( "You are standing.\n\r",		ch );
	}
	break;
    case POS_FIGHTING:
	send_to_char( "You are fighting.\n\r",		ch );
	break;
    }

    if(RIDDEN(ch))
    {
	sprintf( buf, "You are ridden by %s.\n\r",
		IS_NPC(RIDDEN(ch)) ? RIDDEN(ch)->short_descr : RIDDEN(ch)->name);
	send_to_char( buf, ch);
    }

    /* print AC values */
    if (ch->level >= 25)
    {	
	sprintf( buf,"Armor: pierce: %d  bash: %d  slash: %d  magic: %d\n\r",
		 GET_AC(ch,AC_PIERCE),
		 GET_AC(ch,AC_BASH),
		 GET_AC(ch,AC_SLASH),
		 GET_AC(ch,AC_EXOTIC));
    	send_to_char(buf,ch);
    }
	else
	{
    for (i = 0; i < 4; i++)
    {
	char * temp;

	switch(i)
	{
	    case(AC_PIERCE):	temp = "piercing";	break;
	    case(AC_BASH):	temp = "bashing";	break;
	    case(AC_SLASH):	temp = "slashing";	break;
	    case(AC_EXOTIC):	temp = "magic";		break;
	    default:		temp = "error";		break;
	}
	
	send_to_char("You are ", ch);

	if      (GET_AC(ch,i) >=  101 ) 
	    sprintf(buf,"hopelessly vulnerable to %s.\n\r",temp);
	else if (GET_AC(ch,i) >= 80) 
	    sprintf(buf,"defenseless against %s.\n\r",temp);
	else if (GET_AC(ch,i) >= 60)
	    sprintf(buf,"barely protected from %s.\n\r",temp);
	else if (GET_AC(ch,i) >= 40)
	    sprintf(buf,"slightly armored against %s.\n\r",temp);
	else if (GET_AC(ch,i) >= 20)
	    sprintf(buf,"somewhat armored against %s.\n\r",temp);
	else if (GET_AC(ch,i) >= 0)
	    sprintf(buf,"armored against %s.\n\r",temp);
	else if (GET_AC(ch,i) >= -20)
	    sprintf(buf,"well-armored against %s.\n\r",temp);
	else if (GET_AC(ch,i) >= -40)
	    sprintf(buf,"very well-armored against %s.\n\r",temp);
	else if (GET_AC(ch,i) >= -60)
	    sprintf(buf,"heavily armored against %s.\n\r",temp);
	else if (GET_AC(ch,i) >= -80)
	    sprintf(buf,"superbly armored against %s.\n\r",temp);
	else if (GET_AC(ch,i) >= -100)
	    sprintf(buf,"almost invulnerable to %s.\n\r",temp);
	else
	    sprintf(buf,"divinely armored against %s.\n\r",temp);

	send_to_char(buf,ch);
    }
	}

    if (ch->affected_by)
    {
	sprintf(buf, "You are affected by %s\n\r", 
	    affect_bit_name(ch->affected_by));
	send_to_char(buf,ch);
    }

    /* RT wizinvis and holy light */
    if ( IS_IMMORTAL(ch))
    {
      send_to_char("Holy Light: ",ch);
      if (IS_SET(ch->act,PLR_HOLYLIGHT))
        send_to_char("on",ch);
      else
        send_to_char("off",ch);
 
      if (ch->invis_level)
      {
        sprintf( buf, "  Invisible: level %d",ch->invis_level);
        send_to_char(buf,ch);
      }

      if (ch->incog_level)
      {
	sprintf(buf,"  Incognito: level %d",ch->incog_level);
	send_to_char(buf,ch);
      }
      send_to_char("\n\r",ch);
    }

    if ( ch->level >= 15 )
    {
	sprintf( buf, "Hitroll: %d  Damroll: %d.\n\r",
	    GET_HITROLL(ch), GET_DAMROLL(ch) );
	send_to_char( buf, ch );
    }
    
    if ( ch->level >= 10 )
    {
	sprintf( buf, "Alignment: %d.  ", ch->alignment );
	send_to_char( buf, ch );
    }

    send_to_char( "You are ", ch );
         if ( ch->alignment >  900 ) send_to_char( "angelic.\n\r", ch );
    else if ( ch->alignment >  700 ) send_to_char( "saintly.\n\r", ch );
    else if ( ch->alignment >  350 ) send_to_char( "good.\n\r",    ch );
    else if ( ch->alignment >  100 ) send_to_char( "kind.\n\r",    ch );
    else if ( ch->alignment > -100 ) send_to_char( "neutral.\n\r", ch );
    else if ( ch->alignment > -350 ) send_to_char( "mean.\n\r",    ch );
    else if ( ch->alignment > -700 ) send_to_char( "evil.\n\r",    ch );
    else if ( ch->alignment > -900 ) send_to_char( "demonic.\n\r", ch );
    else                             send_to_char( "satanic.\n\r", ch );

    if (IS_SET(ch->comm,COMM_SHOW_AFFECTS))
	do_function(ch, &do_affects, "");
}

void do_affects(CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA *paf, *paf_last = NULL;
    char buf[MAX_STRING_LENGTH];
    
    if ( ch->affected != NULL )
    {
	send_to_char( "You are affected by the following spells:\n\r", ch );
	for ( paf = ch->affected; paf != NULL; paf = paf->next )
	{
	    if (paf_last != NULL && paf->type == paf_last->type)
		if (ch->level >= 20)
		    sprintf( buf, "                      ");
		else
		    continue;
	    else
	    	sprintf( buf, "Spell: %-15s", skill_table[paf->type].name );

	    send_to_char( buf, ch );

	    if ( ch->level >= 20 )
	    {
		sprintf( buf,
		    ": modifies %s by %d ",
		    affect_loc_name( paf->location ),
		    paf->modifier);
		send_to_char( buf, ch );
		if ( paf->duration == -1 )
		    sprintf( buf, "permanently" );
		else
		    sprintf( buf, "for %d hours", paf->duration );
		send_to_char( buf, ch );
	    }

	    send_to_char( "\n\r", ch );
	    paf_last = paf;
	}
    }
    else 
	send_to_char("You are not affected by any spells.\n\r",ch);

    return;
}



char *	day_name	[] =
{
    "Septarias", "Lunarias", "Solarias", "Mendhar", "Loch",
    "Nolwe", "Dumnar"
};

char *	month_name	[] =
{
    "Den'far", "Mulden", "Sha'uri", "Len'fun",
    "Draczjou", "Alurien", "Dinarian", "Leshar",
	"Mundi'in", "Holus", "Mu'unrah", "Zeffin"
};

void do_time( CHAR_DATA *ch, char *argument )
{
    extern char str_boot_time[];
    char buf[MAX_STRING_LENGTH];
    char *suf;
    int day;

    day     = time_info.day + 1;

         if ( day > 4 && day <  20 ) suf = "th";
    else if ( day % 10 ==  1       ) suf = "st";
    else if ( day % 10 ==  2       ) suf = "nd";
    else if ( day % 10 ==  3       ) suf = "rd";
    else                             suf = "th";

    sprintf( buf,
	"\n\rIt is %d o'clock %s, %s the %d%s of %s.\n\r"
	"%d years after the fall of the Great Evil\n\r\n\r",
	(time_info.hour % 12 == 0) ? 12 : time_info.hour %12,
	time_info.hour >= 12 ? "pm" : "am",
	day_name[day % 7],
	day, suf,
	month_name[time_info.month],
	time_info.year);
    send_to_char(buf,ch);
    sprintf(buf,"Shattered Reality started up at %s\n\rThe system time is %s\n\r",
	str_boot_time,
	(char *) ctime( &current_time )
	);

    send_to_char( buf, ch );
    return;
}

void do_help( CHAR_DATA *ch, char *argument )
{
    HELP_DATA *pHelp;
    BUFFER *output;
    bool found = FALSE;
    char argall[MAX_INPUT_LENGTH],argone[MAX_INPUT_LENGTH];
    int level;

    output = new_buf();

    if ( argument[0] == '\0' )
	argument = "summary";

    /* this parts handles help a b so that it returns help 'a b' */
    argall[0] = '\0';
    while (argument[0] != '\0' )
    {
	argument = one_argument(argument,argone);
	if (argall[0] != '\0')
	    strcat(argall," ");
	strcat(argall,argone);
    }


    

	
	for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next )
    {
    	level = (pHelp->level < 0) ? -1 * pHelp->level - 1 : pHelp->level;

        if ( pHelp->level > get_trust( ch )
        && ( ( pHelp->level < LEVEL_IMMORTAL
        && !is_granted_name(ch,pHelp->keyword) )
        || (pHelp->level >= LEVEL_IMMORTAL
        && !is_command( pHelp->keyword ) ) ) )
            continue;
      if (pHelp->level >= LEVEL_IMMORTAL && is_command( pHelp->keyword )
      && !is_granted_name(ch,pHelp->keyword)) continue;
		
//		if (level > get_trust( ch ) )
//	    continue;

	if ( is_name( argall, pHelp->keyword ) )
	{
	    /* add seperator if found */
	    if (found)
		add_buf(output,
    "\n\r{W============================================================{x\n\r\n\r");
//    Someone edited my helps and put all the 0's to -1 so.... :-)
//	    if ( pHelp->level >= 0 && str_cmp( argall, "imotd" ) ) 
	    if ( pHelp->level >= -1 && str_cmp( argall, "imotd" ) )
	    {
		add_buf(output,pHelp->keyword);
		add_buf(output,"\n\r");
	    }

	    /*
	     * Strip leading '.' to allow initial blanks.
	     */
	    if ( strlen( argall ) == 1 )   
	        ;     // DJR HACK - display only keywords 	       	    
	    else if ( pHelp->text[0] == '.' )
		add_buf(output,pHelp->text+1);
	    else
		add_buf(output,pHelp->text);
	    found = TRUE;
	    /* small hack :) */
	    if (ch->desc != NULL && ch->desc->connected != CON_PLAYING 
	    &&  		    ch->desc->connected != CON_GEN_GROUPS)
		break;
	}
    }

    if (!found)
    	send_to_char( "No help on that word.\n\r", ch );
    else
	page_to_char(buf_string(output),ch);
    free_buf(output);
}

/*void do_help( CHAR_DATA *ch, char *argument )
{
    HELP_DATA *pHelp;
    BUFFER *output;
    bool found = FALSE;
    char argall[MAX_INPUT_LENGTH],argone[MAX_INPUT_LENGTH];
    int level;

    output = new_buf();

    if ( argument[0] == '\0' )
	argument = "summary";
*/
    /* this parts handles help a b so that it returns help 'a b' */
  /*  argall[0] = '\0';
    while (argument[0] != '\0' )
    {
	argument = one_argument(argument,argone);
	if (argall[0] != '\0')
	    strcat(argall," ");
	strcat(argall,argone);
    }

    for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next )
    {
    	level = (pHelp->level < 0) ? -1 * pHelp->level - 1 : pHelp->level;

        if ( pHelp->level > get_trust( ch )
        && ( ( pHelp->level < LEVEL_IMMORTAL
        && !is_granted_name(ch,pHelp->keyword) )
        || (pHelp->level >= LEVEL_IMMORTAL
        && !is_command( pHelp->keyword ) ) ) )
            continue;

      if (pHelp->level >= LEVEL_IMMORTAL && is_command( pHelp->keyword )
      && !is_granted_name(ch,pHelp->keyword)) continue;

	if ( is_name( argall, pHelp->keyword ) )
	{*/
	    /* add seperator if found */
	   /* if (found)
		add_buf(output,
    "\n\r============================================================\n\r\n\r");
	    if ( pHelp->level >= 0 && str_cmp( argall, "imotd" ) )
	    {
		add_buf(output,pHelp->keyword);
		add_buf(output,"\n\r");
	    }*/

	    /*
	     * Strip leading '.' to allow initial blanks.
	     */
	   /* if ( pHelp->text[0] == '.' )
		add_buf(output,pHelp->text+1);
	    else
		add_buf(output,pHelp->text);
	    found = TRUE;*/
	    /* small hack :) */
	/*    if (ch->desc != NULL && ch->desc->connected != CON_PLAYING 
	    &&  		    ch->desc->connected != CON_GEN_GROUPS)
		break;
	}
    }

    if (!found)
    	send_to_char( "No help on that word.\n\r", ch );
    else
	page_to_char(buf_string(output),ch);
    free_buf(output);
}

*/

/* whois command */
void do_whois (CHAR_DATA *ch, char *argument)
{
    FILE *fp;
	char strsave[MIL];
    char *buf = "";
    CHAR_DATA *vch;
    bool found_name = FALSE;
	bool found_race = FALSE;
	bool found_sex = FALSE;
	bool found_title = FALSE;
	bool found_pretit = FALSE;
	bool finished = FALSE;
	char *buf2 = "";
	int buf3 = -1;
	char *buf4 = "";
	int buf5 = -1;
  
    if (argument[0] == '\0')
    {
	send_to_char("You must provide a name.\n\r",ch);
	return;
    }

  	if( (( vch = get_char_world(ch,argument) ) != NULL) && (!IS_NPC(vch)))
	{
	
	send_to_one(ch, "%s%s%s:\n\r"
					"Sex: [%s] Race: [%s]", vch->pcdata->pretit, vch->name, vch->pcdata->title, sex_table[vch->sex].name,
					race_table[vch->race].name);
	return;
	}
	sprintf(strsave,"%s%s", PLAYER_DIR, capitalize(argument));
	if ((fp = fopen(strsave,"r"))== NULL)
	{
		send_to_one(ch,"That person does not exist.");
		return;
	}

	for ( ;; )
	{
		char *word;
		fread_to_eol(fp);
		word = feof(fp) ? "End" : fread_word(fp);
		switch (UPPER(word[0]))
		{
		case 'E':
			if(!str_cmp(word,"End"))
			{
				finished = TRUE;
				break;
			}
		case 'N':
			if(!str_cmp(word,"Name"))
			{
				buf = fread_string(fp);
				found_name = TRUE;
			}
			break;
		case 'P':
			if(!str_cmp(word,"Pretit"))
			{
				buf2 = fread_string(fp);
				found_pretit = TRUE;
			}
			break;
		case 'R':
			if(!str_cmp(word,"Race"))
			{
				buf3 = race_lookup(fread_string(fp));
				found_race = TRUE;
			}
			break;
		case 'T':
			if(!str_cmp(word,"Titl"))
			{
				buf4 = fread_string(fp);
				found_title = TRUE;
			}
			if(!str_cmp(word,"TSex"))
			{
				buf5 = fread_number(fp);
				found_sex = TRUE;
			}
			break;
		case '#':
			finished = TRUE;
			break;
		default:
			fread_to_eol(fp);
			break;
		}
		if((finished) || (found_name && found_sex && found_pretit && found_race && found_title))
			break;

	}
	send_to_one(ch,"%s%s%s:\n\r"
				   "Sex: [%s] Race: [%s]",buf2, buf, buf4, sex_table[buf5].name, race_table[buf3].name);
	fclose(fp);
	return;
}



/*
 * New 'who' command originally by Alander of Rivers of Mud.
 */
void do_who( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
 //   char buf2[MAX_STRING_LENGTH];
    BUFFER *output;
    DESCRIPTOR_DATA *d;
	int wlevel;
    int iClass;
    int iRace;
//    int iClan;
    int iLevelLower;
    int iLevelUpper;
 //   int nNumber;
    int nMatch;
    bool rgfClass[MAX_CLASS];
    bool rgfRace[MAX_PC_RACE];
    bool fClassRestrict = FALSE;
 //   bool fClanRestrict = FALSE;
 //   bool fClan = FALSE;
    bool fRaceRestrict = FALSE;
    bool fImmortalOnly = FALSE;
//	char rem[MIL];
//	int nRem;
//	char mar[MIL];

    /*
     * Set default arguments.
     */
    iLevelLower    = 0;
    iLevelUpper    = MAX_LEVEL;
    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
        rgfClass[iClass] = FALSE;
    for ( iRace = 0; iRace < MAX_PC_RACE; iRace++ )
        rgfRace[iRace] = FALSE;
 
    /*
     * Parse arguments.
     */
 
 
    /*
     * Now show matching chars.
     */
    nMatch = 0;
    buf[0] = '\0';
    output = new_buf();

	for ( wlevel = MAX_LEVEL; wlevel>0; wlevel--)
	{
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        CHAR_DATA *wch;
//        char const *class;
 
        /*
         * Check for match against restrictions.
         * Don't use trust as that exposes trusted mortals.
         */
        if ( d->connected != CON_PLAYING || !can_see( ch, d->character ) )
            continue;
 
        wch   = ( d->original != NULL ) ? d->original : d->character;

		if( wch->level != wlevel)
			continue;

	if (!can_see(ch,wch))
	    continue;

        if ( wch->level < iLevelLower
        ||   wch->level > iLevelUpper
        || ( fImmortalOnly  && wch->level < LEVEL_IMMORTAL )
        || ( fClassRestrict && !rgfClass[wch->class] )
        || ( fRaceRestrict && !rgfRace[wch->race]))
            continue;
 
        nMatch++;
 

	/*
	 * Format it up.
	 */
	sprintf( buf, "\n\r%s%s%s%s%s%s%s%s",
	    wch->incog_level >= LEVEL_HERO ? "(Incog) " : "",
	    wch->invis_level >= LEVEL_HERO ? "(Wizi) " : "",
	    IS_SET(wch->comm, COMM_AFK) ? "[AFK] " : "",
            IS_SET(wch->act, PLR_KILLER) ? "(KILLER) " : "",
            IS_SET(wch->act, PLR_THIEF)  ? "(THIEF) "  : "",
	    wch->pcdata->pretit, wch->name,
	    IS_NPC(wch) ? "" : wch->pcdata->title );
	add_buf(output,buf);
    }
	}

    page_to_char( buf_string(output), ch );
    free_buf(output);
	do_count(ch, "");
    return;
}

void do_count ( CHAR_DATA *ch, char *argument )
{
    int count;
    DESCRIPTOR_DATA *d;
    char buf[MAX_STRING_LENGTH];

    count = 0;

    for ( d = descriptor_list; d != NULL; d = d->next )
        if ( d->connected == CON_PLAYING && can_see( ch, d->character ) )
	    count++;

    max_on = UMAX(count,max_on);

    if (max_on == count)
        sprintf(buf,"\n\r\n\rThere are %d characters on, the most so far today.\n\r",
	    count);
    else
	sprintf(buf,"\n\r\n\rThere are %d characters on, the most on today was %d.\n\r",
	    count,max_on);

    send_to_char(buf,ch);
}

void do_inventory( CHAR_DATA *ch, char *argument )
{
    send_to_char( "You are carrying:\n\r", ch );
    show_list_to_char( ch->carrying, ch, TRUE, TRUE );
    return;
}



void do_equipment( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    int iWear;
    bool found;

    send_to_char( "You are using:\n\r", ch );
    found = FALSE;
    for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
    {
	if ( ( obj = get_eq_char( ch, iWear ) ) == NULL )
	    continue;

	send_to_char( where_name[iWear], ch );
	if ( can_see_obj( ch, obj ) )
	{
	    send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
	    send_to_char( "\n\r", ch );
	}
	else
	{
	    send_to_char( "something.\n\r", ch );
	}
	found = TRUE;
    }

    if ( !found )
	send_to_char( "Nothing.\n\r", ch );

    return;
}



void do_compare( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj1;
    OBJ_DATA *obj2;
    int value1;
    int value2;
    char *msg;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Compare what to what?\n\r", ch );
	return;
    }

    if ( ( obj1 = get_obj_carry( ch, arg1, ch ) ) == NULL )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    if (arg2[0] == '\0')
    {
	for (obj2 = ch->carrying; obj2 != NULL; obj2 = obj2->next_content)
	{
	    if (obj2->wear_loc != WEAR_NONE
	    &&  can_see_obj(ch,obj2)
	    &&  obj1->item_type == obj2->item_type
	    &&  (obj1->wear_flags & obj2->wear_flags & ~ITEM_TAKE) != 0 )
		break;
	}

	if (obj2 == NULL)
	{
	    send_to_char("You aren't wearing anything comparable.\n\r",ch);
	    return;
	}
    } 

    else if ( (obj2 = get_obj_carry(ch,arg2,ch) ) == NULL )
    {
	send_to_char("You do not have that item.\n\r",ch);
	return;
    }

    msg		= NULL;
    value1	= 0;
    value2	= 0;

    if ( obj1 == obj2 )
    {
	msg = "You compare $p to itself.  It looks about the same.";
    }
    else if ( obj1->item_type != obj2->item_type )
    {
	msg = "You can't compare $p and $P.";
    }
    else
    {
	switch ( obj1->item_type )
	{
	default:
	    msg = "You can't compare $p and $P.";
	    break;

	case ITEM_ARMOR:
	    value1 = obj1->value[0] + obj1->value[1] + obj1->value[2];
	    value2 = obj2->value[0] + obj2->value[1] + obj2->value[2];
	    break;

	case ITEM_WEAPON:
	    if (obj1->pIndexData->new_format)
		value1 = (1 + obj1->value[2]) * obj1->value[1];
	    else
	    	value1 = obj1->value[1] + obj1->value[2];

	    if (obj2->pIndexData->new_format)
		value2 = (1 + obj2->value[2]) * obj2->value[1];
	    else
	    	value2 = obj2->value[1] + obj2->value[2];
	    break;
	}
    }

    if ( msg == NULL )
    {
	     if ( value1 == value2 ) msg = "$p and $P look about the same.";
	else if ( value1  > value2 ) msg = "$p looks better than $P.";
	else                         msg = "$p looks worse than $P.";
    }

    act( msg, ch, obj1, obj2, TO_CHAR );
    return;
}



void do_credits( CHAR_DATA *ch, char *argument )
{
    do_function(ch, &do_help, "diku" );
    return;
}



void do_where( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    DESCRIPTOR_DATA *d;
    bool found;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Players near you:\n\r", ch );
	found = FALSE;
	for ( d = descriptor_list; d; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
	    && ( victim = d->character ) != NULL
	    &&   !IS_NPC(victim)
	    &&   victim->in_room != NULL
	    &&   !IS_SET(victim->in_room->room_flags,ROOM_NOWHERE)
 	    &&   (is_room_owner(ch,victim->in_room) 
	    ||    !room_is_private(victim->in_room))
	    &&   victim->in_room->area == ch->in_room->area
	    &&   can_see( ch, victim ) )
	    {
		found = TRUE;
		sprintf( buf, "%-28s %s\n\r",
		    victim->name, victim->in_room->name );
		send_to_char( buf, ch );
	    }
	}
	if ( !found )
	    send_to_char( "None\n\r", ch );
    }
    else
    {
	found = FALSE;
	for ( victim = char_list; victim != NULL; victim = victim->next )
	{
	    if ( victim->in_room != NULL
	    &&   victim->in_room->area == ch->in_room->area
	    &&   !IS_AFFECTED(victim, AFF_HIDE)
	    &&   !IS_AFFECTED(victim, AFF_SNEAK)
	    &&   can_see( ch, victim )
	    &&   is_name( arg, victim->name ) )
	    {
		found = TRUE;
		sprintf( buf, "%-28s %s\n\r",
		    PERS(victim, ch), victim->in_room->name );
		send_to_char( buf, ch );
		break;
	    }
	}
	if ( !found )
	    act( "You didn't find any $T.", ch, NULL, arg, TO_CHAR );
    }

    return;
}




void do_consider( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    char *msg;
    int diff;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Consider killing whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
    {
	send_to_char( "They're not here.\n\r", ch );
	return;
    }

    if (is_safe(ch,victim))
    {
	send_to_char("Don't even think about it.\n\r",ch);
	return;
    }

    diff = victim->level - ch->level;

         if ( diff <= -10 ) msg = "You can kill $N naked and weaponless.";
    else if ( diff <=  -5 ) msg = "$N is no match for you.";
    else if ( diff <=  -2 ) msg = "$N looks like an easy kill.";
    else if ( diff <=   1 ) msg = "The perfect match!";
    else if ( diff <=   4 ) msg = "$N says 'Do you feel lucky, punk?'.";
    else if ( diff <=   9 ) msg = "$N laughs at you mercilessly.";
    else                    msg = "Death will thank you for your gift.";

    act( msg, ch, NULL, victim, TO_CHAR );
    return;
}

void do_pretitle( CHAR_DATA *ch, char *argument )
{
   int value;

    if ( IS_NPC(ch) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( ch->pcdata->pretit == '\0' )
        ch->pcdata->pretit = "{x";

    if ( argument[0] == '\0' )
    {
		ch->pcdata->pretit = strdup("");
		send_to_one(ch,"Pretitle cleared.");
        return;
    }


    if ( strlen(argument) > 45 )
    {
        argument[45] = '{';
        argument[46] = 'x';
        argument[47] = '\0';
    }
    else
    {
        value = strlen(argument);
        argument[value] = '{';
        argument[value+1] = 'x';
        argument[value+2] = '\0';
    }

	strcat(argument," ");
    ch->pcdata->pretit = str_dup( argument );
    send_to_char("Done.\n\r",ch);
    return;

}

void crushcaps (char *string)
{
	int i;

	string[0] = UPPER(string[0]);

	i = strlen(string);
		if(string[i-1] == '.' || string[i-1] == '?' || string[i-1] == '!')
			return;
		else
		strcat(string,".");
		return;
}

void set_title( CHAR_DATA *ch, char *title )
{
    char buf[MAX_STRING_LENGTH];

    if ( IS_NPC(ch) )
    {
	bug( "Set_title: NPC.", 0 );
	return;
    }

    if ( title[0] != '.' && title[0] != ',' && title[0] != '!' && title[0] != '?' )
    {
	buf[0] = ' ';
	strcpy( buf+1, title );
    }
    else
    {
	strcpy( buf, title );
    }

    free_string( ch->pcdata->title );
    ch->pcdata->title = str_dup( buf );
    return;
}



void do_title( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
	return;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Change your title to what?\n\r", ch );
	return;
    }

    if ( strlen(argument) > 45 )
	argument[45] = '\0';

    smash_tilde( argument );
    set_title( ch, argument );
    send_to_char( "Ok.\n\r", ch );
}



void do_description( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
/*
    if ( argument[0] != '\0' )
    {
	buf[0] = '\0';
	smash_tilde( argument );

    	if (argument[0] == '-')
    	{
            int len;
            bool found = FALSE;
 
            if (ch->description == NULL || ch->description[0] == '\0')
            {
                send_to_char("No lines left to remove.\n\r",ch);
                return;
            }
	
  	    strcpy(buf,ch->description);
 
            for (len = strlen(buf); len > 0; len--)
            {
                if (buf[len] == '\r')
                {
                    if (!found) */ /* back it up */
/*                    {
                        if (len > 0)
                            len--;
                        found = TRUE;
                    }
                    else *//* found the second one */
/*                    {
                        buf[len + 1] = '\0';
			free_string(ch->description);
			ch->description = str_dup(buf);
			send_to_char( "Your description is:\n\r", ch );
			send_to_char( ch->description ? ch->description : 
			    "(None).\n\r", ch );
                        return;
                    }
                }
            }
            buf[0] = '\0';
	    free_string(ch->description);
	    ch->description = str_dup(buf);
	    send_to_char("Description cleared.\n\r",ch);
	    return;
        }
	if ( argument[0] == '+' )
	{
	    if ( ch->description != NULL )
		strcat( buf, ch->description );
	    argument++;
	    while ( isspace(*argument) )
		argument++;
	}

        if ( strlen(buf) >= 1024)
	{
	    send_to_char( "Description too long.\n\r", ch );
	    return;
	}

	strcat( buf, argument );
	strcat( buf, "\n\r" );
	free_string( ch->description );
	ch->description = str_dup( buf );
    }

    send_to_char( "Your description is:\n\r", ch );
    send_to_char( ch->description ? ch->description : "(None).\n\r", ch );
    return;*/
	string_append( ch, &ch->description);
	return;
}



void do_report( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_INPUT_LENGTH];

    sprintf( buf,
	"You say 'I have %ld/%ld hp %ld/%ld mana %ld/%ld mv %d xp.'\n\r",
	ch->hit,  ch->max_hit,
	ch->mana, ch->max_mana,
	ch->move, ch->max_move,
	ch->exp   );

    send_to_char( buf, ch );

    sprintf( buf, "$n says 'I have %ld/%ld hp %ld/%ld mana %ld/%ld mv %d xp.'",
	ch->hit,  ch->max_hit,
	ch->mana, ch->max_mana,
	ch->move, ch->max_move,
	ch->exp   );

    act( buf, ch, NULL, NULL, TO_ROOM );

    return;
}



void do_practice( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int sn;

    if ( IS_NPC(ch) )
	return;

    if ( argument[0] == '\0' )
    {
	int col;

	col    = 0;
	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if ( skill_table[sn].name == NULL )
		break;
	    if ( ch->level < skill_table[sn].skill_level[ch->class] 
	      || ch->pcdata->learned[sn] < 1 /* skill is not known */)
		continue;

	    sprintf( buf, "%-18s %3d%%  ",
		skill_table[sn].name, ch->pcdata->learned[sn] );
	    send_to_char( buf, ch );
	    if ( ++col % 3 == 0 )
		send_to_char( "\n\r", ch );
	}

	if ( col % 3 != 0 )
	    send_to_char( "\n\r", ch );

	sprintf( buf, "You have %d practice sessions left.\n\r",
	    ch->practice );
	send_to_char( buf, ch );
    }
    else
    {
	CHAR_DATA *mob;
	int adept;

	if ( !IS_AWAKE(ch) )
	{
	    send_to_char( "In your dreams, or what?\n\r", ch );
	    return;
	}

	for ( mob = ch->in_room->people; mob != NULL; mob = mob->next_in_room )
	{
	    if ( IS_NPC(mob) && IS_SET(mob->act, ACT_PRACTICE) )
		break;
	}

	if ( mob == NULL )
	{
	    send_to_char( "You can't do that here.\n\r", ch );
	    return;
	}

	if ( ch->practice <= 0 )
	{
	    send_to_char( "You have no practice sessions left.\n\r", ch );
	    return;
	}

	if ( ( sn = find_spell( ch,argument ) ) < 0
	|| ( !IS_NPC(ch)
	&&   (ch->level < skill_table[sn].skill_level[ch->class] 
 	||    ch->pcdata->learned[sn] < 1 /* skill is not known */
	||    skill_table[sn].rating[ch->class] == 0)))
	{
	    send_to_char( "You can't practice that.\n\r", ch );
	    return;
	}

	adept = IS_NPC(ch) ? 100 : class_table[ch->class].skill_adept;

	if ( ch->pcdata->learned[sn] >= adept )
	{
	    sprintf( buf, "You are already learned at %s.\n\r",
		skill_table[sn].name );
	    send_to_char( buf, ch );
	}
	else
	{
	    ch->practice--;
	    ch->pcdata->learned[sn] += 
		int_app[get_curr_stat(ch,STAT_INT)].learn / 
	        skill_table[sn].rating[ch->class];
	    if ( ch->pcdata->learned[sn] < adept )
	    {
		act( "You practice $T.",
		    ch, NULL, skill_table[sn].name, TO_CHAR );
		act( "$n practices $T.",
		    ch, NULL, skill_table[sn].name, TO_ROOM );
	    }
	    else
	    {
		ch->pcdata->learned[sn] = adept;
		act( "You are now learned at $T.",
		    ch, NULL, skill_table[sn].name, TO_CHAR );
		act( "$n is now learned at $T.",
		    ch, NULL, skill_table[sn].name, TO_ROOM );
	    }
	}
    }
    return;
}



/*
 * 'Wimpy' originally by Dionysos.
 */
void do_wimpy( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    int wimpy;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
	wimpy = ch->max_hit / 5;
    else
	wimpy = atoi( arg );

    if ( wimpy < 0 )
    {
	send_to_char( "Your courage exceeds your wisdom.\n\r", ch );
	return;
    }

    if ( wimpy > ch->max_hit/2 )
    {
	send_to_char( "Such cowardice ill becomes you.\n\r", ch );
	return;
    }

    ch->wimpy	= wimpy;
    sprintf( buf, "Wimpy set to %d hit points.\n\r", wimpy );
    send_to_char( buf, ch );
    return;
}



void do_password( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char *pArg;
    char *pwdnew;
    char *p;
    char cEnd;

    if ( IS_NPC(ch) )
	return;

    /*
     * Can't use one_argument here because it smashes case.
     * So we just steal all its code.  Bleagh.
     */
    pArg = arg1;
    while ( isspace(*argument) )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;

    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*pArg++ = *argument++;
    }
    *pArg = '\0';

    pArg = arg2;
    while ( isspace(*argument) )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;

    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*pArg++ = *argument++;
    }
    *pArg = '\0';

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Syntax: password <old> <new>.\n\r", ch );
	return;
    }

    if ( strcmp( crypt( arg1, ch->pcdata->pwd ), ch->pcdata->pwd ) )
    {
	WAIT_STATE( ch, 40 );
	send_to_char( "Wrong password.  Wait 10 seconds.\n\r", ch );
	return;
    }

    if ( strlen(arg2) < 5 )
    {
	send_to_char(
	    "New password must be at least five characters long.\n\r", ch );
	return;
    }

    /*
     * No tilde allowed because of player file format.
     */
    pwdnew = crypt( arg2, ch->name );
    for ( p = pwdnew; *p != '\0'; p++ )
    {
	if ( *p == '~' )
	{
	    send_to_char(
		"New password not acceptable, try again.\n\r", ch );
	    return;
	}
    }

    free_string( ch->pcdata->pwd );
    ch->pcdata->pwd = str_dup( pwdnew );
    save_char_obj( ch );
    send_to_char( "Ok.\n\r", ch );
    return;
}
