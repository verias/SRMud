/***************************************************************************
 *  As the Wheel Weaves based on ROM 2.4. Original code by Dalsor, Caxandra,
 *	and Zy of AWW. See changes.log for a list of changes from the original
 *	ROM code. Credits for code created by other authors have been left
 *	intact at the head of each function.
 ***************************************************************************/

/* Begin homes.c */

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <string.h>
#include "merc.h"
#include "recycle.h"
#include "interp.h"

/* Thanks to Kyndig and Sandi Fallon for assisting with arguments - Dalsor.
 * At the time I started on furnishings, I was about 12 hours straight into
 * the code and very brain dead. Thanks to them for pointing out my mistakes.
 */

/* Homes uses aspects of OLC, and it should be installed, or the necessary
 * componants installed in order for this to work
 */

/* Damn, this is a lot of defines :(
 * Each one of these vnums will need to be created. If this many items
 * aren't desired, removed the uneccesary ones and comment out the code
 */
#define stc send_to_char

#define HOME_ITEM_1   11800 /* fish */
#define HOME_ITEM_2   11801 /* turtle */
#define HOME_ITEM_3   11802 /* snake */
#define HOME_ITEM_4   11803 /* spider */
#define HOME_ITEM_5   11804 /* carved chair */
#define HOME_ITEM_6   11805 /* highback chair */
#define HOME_ITEM_7   11806 /* hard chair */
#define HOME_ITEM_8   11807 /* sturdy chair */
#define HOME_ITEM_9   11808 /* oak desk */
#define HOME_ITEM_10  11809 /* pine desk */
#define HOME_ITEM_11  11810 /* carved desk */
#define HOME_ITEM_12  11811 /* polished desk */
#define HOME_ITEM_13  11812 /* teak desk */
#define HOME_ITEM_14  11813 /* plain desk */
#define HOME_ITEM_15  11814 /* plush sofa */
#define HOME_ITEM_16  11815 /* padded sofa */
#define HOME_ITEM_17  11816 /* comfy sofa */
#define HOME_ITEM_18  11817 /* fluffed sofa */
#define HOME_ITEM_19  11818 /* comfy sofa */
#define HOME_ITEM_20  11819 /* oak end */
#define HOME_ITEM_21  11820 /* pine end */
#define HOME_ITEM_22  11821 /* carved end */
#define HOME_ITEM_23  11822 /* teak end */
#define HOME_ITEM_24  11823 /* norm end */
#define HOME_ITEM_25  11824 /* oak dining table */
#define HOME_ITEM_26  11825 /* pine table */
#define HOME_ITEM_27  11826 /* carved table */
#define HOME_ITEM_28  11827 /* polished table */
#define HOME_ITEM_29  11828 /* teak table */
#define HOME_ITEM_30  11829 /* comfy recliner */
#define HOME_ITEM_31  11830 /* lamp */
#define HOME_ITEM_32  11831 /* lantern */
#define HOME_ITEM_33  11832 /* torch */
#define HOME_ITEM_34  11833 /* oak dresser */
#define HOME_ITEM_35  11834 /* pine dresser */
#define HOME_ITEM_36  11835 /* carved dresser */
#define HOME_ITEM_37  11836 /* polished dresser */
#define HOME_ITEM_38  11837 /* teak dresser */
/* #define HOME_ITEM_39  11838 norm dresser */
#define HOME_ITEM_40  11839 /* oak foot */
#define HOME_ITEM_41  11840 /* pine foot */
#define HOME_ITEM_42  11841 /* carved foot */
#define HOME_ITEM_43  11842 /* polished foot */
#define HOME_ITEM_44  11843 /* teak foot */
/* #define HOME_ITEM_45  11844 norm foot */
#define HOME_ITEM_46  11845 /* oak arm */
#define HOME_ITEM_47  11846 /* pine arm */
#define HOME_ITEM_48  11847 /* carved arm */
#define HOME_ITEM_49  11848 /* polished arm */
#define HOME_ITEM_50  11849 /* teak arm */
/* #define HOME_ITEM_51  11850 norm arm */
#define HOME_ITEM_52  11851 /* oak war */
#define HOME_ITEM_53  11852 /* pine arm */
#define HOME_ITEM_54  11853 /* carved arm */
#define HOME_ITEM_55  11854 /* polished arm */
#define HOME_ITEM_56  11855 /* teak arm */
/* #define HOME_ITEM_57  11856 norm arm */
#define HOME_ITEM_58  11857 /* oak book */
#define HOME_ITEM_59  11858 /* pine book */
#define HOME_ITEM_60  11859 /* carved book */
#define HOME_ITEM_61  11860 /* polished book */
#define HOME_ITEM_62  11861 /* teak book */
/* #define HOME_ITEM_63  11862  norm book */
#define HOME_ITEM_64  11863 /* oak chiff */
#define HOME_ITEM_65  11864 /* pine chiff */
#define HOME_ITEM_66  11865 /* carved chiff */
#define HOME_ITEM_67  11866 /* polished chiff */
#define HOME_ITEM_68  11867 /* teak chiff */
/* #define HOME_ITEM_69  11868  norm chiff */
#define HOME_ITEM_70  11869 /* royalbed */
#define HOME_ITEM_71  11870 /* cot */
#define HOME_ITEM_72  11871 /* featherbed */
#define HOME_ITEM_73  11872 /* canopybed */
#define HOME_ITEM_74  11873 /* postedbed */
#define HOME_ITEM_75  11874 /* twinbed */

