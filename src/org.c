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
*       ROM 2.4 is copyright 1993-1998 Russ Taylor                         *
*       ROM has been brought to you by the ROM consortium                  *
*           Russ Taylor (rtaylor@hypercube.org)                            *
*           Gabrielle Taylor (gtaylor@hypercube.org)                       *
*           Brian Moore (zump@rom.org)                                     *
*       By using this code, you have agreed to follow the terms of the     *
*       ROM license, in the file Rom24/doc/rom.license                     *
***************************************************************************/
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

// For OLC
#define ORGEDIT( fun )		bool fun( CHAR_DATA *ch, char *argument )

// Locals protos

ORGANIZATION *new_org	args( (void ) );
ORGANIZATION *org_lookup		args( ( const char *name) );
int orgrank_lookup	args( ( const char *name) );
void free_org		args( ( ORGANIZATION *pOrg ) );
void join_org		args( ( CHAR_DATA *ch, ORGANIZATION *pOrg));
bool same_org		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );

void denounce_org args((CHAR_DATA *ch, bool force ));
// OLC functions

ORGEDIT( orgedit_show )
{
	ORGANIZATION *pOrg;//, *pR;
//	BUFFER *buffer;
	int rank;//, iClass;
	char buf[MSL], arg[MSL];//, arg2[MSL], buf2[MSL];

	argument = one_argument(argument,arg);
	EDIT_ORG(ch,pOrg);

	if(arg[0] == '\0')
	{
		send_to_one(ch,"Organization Editor for: %s\n\r",pOrg->name);
		send_to_one(ch,"File name: %s\n\r",pOrg->filename);
		send_to_one(ch,"Patron:            %10s", pOrg->patron);
		send_to_one(ch,"Exalt Message:    %s",pOrg->pmt_msg);
		send_to_one(ch,"Rescind Message:  %s",pOrg->dmt_msg);
		send_to_one(ch,"Sacrifice Message: %s",pOrg->sac_msg);
		send_to_one(ch,"Channel Name:      %s",pOrg->chan_name);

		send_to_one(ch,"-----------------------------------------------------");
		send_to_one(ch,"Type 'show ranks' to view the rank info.");
		return TRUE;
	}
	if(!str_cmp(arg,"ranks"))
	{
		send_to_one(ch,"These are the ranks for %s.\n\r",pOrg->name);
		send_to_one(ch,"%10s|%20s|","Rank Name", "Rank Title" );
		for(rank = 0; rank < MAX_RELG_RANK; rank++)
		{
				
			sprintf(buf,"%10s",orgrank_table[rank].name);
			send_to_char(buf,ch);
				send_to_one(ch,"|%20s|",pOrg->rank[rank]);
		}
			send_to_one(ch,"****************************************************************");

			return TRUE;
	}

	return FALSE;
}

ORGEDIT( orgedit_new )
{
	ORGANIZATION * pOrg;

	if(argument[0] == '\0')
	{
		send_to_one(ch,"Syntax: OrgEdit new <name>");
		return FALSE;
	}

	if( ( pOrg = org_lookup(argument)) != NULL)
	{
		send_to_one(ch,"That organization already exists. Try another name.");
		return FALSE;
	}

	pOrg = new_org();
	free_string(pOrg->name);
	pOrg->name = str_dup(argument);
	pOrg->name[0] = UPPER(pOrg->name[0]);
	pOrg->filename = str_dup("null.org");
	pOrg->city = FALSE;
	pOrg->guild = FALSE;
	pOrg->next = org_list;
	org_list = pOrg;
	send_to_one(ch,"The organization of %s, has now been created.",pOrg->name);
	ch->desc->editor = ED_ORG;
	ch->desc->pEdit = (void *) pOrg;
	return TRUE;
}

