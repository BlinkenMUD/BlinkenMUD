From:        George Zolas, zolasg@weiss.che.utexas.edu

Here is my teleport room code: 

Enjoy!!!


***************************************************************************
*
I got the deathtrap room code off of ftp.game.org in a merc directory, I
think.  the teleport code is based very much on it, 


/*************************************************************************
* Dyre MUD Deathtrap 1.0 was written in 1996.                            *
* Based on Helix's Deathtrap code for Merc.                              *
* There is no copyright, just give us credit!                            *
* Dyre MUD is brought to you by :                                        *
*  George Zolas     - Froth                                              *
*  Nathan Lefebvre  - Pinto                                              *
*  Joe Fresch       - JEB                                                *
*  Jonathan Gray    - Hordar                                             *
*                                                                        *
* We all stayed up really, really late for this thing                    *
*                                      ...please don't screw us.         *
*                                                                        *
*************************************************************************/

make these additions to  update.c
to get the do_look and other functions....

#include "interp.h"

in update_handler, declare:

    static  int	    pulse_tele;

and add:

    if ( --pulse_tele <= 0 )
    {
	pulse_tele	= PULSE_TELEPORT;
	tele_update	( );
    }


add the function prototype :

void	tele_update	args( ( void ) );

at the top with the other updates...
and the function:

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
        	if ( ch->in_room->tele_dest == 0 )
		  pRoomIndex = get_random_room (ch);
		else
		  pRoomIndex = get_room_index(ch->in_room->tele_dest);

		send_to_char ("You have been teleported!!!\n\r", ch);
		act("$n vanishes!!!\n\r", ch, NULL, NULL, TO_ROOM);
		char_from_room(ch);
		char_to_room(ch, pRoomIndex);
		act("$n slowly fades into existence.\n\r", ch, NULL, NULL,
TO_ROOM);
		do_look(ch, "auto");
	    }
	}
}


Changes to merc.h:

add

#define ROOM_TELEPORT   ##### ( free letter, I used G )
#define PULSE_TELEPORT		  (20 * PULSE_PER_SECOND) /* Froth */

and add the following cariable to struct room_index_data:

int tele_dest;


changes in db.c, 

change line in load_rooms function from :
	/* Area number */		  fread_number( fp ); 

( this line just skips the zero and doesn't do anything with it!!! 
wasteful!

to:
	pRoomIndex->tele_dest		= fread_number( fp ); 
        /* teleport destination room vnum */

It's easier than adding another fread line and adding a new field to the
area files.  This works by using that unused bit in the area file, the one
they tell you to set to 0.  If it's zero, and the room is a teleport room,
the desination is random, if it is nonzero, it is the vnum of the room you
teleport to.

add an extra descrip to the room, like this:

E
tele~
You are surrounded by an eerie glowing mist.  It glows bright, and you 
black
out for a second!!
~

or something, as when you are teleported, the function does a look to see
the  extra description for the word "tele".  This allows you to customize
the teleport message for each room!  I really did do some heavy borrowing
from the deathtrap room for this idea, but it's all good code, right?  
<GRIN?

Enjoy it, and let me know if you have any problems.

*********************************************************
*                     George Zolas                      *
*              zolasg@weiss.che.utexas.edu              *
*    Froth of DyreMUD - vanwinkle.che.utexas.edu 2222   *
*                    Testing only                       *
********************************************************* 