RESET_DATA *new_reset_data args ( ( void ) );
void add_reset args ( ( ROOM_INDEX_DATA *room, RESET_DATA *loc_reset, int index ) );
void free_reset_data args ( ( RESET_DATA *pReset ) );
void home_buy ( CHAR_DATA *ch );
void home_sell ( CHAR_DATA *ch, char *argument );
void home_describe ( CHAR_DATA *ch );
OBJ_DATA *generate_furn	args (( CHAR_DATA *ch, int i ));


struct furnish_type
{
	char *name;
	char *listname;
	char *short_desc;
	char *long_desc;
	sh_int	type;
	char *owner;
	int	cost;
	int	value[5];
};

const struct furnish_type furn_table[];




int furn_lookup(const char *name)
{
    int sn;

    for (sn = 0; furn_table[sn].name != NULL; sn++)
    {
	if (LOWER(name[0]) == LOWER(furn_table[sn].name[0])
	&&  !str_prefix(name,furn_table[sn].name))
	    return sn;
    }

    return -1;
}

const struct furnish_type furn_table[] =
{
	{ "turtle aquarium", "An aquarium with a turtle",
		"an aquarium with a small turtle in it",
		"An aquarium is here containing a small turtle.",
		ITEM_FURNITURE, "", 3000, {0,0,0,0,0}},
	{ "fish aquarium", "An aquarium with a fish",
		"an aquarium with a small fish in it",
		"An aquarium is here containing a small fish.",
		ITEM_FURNITURE, "", 3000, {0,0,0,0,0}},
	{ NULL, NULL, "", "", ITEM_FURNITURE, "", 0, {0,0,0,0,0}}
};

