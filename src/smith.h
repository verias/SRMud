

struct smith_items_type
{
	char *name;
	int   quantity;
	int   weight;
	int   difficulty;
	int   beats;
	int   wear_flags;
	int   item_type;
	int   base_v0;
	int   base_v1;
	int   base_v2;
	int   base_pierce;
	int   base_bash;
	int   base_slash;
	int   base_exotic;
	char *dam_noun;
};
struct smith_ores_type
{
	char *name;
	int   cost_stone;
	int   armor_mod;
	int   workability;
	char *color;
};

extern const struct smith_items_type smith_items_table[];
extern const struct smith_ores_type smith_ores_table[];