ORGEDIT (orgedit_file)
{
	ORGANIZATION *pOrg;

	EDIT_ORG(ch,pOrg);

	if(argument[0] == '\0')
	{
		send_to_one(ch,"Syntax: file <filename>");
		return FALSE;
	}

	send_to_one(ch,"File name set.");
	free_string(pOrg->filename);
	pOrg->filename = str_dup(argument);
	strcat(pOrg->filename,".org");
	return TRUE;
}


ORGEDIT( orgedit_name)
{
	ORGANIZATION *pOrg;

	EDIT_ORG(ch, pOrg);

	if (argument[0] == '\0')
	{
		send_to_one(ch,"Syntax: name <name>");
		return FALSE;
	}

	if( org_lookup(argument)!= NULL)
	{
		send_to_one(ch,"That organization already exists. Please use another name.");
			return FALSE;
	}

	send_to_one(ch,"The organization of %s has been renamed to the organization of %s",pOrg->name, argument);
	free_string(pOrg->name);
	pOrg->name = str_dup(argument);
	pOrg->name[0] = UPPER(pOrg->name[0]);

	return TRUE;
}

ORGEDIT( orgedit_patron)
{
	ORGANIZATION *pOrg;
//	char *name;

	EDIT_ORG(ch,pOrg);

	if(argument[0] == '\0')
	{
		send_to_one(ch,"Syntax: patron <god name>");
		return FALSE;
	}

	free_string(pOrg->patron);
	pOrg->patron = str_dup(capitalize(argument));
	send_to_one(ch,"The patron for %s has been set to %s.",pOrg->name, pOrg->patron);
	return TRUE;
}

ORGEDIT (orgedit_rank)
{
	ORGANIZATION *pOrg;
	char arg[MSL];//, arg2[MSL];
	int rank;//, iClass;

	EDIT_ORG(ch,pOrg);

	argument = one_argument(argument, arg);
//	argument = one_argument(argument, arg2);

	if( argument[0] == '\0'||arg[0] == '\0')
	{	send_to_char("Syntax: rank <rank name> <title>\n\r",ch);
		return FALSE;
	}
	
	if( ( rank = orgrank_lookup(arg) ) == -1 )
	{	send_to_char("That isn't a valid rank.\n\r",ch);
		return FALSE;
	}

	free_string(pOrg->rank[rank]);
	pOrg->rank[rank] = str_dup(argument);
	pOrg->rank[rank][0] = UPPER(pOrg->rank[rank][0]);
	printf_to_char(ch, "%s, %s rank was changed to %s.\n\r", pOrg->name, orgrank_table[rank].name, argument);
	return TRUE;
}


ORGEDIT( orgedit_list)
{
	ORGANIZATION *pOrg;

	for(pOrg = org_list; pOrg; pOrg = pOrg->next)
		send_to_one(ch,"%-15 - %s",pOrg->name, pOrg->patron);
	return TRUE;
}

ORGEDIT( orgedit_delete )
{
	ORGANIZATION *pOrg,*temp;

	if(argument[0] == '\0' )
	{	send_to_char("Syntax: Delete <organization>\n\r",ch);
		return FALSE;
	}

	if( ( pOrg = org_lookup(argument) ) == NULL )
	{	send_to_char("There is no such Organization.\n\r",ch);
		return FALSE;
	}

	for(temp = org_list ; temp ; temp = temp->next )
		if(temp->next == pOrg )
			temp->next = pOrg->next;
	if(org_list == pOrg )
		org_list = pOrg->next;
	free_org(pOrg);
	send_to_char("Organization Deleted!\n\r",ch);
	return TRUE;
}

