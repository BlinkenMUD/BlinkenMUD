
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

/*
 * Copyright (C) 2007-2011 See the AUTHORS.BlinkenMUD file for details
 * By using this code, you have agreed to follow the terms of the   
 * ROT license, in the file doc/rot.license        
*/

#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "db.h"
#include "recycle.h"
#include "magic.h"

/* command procedures needed */

DECLARE_DO_FUN(do_say);

void wear_obj (CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace );


/***************************************************************************/
/*                                                                         */
/*         One ROOM RANGED ATTACK system ( the throw )                     */
/*                                                                         */
/***************************************************************************/

/* _________________________________________________________________________

  Of course you have to create a gsn_throw and associate it to do_throw then:

  You have to create an ITEM_THROWING item type it should look like the 
  following example :


  v0: The dice damage.            number
  v1: The sides of the dice.      number
  v2: Attack type.                string
  v3: Spell.                      string
  v4: Spell level.                number

  oedit create <vnum>
  name dart
  short dart
  long dart
  desc dart
  material wood
  type throwing
  wield hold take
  level 10
  cost 10
  weight 1

  v0 10
  v1 6
  v2 pierce
  v3 acid blast
  v4 30
 _________________________________________________________________________ */





/***************************************************************************

  The throwing code allow now a one room ranged target.
  following things are tested and forbid.
     throw trought a closed door
     targeting a sentinel mob
     targeting a mob that isn't allowed to enter the ch room
     targeting a mob in another area ( specialy for frontier ) 

****************************************************************************/

void
do_throw( CHAR_DATA *ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  OBJ_DATA *obj,*obj_next;
  ROOM_INDEX_DATA *was_in_room;
  EXIT_DATA *pexit;
  char buf[256];
  int damm,damm_type,door,outside;

  outside=0;  /* used to make mob move */

  one_argument( argument, arg );
  if ( arg[0] == '\0' && ch->fighting == NULL )
    {
      send_to_char( "Throw at whom or what?\n\r", ch );
      return;
    }

  if ( ( obj = get_eq_char( ch, WEAR_HOLD ) ) == NULL )
    {
      send_to_char( "You hold nothing in your hand.\n\r", ch );
      return;
    }

  if ( obj->item_type != ITEM_THROWING )
    {
      send_to_char( "You can throw only a throwing {!weapon{x.\n\r", ch );
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
	  send_to_char( "Throw at whom or what?\n\r", ch );
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
	    {
	      if(IS_SET(pexit->exit_info,EX_CLOSED))
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
		      act("$N {2avoids{x your deadly throw !!!",ch,obj,victim,TO_CHAR);
		      was_in_room=ch->in_room;
		      ch->in_room=victim->in_room;
		      act("$N {2avoid{x $p thrown by $n.",ch,obj,victim,TO_NOTVICT);
		      act("You {2avoid{x $p thrown by $n.",ch,obj,victim,TO_VICT);
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

  sprintf(buf,"That was your last %s.\n\r",obj->short_descr);

  /* ITEM THROWING */

  if(obj->item_type== ITEM_THROWING )
    {
        
      /* check if there is a victim */
        
      if ( victim != NULL )
        {
	  act( "$n throws $p at $N.", ch,   obj, victim, TO_NOTVICT );
	  act( "You throw $p at $N.", ch,   obj, victim, TO_CHAR );
	  act( "$n throws $p at {1you{x.",ch,   obj, victim, TO_VICT );
	  if(outside)
	    { was_in_room=ch->in_room;
	      ch->in_room=victim->in_room;
	      act( "$n throws $p at $N.", ch,   obj, victim, TO_NOTVICT );
	      ch->in_room=was_in_room;
	    }
        }

      /* throw the dices :) */
      
      if (ch->level <   obj->level
	  ||  number_percent() >= 20 + get_skill(ch,gsn_throw) * 4/5 - (10 * outside) )
        {       
	  /* it is a normal miss */
                     
	  act( "You throw $p aimlessly on the ground and it {!broke{x.",
	       ch,  obj,NULL,TO_CHAR);
	  act( "$n throw $p aimlessly on the ground and it {!broke{x.",
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

	      move_char( victim, door, FALSE, FALSE);

	      (victim)->wait = UMAX((victim)->wait, (8));
	      act("$N screams and attacks $n !!!",ch,NULL,victim,TO_NOTVICT);
	      act("$N screams and attacks {!You{x!!!",ch,NULL,victim,TO_CHAR);
	      multi_hit( victim, ch, TYPE_UNDEFINED );
	    }
	}
      else 
	{ 
	  switch(door) 
	    { case 0 : sprintf(buf,"The throw came from {6NORTH{x !!!\n\r");
		break;
	    case 1 : sprintf(buf,"The throw came from {6EAST{x !!!\n\r");
	      break;
	    case 2 : sprintf(buf,"The throw came from {6SOUTH{x !!!\n\r");
	      break;
	    case 3 : sprintf(buf,"The throw came from {6WEST{x !!!\n\r");
	      break;
	    case 4 : sprintf(buf,"The throw came from {6UP{x !!!\n\r");
	      break;
	    case 5 : sprintf(buf,"The throw came from {6DOWN{n !!!\n\r");
	      breakx
	    default : sprintf(buf,"Throw {3ERROR{x tell an IMM\n\r");
	      break;
	    }
	  send_to_char(buf,victim);
	}
    }

  return;
}
