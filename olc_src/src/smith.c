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
#include "olc.h"
#include "interp.h"
#include "recycle.h"
#include "tables.h"
#include "smith.h"

#define ITEM_VNUM_FORGED		3396

OBJ_DATA *generate_forged  args(( CHAR_DATA *ch, int forged));


const   struct  smith_ores_type smith_ores_table [] =
{
	{   "bronze",    10,   10, 25, "reddish-gold"  }, /* 1 */
	{   "iron",      25,   15, 22, "bright-gray"   }, /* 2 */
	{   "steel",     50,   25, 20, "silvery-gray"  }, /* 3 */
	{   "silver",    120,  8,  18,  "bright"        }, /* 4 */
	{   "gold",      225,  5,  16,  "gleaming"      }, /* 5 */
	{   "gemstone",  500,  25, 11,  "sparkling"     }, /* 6 */
	{   NULL,        0, 0, 0, NULL   }
};

const   struct  smith_items_type smith_items_table [] =
{
	{   "knife",          10, 10, 10,  5,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_DAGGER,
		1, 4, 0, 0, 0, 0, "pierce"      }, /* 1 */
	{   "cock-spur",      5,  5,  10,  4,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_DAGGER,
		1, 3, 0, 0, 0, 0, "pierce"  }, /* 2 */
	{   "stiletto",       12, 10, 15,  5,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_DAGGER,
		1, 5, 0, 0, 0, 0, "stab"	      }, /* 3 */
	{   "dagger",         15, 10, 20,  6,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_DAGGER,
		1, 6, 0, 0, 0, 0, "pierce"	      }, /* 4 */
	{   "keris",          16, 12, 22,  7,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_DAGGER,
		1, 7, 0, 0, 0, 0, "jab"	      }, /* 5 */
	{   "dirk",           16, 12, 22,  6,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_DAGGER,
		1, 7, 0, 0, 0, 0, "thrust"	      }, /* 6 */
	{   "poinard",        18, 12, 22,  6,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_DAGGER,
		1, 8, 0, 0, 0, 0, "thrust"	      }, /* 7 */
	{   "long-knife",     20, 15, 25,  6,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_DAGGER,
		1, 9, 0, 0, 0, 0, "thrust"	      }, /* 8 */
	{   "shortsword",     22, 15, 25,  7,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_DAGGER,
		1, 10, 0, 0, 0, 0, "slice"	      }, /* 9 */
	{   "langsax",        22, 15, 25,  7,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_SWORD,
		1, 11, 0, 0, 0, 0, "slice"	      }, /* 10 */
	{   "leaf-sword",     22, 16, 26,  7,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_SWORD,
		1, 12, 0, 0, 0, 0, "slice"	      }, /* 11 */
	{   "gladius",        25, 20, 30,  7,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_SWORD,
		1, 13, 0, 0, 0, 0, "stab"	      }, /* 12 */
	{   "cutlass",        26, 22, 30,  7,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_SWORD,
		1, 14, 0, 0, 0, 0, "slice"	      }, /* 13 */
	{   "sabre",          26, 22, 30,  7,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_SWORD,
		1, 15, 0, 0, 0, 0, "slash"	      }, /* 14 */
	{   "rapier",         26, 22, 32,  7,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_SWORD,
		1, 16, 0, 0, 0, 0, "pierce"	      }, /* 15 */
	{   "cinqueda",       26, 23, 32,  7,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_SWORD,
		1, 17, 0, 0, 0, 0, "slash"	      }, /* 16 */
	{   "spatha",         26, 24, 33,  7,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_SWORD,
		1, 18, 0, 0, 0, 0, "slash"	      }, /* 17 */
	{   "falchion",       27, 25, 35,  7,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_SWORD,
		1, 19, 0, 0, 0, 0, "slash"	      }, /* 18 */
	{   "scimitar",       28, 28, 35,  7,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_SWORD,
		1, 20, 0, 0, 0, 0, "slash"	      }, /* 19 */
	{   "nimcha",         30, 30, 35,  7,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_SWORD,
		1, 21, 0, 0, 0, 0, "slash"	      }, /* 20 */
	{   "hanger",         30, 30, 36,  7,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_SWORD,
		1, 22, 0, 0, 0, 0, "slash"	      }, /* 21 */
	{   "kastane",        28, 28, 38,  7,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_SWORD,
		1, 23, 0, 0, 0, 0, "slash"	      }, /* 22 */
	{   "yatagan",        28, 28, 38,  7,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_SWORD,
		1, 24, 0, 0, 0, 0, "slice"	      }, /* 23 */
	{   "longsword",      32, 32, 40,  8,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_SWORD,
		1, 25, 0, 0, 0, 0, "slash"	      }, /* 4 */
	{   "backsword",      30, 30, 42,  8,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_SWORD,
		1, 26, 0, 0, 0, 0, "slash"	      }, /* 5 */
	{   "ringsword",      32, 32, 45,  8,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_SWORD,
		1, 27, 0, 0, 0, 0, "slice"	      }, /* 6*/
	{   "broadsword",     32, 35, 45,  8,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_SWORD,
		1, 28, 0, 0, 0, 0, "slash"	      }, /* 7 */
	{   "flamberge",      38, 38, 48,  8,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_SWORD,
		1, 29, 0, 0, 0, 0, "slash"	      }, /* 8 */
	{   "bastard-sword",  45, 50, 55,  9,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_SWORD,
		1, 30, 0, 0, 0, 0, "chop"	      }, /* 9 */
	{   "greatsword",     48, 58, 60,  9,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_SWORD,
		1, 31, 0, 0, 0, 0, "smash"	      }, /* 30 */
	{   "war-sword",      50, 60, 65,  9,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_SWORD,
		1, 32, 0, 0, 0, 0, "cleave"	      }, /* 31 */
	{   "claymore",       55, 65, 70,  9,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_SWORD,
		1, 33, 0, 0, 0, 0, "sunder"	     }, /* 2 */
	{   "hand-axe",       26, 23, 32,  6,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_AXE,
		1, 14, 0, 0, 0, 0, "chop"	      }, /* 3 */
	{   "hatchet",        26, 25, 34,  6,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_AXE,
		1, 15, 0, 0, 0, 0, "chop"	     }, /* 4 */
	{   "recade",         28, 23, 35,  6,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_AXE,
		1, 16, 0, 0, 0, 0, "chop"	      }, /* 5 */
	{   "francisca",      28, 28, 34,  6,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_AXE,
		1, 22, 0, 0, 0, 0, "cleave"	      }, /* 6 */
	{   "battle-axe",     32, 32, 45,  7,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_AXE,
		1, 28, 0, 0, 0, 0, "cleave"	      }, /* 7 */
	{   "broad-axe",      35, 35, 50,  7,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_AXE,
		1, 33, 0, 0, 0, 0, "cleave"	      }, /* 8 */
	{   "war-axe",        36, 38, 55,  8,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_AXE,
		1, 40, 0, 0, 0, 0, "sunder"	      }, /* 9 */
	{   "horsemans-mace", 28, 28, 40,  5,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_MACE,
		1, 16, 0, 0, 0, 0, "pound"	      }, /* 40 */
	{   "war-hammer",     30, 30, 30,  5,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_MACE,
		1, 18, 0, 0, 0, 0, "beating"	     }, /* 41 */
	{   "morning-star",   32, 32, 30,  6,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_MACE,
		1, 22, 0, 0, 0, 0, "pound"	      }, /* 2 */
	{   "battle-hammer",  34, 34, 30,  6,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_MACE,
		1, 25, 0, 0, 0, 0, "crunch"	     }, /* 3 */
	{   "footmans-mace",  32, 30, 32,  6,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_MACE,
		1, 28, 0, 0, 0, 0, "smash"	     }, /* 4 */
	{   "angon",          10, 10, 30,  5,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_SPEAR,
		1, 20, 0, 0, 0, 0, "pierce"	      }, /* 5 */
	{   "assegai",        10, 10, 33,  5,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_SPEAR,
		1, 28, 0, 0, 0, 0, "pierce"	      }, /* 6 */
	{   "spear",          10, 10, 34,  5,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_SPEAR,
		1, 35, 0, 0, 0, 0, "pierce"	      }, /* 7 */
	{   "pilum",          10, 10, 36,  5,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_SPEAR,
		1, 36, 0, 0, 0, 0, "thrust"	     }, /* 8 */
	{   "javelin",        12, 15, 39,  6,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_SPEAR,
		1, 38, 0, 0, 0, 0, "jab"	      }, /* 9 */
	{   "lance",          15, 18, 45,  6,  ITEM_TAKE|ITEM_WIELD, ITEM_WEAPON, WEAPON_SPEAR,
		1, 40, 0, 0, 0, 0, "lunge"	      }, /* 50 */
	{   "scale-tunic",    20, 20, 75,  9,  ITEM_TAKE|ITEM_WEAR_BODY, ITEM_ARMOR, 0,
		0, 0, 20, 20, 20, 20, NULL	 }, /* 51 */
	{   "hauberk",        25, 25, 80,  10, ITEM_TAKE|ITEM_WEAR_BODY, ITEM_ARMOR, 0,
		0, 0, 23, 23, 23, 23, NULL	  }, /* 2 */
	{   "haubergeon",     35, 35, 85,  11, ITEM_TAKE|ITEM_WEAR_BODY, ITEM_ARMOR, 0,
		0, 0, 26, 26, 26, 26, NULL	  }, /* 3 */
	{   "chain-hauberk",  40, 45, 90,  12, ITEM_TAKE|ITEM_WEAR_BODY, ITEM_ARMOR, 0,
		0, 0, 30, 30, 30, 30, NULL	  }, /* 4 */
	{   "scale-hauberk",  50, 50, 95,  13, ITEM_TAKE|ITEM_WEAR_BODY, ITEM_ARMOR, 0,
		0, 0, 35, 35, 35, 35, NULL	  }, /* 5 */
	{   "chain-mail",     55, 55, 96,  14, ITEM_TAKE|ITEM_WEAR_BODY, ITEM_ARMOR, 0,
		0, 0, 39, 39, 39, 39, NULL	  }, /* 6 */
	{   "scale-mail",     60, 60, 97,  15, ITEM_TAKE|ITEM_WEAR_BODY, ITEM_ARMOR, 0,
		0, 0, 46, 46, 46, 46, NULL	  }, /* 7 */
	{   "plate-mail",     65, 65, 98,  16, ITEM_TAKE|ITEM_WEAR_BODY, ITEM_ARMOR, 0,
		0, 0, 53, 53, 53, 53, NULL	  }, /* 8 */
	{   "field-plate",    70, 70, 99,  18, ITEM_TAKE|ITEM_WEAR_BODY, ITEM_ARMOR, 0,
		0, 0, 61, 61, 61, 61, NULL	  }, /* 9 */
	{   "full-plate",     75, 75, 100, 20, ITEM_TAKE|ITEM_WEAR_BODY, ITEM_ARMOR, 0,
		0, 0, 70, 70, 70, 70, NULL	  }, /* 60 */
	{   "finger-ring",    2,  2,  20,  3,  ITEM_TAKE|ITEM_WEAR_FINGER, ITEM_ARMOR, 0,
		0, 0, 1, 1, 1, 1, NULL	}, /* 61 */
	{   "finger-band",    2,  2,  25,  3,  ITEM_TAKE|ITEM_WEAR_FINGER, ITEM_ARMOR, 0,
		0, 0, 1, 1, 1, 1, NULL	}, /* 2 */
	{   "nose-pendant",   3,  3,  35,  5,  ITEM_TAKE|ITEM_WEAR_FINGER, ITEM_ARMOR, 0,
		0, 0, 1, 1, 1, 1, NULL	}, /* 6 */
	{   "scarab",         5,  5,  75,  6,  ITEM_TAKE|ITEM_WEAR_NECK, ITEM_ARMOR, 0,
		0, 0, 1, 1, 1, 1, NULL	  }, /* 7 */
	{   "torque",         8,  8,  80,  9,  ITEM_TAKE|ITEM_WEAR_NECK, ITEM_ARMOR, 0,
		0, 0, 1, 1, 1, 1, NULL	  }, /* 8 */
	{   "brooch",         5,  5,  30,  4,  ITEM_TAKE|ITEM_WEAR_NECK, ITEM_ARMOR, 0,
		0, 0, 1, 1, 1, 1, NULL	 }, /* 9 */
	{   "pendant",        5,  5,  32,  5,  ITEM_TAKE|ITEM_WEAR_NECK, ITEM_ARMOR, 0,
		0, 0, 1, 1, 1, 1, NULL	  }, /* 70 */
	{   "necklace",       5,  5,  30,  6,  ITEM_TAKE|ITEM_WEAR_NECK, ITEM_ARMOR, 0,
		0, 0, 1, 1, 1, 1, NULL	  }, /* 71 */
	{   "medallion",      6,  6,  35,  6,  ITEM_TAKE|ITEM_WEAR_NECK, ITEM_ARMOR, 0,
		0, 0, 1, 1, 1, 1, NULL	  }, /* 2 */
	{   "fibula",         8,  8,  40,  7,  ITEM_TAKE|ITEM_WEAR_NECK, ITEM_ARMOR, 0,
		0, 0, 1, 1, 1, 1, NULL	  }, /* 4 */
	{   "neck-chain",     5,  5,  30,  6,  ITEM_TAKE|ITEM_WEAR_NECK, ITEM_ARMOR, 0,
		0, 0, 1, 1, 1, 1, NULL	 }, /* 5 */
	{   "neck-pin",       2,  2,  27,  5,  ITEM_TAKE|ITEM_WEAR_NECK, ITEM_ARMOR, 0,
		0, 0, 1, 1, 1, 1, NULL	 }, /* 6 */
	{   "amulet",         6,  6,  35,  5,  ITEM_TAKE|ITEM_WEAR_NECK, ITEM_ARMOR, 0,
		0, 0, 1, 1, 1, 1, NULL	 }, /* 7 */
	{   "choker",         6,  6,  65,  7,  ITEM_TAKE|ITEM_WEAR_NECK, ITEM_ARMOR, 0,
		0, 0, 1, 1, 1, 1, NULL	 }, /* 9 */
	{   "cufflink",       2,  2,  55,  6,  ITEM_TAKE|ITEM_WEAR_WRIST, ITEM_ARMOR, 0,
		0, 0, 1, 1, 1, 1, NULL	 }, /* 80 */
	{   "wrist-pin",      2,  2,  29,  4,  ITEM_TAKE|ITEM_WEAR_WRIST, ITEM_ARMOR, 0,
		0, 0, 1, 1, 1, 1, NULL	 }, /* 81 */
	{   "wrist-band",     8,  8,  33,  5,  ITEM_TAKE|ITEM_WEAR_WRIST, ITEM_ARMOR, 0,
		0, 0, 1, 1, 1, 1, NULL	 }, /* 2 */
	{   "armlet",         5,  5,  37,  5,  ITEM_TAKE|ITEM_WEAR_ARMS, ITEM_ARMOR, 0,
		0, 0, 1, 1, 1, 1, NULL	  }, /* 3 */
	{   "armband",        8,  8,  35,  5,  ITEM_TAKE|ITEM_WEAR_ARMS, ITEM_ARMOR, 0,
		0, 0, 1, 1, 1, 1, NULL	  }, /* 4 */
	{   "buckle",         9,  9,  72,  5,  ITEM_TAKE|ITEM_WEAR_WAIST, ITEM_ARMOR, 0,
		0, 0, 1, 1, 1, 1, NULL	}, /* 9 */
	{   "girdle",         15, 15, 80,  7,  ITEM_TAKE|ITEM_WEAR_WAIST, ITEM_ARMOR, 0,
		0, 0, 1, 1, 1, 1, NULL	 }, /* 90 */
	{   "tiara",          12, 12, 90,  12, ITEM_TAKE|ITEM_WEAR_HEAD, ITEM_ARMOR, 0,
		0, 0, 1, 1, 1, 1, NULL	 }, /* 91 */
	{   "comb",           8,  8,  58,  6,  ITEM_TAKE|ITEM_WEAR_HEAD, ITEM_ARMOR, 0,
		0, 0, 1, 1, 1, 1, NULL	  }, /* 2 */
	{   "hairpin",        4,  4,  32,  4,  ITEM_TAKE|ITEM_WEAR_HEAD, ITEM_ARMOR, 0,
		0, 0, 1, 1, 1, 1, NULL	 }, /* 3 */
	{   "diadem",         18, 18, 95,  20, ITEM_TAKE|ITEM_WEAR_HEAD, ITEM_ARMOR, 0,
		0, 0, 1, 1, 1, 1, NULL	  }, /* 4 */
	{   NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL  }  /* 101 */
};