ORGEDIT( orgedit_message)
{
	ORGANIZATION *pOrg;
	char arg[MSL];

	argument = one_argument(argument,arg);
	EDIT_ORG(ch,pOrg);

	if( arg[0] == '\0' )
	{	send_to_char("Syntax:  message <sac|exalt|rescind|channame> <string>\n\r",ch);
		return FALSE;
	}

	if(!str_cmp(arg, "sac" ) )
	{	if(argument[0] == '\0' )
		{	send_to_char("What would you like your sacrifice message to be?\n\r",ch );
			return FALSE;
		}
		free_string(pOrg->sac_msg );
		pOrg->sac_msg = str_dup(argument);
		send_to_char("Sacrifice message set!\n\r", ch);
	}
	if(!str_cmp(arg, "exalt" ) )
	{	if(argument[0] == '\0' )
		{	send_to_char("What would you like your exalt message to be?\n\r",ch );
			return FALSE;
		}
		free_string(pOrg->pmt_msg );
		pOrg->pmt_msg = str_dup(argument);
		send_to_char("Exalt message set!\n\r", ch);
	}
	if(!str_cmp(arg, "rescind" ) )
	{	if(argument[0] == '\0' )
		{	send_to_char("What would you like your rescind message to be?\n\r",ch );
			return FALSE;
		}
		free_string(pOrg->dmt_msg );
		pOrg->dmt_msg = str_dup(argument);
		send_to_char("Rescind message set!\n\r", ch);
	}
	if(!str_cmp(arg, "channame" ) )
	{	if(argument[0] == '\0' )
		{	send_to_char("What would you like your channel name to be?\n\r",ch );
			return FALSE;
		}
		free_string(pOrg->chan_name );
		pOrg->chan_name = str_dup(argument);
		send_to_char("Channel name set!\n\r", ch);
	}
	return FALSE;
}

ORGEDIT( orgedit_type)
{
	ORGANIZATION *pOrg;
	EDIT_ORG(ch,pOrg);

	if(argument[0] == '\0')
	{
		send_to_one(ch,"Type <city/clan/guild>");
		return FALSE;
	}

	if(!str_cmp(argument,"city"))
	{
		send_to_one(ch,"%s now set to be a city.",pOrg->name);
		pOrg->city = TRUE;
		return TRUE;
	}


	if(!str_cmp(argument,"guild"))
	{
		send_to_one(ch,"%s now set to be a guild.",pOrg->name);
		pOrg->guild = TRUE;
		return TRUE;
	}

	return FALSE;

}
//End OLC stuff

void save_org()
{
	FILE *fp;//*rList
	ORGANIZATION *pRlg;
//	char buf[MSL];
	int rank;//, iClass;
	char file[200];

	fclose(fpReserve);


	for(pRlg = org_list ; pRlg ; pRlg = pRlg->next )
	{	
	sprintf(file, ORG_DIR);
	strcat(file, pRlg->filename);

	if (! (fp =fopen(file, "w")))
	{
		bug("Open_Org: fopen",0);
		perror(pRlg->filename);
	}
		fprintf(fp, "Name %s~\n", pRlg->name );
		fprintf(fp, "File %s~\n", pRlg->filename);
		fprintf(fp, "Patron %s~\n", pRlg->patron );
		fprintf(fp, "Gold %ld\n", pRlg->gold);
		fprintf(fp, "City %d\n",pRlg->city);
		fprintf(fp, "Guild %d\n",pRlg->guild);
		fprintf(fp, "Ranks\n" );
		for( rank = 0; rank < MAX_RELG_RANK ; rank++ )
		{
			fprintf(fp, "%s~\n", pRlg->rank[rank] );
		}
		fprintf(fp, "$\n");
		fclose(fp);
	}
	fpReserve = fopen(NULL_FILE, "r");
	return;
}

void save_org_list()
{
	FILE *fp;
	ORGANIZATION *pOrg;

	if (!(fp = fopen(ORG_LIST, "w")))
	{
		bug("Save_org_list: fopen", 0);
		perror ("org.lst");
	}
	else
	{
		for (pOrg = org_list; pOrg; pOrg = pOrg->next)
		{
			fprintf(fp, "%s\n", pOrg->filename);
		}
		fprintf(fp, "$\n");
		fclose(fp);
	}
	return;
}

#if defined(KEY)
#undef KEY
#endif

#define KEY( field, value )					\
				    field  = value;			\
		

#define KEYS( field, value )					\
					free_string(field);			\
				    field  = value;			\



