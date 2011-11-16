/*
 * Copywrite 1996 by Amadeus of AnonyMUD, AVATAR, Horizon MUD, and Despair
 *			( amadeus@myself.com )
 *
 * Public use authorized with this header intact.
 */		

/*
 * Copyright (C) 2007-2011 See the AUTHORS.BlinkenMUD file for details
 * By using this code, you have agreed to follow the terms of the   
 * ROT license, in the file doc/rot.license        
*/

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"


void do_wpeace(CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *rch;
  char buf[MAX_STRING_LENGTH];
  
  //rch = get_char( ch );
  
  for ( rch = char_list; rch; rch = rch->next )
    {
      if ( ch->desc == NULL || ch->desc->connected != CON_PLAYING )
	continue;
      
      if ( rch->fighting )
	{
	  sprintf( buf, "%s has declared World Peace.\n\r", ch->name );
	  send_to_char( buf, rch );
	  stop_fighting( rch, TRUE );
	}
    }
  
  send_to_char( "You have declared World Peace.\n\r", ch );

  return;

}
