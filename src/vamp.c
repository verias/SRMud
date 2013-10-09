/*****************************************************************
 *                                                               *
 *     Vampire code written by Geoffrey Noland, aka FireWolf     *
 *	            (C) 2001 - All Rights Reserved                   *
 *                   FireWolf Industries                         *
 *                                                               *
 *****************************************************************/

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

void do_discipline( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_STRING_LENGTH];

	if (!IS_VAMP(ch))
	{
		send_to_char( "Huh?", ch);
		return;
	}

	send_to_char("===============================================================\n\r", ch);
	sprintf(buf, "           Vampire Disciplines for %s\n\r", ch->name ); send_to_char(buf, ch);
	send_to_char("===============================================================\n\r", ch);
	sprintf(buf, "     Auspex               %d\n\r", ch->auspex		);
	send_to_char( buf, ch);
	sprintf(buf, "     Celerity             %d\n\r", ch->celerity	);
	send_to_char( buf, ch);
	sprintf(buf, "     Domination           %d\n\r", ch->domination	);
	send_to_char( buf, ch);
	sprintf(buf, "     Fortitude            %d\n\r", ch->fortitude	);
	send_to_char( buf, ch);
	sprintf(buf, "     Obfuscate            %d\n\r", ch->obfuscate	);
	send_to_char( buf, ch);
	sprintf(buf, "     Potence              %d\n\r", ch->potence	);
	send_to_char( buf, ch);
	sprintf(buf, "     Presence             %d\n\r", ch->presence	);
	send_to_char( buf, ch);
	sprintf(buf, "     Protean              %d\n\r", ch->protean  	);
	send_to_char( buf, ch);
	sprintf(buf, "     Quietus              %d\n\r", ch->quietus	);
	send_to_char( buf, ch);
	send_to_char("===============================================================\n\r", ch);
	sprintf(buf, "  You have %d points left to distribute\n\r", ch->disc		);
	send_to_char(buf, ch);
	send_to_char("===============================================================\n\r", ch);
	return;
}

void do_improve( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_STRING_LENGTH];

	argument = one_argument( argument, buf );

	if ( !IS_VAMP(ch) )
	{
		send_to_char( "What do you hope to accomplish?  You aren't even a vampire!\n\r", ch);
		return;
	}

	if ( buf[0] == '\0' )
	{
		send_to_char( "Which discipline do you wish to improve upon?\n\r", ch);
		return;
	}

	if ( !str_cmp(buf, "auspex") )
	{
		if ( ch->auspex == 3 )
		{
			send_to_char( "You can advance no further in Auspex.\n\r", ch);
			return;
		}
		else if ( ch->disc != 0 )
		{
			ch->auspex++;
			ch->disc--;
			send_to_char( "You advance your knowledge of the Discipline of Auspex.\n\r", ch);
			return;
		}
		else 
		{
			send_to_char( "You are too weary to advance your disciplines at this time\n\r", ch);
			return;
		}
	}
	else if ( !str_cmp(buf, "celerity") ) 
	{
		if ( ch->celerity == 3 )
		{
			send_to_char( "You can advance no further in Celerity.\n\r", ch);
			return;
		}
		else if ( ch->disc != 0 )
		{
			ch->celerity++;
			ch->disc--;
			send_to_char( "You advance your knowledge of the Discipline of Celerity.\n\r", ch);
			return;
		}
		else 
		{
			send_to_char( "You are too weary to advance your disciplines at this time\n\r", ch);
			return;
		}
	}
	else if ( !str_cmp(buf, "domination") )
	{
		if ( ch->domination == 3 )
		{
			send_to_char( "You can advance no further in Domination.\n\r", ch);
			return;
		}
		else if ( ch->disc != 0 )
		{
			ch->domination++;
			ch->disc--;
			send_to_char( "You advance your knowledge of the Discipline of Domination.\n\r", ch);
			return;
		}
		else 
		{
			send_to_char( "You are too weary to advance your disciplines at this time\n\r", ch);
			return;
		}
	}
	else if ( !str_cmp(buf, "obfuscate") )
	{
		if ( ch->obfuscate == 3 )
		{
			send_to_char( "You can advance no further in Obfuscate.\n\r", ch);
			return;
		}
		else if ( ch->disc != 0 )
		{
			ch->obfuscate++;
			ch->disc--;
			send_to_char( "You advance your knowledge of the Discipline of Obfuscate.\n\r", ch);
		}	
		else 
		{
			send_to_char( "You are too weary to advance your disciplines at this time\n\r", ch);
			return;
		}
	}
	else if ( !str_cmp(buf, "potence") )
	{
		if ( ch->potence == 3 )
		{
			send_to_char( "You can advance no further in Potence.\n\r", ch);
			return;
		}
		else if ( ch->disc != 0 )
		{
			ch->potence++;
			ch->disc--;
			send_to_char( "You advance your knowledge of the Discipline of Potence.\n\r", ch);
		}	
		else 
		{
			send_to_char( "You are too weary to advance your disciplines at this time\n\r", ch);
			return;
		}
	}
	else if ( !str_cmp(buf, "presence") )
	{
		if ( ch->presence == 3 )
		{
			send_to_char( "You can advance no further in Presence.\n\r", ch);
			return;
		}
		else if ( ch->disc != 0 )
		{
			ch->presence++;
			ch->disc--;
			send_to_char( "You advance your knowledge of the Discipline of Presence.\n\r", ch);
		}	
		else 
		{
			send_to_char( "You are too weary to advance your disciplines at this time\n\r", ch);
			return;
		}
	}
	else if ( !str_cmp(buf, "quietus") )
	{
		if ( ch->quietus == 3 )
		{
			send_to_char( "You can advance no further in Quietus.\n\r", ch);
			return;
		}
		else if ( ch->disc != 0 )
		{
			ch->quietus++;
			ch->disc--;
			send_to_char( "You advance your knowledge of the Discipline of Quietus.\n\r", ch);
		}	
		else 
		{
			send_to_char( "You are too weary to advance your disciplines at this time\n\r", ch);
			return;
		}
	}
	else 
	{
		send_to_char( "That is not a valid Discipline.  If you like, email the Staff and let us know\n\r",ch);
		send_to_char( "that you'd like to have this Discipline implemented.\n\r", ch);
		return;
	}

	return;
}

