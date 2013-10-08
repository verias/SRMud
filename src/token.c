//////////////////////////////////BEGIN TOKEN.C///////////////////////////////////////////
//  Token code written by Verias/Arleiyn of Shattered Reality/Alurien Dreams
//////////////////////////////////////////////////////////////////////////////////////////


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
#include "lookup.h"
#include "olc.h"


#define OBJ_VNUM_TOKEN   27
bool chance             args(( int num ));

bool chance(int num)
{
    if (number_range(1,100) <= num) return TRUE;
    else return FALSE;
}



OBJ_DATA *generate_token(CHAR_DATA *ch)
{
	char buf[MIL];
	OBJ_DATA *obj;
	int i, token;
	CHAR_DATA *to, *to_next;

	obj = create_object(get_obj_index(OBJ_VNUM_TOKEN),0);

	//for randomizing tokens
	for( i=0; i < MAX_TOKEN; i++)
	{

		 if(ch->inserted->value[0] < 4 && (chance(7)))
		//we win an exp token
		 {
		obj->value[0] = token_lookup("experience");
		obj->value[1] = 100;
		obj->value[2] = number_fuzzy(token_table[obj->value[0]].max_win);
		obj->value[3] = number_fuzzy(token_table[obj->value[0]].min_win);
		obj->value[4] =	0;
		obj->name = str_dup(token_table[obj->value[0]].name);
		free_string(obj->short_descr);
		obj->short_descr = str_dup(token_table[obj->value[0]].short_descr);
		free_string(obj->description);
		obj->description = str_dup(token_table[obj->value[0]].long_descr);
		obj_to_char(obj,ch);
		if(ch->inserted->value[0] < 4)
		{
		send_to_one(ch,"WooHoo!!! You won an experience token.");
		for(to = ch->in_room->people; to; to = to_next)
		{
			to_next = to->next_in_room;
			if(to != ch && to->in_room == ch->in_room)
			{
				send_to_one(to,"%s just won an experience token.",PERS(ch,to));
			}
		}
		}
		 else
		 send_to_one(ch,"You lost your %s token.",token_table[ch->inserted->value[0]].name);
		return obj;
		
	 }
	else if(ch->inserted->value[0] != 3 && (chance(20)))//20 percent chance you get a quest token
	{
			obj->value[0] = token_lookup("quest");
		obj->value[1] = number_range(0,100);
		obj->value[2] = number_fuzzy(token_table[obj->value[0]].max_win);
		obj->value[3] = number_fuzzy(token_table[obj->value[0]].min_win);
		obj->value[4] =	0;
		obj->name = str_dup(token_table[obj->value[0]].name);
		free_string(obj->short_descr);
		obj->short_descr = str_dup(token_table[obj->value[0]].short_descr);
		free_string(obj->description);
		obj->description = str_dup(token_table[obj->value[0]].long_descr);
		obj_to_char(obj,ch);
		if(ch->inserted->value[0] < 3)
		{
		send_to_one(ch,"Hot Damn! You won a quest token.");
				for(to = ch->in_room->people; to; to = to_next)
		{
			to_next = to->next_in_room;
			if(to->in_room == ch->in_room && to != ch)
			{
				send_to_one(to,"%s just won a quest token.",PERS(ch,to));
			}
		}
		}
		else if( ch->inserted->value[0] > 3)
		{
			send_to_one(ch,"Damn you just won back a quest token.");
				for(to = ch->in_room->people; to; to = to_next)
		{
			to_next = to->next_in_room;
			if(to->in_room == ch->in_room && to != ch)
			{
				send_to_one(to,"%s just won a quest token.",PERS(ch,to));
			}
		}
		}
			 else
			 send_to_one(ch,"You lost your %s token.",token_table[ch->inserted->value[0]].name);
		return obj;
		
	 }
	 else if(ch->inserted->value[0] != 2  && (chance(25))) //25 percent chance you get a train token
		 {
		 //we win a train token
		obj->value[0] = token_lookup("train");
		obj->value[1] = number_range(0,100);
		obj->value[2] = number_fuzzy(token_table[obj->value[0]].max_win);
		obj->value[3] = number_fuzzy(token_table[obj->value[0]].min_win);
		obj->value[4] =	0;
		obj->name = str_dup(token_table[obj->value[0]].name);
		free_string(obj->short_descr);
		obj->short_descr = str_dup(token_table[obj->value[0]].short_descr);
		free_string(obj->description);
		obj->description = str_dup(token_table[obj->value[0]].long_descr);
		obj_to_char(obj,ch);
		if(ch->inserted->value[0] < 2)
		{
		send_to_one(ch,"Nifty, you won a train token.");
				for(to = ch->in_room->people; to; to = to_next)
		{
			to_next = to->next_in_room;
			if(to->in_room == ch->in_room && to != ch)
			{
				send_to_one(to,"%s just won a train token.",PERS(ch,to));
			}
		}
		}
		else if( ch->inserted->value[0] > 2)
		{
			send_to_one(ch,"Shit you just won back a train token.");
				for(to = ch->in_room->people; to; to = to_next)
		{
			to_next = to->next_in_room;
			if(to->in_room == ch->in_room && to != ch)
			{
				send_to_one(to,"%s just won a train token.",PERS(ch,to));
			}
		}
		}
			 else
			 send_to_one(ch,"You lost your %s token.",token_table[ch->inserted->value[0]].name);
		return obj;
		
	 }
 	 else if(ch->inserted->value[0] != 1 && (chance(35))) //35 percent chance you get a practice token
		 {
		 //we win a prac token
		obj->value[0] = token_lookup("practice");
		obj->value[1] = number_range(0,100);
		obj->value[2] = number_fuzzy(token_table[obj->value[0]].max_win);
		obj->value[3] = number_fuzzy(token_table[obj->value[0]].min_win);
		obj->value[4] =	0;
		obj->name = str_dup(token_table[obj->value[0]].name);
		free_string(obj->short_descr);
		obj->short_descr = str_dup(token_table[obj->value[0]].short_descr);
		free_string(obj->description);
		obj->description = str_dup(token_table[obj->value[0]].long_descr);
		obj_to_char(obj,ch);
		if(ch->inserted->value[0] < 1 )
		{
				send_to_one(ch,"You won a practice token.");
		for(to = ch->in_room->people; to; to = to_next)
		{
			to_next = to->next_in_room;
			if(to->in_room == ch->in_room && to != ch)
			{
				send_to_one(to,"%s just won a practice token.",PERS(ch,to));
			}
		}
		}
		else if (ch->inserted->value[0] > 1)
		{
				send_to_one(ch,"Bah, you won back a practice token.");
		for(to = ch->in_room->people; to; to = to_next)
		{
			to_next = to->next_in_room;
			if(to->in_room == ch->in_room && to != ch)
			{
				send_to_one(to,"%s just won a practice token.",PERS(ch,to));
			}
		}
		}
			 else
			 send_to_one(ch,"You lost your %s token.",token_table[ch->inserted->value[0]].name);
		return obj;
	 }
	 else if(ch->inserted->value[0] !=0 && (chance(15)))
		 {
			 //win back a normal token
			 obj->value[0] = token_lookup("normal");
			 obj->value[1] = number_range(0,100);
			 obj->value[2] = number_range(250,400);
			 obj->value[3] = number_range(25,249);
			 obj->value[4] = 0;
			 obj->name = str_dup(token_table[obj->value[0]].name);
			 free_string(obj->short_descr);
			 obj->short_descr  = str_dup(token_table[obj->value[0]].short_descr);
			 free_string(obj->description);
			 obj->description = str_dup(token_table[obj->value[0]].long_descr);
			 obj_to_char(obj,ch);
			 if(ch->inserted->value[0] > 1)
			 {
			 send_to_one(ch,"You just won back a normal token.");
		for(to = ch->in_room->people; to; to = to_next)
		{
			to_next = to->next_in_room;
			if(to->in_room == ch->in_room && to != ch)
			{
				send_to_one(to,"%s just won a normal token.",PERS(ch,to));
			}
		}
			 }
			 else
			 send_to_one(ch,"You lost your %s token.",token_table[ch->inserted->value[0]].name);
			 return obj;
		 
	 }
	}
	return NULL;
}
	

