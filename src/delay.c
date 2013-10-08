/***************************************************************************
 * You may use this code as you see fit within any non-commercial MUD.     *
 * My only conditions are that my comments remain in tact, that you do not *
 * hesitate to give me feedback on the code, and that you don't claim my   *
 * work as your own.  Enjoy.        -- Midboss (eclipsing.souls@gmail.com) *
 * *********************************************************************** *
 * 'Delay' command to execute commands at a later time.  It will let you   *
 * delay a command for five seconds to thirty minutes, list your delayed   *
 * actions, or cancel a delayed action.  Also useful for mob progs, but if *
 * you intend to use them for that, you should add some other kind of ID,  *
 * perhaps a keyword, to delays, and a seperate command for mobs that will *
 * make use of those keywords.  Otherwise it's a little unreliable.  Also, *
 * it will optionally log all delayed actions as they fire.                *
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merc.h"

//Uncomment this if players can use it.
#define DELAY_MOB

//Uncomment this if you want it to log players' delayed actions.
//#define DELAY_LOG


/*
 * Update all delayed actions.
 */
void delay_update (void)
{
    static int pulse_delay;
	CHAR_DATA * ch, * ch_next;
	DELAY * delay, * del_next;
#ifdef DELAY_LOG
	bool unset = FALSE;
#endif

	//Subtract from the pulse timer...
	if (--pulse_delay <= 0)
    {
		//Reset the pulse timer.
        pulse_delay = PULSE_PER_SECOND;

		/*
		 * We have to loop through the entire char_list.  I know, I know,
		 * it's a painful prospect, but this is mostly for mobprogs.
		 */
		for (ch = char_list; ch != NULL; ch = ch_next)
		{
			/*
			 * Player using delayed quit?  I'm a bit worried about ch->next
			 * dying as a result of this command...
			 */
			ch_next = ch->next;

			//Skip this character if there's not an action.
			if (ch->actions == NULL)
				continue;


			//Loop through the actions...
			for (delay = ch->actions; delay != NULL;
				 delay = del_next)
			{
				del_next = delay->next;

				//Decrement the timer.
				--delay->timer;

				//Time to go off?
				if (delay->timer <= 0)
				{
					//Check for logging.
#ifdef DELAY_LOG
					if (!IS_NPC (ch) && !IS_SET (ch->act, PLR_LOG))
					{
						unset = TRUE;
						SET_BIT (ch->act, PLR_LOG);
					}
#endif
					//Interpret.
					interpret (ch, delay->cmd);

#ifdef DELAY_LOG
					if (!IS_NPC (ch) && unset)
						REMOVE_BIT (ch->act, PLR_LOG);
#endif

					//Make sure ch is still here.
					if (ch == NULL)
						break;

					//Remove the delay from the list.
					if (delay == ch->actions)
						ch->actions = NULL;
					else
					{
						DELAY * prev;

						for (prev = ch->actions; prev != NULL;
							 prev = prev->next)
						{
							if (prev->next == delay)
							{
								prev->next = delay->next;
								break;
							}
						}
					}

					//Free the delay.
					free_delay (delay);
				}
			}

			/*
			 * If we still have ch, make sure there's a next ch.  This
			 * should hopefully protect against delayed spells killing
			 * characters and breaking the loop...
			 */
			if (ch)
				ch_next = ch->next;
		}


    }
}

/*
 * Enqueues a delayed command to be executed later.  Now also lists all
 * delayed actions, or cancels a delayed action.
 */
void do_delay (CHAR_DATA * ch, char * argument)
{
	DELAY * delay = NULL;
	char arg[MIL];
	int time;

/*
 * Is this being used solely for mob progs?
 */
#ifdef DELAY_MOB
	if (!IS_NPC(ch))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}
#endif

	argument = one_argument (argument, arg);

	/*
	 * Lists all actions.
	 */
/*	if (!str_prefix (arg, "list"))
	{
		char buf[MSL];
		int cnt = 0;

		buf[0] = '\0';

		//Go through the list...
		for (delay = ch->actions; delay != NULL; delay = delay->next)
		{
			//Increment the counter.
			++cnt;

			//Add the data.
			sprintf (buf + strlen (buf), "%2d) '%s' in %d seconds.\n\r",
				cnt, delay->cmd, delay->timer);
		}

		//Now send it.
		if (cnt <= 0)
			send_to_char ("You have no delayed actions.\n\r", ch);
		else
			send_to_char (buf, ch);

		return;
	}
	*/
	/*
	 * Cancels an action.
	 */