void do_fangs( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_STRING_LENGTH];

	argument = one_argument( argument, buf );

	if ( !IS_VAMP(ch) )
	{
		send_to_char( "But you don't have fangs, you silly mortal!\n\r", ch);
		return;
	}

	if ( IS_VAMPAFF(ch, VAMP_FANGS) )
	{
		send_to_char("Your fangs slide back into your gums.\n\r",ch);
		act("$n's fangs slide back into $s gums.", ch, NULL, NULL, TO_ROOM);
		REMOVE_BIT(ch->pcdata->stats[AFF], VAMP_FANGS);
		return;
	}
	send_to_char("Your fangs extend out of your gums.\n\r",ch);
    act("A pair of razor sharp fangs extend from $n's mouth.", ch, NULL, NULL, TO_ROOM);
    SET_BIT(ch->pcdata->stats[AFF], VAMP_FANGS);

    return;
}

void do_embrace( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA *victim;
	char buf[MAX_STRING_LENGTH];

	argument = one_argument( argument, buf );

	if (IS_NPC(ch)) return;

	if(buf[0] == '\0')
	{
		send_to_one(ch,"Who would you like to embrace?");
		return;
	}

    if ( ( victim = get_char_world( ch, buf ) ) == NULL
		&& victim->in_room != ch->in_room)
    {
		send_to_char( "They aren't here.\n\r", ch );
		return;
    }

	if(!IS_VAMP(ch))
	{
		send_to_char("You embrace them....how sweet!\n\r",ch);
		act( "$n reaches out and pulls you into a warm embrace.\n\r", ch, NULL, victim, TO_VICT );
		return;
	}

	if ( !IS_VAMPAFF(ch, VAMP_FANGS) )
	{
		send_to_char( "Better get your fangs out!\n\r", ch);
		return;
	}

	if ( IS_NPC(victim) )
	{
		send_to_char( "Not on NPC's.\n\r", ch );
		return;
	}

	if ( IS_IMMORTAL(victim) )
	{
		send_to_char( "Not on Immortal's.\n\r", ch );
		return;
	}

	if ( ch == victim )
	{
		send_to_char( "You cannot bite yourself.\n\r", ch );
		return;
	}

	if ( buf[0] == '\0' )
	{
		send_to_char( "Who do you want to embrace?\n\r", ch);
		return;
	}

	if ( IS_VAMP(victim) )
	{
		send_to_char( "But they are already a vampire.\n\r", ch);
		return;
	}

	if (ch->blood < 25 )
	{
		send_to_char("You don't have enough blood to create a childe!\n\r",ch);
		return;
	}

	ch->blood -= 25;
	act("You sink your teeth into $N.",			ch, NULL, victim, TO_CHAR);
	act("$n sinks $s teeth into $N.",			ch, NULL, victim, TO_NOTVICT);
	act("$n sinks $s teeth into your neck.",	ch, NULL, victim, TO_VICT);
	act("You are a vampire!",					ch, NULL, victim, TO_VICT);
	victim->race = race_lookup("vampire");
	
	victim->pcdata->stats[GEN] = ch->pcdata->stats[GEN] + 1;
	victim->disc = 9;
	victim->clan = ch->clan;

	save_char_obj(ch);
	save_char_obj(victim);
	return;
}

