#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "merc.h"
#include "tables.h"
#include "olc.h"
#include "lookup.h"
#include "recycle.h"

#define HEDIT( fun )           bool fun(CHAR_DATA *ch, char*argument)
#define EDIT_HELP(ch, help)	( help = (HELP_DATA *) ch->desc->pEdit )

extern HELP_AREA * had_list;

const struct olc_cmd_type hedit_table[] =
{
/*	{	command		function	}, */

	{	"keyword",	hedit_keyword	},
	{	"text",	hedit_text	},
	{	"make",		hedit_new	},
	{	"level",	hedit_level	},
	{	"commands",	show_commands	},
	{	"delete",	hedit_delete	},
	{	"list",		hedit_list	},
	{	"show",		hedit_show	},
	{	"?",		show_help	},

	{	NULL,		0		}
};

HELP_AREA * get_help_area( HELP_DATA *help )
{
	HELP_AREA * temp;
	HELP_DATA * thelp;

	for ( temp = had_list; temp; temp = temp->next )
		for ( thelp = temp->first; thelp; thelp = thelp->next_area )
			if ( thelp == help )
				return temp;

	return NULL;
}

HEDIT(hedit_show)
{
	HELP_DATA * help;
	char buf[MSL*2];

	EDIT_HELP(ch, help);

	sprintf( buf, "Keyword : [%s]\n\r"
		      "Level   : [%d]\n\r"
		      "Text    :\n\r"
		      "%s-END-\n\r",
		      help->keyword,
		      help->level,
		      help->text );
	send_to_char( buf, ch );

	return FALSE;
}

HEDIT(hedit_level)
{
	HELP_DATA *help;
	int lev;

	EDIT_HELP(ch, help);

	if ( IS_NULLSTR(argument) || !is_number(argument) )
	{
		send_to_char( "Syntax : level [-1..MAX_LEVEL]\n\r", ch );
		return FALSE;
	}

	lev = atoi(argument);

	if ( lev < -1 || lev > MAX_LEVEL )
	{
		printf_to_char( ch, "HEdit : Level must be between -1 and %d, inclusive.\n\r", MAX_LEVEL );
		return FALSE;
	}

	help->level = lev;
	send_to_char( "Ok.\n\r", ch );
	return TRUE;
}

HEDIT(hedit_keyword)
{
	HELP_DATA *help;

	EDIT_HELP(ch, help);

	if ( IS_NULLSTR(argument) )
	{
		send_to_char( "Syntax : keyword [keyword(s)]\n\r", ch );
		return FALSE;
	}

	free_string(help->keyword);
	help->keyword = str_dup(argument);

	send_to_char( "Ok.\n\r", ch );
	return TRUE;
}

HEDIT(hedit_new)
{
//	char arg[MIL];//, fullarg[MIL];
//	HELP_AREA *had;
	HELP_DATA *help;
//	extern HELP_DATA *help_last;

	if ( IS_NULLSTR(argument) )
	{
		send_to_char( "Syntax : hedit make [name]\n\r", ch );
		return FALSE;
	}

	help		= new_help();
	help->keyword	= str_dup(argument);
//	help->text	= str_dup( "" );
	help->next	= help_first;
	help_first	= help;

	ch->desc->pEdit		= (void *) help;
	ch->desc->editor	= ED_HELP;

	send_to_char( "New Help Entry Created.\n\r", ch );
	return FALSE;
}

HEDIT( hedit_text )
{
	HELP_DATA *help;

	EDIT_HELP(ch, help);

	if ( !IS_NULLSTR(argument) )
	{
		send_to_char( "Syntax: text\n\r", ch );
		return FALSE;
	}

	string_append( ch, &help->text );

	return TRUE;
}

 /* Help Editor - kermit 1/98 */
 void hedit( CHAR_DATA *ch, char *argument)
 {
     char command[MIL];
     char arg[MIL];
     int cmd;
     
     smash_tilde(argument);
     strcpy(arg, argument);
     argument = one_argument( argument, command);
 
     if (ch->pcdata->security < 9)
     {
         send_to_char("HEdit: Insufficient security to modify code\n\r",ch);
        edit_done(ch);
     }
 
     if ( !str_cmp(command, "done") )
     {
 	edit_done( ch );
 	return;
     }
 
     if ( command[0] == '\0' )
     {
         hedit_show( ch, argument );
 	return;
     }
 
     for ( cmd = 0; hedit_table[cmd].name != NULL; cmd++ )
     {
 	if ( !str_prefix( command, hedit_table[cmd].name ) )
 	{
             (*hedit_table[cmd].olc_fun) ( ch, argument );
             return;
 	}
     }
 
     interpret( ch, arg );
     return;    
 }