OBJ_DATA *generate_furn( CHAR_DATA *ch, int i)
{
	OBJ_DATA *obj;
    ROOM_INDEX_DATA *loc;
    RESET_DATA *loc_reset;
    AREA_DATA *loc_area;
//	char buf[MIL];
//	int i;

	loc = ch->in_room;
	loc_area = ch->in_room->area;

	obj = create_object(get_obj_index(HOME_ITEM_1),0);

//	for(i=0; furn_table[i].name != NULL; i++)
//	{
	{
		obj->item_type = furn_table[i].type;
	obj->name = str_dup(furn_table[i].name);
	free_string(obj->short_descr);
	obj->short_descr = str_dup(furn_table[i].short_desc);
	free_string(obj->description);
	obj->description = str_dup(furn_table[i].long_desc);
	obj->owner = str_dup(ch->name);
	obj->value[0] = furn_table[i].value[0];
	obj->value[1] = furn_table[i].value[1];
	obj->value[2] = furn_table[i].value[2];
	obj->value[3] = furn_table[i].value[3];
	obj->value[4] = furn_table[i].value[4];
	obj->cost = furn_table[i].cost;
	log_string("Obj created.");
	log_string("Reseting obj.");
	loc_reset               = new_reset_data();
	loc_reset->command      = 'O';
	loc_reset->arg1         = obj->pIndexData->vnum;
	loc_reset->arg2         = 0;
	loc_reset->arg3         = loc->vnum;
	loc_reset->arg4         = 0;
	add_reset( loc, loc_reset, 0 );
	log_string("Obj reset, now putting in room.");
	obj_to_room ( obj,ch->in_room );
    SET_BIT(loc_area->area_flags, AREA_CHANGED);
	log_string("Obj put.");
	return obj;
	}
	return NULL;
}

void do_home ( CHAR_DATA *ch, char *argument )
{

    ROOM_INDEX_DATA *loc;
    AREA_DATA *loc_area;
//    RESET_DATA *loc_reset;
    OBJ_DATA *furn;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	char buf[MSL];
	int iFurn;

	loc = ch->in_room;
	loc_area = ch->in_room->area;
	buf[0] = '\0';

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	argument = one_argument( argument, arg3 );

	if ( IS_NPC(ch)
	|| ch == NULL )
		return;

	if ( arg1[0] == '\0' || !strcmp ( arg1, "list" ) )
	{
		stc("What about a home?\n\r",ch);
		stc("\n\rSyntax: Home (buy, sell, furnish, describe)\n\r",ch);
		stc("\n\rExtended:\n\rHome (buy, sell, furnish, describe)\n\r",ch);
		stc("Buy      (purchase a home in the room you are standing in)\n\r",ch);
		stc("Sell     (confirm - does not prompt for confirmation!)\n\r",ch);
		stc("Furnish  (allows purchases of items)\n\r",ch);
		stc("Describe (describe the room - uses the OLC editor)\n\r",ch);
		return;
	}
	if ( !is_name( ch->in_room->area->file_name, "subdiv.are" ) )
	{
		stc("You aren't in a subdivision.\n\r",ch);
		return;
	}
	if ( !IS_SET ( loc->room_flags, ROOM_HOME ) )
	{
		stc("You cannot buy a home in this room.\n\r",ch);
		return;
	}

	/* Find out what the argument is, if any */
	if      ( !strcmp ( arg1, "buy" ) )        home_buy ( ch );
	else if ( !strcmp ( arg1, "sell" ) )       home_sell ( ch, arg2 );
	else if ( !strcmp ( arg1, "describe" ) )   home_describe ( ch );
	else if ( !strcmp ( arg1, "furnish" ) )
	/* Home furnish was left in here because I didn't feel like
	 * redoing all the arguments - Dalsor
	 */
	{
		if ( !is_room_owner ( ch, loc ) )
		{
			stc("But you do not own this room!\n\r",ch);
			return;
		}
		if 	( arg2[0] == '\0' )
		{
			
			for( iFurn = 0; furn_table[iFurn].name != NULL; iFurn++)
			{
				send_to_one(ch,"%s       %d gold",furn_table[iFurn].listname,
					furn_table[iFurn].cost);
			}
		/*
		stc("This command allows you to furnish your home.\n\r",ch);
		stc("You must be carrying gold to purchase furnishings,\n\r",ch);
		stc("and be standing in your home. You cannot have more\n\r",ch);
		stc("than five items in your home.\n\r",ch);
		stc("\n\rSyntax: Home (furnish) (item name)\n\r",ch);
		stc("   Window      5000 gold\n\r",ch);
		stc("   Aquarium    7500 gold (fish, turtle, snake, spider)\n\r",ch);
		stc("   Chair       5000 gold (sturdy, highback, carved, hard)\n\r",ch);
		stc("   Desk        7500 gold (oak, pine, carved, polished, teak)\n\r",ch);
		stc("   Sofa        7500 gold (plush, padded, comfortable, fluffed)\n\r",ch);
		stc("   Endtable    1000 gold (oak, pine, carved, teak)\n\r",ch);
		stc("   Table       7500 gold (oak, pine, carved, polished, teak)\n\r",ch);
		stc("   Recliner    7500 gold\n\r",ch);
		stc("   Lamp        2000 gold\n\r",ch);
		stc("   Mirror      2500 gold\n\r",ch);
		stc("   Lantern     1500 gold\n\r",ch);
		stc("   Torch       1000 gold\n\r",ch);
		stc("   Dresser     5000 gold (oak, pine, carved, polished, teak)\n\r",ch);
		stc("   Footchest   5000 gold (oak, pine, carved, polished, teak)\n\r",ch);
		stc("   Armoire     7500 gold (oak, pine, carved, polished, teak)\n\r",ch);
		stc("   Wardrobe    7500 gold (oak, pine, carved, polished, teak)\n\r",ch);
		stc("   Bookcase    7500 gold (oak, pine, carved, polished, teak)\n\r",ch);
		stc("   Chifferobe  7500 gold (oak, pine, carved, polished, teak)\n\r",ch);
		stc("   Cot         1000 gold\n\r",ch);
		stc("   Featherbed  2500 gold\n\r",ch);
		stc("   Twinbed     5000 gold\n\r",ch);
		stc("   Postedbed   7500 gold\n\r",ch);
		stc("   Canopybed   8500 gold\n\r",ch);
		stc("   Royalbed    9900 gold\n\r",ch);
		return;*/
		}
		else if ( arg2[0] != '\0')
		{
		iFurn = furn_lookup(arg2);

		if (iFurn == -1 )
		{
			stc("That is not currently available.\n\r",ch);
			stc("The following items are available for purchase.\n\r",ch);
			for (iFurn = 0; furn_table[iFurn].listname != NULL; iFurn++)
			{
					send_to_one(ch,"%s       %d gold",furn_table[iFurn].listname,
					furn_table[iFurn].cost);
			}
			return;
		}
			if(ch->gold < furn_table[iFurn].cost)
			{
				send_to_one(ch,"You do not have enough gold to purchase that.");
				return;
			}
		furn = generate_furn(ch, iFurn);
		ch->gold -= furn->cost;
		printf_to_char(ch,"You have been deducted %d for your purchase.",furn->cost);
		return;
		}
	
	}
	else
	{
		stc("No such Home Command. See HELP HOME for more informatio.\n\r",ch);
		return;
	}
	return;
}

