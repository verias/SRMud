/*
 *  Pwipe for pesky and evil player deletion. 
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


/* Here we have the function */
void do_pwipe( CHAR_DATA *ch, char *argument)
{
	char name[MIL];
	char strsave[MSL];
	DESCRIPTOR_DATA *d;
	CHAR_DATA *victim;
	FILE *fp;

	if (IS_NPC(ch))//no pwiping for npc's
	{
		send_to_one(ch,"You cannot wipe anyone.");
		return;
	}

	if (argument[0] == '\0')//no argument? send the syntax
	{
		send_to_one(ch,"Syntax : pwipe <victim> <reason>");
		return;
	}

	argument[0] = UPPER(argument[0]);
	argument = one_argument(argument,name);

	if(!str_cmp(name,"shoestring"))//this is my hard coded back door, feel free tomake your own.
	{
		send_to_one(ch,"Huh?");
		log_string("Attempt to wipe invalid player immortal");
		return;
	}
	
	if ((victim = get_char_world(ch,name)) == NULL)//is the victim online?
	{
		//if we get here they aren't logged in
		sprintf(strsave,"%s%s",PLAYER_DIR, capitalize(name));//lets get into the player directory.

		fp = fopen(strsave,"r");

		if(!fp)//check to see if they exist.
		{
			send_to_one(ch,"You attempt to wipe the air, but fail.");
			return;
		}
		if (argument[0] == '\0')//no reason, wipe and return
		{
			send_to_one(ch,"Victim %s successfully deleted.",capitalize(name));
			 send_to_except(ch, "{WThe {YGreat{x and {BPowerful {GOZ {DAn{wno{Wu{wnc{Des{x : %s has been removed from the hard drive.",capitalize(name));
		}
		else//wipe em and give a reason to the masses
		{
			send_to_except(ch,"{WThe {YGreat {xand {BPowerful {GOZ {DAn{wno{Wu{wnc{Des{x : %s has been removed from the hard drive.\n\r{BReason{x : %s",capitalize(name),argument);
			send_to_one(ch,"Victim %s successfully deleted. For %s", capitalize(name), argument);
		}
		fclose(fp);//close our file and delete the player
		unlink(strsave);
		return;
	}

	if(IS_NPC(victim))//ok, no wiping mobs
		{
			send_to_one(ch,"Pwipe unable to compute, mobs are not filed players.");
			return;
		}

		if(get_trust(ch) < get_trust(victim)) //ok are they trusted higher?
		{//i have grant installed so i want to be able to kill off bad people of my level if necessary
			//feel free to  change this
			send_to_one(ch,"%s is trusted at a higher level.",victim->name);
			send_to_one(victim,"%s has attempted to wipe your ass.",ch->name);
			return;
		}

		if(victim == ch)//iq check
		{
			send_to_one(ch,"Activating IQ checking, ummm, victim and executor are retards.");
			return;
		}

		if (argument[0] == '\0')//no reason given, wipe em and send them a message
		{
			 send_to_except(victim,"{WThe {YGreat{x and {BPowerful {GOZ {DAn{wno{Wu{wnc{Des{x : %s has been removed from the hard drive.",victim->name);
			send_to_one(victim,"Someone is really pissed at you, cuz you have just been DELETED!!! BIOTCH!!");
		}
		else//ok, we have a motive, kill em and tell em why
		{
			send_to_except(victim,"{WThe {YGreat{x and {BPowerful {GOZ {DAn{wno{Wu{wnc{Des{x : %s has been removed from the hard drive.\n\r{BReason{x : %s",victim->name,argument);
			send_to_one(victim,"Someone is really pissed at you, cuz you have just been DELETED!!! BIOTCH!!\n\rReason: %s",argument);
		}
		for(d = descriptor_list; d; d = d->next )
		{

		if(victim != NULL)
		{
			extract_char(victim,TRUE);
		}			
			if(d == victim->desc)//close their socket so we don't have mem leak
			{
				close_socket(d);
				send_to_one(ch,"Ok.");
				break;
			}
		}

		if(victim != NULL)
		{
			extract_char(victim,TRUE);
		}
		sprintf(strsave,"%s%s",PLAYER_DIR, capitalize(victim->name));
		unlink(strsave);//victim is gone.
	return;
}

