
/***************************************************************************
*       FALLEN ANGELS mud is protected by french law of intelectual        *
*                            property. we share freely all we have coded   *
*                            provided this message isn't removed from the  *
*                            files and you respect the name of all the     *
*                            coders,area builders, and all the Diku,Merc,  *
*                            Rom licences.                                 *
*                                                                          *
*                   Thank to : ROM consortium .                            *
*                   Big thank to : Gary Gygax !!!!!!!!!!!!!!               *
*                                                                          *
*       Fallen Angel project by : Loran      ( laurent zilbert )           *
*                                 Silfen or                                *
*                                 Gwendoline ( jerome despret  )           *
*                                                                          *
*       Despret@ecoledoc.lip6.fr ...                                       *
***************************************************************************/

/***************************************************************************
*                                                                          *
*  To use this snipet you must set the following line in the "throw" help  *
*                                                                          *
*    Coded for Fallen Angels by : Zilber laurent,Despret jerome.           *
*                                                                          *
*  And send a mail to say you use it ( feel free to comment ) at :         *
*                                                                          *
*  [despret@ecoledoc.lip6.fr] or/and at [loran@hotmail.com]                *
****************************************************************************/

/***************************************************************************
*                                                                          *
* If you want to put this snipet on your web site you are allowed to but   *
*  the file must remain unchanged and you have to send us a mail at :      *
*                                                                          *
*  [despret@ecoledoc.lip6.fr] or/and at [loran@hotmail.com]                *
*  with the site URL.                                                      *
*                                                                          *
***************************************************************************/


#if defined(macintosh)
#include <types.h>
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "db.h"
#include "recycle.h"
#include "magic.h"

/* command procedures needed */

DECLARE_DO_FUN(do_say		);

void    wear_obj        args( (CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace ) );

/***************************************************************************/
/*                                                                         */
/*         One ROOM RANGED ATTACK system ( the throw )                     */
/*                                                                         */
/***************************************************************************/

/* _________________________________________________________________________

  Of course you have to create a gsn_throw and associate it to do_throw then:

  You have to create an ITEM_THROWING item type it should look like the 
  following example :


#10002
dart~
a strange dart~
You see a little dart here.~
wood~
throwing 0 AO
5 6 pierce 15 'fireball'
0 10 210 P

v0: is the number of dice dammage
v1: is the number of side of each dice
v3: the type of dammage ( same as weapon's ones )

v4: usualy filled with a 0 but hightly powered dart can be filled with
    a spell effect if so this hold the level of the spell.
v5: hold the spell, if you wanna make a clumsy dart fill it with
    a benefit spell.. would you throw a spear that make a heal on
    the victim... dart and flask are't the same on our mud
    flask are more powerful, but dart is more a warrior skill to make
    distant attack however both use throw command... the code make
    the difference between classes using flask and throwing weapon.

_________________________________________________________________________ */





/***************************************************************************

  The throwing code allow now a one room ranged target.
  following things are tested and forbid.
     throw trought a closed door
     targeting a sentinel mob
     targeting a mob that isn't allowed to enter the ch room
     targeting a mob in another area ( specialy for frontier ) 

****************************************************************************/