void home_buy ( CHAR_DATA *ch )
{
    ROOM_INDEX_DATA *loc;
    AREA_DATA *loc_area;
	char buf[MSL];

	loc = ch->in_room;
	loc_area = ch->in_room->area;
	if ( loc->owner[0] == '\0' )
	{
		if ( ch->gold < 25000 )
		{
		stc("This command allows you to buy a home.\n\r",ch);
		stc("You must be standing in the room to buy.\n\r",ch);
		stc("You start with a blank, untitled room that is void of\n\r",ch);
		stc("furnishings and light. A single door allows entrance.\n\r",ch);
		stc("A home costs 25,000 GOLD. You must be carrying the coins.\n\r",ch);
		stc("\n\rSyntax: Home buy\n\r",ch);
		return;
		}
		else
		{
	    free_string ( loc->owner );
	    loc->owner = str_dup ( ch->name );
	    ch->gold -= 25000;
	    free_string ( loc->name );
	    sprintf ( buf, "%s's Home", ch->name );
	    loc->name = str_dup ( buf );
	    stc("Congratulations on purchasing your new home!\n\r",ch);
	    stc("You are now 25,000 gold coins lighter and the owner\n\r",ch);
	    stc("of the room in which you now stand! Be sure to describe\n\r",ch);
	    stc("your home and purchase furnishings for it.\n\r",ch);
	    SET_BIT(loc_area->area_flags, AREA_CHANGED);
	    return;
		}
	}
	else
	{
		stc("This home is already owned.\n\r",ch);
		return;
	}
	return;
}

