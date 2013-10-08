/*******************************************************************************
In handler.c, add this to extract_char, somewhere before ch is removed from
char_list:

	//Free delayed actions.
	if (ch->actions != NULL)
	{
		DELAY * list, * next;

		for (list = ch->actions; list != NULL; list = next)
		{
			next = list->next;
			free_delay (list);
		}
	}


Now, add this somewhere in merc.h (above struct char_data):

#include "delay.h"


Add this to struct char_data (also in merc.h):

	DELAY *            actions;


Now, add this at the top of update_handler() in update.c:

	delay_update ();


Now, add the command entry to interp.c.  Make it hidden (0 in the show field) 
if you have it disabled to players.  

Add these files to your src directory, if necessary, add delay.o to your 
makefile, clean compile, and everything should work.  Enjoy it.
*******************************************************************************/
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

//Delayed command structure.
typedef struct delay DELAY;

struct delay
{
    DELAY * next;
    char *  cmd;
	int		timer;
};


//Declarations and prototypes.
DECLARE_DO_FUN(	do_delay		);
DECLARE_OBJ_FUN( do_odelay		);
DECLARE_ROOM_FUN(	do_rdelay		);
void			delay_update	(void);
DELAY *			new_delay		(void);
void			free_delay		(DELAY * delay);
