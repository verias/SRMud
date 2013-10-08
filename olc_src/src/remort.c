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
 **************************************************************************/

/***************************************************************************
*       ROM 2.4 is copyright 1993-1996 Russ Taylor                         *
*       ROM has been brought to you by the ROM consortium                  *
*           Russ Taylor (rtaylor@pacinfo.com)                              *
*           Gabrielle Taylor (gtaylor@pacinfo.com)                         *
*           Brian Moore (rom@rom.efn.org)                                  *
*       By using this code, you have agreed to follow the terms of the     *
*       ROM license, in the file Rom24/doc/rom.license                     *
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
#include <time.h>
#include "merc.h"
#include "interp.h"

void do_remor( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to REMORT, you must spell it out.\n\r", ch );
    return;
}

void do_remort( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
    char strsave[MAX_INPUT_LENGTH], player_name[MAX_INPUT_LENGTH];
    char *pwdnew, buf[MAX_STRING_LENGTH];
    int player_incarnations;
	char arg[MIL];
	char arg2[MIL];
	char *p;
	FILE *fp;

	argument = one_argument (argument, arg);
	one_argument (argument, arg2);

    if ( IS_NPC(ch) || ( d = ch->desc ) == NULL )
	return;

	if (arg[0]=='\0' || argument[0] == '\0' || arg2[0] == '\0')
	{
		send_to_one(ch,"Syntax: remort <new name> <new password>");
		return;
	}

    if ( ch->level < LEVEL_HERO  )
    {
	sprintf( buf,
	    "You must be level %d to remort.\n\r",
	    LEVEL_HERO );
	send_to_char( buf, ch );
	return;
    }

	player_incarnations = ch->pcdata->incarnations;

	if (ch->pcdata->incarnations >= 5)
	{
		send_to_one(ch,"You cannot remort further.");
		return;
	}

	if(!check_parse_name(arg))
	{
		send_to_one(ch,"Illegal name, try another.");
		return;
	}

	if( strlen(arg2) < 5)
	{
		send_to_one(ch,"Passwords must be at least 5 characters in length.");
		return;
	}

	    /*
	     * Get ready to delete the pfile, send a nice informational message.
	     */
	    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( arg ) );

			fp = fopen(strsave, "r");

	if( fp != NULL)
	{
		send_to_one(ch,"There is already someone with that name.\n\r"
						"Choose another.");
		fclose(fp);
		return;
	}


	    stop_fighting( ch, TRUE );
	    send_to_char( "You have chosen to remort.  You will now be dropped in at the race\n\r", ch );
	    send_to_char( "selection section of character creation, and will be allowed to choose from\n\r", ch );
	    send_to_char( "a wider selection of races and classes.\n\r\n\r", ch );
	    send_to_char( "\n\r[Hit Enter to Continue]\n\r", ch );
	    wiznet( "$N has remorted.", ch, NULL, 0, 0, 0 );

	    /*
	     * I quote:
	     * "After extract_char the ch is no longer valid!"
	     */
	    sprintf( player_name, "%s", capitalize( arg ) );
	    extract_char( ch, TRUE );

	    /*
	     * Delete the pfile, but don't boot the character.
	     * Instead, do a load_char_obj to get a new ch,
	     * saving the password, and the incarnations.  Then,
	     * set the PLR_REMORT bit and drop the player in at
	     * CON_BEGIN_REMORT.
	     */

	    load_char_obj( d, player_name );
	
		pwdnew = crypt( arg2, player_name );
		for ( p = pwdnew; *p != '\0'; p++ )
		{
		    if ( *p == '~' )
			{
			write_to_buffer( d,
		    "Password not acceptable, try again.\n\rPassword: ",
		    0 );
			return;
		    }
		}

		free_string( d->character->pcdata->pwd );
		d->character->pcdata->pwd	= str_dup( pwdnew );
	    d->character->pcdata->incarnations = ++player_incarnations;
//	    if( !IS_SET( ch->act, PLR_REMORT ) )
//		SET_BIT( ch->act, PLR_REMORT );
	    d->connected = CON_BEGIN_REMORT;
	    return;

 

}