void load_org()
{
	ORGANIZATION *pRlg;
	FILE *fp, *rList;
	char *word, *string;
	char buf[MSL];
//	AREA_DATA *pArea;
	int rank, iClass;

	if(( rList = fopen(ORG_LIST,"r" )) == NULL )
	{	log_string("Org_list not there. No organizations!");
		return;
	}

	
	for( word = fread_word(rList); str_cmp(word, "$" ); word = fread_word(rList) )
	{	sprintf(buf, "%s%s", ORG_DIR, word);
		if((fp = fopen(buf,"r")) == NULL )
		{	log_string("organization file missing.");
			continue;
		}
		rank = 0; iClass = rank;
		for(string = fread_word(fp); str_cmp(word, "$" ); word = fread_word(fp) )
		{
			if(!str_cmp(string,"Gold"))
			{
				KEY(pRlg->gold, fread_number(fp));
			}

			if(!str_cmp(string,"City"))
			{
				KEY(pRlg->city, fread_number(fp));
			}


			if(!str_cmp(string,"Guild"))
			{
				KEY(pRlg->guild, fread_number(fp));
			}

			if(!str_cmp(string, "Name" ) )
			{
				pRlg = new_org();
				KEYS(pRlg->name, fread_string(fp) );
				pRlg->next = org_list;
				org_list = pRlg;
				continue;
			}
			if(!str_cmp(string,"File"))
			{
				KEYS(pRlg->filename, fread_string(fp));
				continue;
			}
			if(!str_cmp(string, "Patron" ) )
			{
				KEYS(pRlg->patron, fread_string(fp) );
				continue;
			}
			if(!str_cmp(string, "Ranks" ) )
			{
				
				for(rank = 0; rank < MAX_RELG_RANK ; rank++ )
				{
				KEYS(pRlg->rank[rank], fread_string(fp) );
						}
				continue;
			}
			log_string("Invalid string in Organization." );
		}
	}
}
//end file I/O

/*Begining of do_functions*/

void do_exalt(CHAR_DATA *ch, char *argument )
{	CHAR_DATA *victim;
	char arg[MSL];

	if(IS_NPC(ch) )
		return;

	if(!HAS_ORGANIZATION(ch) )
	{	send_to_char("You aren't organized.\n\r",ch);
		return;
	}
	if( ch->pcdata->rank < RANK_PRIEST )
	{	send_to_char("You cannot exalt people!\n\r",ch );
		return;
	}
	one_argument(argument, arg );

	if(arg[0] == '\0' )
	{	send_to_char("Syntax: Exalt <person>\n\r",ch);
		return;
	}
	if(!IS_IMMORTAL(ch) )
	{	if( ( victim = get_char_room(ch, NULL, arg) )  == NULL )
		{	send_to_char("They aren't here.\n\r",ch);
			return;
		}
	}
	else
	{	if( ( victim = get_char_world(ch, arg) ) == NULL )
		{	send_to_char("They aren't here.\n\r",ch);
			return;
		}
	}
	if(IS_NPC(victim) )
	{	send_to_char("Not on NPC's.\n\r",ch);
		return;
	}
	if(!HAS_ORGANIZATION(victim) )
	{	send_to_char("They aren't organized.\n\r",ch);
		return;
	}

    if(!IS_SAME_ORGANIZATION(ch, victim) && ch->pcdata->rank != RANK_GOD  )
	{	send_to_char("They aren't in your organization!\n\r",ch); 
		return; 
	}

	if( ch->pcdata->rank <= victim->pcdata->rank )
	{	send_to_char("You cannot exalt them.\n\r", ch );
		return;
	}

	if(!IS_IMMORTAL(ch) && ch->pcdata->rank <= RANK_PRIEST && victim->pcdata->rank >= RANK_BISHOP )
	{	send_to_char("You cannot exalt them.\n\r",ch);
		return;
	}

	if(victim->pcdata->rank == RANK_DEITY && ch->pcdata->rank != RANK_GOD )
	{	send_to_char("You cannot exalt to God Rank.\n\r",ch);
		return;
	}
	if(victim->pcdata->rank == RANK_GOD )
	{	send_to_char("They cannot get any higher!\n\r",ch);
		return;
	}

	victim->pcdata->rank++;
	printf_to_char(ch, "%s has been exalted to %s.\n\r", victim->name, orgrank_table[victim->pcdata->rank].name );
	act_new(victim->pcdata->organization->pmt_msg, victim, NULL, NULL, TO_CHAR, POS_SLEEPING);
	return;
}

