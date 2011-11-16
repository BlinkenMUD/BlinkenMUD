NAME    = BlinkenMUD
CC      = gcc
PROF    = -O -ggdb -lcrypt
C_FLAGS =  $(PROF) $(NOCRYPT) -std=gnu99 -Wall -Wformat -Wformat-security -Wunused-value \
 -Wextra -Wno-unused-parameter  
L_FLAGS =  $(PROF)

ROT_FILES = act_comm.o act_enter.o act_info.o act_move.o act_obj.o act_wiz.o \
          alias.o ban.o clans.o comm.o const.o curse.o db.o db2.o effects.o \
	  fight.o finger.o flags.o forget.o handler.o healer.o interp.o note.o \
	  lookup.o magic.o magic2.o music.o recycle.o repent.o save.o scan.o \
	  sign.o skills.o special.o tables.o update.o wizlist.o \
          mob_cmds.o mob_prog.o olc.o olc_act.o olc_save.o bit.o mem.o \
          string.o olc_mpcode.o minimap.o bank.o reward.o remort.o quest.o  \
	  teleport.o wpeace.o



all:: rot

rot: $(ROT_FILES)
	rm -f $(NAME)
	$(CC) $(L_FLAGS) -o $(NAME) $(ROT_FILES)
	@mv ./$(NAME) ../bin/

.c.o: merc.h
	$(CC) -c $(C_FLAGS) $<

clean: 
	@rm -fr *.o
	@rm -fr ../bin/$(NAME)
	@echo "BlinkenMuD has been cleaned"
