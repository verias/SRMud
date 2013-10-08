CC      = gcc
PROF    = -O -g
NOCRYPT =
C_FLAGS =  -Wall $(PROF) $(NOCRYPT)
L_FLAGS =  $(PROF)

O_FILES = act_comm.o act_enter.o act_info.o act_move.o act_obj.o act_wiz.o \
          alias.o ban.o bit.o clan.o comm.o const.o db.o db2.o effects.o \
          fight.o flags.o handler.o healer.o hedit.o homes.o interp.o \
          note.o lookup.o magic.o magic2.o mem.o marry.o mob_cmds.o starmap.o \
          mob_prog.o olc.o olc_act.o olc_mpcode.o olc_save.o org.o mount.o \
          pwipe.o quest.o remort.o send.o smith.o token.o vamp.o weather.o \
	  music.o recycle.o save.o scan.o skills.o string.o special.o tables.o \
	  update.o test.o wizlist.o social_edit.o

rom: $(O_FILES)
	rm -f rom
	$(CC) $(L_FLAGS) -o rom $(O_FILES)

.c.o: merc.h
	$(CC) -c $(C_FLAGS) -Dunix $<