void do_throw( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj,*obj_next;
    ROOM_INDEX_DATA *was_in_room;
    EXIT_DATA *pexit;
    char buf[256];
    int damm,damm_type,door,outside,i;

    outside=0;  /* used to make mob move */

    one_argument( argument, arg );
    if ( arg[0] == '\0' && ch->fighting == NULL )
    {
        send_to_char( "Throw on whom or what?\n\r", ch );
        return;
    }

    if ( ( obj = get_eq_char( ch, WEAR_HOLD ) ) == NULL )
    {
        send_to_char( "You hold nothing in your hand.\n\r", ch );
        return;
    }

    if ( obj->item_type != ITEM_THROWING )
    {
        send_to_char( "You can throw only a throwing weapons.\n\r", ch );
        return;
    }

    if ( arg[0] == '\0' )
    {
        if ( ch->fighting != NULL )
        {
            victim = ch->fighting;
        }
        else
        {
            send_to_char( "Throw on whom or what?\n\r", ch );
            return;
        }
    }
    else
    {
        /* try to use 1 range victim allowance */
        /* look if victim is in the room if note look the suroundings rooms */

        if ( ( victim = get_char_room ( ch, arg ) ) == NULL)
        {
            was_in_room=ch->in_room;

            for( door=0 ; door<=5 && victim==NULL ; door++ )
             { 
                if ( (  pexit = was_in_room->exit[door] ) != NULL
                   &&   pexit->u1.to_room != NULL
                   &&   pexit->u1.to_room != was_in_room 
                   &&   !strcmp( pexit->u1.to_room->area->name ,
                                was_in_room->area->name ) )
                   { 
                     ch->in_room = pexit->u1.to_room;
                     victim = get_char_room ( ch, arg ); 
                    }
               
              }

            ch->in_room=was_in_room;
            if(victim==NULL)
              {
               send_to_char( "You can't find it.\n\r", ch );
               return;
               }
            else
              {  if(IS_SET(pexit->exit_info,EX_CLOSED))
                    { send_to_char("You can't throw through a door !!",ch);
                      return;
                     } 

                 outside=1; /* target was outside of the room */
            
                 /* forbid target that cannot move back to revenge on
                    the thrower */

              if(IS_NPC(victim))
               {
                 if ( IS_SET(victim->act, ACT_SENTINEL)
                   || IS_SET(ch->in_room->room_flags, ROOM_NO_MOB)
                   || ( IS_SET(victim->act, ACT_OUTDOORS)
                     && IS_SET(ch->in_room->room_flags,ROOM_INDOORS))
                   || ( IS_SET(victim->act, ACT_INDOORS)
                     && IS_SET(ch->in_room->room_flags,ROOM_INDOORS)))
                     { 
                       act("$N avoid your deadly throw !!!",ch,obj,victim,TO_CHAR);
                       was_in_room=ch->in_room;
                       ch->in_room=victim->in_room;
                       act("$N avoid $p thrown by $n.",ch,obj,victim,TO_NOTVICT);
                       act("You avoid $p thrown by $n.",ch,obj,victim,TO_VICT);
                       ch->in_room=was_in_room;
                       extract_obj(obj);
                       return;
                      }
                 }
             }
        }
    }

/* this is a strong attack i recomande this wait state */

WAIT_STATE( ch, 2 * PULSE_VIOLENCE ); 

sprintf(buf,"You have no more of %s.\n\r",obj->short_descr);

/* ITEM THROWING */

 if(obj->item_type== ITEM_THROWING )
   {
        
      /* check if there is a victim */
        
        if ( victim != NULL )
        {
            act( "$n throw $p on $N.", ch,   obj, victim, TO_NOTVICT );
            act( "You throw $p on $N.", ch,   obj, victim, TO_CHAR );
            act( "$n throw $p on you.",ch,   obj, victim, TO_VICT );
            if(outside)
              { was_in_room=ch->in_room;
                ch->in_room=victim->in_room;
                act( "$n throw $p on $N.", ch,   obj, victim, TO_NOTVICT );
                ch->in_room=was_in_room;
              }
        }

      /* throw the dices :) */
      
        if (ch->level <   obj->level
        ||  number_percent() >= 20 + get_skill(ch,gsn_throw) * 4/5 - (10 * outside) )
        {       
             /* it is a normal miss */
                     
              act( "You throw $p aimlessly on the ground and it broke up.",
                 ch,  obj,NULL,TO_CHAR);
              act( "$n throw $p aimlessly on the ground and it broke up.",
                 ch,  obj,NULL,TO_ROOM);
            
          check_improve(ch,gsn_throw,FALSE,2);
        }
        else
        {      
               /* it is a success hit the target */

            damm      =  dice(obj->value[0],obj->value[1]);
            damm_type =  TYPE_HIT;
            damm_type += attack_table[obj->value[2]].damage;

            damage( ch, victim, damm, damm_type,gsn_throw,TRUE); 

            if(   (obj->value[3] != 0)
               && (obj->value[4] != 0)
               && (victim->position > POS_DEAD) )  
            obj_cast_spell(   obj->value[4],   obj->value[3], ch, victim, obj);

            check_improve(ch,gsn_throw,TRUE,2);
        }

      
      /* look in the inventory for an objet of the same key_word and that is a throwing  */
            
    for(obj_next=ch->carrying; obj_next != NULL 
                               && (    (obj_next->item_type != ITEM_THROWING  ) 
                                    || ( obj == obj_next )
                                    || (strcmp(obj->name, obj_next->name) ) ); 
                               obj_next=obj_next->next_content );
      
    extract_obj(obj);

  /* if found equip the ch with */
    
   if (  obj_next == NULL )
    {
      send_to_char( buf, ch );
    }
   else
    {
      wear_obj( ch, obj_next, TRUE ); 
     }

   } 

/* Routine that make a ranged mob moving 50% 
   to attack the aggressor ..... i m looking for a way
   to make the mob wait a little before doing it         */


if(   outside 
   && (victim != NULL )
   && (victim->position > POS_STUNNED) )
  {  
            pexit = victim->in_room->exit[0];

            for( door=0 ; door<=5 ; door++ )
             {

                  pexit = victim->in_room->exit[door];
                  if(  pexit != NULL
                       && (pexit->u1.to_room == ch->in_room) )
                       break;
              }

        if(door>=6)
           { 
             bug("no back way in throw....",0);
             return;
           }

    if(IS_NPC(victim) && (number_percent() > 50 ) )
     {
        if ( !IS_SET(ch->act, ACT_SENTINEL)
        && ( pexit = victim->in_room->exit[door] ) != NULL
        &&   pexit->u1.to_room != NULL
        &&   !IS_SET(pexit->exit_info, EX_CLOSED)
        &&   !IS_SET(pexit->u1.to_room->room_flags, ROOM_NO_MOB)
        && ( !IS_SET(victim->act, ACT_OUTDOORS)
        ||   !IS_SET(pexit->u1.to_room->room_flags,ROOM_INDOORS))
        && ( !IS_SET(victim->act, ACT_INDOORS)
        ||   IS_SET(pexit->u1.to_room->room_flags,ROOM_INDOORS))
        && ( IS_NPC(victim) )
        && ( (victim->position != POS_FIGHTING) 
             || ( victim->fighting == ch )  ) )
        {

            if( (victim->position == POS_FIGHTING)
             && ( victim->fighting == ch ) )
              {  stop_fighting( victim, TRUE );
                }

            move_char( victim, door, FALSE );

            (victim)->wait = UMAX((victim)->wait, (8));
            act("$N scream and attack $n !!!",ch,NULL,victim,TO_NOTVICT);
            act("$N scream and attack You !!!",ch,NULL,victim,TO_CHAR);
            multi_hit( victim, ch, TYPE_UNDEFINED );
         }
      }
     else 
      { 
         switch(door) 
              { case 0 : sprintf(buf,"The throw came from NORTH !!!\n\r");
                         break;
                case 1 : sprintf(buf,"The throw came from EAST !!!\n\r");
                         break;
                case 2 : sprintf(buf,"The throw came from SOUTH !!!\n\r");
                         break;
                case 3 : sprintf(buf,"The throw came from WEST !!!\n\r");
                         break;
                case 4 : sprintf(buf,"The throw came from UP !!!\n\r");
                         break;
                case 5 : sprintf(buf,"The throw came from DOWN !!!\n\r");
                         break;
                default : sprintf(buf,"Throw ERROR award an IMM\n\r");
                          break;
               }
           send_to_char(buf,victim);
         }
   }

    return;
}