/*	else if (!str_prefix (arg, "cancel"))
	{
		DELAY * list;
		int which, i = 0;

		//We have an argument, right?
		if (argument[0] == '\0' || !is_number (argument))
		{
			send_to_char ("Cancel which delayed action?\n\r", ch);
			return;
		}
		which = atoi (argument);

		//Find the delay...
		for (list = ch->actions; list != NULL; list = list->next)
		{
			++i;

			if (i == which)
			{
				delay = list;
				break;
			}
		}

		//Make sure we HAVE a delay.
		if (delay == NULL)
		{
			send_to_char ("You don't have that many actions.\n\r", ch);
			return;
		}

		//Remove the delay from the list.
		if (delay == ch->actions)
			ch->actions = NULL;
		else
		{
			DELAY * prev;

			for (prev = ch->actions; prev != NULL; prev = prev->next)
			{
				if (prev->next == delay)
				{
					prev->next = delay->next;
					break;
				}
			}
		}

		//Free the delay.
		free_delay (delay);
		send_to_char ("Action cancelled.\n\r", ch);
		return;
	}
	*/
	//We have a timer, right?
	if (!is_number (arg))
	{
		log_string ("ERROR : Mob Delay : Delay with no timer");
		return;
	}

	time = atoi (arg);

	//Make sure we're within the delay limits.
	if (time < 5 || time > 1800)
	{
		log_string ("ERROR : Mob Delay : Time delay too low or too high");
		return;
	}

	//Now we need to make sure there's a command.
	if (argument[0] == '\0')
	{
		log_string("ERROR : Mob Delay : No command issued to be delayed.");
		return;
	}

	//Create the delayed action.
	delay = new_delay ();

	free_string (delay->cmd);
	delay->cmd = str_dup (argument);

	delay->timer = time;

	//Now add it to the list.
	delay->next = ch->actions;
	ch->actions = delay;

	//And return.
//	send_to_char ("Okay!\n\r", ch);
	return;
}

void do_odelay (OBJ_DATA * obj, char * argument)
{
	DELAY * delay = NULL;
	char arg[MIL];
	int time;


	argument = one_argument (argument, arg);

	

	//We have a timer, right?
	if (!is_number (arg))
	{
		log_string ("ERROR : Obj Delay : Delay with no timer");
		return;
	}

	time = atoi (arg);

	//Make sure we're within the delay limits.
	if (time < 5 || time > 1800)
	{
		log_string ("ERROR : Obj Delay : Time delay too low or too high");
		return;
	}

	//Now we need to make sure there's a command.
	if (argument[0] == '\0')
	{
		log_string("ERROR : Obj Delay : No command issued to be delayed.");
		return;
	}

	//Create the delayed action.
	delay = new_delay ();

	free_string (delay->cmd);
	delay->cmd = str_dup (argument);

	delay->timer = time;

	//Now add it to the list.
	delay->next = obj->actions;
	obj->actions = delay;

	//And return.
	return;
}

void do_rdelay (ROOM_INDEX_DATA * room, char * argument)
{
	DELAY * delay = NULL;
	char arg[MIL];
	int time;


	argument = one_argument (argument, arg);

	

	//We have a timer, right?
	if (!is_number (arg))
	{
		log_string ("ERROR : Room Delay : Delay with no timer");
		return;
	}

	time = atoi (arg);

	//Make sure we're within the delay limits.
	if (time < 5 || time > 1800)
	{
		log_string ("ERROR: Room Delay : Time delay too low or too high");
		return;
	}

	//Now we need to make sure there's a command.
	if (argument[0] == '\0')
	{
		log_string("ERROR : Room Delay : No command issued to be delayed.");
		return;
	}

	//Create the delayed action.
	delay = new_delay ();

	free_string (delay->cmd);
	delay->cmd = str_dup (argument);

	delay->timer = time;

	//Now add it to the list.
	delay->next = room->actions;
	room->actions = delay;

	//And return.
	return;
}

/*
 * Memory recycling.
 */
DELAY * delay_free;

DELAY * new_delay (void)
{
    DELAY * delay;

    if (!delay_free)
        delay = alloc_perm (sizeof (*delay));
    else
    {
        delay		= delay_free;
        delay_free	= delay_free->next;
    }

    delay->cmd		= &str_empty[0];
	delay->timer	= 0;
    delay->next		= NULL;
    return delay;
}

void free_delay (DELAY * delay)
{
    free_string (delay->cmd);
    delay->next = delay_free;
    delay_free	= delay;
    return;
}