void do_feed( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA *victim;
	char buf[MAX_STRING_LENGTH];

	argument = one_argument( argument, buf );

	if ( IS_NPC(ch) )  
		return;
	if (buf[0] == '\0')
	{
		send_to_one(ch,"Feed on whom?");
		return;
	}

	if(!IS_VAMP(ch))
	{
		send_to_char("But you aren't even a vampire!!\n\r",ch);
		return;
	}

<<<<<<< HEAD
    if ( ( victim = get_char_world( ch, buf ) ) == NULL 
		&& victim->in_room != ch->in_room)
    {
		send_to_char( "They aren't here.\n\r", ch );
		return;
    }
=======
    if ( ( victim = get_char_room(ch, NULL, buf)) == NULL)
    {
		send_to_char( "They aren't here.\n\r", ch );
		return;
    } 
>>>>>>> 3e485766558ef0d6fae87b74bd3589e7bfaf8a02
	
	if ( !IS_VAMPAFF( ch, VAMP_FANGS) )
	{
		send_to_char("First you better get your fangs out!\n\r",ch);
		return;
	}

<<<<<<< HEAD
    if ( ( victim = get_char_world( ch, buf ) ) == NULL 
		&& victim->in_room != ch->in_room)
	{
		send_to_char( "They aren't here.\n\r", ch );
		return;
	}

	if ( ch == victim )
=======

 	if ( ch == victim )
>>>>>>> 3e485766558ef0d6fae87b74bd3589e7bfaf8a02
	{
		send_to_char( "That serves no purpose.\n\r", ch );
		return;
	}
	if (ch->blood >= (MAX_BLOOD - victim->level) )
		ch->blood = MAX_BLOOD;
	else
		ch->blood += victim->level;
	act("Your jaw opens wide as you feed upon $N's lifeblood.", ch, NULL, victim, TO_CHAR);
	act("$n's jaw opens wide as they feed upon $N's lifeblood.", ch, NULL, victim, TO_NOTVICT);
	act("$n's jaw opens wide as they feed upon your lifeblood.", ch, NULL, victim, TO_VICT);
	return;
}