void do_insert(CHAR_DATA *ch, char *argument)
{
	OBJ_DATA *obj, *obj_next;
	OBJ_DATA *obj2;
	OBJ_DATA *machine, *machine_next;
	int reward;
	bool found;
	CHAR_DATA *to,*to_next;

	if(argument[0] == '\0')
	{
		send_to_one(ch,"Insert what?");
		return;
	}

	if( ( obj = get_obj_carry(ch,argument,ch)) == NULL)
	{
	send_to_one(ch,"You don't have that to insert.");
	return;
	}

	found = FALSE;

	for(machine = ch->in_room->contents; machine; machine = machine_next)
	{
	machine_next = machine->next_content;
	if( machine->item_type == ITEM_TOKEN && machine->value[4] == 1)
	{
		found = TRUE;

		if(  obj->item_type != ITEM_TOKEN)
		{
			send_to_one(ch,"You can't insert that.");
			return;
		}

	//alrighty, let's gamble
		send_to_one(ch,"You insert %s into %s.",obj->short_descr,machine->short_descr);
		for(to = ch->in_room->people; to; to = to_next)
		{
			to_next = to->next_in_room;
			if(to->in_room == ch->in_room && to != ch)
			{
				send_to_one(to,"%s inserts %s into %s.",PERS(ch,to), obj->short_descr,machine->short_descr);
			}
		}
		ch->inserted = obj;
		machine->value[2] += number_range(75,125);
		if(chance(obj->value[1]))
		{
			if(chance(45) && obj->value[0] != 4)//45 percent chance you win a new token
			generate_token(ch);
			else if( chance(5) && machine->value[2] != 0
				&& obj->value[0] == 0) //5 percent chance you hit jackpot with a normal token
			{
				reward = machine->value[2];
				send_to_one(ch,"You hit the {RJ{WA{YC{BK{MP{CO{GT{x!");
				send_to_one(ch,"You win {Y%d gold!{x",reward);
		for(to = ch->in_room->people; to; to = to_next)
		{
			to_next = to->next_in_room;
			if(to->in_room == ch->in_room && to != ch)
			{
				send_to_one(to,"%s just hit the {RJ{WA{YC{BK{MP{CO{GT{x!\n\r%s won {Y%d gold{x!",PERS(ch,to),
					PERS(ch,to), reward);
			}
		}
				ch->gold += reward;
				machine->value[2] = 0;
			}
			else
			{
			reward = number_range(obj->value[3],obj->value[2]);
			send_to_one(ch,"You win %d %s!",reward, token_table[obj->value[0]].reward);
				if (token_table[obj->value[0]].value == 1)
				ch->gold += reward;
				else if(token_table[obj->value[0]].value == 2)
				ch->practice += reward;
				else if(token_table[obj->value[0]].value == 3)
				ch->train += reward;
				else if(token_table[obj->value[0]].value == 4)
				ch->pcdata->questpoints += reward;
				else if(token_table[obj->value[0]].value == 5)
				gain_exp(ch,reward);
			}
				obj_from_char(obj);
				extract_obj(obj);
		}
		else
		{
		send_to_one(ch,"You lost your %s token.",token_table[obj->value[0]].name);
		obj_from_char(obj);
		extract_obj(obj);
		return;
		}
			
	}
	}
	if(!found)
	{
			send_to_one(ch,"You see nothing to insert it into.");
			return;
	}


	return;
}