void do_rescind(CHAR_DATA *ch, char *argument )
{	CHAR_DATA *victim;

	char arg[MSL];

	if(IS_NPC(ch) )
		return;
	if(!HAS_ORGANIZATION(ch) )
	{	send_to_char("You aren't organized.\n\r",ch);
		return;
	}
	if( ch->pcdata->rank < RANK_PRIEST )
	{	send_to_char("You cannot rescind peoples ranks!\n\r",ch );
		return;
	}
	one_argument(argument, arg );

	if(arg[0] == '\0' )
	{	send_to_char("Syntax: rescind <person>\n\r",ch);
		return;
	}
	if(!IS_IMMORTAL(ch) )
	{	if( ( victim = get_char_room(ch, NULL, arg) )  == NULL )
		{	send_to_char("They aren't here.\n\r",ch);
			return;
		}
	}
	else
	{	if( ( victim = get_char_world(ch, arg) ) == NULL )
		{	send_to_char("They aren't here.\n\r",ch);
			return;
		}
		
	}
	if(IS_NPC(victim) )
	{	send_to_char("Not on NPC's.\n\r",ch);
		return;
	}
	if(!HAS_ORGANIZATION(victim) )
	{	send_to_char("They aren't organized.\n\r",ch);
		return;
	}
	if(!IS_SAME_ORGANIZATION(ch, victim) && ch->pcdata->rank != RANK_GOD  )
	{	send_to_char("They aren't in your organization!\n\r",ch); 
		return; 
	}

	if( ch->pcdata->rank <= victim->pcdata->rank )
	{	send_to_char("You cannot rescind their rank!\n\r", ch );
		return;
	}

	if(!IS_IMMORTAL(ch) && ch->pcdata->rank <= RANK_PRIEST && victim->pcdata->rank >= RANK_BISHOP )
	{	send_to_char("You cannot rescind their rank.\n\r",ch);
		return;
	}

	if(victim->pcdata->rank == RANK_INITIATE )
	{	send_to_char("They cannot get any lower!\n\r",ch);
		return;
	}
	--victim->pcdata->rank;
	act_new(victim->pcdata->organization->dmt_msg,ch, NULL, victim, TO_VICT, POS_SLEEPING );
	printf_to_char(ch, "You have lowered their rank to %s.\n\r", orgrank_table[victim->pcdata->rank].name );
	return;
}

void do_denounce(CHAR_DATA *ch, char * argument )
{	char arg[MSL], arg2[MSL];
	CHAR_DATA *victim;
	if(IS_NPC(ch ) )
		return;

	if(!str_cmp(argument, "organization" ) )
	{	denounce_org(ch, FALSE);
		return;
	}
	argument = one_argument(argument, arg );
	argument = one_argument(argument, arg2 );
	
	if(!str_cmp(arg, "follower" ) )
	{	if(ch->pcdata->rank <= RANK_PRIEST )
		{	do_function(ch, &do_denounce, NULL );
			return;
		}

		if(!IS_IMMORTAL(ch) )
		{	if( ( victim = get_char_room(ch, NULL, arg2 ) ) == NULL )
			{	send_to_char("They aren't here.\n\r",ch);
				return;
			}
		}
		else
		{	if( ( victim = get_char_world(ch, arg2 ) ) == NULL )
			{	send_to_char("They aren't here.\n\r", ch);
				return;
			}
		}
		if(IS_NPC(victim) )
		{	send_to_char("Not on NPC's.\n\r",ch);
			return;
		}
		
		if(!IS_SAME_ORGANIZATION(ch, victim) && ch->pcdata->rank != RANK_GOD  )
		{	send_to_char("They aren't in your organization!\n\r",ch); 
			return; 
		}


		if(ch->pcdata->rank <= victim->pcdata->rank )
		{	send_to_char("You cannot denounce their organization.\n\r",ch);
			return;
		}

		send_to_char("You denounce thier organization!\n\r",ch);
		denounce_org(victim, TRUE);
		return;
	}

	if(ch->pcdata->rank <= RANK_PRIEST )
		send_to_char("To denounce a followers organization, use the syntax: 'denounce follower <name>'\n\r",ch);
	send_to_char("To denounce your faith, please type use the syntax: 'denounce organization'\n\r", ch);
	return;
}