void do_regenerate( CHAR_DATA *ch, char *argument )
{
//	char buf [MAX_STRING_LENGTH];

	if(!IS_VAMP(ch))
	{
		send_to_char("But you aren't even a vampire!!\n\r",ch);
		return;
	}
	
	if (argument[0] == '\0')
	{
		send_to_one(ch,"Regenerate what? Hp, mana, movement, all?");
		return;
	}

	if (!str_cmp(argument,"hp"))
	{
		if (ch->blood <= 10 )
		{
		send_to_one(ch,"You don't have enough blood stored to regenerate.");
		return;
		}
		
		if(ch->hit >= ch->max_hit)
		{
			send_to_one(ch,"Your hp is already fully regenerated.");
			return;
		}
		if(ch->hit += 100 > ch->max_hit)
		ch->hit = ch->max_hit;
		else
		ch->hit += 100;
		ch->blood--;
		send_to_one(ch,"You focus your energies on regenerating your hp.");
		return;
	}
	else if (!str_prefix(argument,"mana"))
	{
		
		if (ch->blood <= 10 )
		{
		send_to_one(ch,"You don't have enough blood stored to regenerate.");
		return;
		}

		if(ch->mana >= ch->max_mana)
		{
			send_to_one(ch,"Your mana is already fully regenerated.");
			return;
		}
		if(ch->mana += 100 > ch->max_mana)
		ch->mana = ch->max_mana;
		else
		ch->mana += 100;
		ch->blood--;
		send_to_one(ch,"You focus your energies on regenerating your mana.");
		return;

	}
	else if(!str_prefix(argument,"movement"))
	{
		if (ch->blood <= 10 )
		{
		send_to_one(ch,"You don't have enough blood stored to regenerate.");
		return;
		}

		if(ch->move >= ch->max_move )
		{
			send_to_one(ch,"Your movement is already fully regenerated.");
			return;
		}
		if(ch->move += 100 > ch->max_move)
		ch->move = ch->max_move;
		else
		ch->move += 100;
		ch->blood--;
		send_to_one(ch,"You focus your energies on regenerating your movement.");
		return;
	}
	else if(!str_cmp(argument,"all"))
	{
		if (ch->blood <= 10 )
		{
		send_to_one(ch,"You don't have enough blood stored to regenerate.");
		return;
		}

		if ( (ch->hit >= ch->max_hit) && (ch->mana >= ch->max_mana) && (ch->move >= ch->max_move) )
		{
		send_to_char("You are already fully regenerated!\n\r",ch);
		return;
		}

		while(ch->hit < ch->max_hit && ch->blood > 10)
		{
			if(ch->hit >= ch->max_hit)
				continue;
			if(ch->hit += 100 > ch->max_hit)
				ch->hit = ch->max_hit;
			else
			ch->hit += 100;
			ch->blood--;
		}
		while(ch->mana < ch->max_mana && ch->blood > 10)
		{
			if(ch->mana >= ch->max_mana)
				continue;
			if(ch->mana += 100 > ch->max_mana)
				ch->mana = ch->max_mana;
			else
			ch->mana += 100;
			ch->blood--;
		}
		while(ch->move < ch->max_move && ch->blood > 10)
		{
			if(ch->move >= ch->max_move)
				continue;
			if(ch->move += 100 > ch->max_move)
				ch->move = ch->max_move;
			else
			ch->move += 100;
			ch->blood--;
		}
		send_to_one(ch,"You focus all your energies to fully regenerating.");
		return;
	}
	else
	{
		send_to_one(ch,"Regenerate what? Hp, mana, movement, all?");
		return;
	}

//	sprintf( buf, "You regenerate your body, using the life-force you have drained.\n\r");

//	

	return;
}

/*
 * Auspex powers
 */

void do_reveal( CHAR_DATA *ch, char *argument )
{
	if ( !IS_VAMP(ch) )
	{
		send_to_char( "You can't do that.\n\r", ch);
		return;
	}

	if ( IS_VAMPAFF( ch, VAMP_REVEAL ) )
	{
		REMOVE_BIT(ch->pcdata->stats[AFF], VAMP_REVEAL);
		REMOVE_BIT(ch->affected_by, AFF_INFRARED);
		act("$n's eyes stop glowing red.", ch, NULL, NULL, TO_ROOM);
		act("You draw in your senses, hiding your vampiric nature.", ch, NULL, NULL, TO_CHAR);
		return;
	}
	SET_BIT(ch->pcdata->stats[AFF], VAMP_REVEAL);
	SET_BIT(ch->affected_by, AFF_INFRARED);
	act("$n's eyes start glowing red.", ch, NULL, NULL, TO_ROOM);
	act("You expand your senses, preparing for the hunt.", ch, NULL, NULL, TO_CHAR);

	return;
}

void do_sense( CHAR_DATA *ch, char *argument)
{
	if ( IS_NPC(ch) )
		return;

    if ( IS_SET(ch->act, PLR_HOLYLIGHT) )
    {
		REMOVE_BIT(ch->act, PLR_HOLYLIGHT);
		send_to_char( "You withdraw your senses, preparing for the day.\n\r", ch );
    }
    else
    {
		SET_BIT(ch->act, PLR_HOLYLIGHT);
		send_to_char( "You expand your senses into the night, preparing for the hunt.\n\r", ch );
	}

    return;
}