OBJ_DATA *generate_forged(CHAR_DATA *ch, int forged)
{
//	char buf[MIL];
	OBJ_DATA *item;

	item = create_object(get_obj_index(ITEM_VNUM_FORGED),0);
	{
		item->name = str_dup(smith_items_table[forged].name);
		item->item_type = smith_items_table[forged].item_type;
		item->wear_flags = smith_items_table[forged].wear_flags;
		if(smith_items_table[forged].item_type == ITEM_WEAPON)
		{
		item->value[0] = smith_items_table[forged].base_v0;
		item->value[1] = smith_items_table[forged].base_v1;
		item->value[2] = smith_items_table[forged].base_v2;
		item->value[3] = attack_lookup(smith_items_table[forged].dam_noun);
		}
		if(smith_items_table[forged].item_type == ITEM_ARMOR)
		{
		item->value[0] = smith_items_table[forged].base_pierce;
		item->value[1] = smith_items_table[forged].base_bash;
		item->value[2] = smith_items_table[forged].base_slash;
		item->value[2] = smith_items_table[forged].base_exotic;
		}
		item->weight = smith_items_table[forged].weight;
		obj_to_char(item,ch);
		return item;
	}
	return NULL;
}

void do_forge(CHAR_DATA *ch, char *argument)
{
	char buf[MSL];
	OBJ_DATA *forged, *ore;//, *ore_next;
	int forge;
	int chance;
	bool found;
	int j;//i;
	int columns = 4;
//	int argb;

	if(IS_NPC(ch))
		return;

	if (( chance = get_skill(ch,gsn_forge)) &&
		ch->level < skill_table[gsn_forge].skill_level[ch->class])
	{
		send_to_one(ch,"Forging? What's that?");
		return;
	}

	j=1;
	if(argument[0] == '\0')
	{
		send_to_one(ch,"Forge what?");
		send_to_one(ch,"===========================================================================");
		for( forge =0; smith_items_table[forge].name != NULL; forge++)
		{
			if(get_skill(ch,gsn_forge) >= smith_items_table[forge].difficulty)
			{
				sprintf(buf,"%15s",smith_items_table[forge].name);
			send_to_char(buf,ch);
				if( j == columns) 
				{
					send_to_char("\n\r",ch);
					j = 0;
				}
				else
				j++;
			}

		}
		return;
	}

	found = FALSE;
/*	if( (ore  = get_eq_char(ch,WEAR_HOLD)) == NULL)
	{
		send_to_one(ch,"You must be holding a piece of ore.");
		return;
	}
*/

		for(ore = ch->carrying; ore != NULL; ore = ore->next_content)
		{

		if( ore->item_type == ITEM_ORE)
		{
			forge = smith_item_lookup(argument);

		
			found = TRUE;
			
		
			if(forge == -1 || ( get_skill(ch,gsn_forge) < smith_items_table[forge].difficulty))
			{
			send_to_one(ch,"That is not something you can forge.");
			return;
			}
			else
			{
				if(number_percent() < chance)
				{
				forged = generate_forged(ch, forge);
				send_to_one(ch,"You begin forging a %s",forged->name);
				free_string(forged->short_descr);
				sprintf(buf,"A %s %s",smith_ores_table[ore->value[4]].color, forged->name);
				forged->short_descr = str_dup(buf);
				strcat(buf, " is here.");
				free_string(forged->description);
				forged->description = str_dup(buf);
				obj_from_char(ore);
				extract_obj(ore);
				check_improve(ch,gsn_forge,TRUE,1);
				WAIT_STATE(ch,smith_items_table[forge].beats);
				send_to_one(ch,"You have successfully forged a %s",forged->name);
				return;
				}
				else
				{
					send_to_one(ch,"You begin forging a %s",argument);
					WAIT_STATE(ch,smith_items_table[forge].beats *3/2);
				send_to_one(ch,"Due to your lack of experience you fail miserably.");
				check_improve(ch,gsn_forge,FALSE,1);
				obj_from_char(ore);
				extract_obj(ore);
				return;
				}
			}
		}
		}
		
			if(!found)
			{
				send_to_one(ch,"You must forge from an ore.");
			return;
			}

	
	return;
}