void do_initiate(CHAR_DATA *ch, char *argument )
{	char arg[MSL], arg2[MSL], arg3[MSL];
	CHAR_DATA *victim;

	if(IS_NPC(ch ) )
		return;

	argument = one_argument(argument, arg  );
	argument = one_argument(argument, arg2 );
	argument = one_argument(argument, arg3 );
	if(!HAS_ORGANIZATION(ch) )
	{	send_to_char("But you aren't organized!\n\r",ch);
		return;
	}
	if(ch->pcdata->rank < RANK_PRIEST )
	{	send_to_char("You can't initiate people!\n\r",ch);
		return;
	}

	if(arg[0] == '\0' )
	{	send_to_char("Syntax: Initiate <person>\n\r",ch);
		return;
	}

	if(ch->pcdata->rank < RANK_PRIEST )
	{	send_to_char("You can't initiate people!\n\r",ch);
		return;
	}

	if(!IS_IMMORTAL(ch) )
	{	if( ( victim = get_char_room(ch, NULL, arg ) ) == NULL )
		{	send_to_char("They aren't here.\n\r",ch);
			return;
		}
	}
	else
	{	if( ( victim = get_char_world(ch, arg ) ) == NULL )
		{	send_to_char("They aren't here.\n\r", ch);
			return;
		}
	}
	if(IS_NPC(victim) )
	{	send_to_char("Not on NPC's.\n\r",ch);
		return;
	}
	if(arg2[0] == '\0' )
	{	if( HAS_ORGANIZATION(victim) )
		{	send_to_char("They are already in an organization!\n\r",ch);
			return;
		}
		printf_to_char(ch, "They have been initiated.\n\r" );
		join_org(victim, ch->pcdata->organization );
	}
	return;
}

/*This is made so the first time you create a 
  religion you can make yourself a god! Make sure 
  only responsable people have this. */

void do_makegod(CHAR_DATA *ch, char *argument ) 
{	char arg[MSL];
	ORGANIZATION *pRlg;

	argument = one_argument(argument, arg);

	if(arg[0] == '\0' )
	{	send_to_char("Syntax: makegod <organization>\n\r",ch);
		return;
	}

	if( ( pRlg = org_lookup(arg) ) == NULL )
	{
		send_to_char("That isn't a valid organization.\n\r",ch);
			return;
	}

	ch->pcdata->organization = pRlg;
	ch->pcdata->rank = RANK_GOD;
	return;
}
/*End of do_functions*/

/*Stuff for joining/leaving a religion or Faction */

void join_org(CHAR_DATA *ch, ORGANIZATION *pRlg )
{	if(IS_NPC(ch) )
		return;
	if(ch->pcdata->organization )
	{	printf_to_char(ch, "You must denounce your organization to %s first.\n\r",ch->pcdata->organization->name );
		return;
	}
	
	ch->pcdata->organization = pRlg;
	printf_to_char(ch, "You now find solace in %s.\n\r", pRlg->name );
	ch->pcdata->rank = 0;
}

