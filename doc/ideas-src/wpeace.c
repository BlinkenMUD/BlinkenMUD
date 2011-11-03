This one is so simple, it's almost embarrasing :) ...IMMORTAL command that
when executed stops all fighting.  Think of it as 'at all peace'.

Amadeus
-------------

/*
 * I don't think ROM muds have this particular function, if you do...
 * just don't include it :)
 */
CHAR_DATA *get_char( CHAR_DATA *ch )
{
    if ( !ch->pcdata )
        return ch->desc->original;
    else
        return ch;
}


/*
 * Copywrite 1996 by Amadeus of AnonyMUD, AVATAR, Horizon MUD, and Despair
 *			( amadeus@myself.com )
 *
 * Public use authorized with this header intact.
 */		
void do_wpeace(CHAR_DATA *ch, char *argument )
{
     CHAR_DATA *rch;
     char buf[MAX_STRING_LENGTH];

     rch = get_char( ch );

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