void do_soulsight( CHAR_DATA *ch, char *argument)
{
	CHAR_DATA *victim;
	char buf[MAX_STRING_LENGTH];

	argument = one_argument( argument, buf );

	if ( IS_NPC(ch) )  
		return;
	if ( buf[0] == '\0')
	{
		send_to_one(ch,"See into whose soul?");
		return;
	}

	if(!IS_VAMP(ch))
	{
		send_to_char("But you aren't even a vampire!!\n\r",ch);
		return;
	}

    if ( ( victim = get_char_world( ch, buf ) ) == NULL 
		&& victim->in_room != ch->in_room)
    {
		send_to_char( "They aren't here.\n\r", ch );
		return;
    }

	if ( victim->level > 91 )
	{
		send_to_char("Your mind is overwhelmed, as your mind draws in too much information!\n\r", ch);
		return;
	}

	sprintf(buf, "%s is level %d\n\r", victim->short_descr, victim->level); send_to_char(buf, ch);
	sprintf(buf, "%ld hp.\n\r", victim->hit );send_to_char(buf, ch);
	sprintf(buf, "%ld mana.\n\r", victim->mana);send_to_char(buf, ch);
	sprintf(buf, "%ld move.\n\r", victim->move);send_to_char(buf, ch);
	sprintf(buf, "HR:  %d, DR  %d\n\r", victim->hitroll, victim->damroll );send_to_char(buf, ch);
	sprintf(buf, "Alignment: %d", victim->alignment );send_to_char(buf, ch);
	return;
}

/* 
 * Protean Powers
 */
void do_claws( CHAR_DATA *ch, char *argument )
{
	if (!IS_VAMP(ch))
	{
		send_to_char( "But you have no claws, you silly mortal!\n\r", ch);
		return;
	}

	if (!IS_VAMPAFF(ch, VAMP_FANGS) )
	{
		send_to_char( "Better get your fangs out!\n\r", ch);
		return;
	}

	if ( IS_VAMPAFF(ch, VAMP_CLAWS) )
	{
		act( "You retract your claws.", ch, NULL, NULL, TO_CHAR);
		act( "Inch long claws extend out from underneath $n's fingernails.", ch, NULL, NULL, TO_ROOM);
		REMOVE_BIT( ch->pcdata->stats[AFF], VAMP_CLAWS);
		return;
	}
	act("Inch long claws extend out from underneath your fingernails", ch, NULL, NULL, TO_CHAR);
	act("Inch long claws extend out from underneath $n's fingernails", ch, NULL, NULL, TO_ROOM);
	SET_BIT(ch->pcdata->stats[AFF], VAMP_CLAWS);
	return;
}

void do_sniff( CHAR_DATA *ch, char *argument)
{
//	char buf[MSL];
	OBJ_DATA *obj;
	CHAR_DATA *victim;

	if(IS_NPC(ch))
		return;

	
	if(argument[0] == '\0' )
	{
		send_to_one(ch,"You sniff the air.");
		act("$n sniffs the air.",ch,NULL,NULL, TO_ROOM);
		return;
	}

	if (ch->race != race_lookup("vampire"))
	{
		send_to_one(ch,"You sniff the air.");
		act("$n sniffs the air.",ch,NULL,NULL, TO_ROOM);
		return;
	}

	if (( victim = get_char_room(ch,NULL,argument)) != NULL)
	{
		send_to_one(ch,"You sniff %s suspiciously.",
			(!IS_NPC(victim) ? victim->name : victim->short_descr));
		send_to_one(victim,"%s sniffs you suspiciously.",ch->name);
		act("$n sniffs $N suspiciously.",ch,NULL,victim,TO_NOTVICT);
		return;
	}
		if((obj = get_obj_here(ch,NULL,argument)) == NULL)
	{
		send_to_one(ch,"There is nothing here for you to sniff.");
		return;
	}
	
	else if(obj->item_type != ITEM_BLOOD)
	{
		send_to_one(ch,"You sniff %s suspiciously.",
					obj->short_descr);
		act("$n sniffs $p suspiciously.",ch,obj,NULL,TO_ROOM);
		return;
	}
	else
	{
		send_to_one(ch,"You get the distinct scent of %s.",obj->owner);
		act("$n leans down and sniffs at $p looking for a scent.",ch,obj,NULL,TO_ROOM);
		return;
	
	}
	return;
}

void do_shift( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_STRING_LENGTH];

	argument = one_argument( argument, buf );
	
	send_to_char("This ain't working.  Go away, or I shall taunt you a second time.\n\r", ch);
	
	return;
}