/*
void hedit( CHAR_DATA *ch, char *argument)
{
    HELP_DATA * pHelp;
    HELP_AREA *had;
    char arg[MAX_INPUT_LENGTH];
    char command[MAX_INPUT_LENGTH];
    int cmd;

    smash_tilde(argument);
    strcpy(arg, argument);
    argument = one_argument( argument, command);

    EDIT_HELP(ch, pHelp);

    had = get_help_area(pHelp);

    if (had == NULL)
    {
    	bugf( "hedit : helpfil '%s' is NULL", pHelp->keyword );
    	edit_done(ch);
    	return;
    }

    if (ch->pcdata->security < 9)
    {
        send_to_char("HEdit: Sorry, but your security rating is too low.\n\r",ch);
	edit_done(ch);
	return;
    }

    if (command[0] == '\0')
    {
	hedit_show(ch, argument);
        return;
    }


    if (!str_cmp(command, "done") )
    {
        edit_done(ch);
        return;
    }

    for (cmd = 0; hedit_table[cmd].name != NULL; cmd++)
    {
	if (!str_prefix(command, hedit_table[cmd].name) )
	{
		if ((*hedit_table[cmd].olc_fun) (ch, argument))
			had->changed = TRUE;
		return;
	}
    }

    interpret(ch, arg);
    return;
}
*/
void do_hedit(CHAR_DATA *ch, char *argument)
{
	HELP_DATA * pHelp;
	char arg1[MIL];
	char argall[MIL], argone[MIL];
	bool found = FALSE;

	strcpy( arg1, argument);

	if ( IS_NPC(ch) )
		return;

	if( argument != '\0')
	{
		argall[0] = '\0';
		while(argument[0] != '\0')
		{
			argument = one_argument(argument,argone);
			if (argall[0] != '\0')
				strcat(argall, " ");
			strcat(argall,argone);
		}
	for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next )
	{
		if ( is_name(argall, pHelp->keyword))
		{
			ch->desc->pEdit=(void*)pHelp;
			ch->desc->editor= ED_HELP;
			found = TRUE;
			return;
		}
	}
	}

	if( !found)
	{
		argument = one_argument(arg1, arg1);

		if(!str_cmp(arg1,"make"))
         {
             if (argument[0] == '\0')
             {
                 send_to_char("Syntax: edit help make [topic]\n\r",ch);
                 return;
             }
             if (hedit_new(ch, argument) )
                 ch->desc->editor = ED_HELP;
             return;
         }
     }
		send_to_char( "HEdit : There is no default help file to edit.\n\r", ch );
		return;
}

HEDIT(hedit_delete)
{
	HELP_DATA * pHelp, * temp;
	HELP_AREA * had;
	DESCRIPTOR_DATA *d;
	bool found = FALSE;

	EDIT_HELP(ch, pHelp);

	for ( d = descriptor_list; d; d = d->next )
		if ( d->editor == ED_HELP && pHelp == (HELP_DATA *) d->pEdit )
			edit_done(d->character);

	if (help_first == pHelp)
		help_first = help_first->next;
	else
	{
		for ( temp = help_first; temp; temp = temp->next )
			if ( temp->next == pHelp )
				break;

		if ( !temp )
		{
			bugf( "hedit_delete : help '%s' not found in the help_first list!", pHelp->keyword );
			return FALSE;
		}

		temp->next = pHelp->next;
	}

	for ( had = had_list; had; had = had->next )
		if ( pHelp == had->first )
		{
			found = TRUE;
			had->first = had->first->next_area;
		}
		else
		{
			for ( temp = had->first; temp; temp = temp->next_area )
				if ( temp->next_area == pHelp )
					break;

			if ( temp )
			{
				temp->next_area = pHelp->next_area;
				found = TRUE;
				break;
			}
		}

	if ( !found )
	{
		bugf( "hedit_delete : help %s not found in had_list!", pHelp->keyword );
		return FALSE;
	}

	free_help(pHelp);

	send_to_char( "Ok.\n\r", ch );
	return TRUE;
}

HEDIT(hedit_list)
{
	char buf[MIL];
	int cnt = 0;
	HELP_DATA *pHelp;
	BUFFER *buffer;

	EDIT_HELP(ch, pHelp);

	if ( !str_cmp( argument, "all" ) )
	{
		buffer = new_buf();

		for ( pHelp = help_first; pHelp; pHelp = pHelp->next )
		{
			sprintf( buf, "%3d. %-14.14s%s", cnt, pHelp->keyword,
				cnt % 4 == 3 ? "\n\r" : " " );
			add_buf( buffer, buf );
			cnt++;
		}

		if ( cnt % 4 )
			add_buf( buffer, "\n\r" );

		page_to_char( buf_string(buffer), ch );
		return FALSE;
	}

	if ( !str_cmp( argument, "area" ) )
	{
		if ( ch->in_room->area->helps == NULL )
		{
			send_to_char( "There are no helps in this area.\n\r", ch );
			return FALSE;
		}

		buffer = new_buf();

		for ( pHelp = ch->in_room->area->helps->first; pHelp; pHelp = pHelp->next_area )
		{
			sprintf( buf, "%3d. %-14.14s%s", cnt, pHelp->keyword,
				cnt % 4 == 3 ? "\n\r" : " " );
			add_buf( buffer, buf );
			cnt++;
		}

		if ( cnt % 4 )
			add_buf( buffer, "\n\r" );

		page_to_char( buf_string(buffer), ch );
		return FALSE;
	}

	if ( IS_NULLSTR(argument) )
	{
		send_to_char( "Syntax : list all\n\r", ch );
		send_to_char( "         list area\n\r", ch );
		return FALSE;
	}

	return FALSE;
}