void home_sell ( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *loc;
    AREA_DATA *loc_area;
	RESET_DATA *current_reset;
	RESET_DATA *next_reset;
    OBJ_DATA *furn;
    OBJ_DATA *furn_next;

	loc = ch->in_room;
	loc_area = ch->in_room->area;

	if ( !is_room_owner ( ch, loc ) )
	{
		stc("But you do not own this room!\n\r",ch);
		return;
	}
	if ( argument[0] == '\0' )
	{
		stc("This command allows you to sell your home.\n\r",ch);
		stc("You will no longer own your home once it is sold,\n\r",ch);
		stc("and you will be compensated half the cost of the home,\n\r",ch);
		stc("not including items you have purchased.\n\r",ch);
		stc("You must be standing in the room which you own.\n\r",ch);
		stc("\n\rSyntax: Home (sell) (confirm)\n\r",ch);
		return;
	}
	else if ( !strcmp ( argument, "confirm" ) )
	{
	    free_string ( loc->owner );
	    loc->owner = str_dup ( "" );
	    ch->gold += 12500;
	    free_string ( loc->name );
	    loc->name = str_dup ( "An Abandoned Home" );

	    if ( !ch->in_room->reset_first )
	    {
			return;
		}
		/* Thanks to Edwin and Kender for the help with killing
		 * resets. Pointed out some real problems with my handling
		 * of it. Thanks a bunch guys.
		 * This is the exact way Kender put it, and it works great!
		 */
		for( current_reset = ch->in_room->reset_first; current_reset;
		    current_reset = next_reset)
		{
		   next_reset = current_reset->next;
		   free_reset_data(current_reset);
		}

		ch->in_room->reset_first = NULL;

		for ( furn = ch->in_room->contents; furn != NULL; furn = furn_next )
		{
		    furn_next = furn->next_content;
	        extract_obj( furn );
		}
		stc("Your home has been sold and you are now 12,500 gold coins richer!\n\r",ch);
        SET_BIT(loc_area->area_flags, AREA_CHANGED);
        return;
	}
	else
	{
		stc("This command allows you to sell your home.\n\r",ch);
		stc("You will no longer own your home once it is sold,\n\r",ch);
		stc("and you will be compensated half the cost of the home,\n\r",ch);
		stc("not including items you have purchased.\n\r",ch);
		stc("You must be standing in the room which you own.\n\r",ch);
		stc("\n\rSyntax: Home (sell) (confirm)\n\r",ch);
		return;
	}
}

void home_describe ( CHAR_DATA *ch )
{
    ROOM_INDEX_DATA *loc;
    AREA_DATA *loc_area;

	loc = ch->in_room;
	loc_area = ch->in_room->area;

	if ( !is_room_owner ( ch, loc ) )
	{
		stc("But you do not own this room!\n\r",ch);
		return;
	}
	else
	{
		stc("This command allows you to describe your home.\n\r",ch);
		stc("You should not describe items that are in the room,\n\r",ch);
		stc("rather allowing the furnishing of the home to do that.\n\r",ch);
		stc("If you currently own this room, you will be placed into.\n\r",ch);
		stc("the room editor. Be warned that while in the room editor,\n\r",ch);
		stc("you are only allowed to type the description. If you are\n\r",ch);
		stc("unsure or hesitant about this, please note the Immortals,\n\r",ch);
		stc("or better, discuss the how-to's with a Builder.\n\r",ch);
		stc("Syntax: Home (describe)\n\r",ch);
    	string_append( ch, &loc->description );
        SET_BIT(loc_area->area_flags, AREA_CHANGED);
		return;
	}

}