void denounce_org(CHAR_DATA *ch, bool force )
{
//	int rank, sn;
	if(IS_NPC(ch) )
		return;

	if(!ch->pcdata->organization )
	{	send_to_char("You cannot denounce your organization when you do not have one!\n\r",ch);
		return;
	}

	if(!force)
		printf_to_char(ch, "You denounce %s as your organization.\n\r", ch->pcdata->organization->name );
	else
		printf_to_char(ch, "You have been expunged!\n\r" );

	ch->pcdata->organization = NULL;
	ch->pcdata->rank = 0;
	return;
}


/*Begining of Recycling functions*/
ORGANIZATION *org_free;
#define STRING_NEW(string)           \
				free_string(string);  \
				string = str_dup("Not Set"); \

ORGANIZATION *new_org(void)
{
    static ORGANIZATION relg_zero;
    ORGANIZATION *pRlg;
	int rank;//, faction, iClass;

    if (org_free == NULL)
        pRlg = alloc_perm(sizeof(*pRlg));
    else
    {
        pRlg = org_free;
        org_free = org_free->next;
    }
	*pRlg = relg_zero;
	

	
	STRING_NEW(pRlg->pmt_msg);
	STRING_NEW(pRlg->dmt_msg);
	STRING_NEW(pRlg->chan_name);
	STRING_NEW(pRlg->sac_msg );
	STRING_NEW(pRlg->patron);
	STRING_NEW(pRlg->name );

	for(rank = 0; rank < MAX_RELG_RANK ; rank++ )
	{
		
		free_string(pRlg->rank[rank]);
			pRlg->rank[rank] = str_dup(orgrank_table[rank].name );
	}


    VALIDATE(pRlg);
    return pRlg;
}


void free_org(ORGANIZATION *pRlg)
{	
	int rank;//faction, iClass;

	if (!IS_VALID(pRlg))
        return;

	free_string(pRlg->name);
	
	free_string(pRlg->patron);
	
	for(rank = 0; rank < MAX_RELG_RANK ; rank++ )
	
		{
			free_string(pRlg->rank[rank]);
		}

    INVALIDATE(pRlg);
    pRlg->next = org_free;
    org_free = pRlg;
}
/*End of Recycling functions*/
/*Begining of lookup functions*/
/*int god_lookup ( const char *name )
{
    int god;

    for ( god = 0 ; god_table[god].name != NULL ; god++ )
    {
        if (LOWER(name[0]) == LOWER(god_table[god].name[0])
        &&  !str_prefix(name,god_table[god].name))
            return god;
    }
    return -1;
}*/

int orgrank_lookup ( const char *name )
{
    int relgrank;

    for ( relgrank = 0 ; orgrank_table[relgrank].name != NULL ; relgrank++ )
    {
        if (LOWER(name[0]) == LOWER(orgrank_table[relgrank].name[0])
        &&  !str_prefix(name,orgrank_table[relgrank].name))
            return relgrank;
    }
    return -1;
}

ORGANIZATION *org_lookup ( const char *name )
{
    ORGANIZATION * pRlg;

    for ( pRlg = org_list ; pRlg ; pRlg = pRlg->next )
    {
        if (LOWER(name[0]) == LOWER(pRlg->name[0])
        &&  !str_prefix(name,pRlg->name))
            return pRlg;
    }
    return NULL;
}

/*End of Lookup Functions */
/*Begining of Structures */
struct sex_type orgrank_table[] =
{
	{  "Initiate" },
	{  "Acolyte"  },
	{  "Disciple" },
	{  "Bishop"	  },
	{  "Priest"   },
	{  "Deity"    },
	{  "God"	  }
};

struct sex_type god_table[] = 
{
	{ "Davion" },
	{ "Synon"  },
	{ "Sola"   },
	{ NULL	   },
};

/*End of Structures */


char *god_name(CHAR_DATA *ch )
{	if( org_lookup(ch->pcdata->organization->name ) != NULL )
		return ch->pcdata->organization->patron;
	return "ShadowStorm";
}