void do_token(CHAR_DATA *ch, char *argument)
{

	OBJ_DATA *obj;
	char arg1[MSL];
	char arg2[MSL];
	char arg3[MSL];
	char arg4[MSL];
	CHAR_DATA *to,*to_next;

	argument = one_argument(argument,arg1);
	argument = one_argument(argument,arg2);
	argument = one_argument(argument,arg3);

	if( arg1[0] == '\0'||
		arg2[0] == '\0' ||
		arg3[0] == '\0' ||
		argument[0] == '\0')
	{
		send_to_one(ch,"Syntax: token <type> <winchance> <max_win> <min_win>\n\r"
			"Token Types are:\n\r"
			"Normal   0\n\r"
			"Practice 1\n\r"
			"Train    2\n\r"
			"Quest    3\n\r"
			"Exp      4\n\r"
			"All arguments must be numerical.");
		return;
	}

	obj = create_object(get_obj_index(OBJ_VNUM_TOKEN),0);


	obj->value[0] = atoi(arg1);
	obj->value[1] = atoi(arg2);
	obj->value[2] = atoi(arg3);
	obj->value[3] = atoi(argument);
	obj->name = str_dup(token_table[obj->value[0]].name);
	free_string(obj->short_descr);
	obj->short_descr = str_dup(token_table[obj->value[0]].short_descr);
	free_string(obj->description);
	obj->description = str_dup(token_table[obj->value[0]].long_descr);
	obj_to_char(obj,ch);

	send_to_one(ch,"You have created a %s token.",token_table[obj->value[0]].name);
	for(to = ch->in_room->people; to; to = to_next)
	{
			to_next = to->next_in_room;
			if(to->in_room == ch->in_room && to != ch)
			{
				send_to_one(to,"%s has created %s token.",PERS(ch,to),token_table[obj->value[0]].name);
			}
	}
	return;
}
	


