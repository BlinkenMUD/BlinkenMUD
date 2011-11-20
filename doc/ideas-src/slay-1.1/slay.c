/* -----------------------------------------------------------------------
The following snippet was written by Gary McNickle (dharvest) for
Rom 2.4 specific MUDs and is released into the public domain. My thanks to
the originators of Diku, and Rom, as well as to all those others who have
released code for this mud base.  Goes to show that the freeware idea can
actually work. ;)  In any case, all I ask is that you credit this code
properly, and perhaps drop me a line letting me know it's being used.

from: gary@dharvest.com
website: http://www.dharvest.com
or http://www.dharvest.com/resource.html (rom related)

Send any comments, flames, bug-reports, suggestions, requests, etc... 
to the above email address.
----------------------------------------------------------------------- */



/** Function: do_slay
  * Descr   : Slays (kills) a player, optionally sending one of several
  *           predefined "slay option" messages to those involved.
  * Returns : (void)
  * Syntax  : slay (who) [option]
  * Written : v1.0 12/97
  * Author  : Gary McNickle <gary@dharvest.com>
  */
void do_slay( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char type[MAX_INPUT_LENGTH];
    char who[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int i=0;
    bool found=FALSE;

    argument = one_argument(argument, who);
    argument = one_argument(argument, type);

    if ( !str_prefix(who, "list") || who == NULL )
    {
      send_to_char("\n\rSyntax: slay [who] <type>\n\r", ch);
      send_to_char("where type is one of the following...\n\r\n\r", ch);

      for ( i=0; i < MAX_SLAY_TYPES-1; i++ )

        if ( (slay_table[i].owner == NULL)               ||
             (!str_prefix(slay_table[i].owner,ch->name)  &&
              slay_table[i].title[0] != '\0') )
        {
           sprintf(buf, "%s\n\r", slay_table[i].title);
           send_to_char(buf, ch);
        }

       send_to_char(
          "\n\rTyping just 'slay <player>' will work too...\n\r",ch);
       return;
     }

     if ( ( victim = get_char_room( ch, who ) ) == NULL )
     {
        send_to_char( "They aren't here.\n\r", ch );
        return;
     } else

     if ( ch == victim )
     {
        send_to_char( "The creator has fixed his cannon against self slaughter!\n\r", ch)
        return;
     } else

     if ( !IS_NPC(victim) && get_trust( victim ) >= get_trust( ch ) )
     {
        send_to_char( "You failed.\n\r", ch );
        return;
     } else

     if ( type[0] == '\0' )
       {
          raw_kill(victim);
          return;
       }
     else

     for (i=0; i < MAX_SLAY_TYPES; i++)
     {
       if (
            !str_prefix(type, slay_table[i].title) &&
             ( slay_table[i].owner ==  NULL        ||
              !str_prefix(slay_table[i].owner, ch->name) )
          )
          {
             found=TRUE;
             sprintf(buf, "%s\n\r", slay_table[i].char_msg);
             act(buf, ch, NULL, victim, TO_CHAR );
             sprintf(buf, "%s\n\r", slay_table[i].vict_msg);
             act(buf, ch, NULL, victim, TO_VICT );
             sprintf(buf, "%s\n\r", slay_table[i].room_msg);
             act(buf, ch, NULL, victim, TO_NOTVICT );
             raw_kill(victim);
             return;
          }
       }

    if (!found)
      send_to_char("Slay type not defined. Type \"slay list\" for a complete listing of types available to you.\n\r", ch);

    return;
} /* end of func: "do_slay" */

