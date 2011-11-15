/*
 * Copyright (C) 2007-2011 See the AUTHORS.BlinkenMUD file for details
 * By using this code, you have agreed to follow the terms of the   
 * ROT license, in the file doc/rot.license        
*/


#include <sys/types.h>
#include <sys/time.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "interp.h"

ROOM_INDEX_DATA * tele_random_room (CHAR_DATA * ch);

void tele_update ( void ) 
{
  CHAR_DATA 	*ch;
  CHAR_DATA	*ch_next;
  ROOM_INDEX_DATA *pRoomIndex;
  
  for (ch = char_list ; ch != NULL; ch = ch_next )
    {
      ch_next = ch->next;
      if ( IS_SET(ch->in_room->room_flags, ROOM_TELEPORT ) )
	{
	  do_look ( ch, "tele" );
	  if ( ch->in_room->tele_dest <= 0 )
	    pRoomIndex = tele_random_room (ch);
	  else
	    pRoomIndex = get_room_index(ch->in_room->tele_dest);
	  send_to_char ("You have been teleported!!!\n\r", ch);
	  act("$n vanishes!!!\n\r", ch, NULL, NULL, TO_ROOM);
	  char_from_room(ch);
	  char_to_room(ch, pRoomIndex);
	  act("$n slowly fades into existence.\n\r", ch, NULL, NULL, TO_ROOM);
	  do_look(ch, "auto");
	}
    }
}

/* random room generation procedure */
ROOM_INDEX_DATA *
tele_random_room (CHAR_DATA * ch)
{
  ROOM_INDEX_DATA *room;

  for (;;)
    {
      room = get_room_index (number_range (0, 65535));
      if (room != NULL)
	if (can_see_room (ch, room)
	    && !room_is_private (ch, room)
	    && !IS_SET (room->room_flags, ROOM_PRIVATE)
	    && !IS_SET (room->room_flags, ROOM_SOLITARY)
	    && !IS_SET (room->room_flags, ROOM_LOCKED)
	    && !IS_SET (room->room_flags, ROOM_SAFE)
	    && (IS_NPC (ch) || IS_SET (ch->act, ACT_AGGRESSIVE)
		|| !IS_SET (room->room_flags, ROOM_LAW)))
	  break;
    }

  return room;
}
