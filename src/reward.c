/* * * * * * * * * * * * * *    reward.c    * * * * * * * * * * * * * * *
*                                                                       *
*    Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,    *
*   Michael Seifert, Hans-Henrik Stæfeldt, Tom Madsen and Katja Nyboe   *
*                                                                       *
*         Merc Diku Mud improvements copyright (C) 1992, 1993 by        *
*            Michael Chastain, Michael Quan, and Mitchell Tse           *
*                                                                       *
*         ROM 2.4 is copyright 1993-1998 Russ Taylor                    *
*         ROM has been brought to you by the ROM consortium:            *
*               Russ Taylor (rtaylor@hypercube.org)                     *
*               Gabrielle Taylor (gtaylor@hypercube.org)                *
*               Brian Moore (zump@rom.org)                              *
*                                                                       *
*    reward.c and associated patches copyright 2001 by Sandi Fallon     *
*                                                                       *
*  In order to use any part of this ROM Merc Diku code you must comply  *
*  the original Diku license in 'license.doc' as well the Merc license  *
*  in 'license.txt' and also the ROM license in 'rom.license', each to  *
*  be found in doc/. Using the reward.c code without conforming to the  *
*  requirements of each of these documents is violation of any and all  *
*  applicable copyright laws. In particular, you may not remove any of  *
*  these copyright notices or claim other's work as your own.           *
*                                                                       *
*    Much time and thought has gone into this software you are using.   *
*            We hope that you share your improvements, too.             *
*                   "What goes around, comes around."                   *
*                                                                       *
* * tabs = 4 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

/* imported stuff */
DECLARE_DO_FUN( do_say );
DECLARE_DO_FUN( do_emote );


/* adapted from healer.c */
CHAR_DATA *find_captain( CHAR_DATA *ch )
{
	CHAR_DATA *captain;

	for( captain = ch->in_room->people; captain; captain = captain->next_in_room )
	{
		if( IS_NPC(captain) && IS_SET(captain->act, ACT_BOUNTY) )
		break;
	}
	if( captain == NULL )
	{
		return NULL;
	}
	if( !can_see( captain, ch ) )
	{
		send_to_char( "Huh? I hear something, but see no one!\n\r", ch );
		return NULL;
	}
	if( (IS_NPC(ch) || IS_SET(ch->act,ACT_PET)) )
	{
	  	act( "$n waves $s hand at $N.", captain, NULL, ch, TO_ROOM );
		do_say( captain, "I don't have time for animals!" );
		return NULL;
	}
	return captain;
}

/* adapted from healer.c */
CHAR_DATA *find_rewarder( CHAR_DATA *ch )
{
	CHAR_DATA *rewarder;

	for( rewarder = ch->in_room->people; rewarder; rewarder = rewarder->next_in_room )
	{
		if( IS_NPC(rewarder) && rewarder->pIndexData->vnum == ch->pcdata->rewarder )
		break;
	}
	if( rewarder == NULL )
	{
		return NULL;
	}
	if( !can_see( rewarder, ch ) )
	{
		send_to_char( "Huh? I hear something, but see no one!\n\r", ch );
		return NULL;
	}
	if( (IS_NPC(ch) || IS_SET(ch->act,ACT_PET)) )
	{
	  	act( "$n waves $s hand at $N.", rewarder, NULL, ch, TO_ROOM );
		do_say( rewarder, "I don't have time for animals!" );
		return NULL;
	}
	return rewarder;
}

/* this is where we pretend to decrement a counter - Fallon */
/* the magic number 720 is the number of seconds in 12 minutes */
void reset_hunt( CHAR_DATA *ch )
{
	int diff = 0;

	if( ( diff = (current_time - ch->pcdata->hunt_time) ) < 720 )
	{
		if( diff < 0 )
		{
			ch->pcdata->recovery = current_time + 720;
		}
		else
		{
			ch->pcdata->recovery = current_time + 720 - diff;
		}
	}

	else
	{
		ch->pcdata->recovery  = 0;
		ch->pcdata->hunt_time = 0;
	}

	ch->pcdata->rewarder   = 0;
	ch->pcdata->bounty_mob = 0;
	ch->pcdata->bounty_obj = 0;
}

/* adapted from Furey's mfind() code */
void bounty( CHAR_DATA *ch, CHAR_DATA *captain )
{
	char 			buf[MAX_STRING_LENGTH];
	char 			first[MAX_INPUT_LENGTH];
	char const 		*prep;
	char			*where;
	ROOM_INDEX_DATA *room;
	CHAR_DATA 		*victim = NULL;
	CHAR_DATA		*guard;
	OBJ_DATA 		*obj = NULL;
	OBJ_DATA 		*obj_next;
	int vnum 		= 0;
	bool obj_found	= 0;
	bool aggie_found;


	/* start with a random number */
	/* note that the condition in the for() will never be met, so be careful
	 * about changing things here or you'll have an infinite loop.
	 */
	for ( vnum = 1202; vnum < 30001; vnum = number_range(100, 30000) )
	{
		/* make it a random room */
		if ( ( room = get_room_index( vnum ) ) != NULL )
		{
			/* go through the mobs in the room and check for aggies */
			aggie_found = FALSE;
			for ( victim = room->people; victim != NULL; victim = victim->next_in_room )
			{
				if ( IS_NPC(victim) && IS_SET(victim->act, ACT_AGGRESSIVE) && victim->level > ch->level )
				{
					aggie_found = TRUE;
					break;
				}
			}

			if ( !aggie_found )
			{
				/* go through the mobs in the room and check their level and flags */
				for ( victim = room->people; victim != NULL; victim = victim->next_in_room )
				{
					if ( IS_NPC(victim)
					&&   ( IS_NEUTRAL(victim)  ||  (ch->alignment > 0 ? IS_EVIL(victim) : IS_GOOD(victim)) )
					&&   ch->level + (ch->level / 4) >= victim->level
					&&   ch->level - (ch->level / 2) <= victim->level
					&&   !IS_SET(victim->act, ACT_TRAIN)
					&&   !IS_SET(victim->act, ACT_PRACTICE)
					&&   !IS_SET(victim->act, ACT_IS_HEALER)
					&&   !IS_SET(victim->act, ACT_IS_CHANGER)
					/*   this idea is from Addams: add IMM_SUMMON to all the healers, shopkeeps,
					 *   trainers, school mobs, etc. that you don't want to be hunted.
					 *   they really should be, anyway. Otherwise, uncomment the code above. */
					&&   !IS_SET(victim->imm_flags, IMM_SUMMON)
					&&   !IS_SET(victim->affected_by, AFF_CHARM) )
					{
						/* see if the mob has an object */
						/* you might want to add a check for wear_loc - the following
						*  includes inventory but without 'peek' they can only see what
						*  the mob is wearing. This does, however, give peek more value.
						*/
						obj_found = FALSE;
						for (obj = victim->carrying; obj != NULL; obj= obj_next)
						{
							obj_next = obj->next_content;

							if ( !IS_SET(obj->extra_flags, ITEM_INVENTORY)
							&&   !IS_SET(obj->extra_flags, ITEM_ROT_DEATH)
							&&    IS_SET(obj->wear_flags,  ITEM_TAKE) )
							{
								obj_found = TRUE;
								break;
							}
						}
					}
					if ( obj_found )
					break;
				}
			}
		}
		if ( obj_found )
		break;
	}

	if ( !obj_found )
	{
		sprintf( buf, "Right now, %s, there are no rewards worthy of your talents.", ch->name );
		do_say( captain, buf );
		return;
	}

	/* make sure we didn't screw up - if any of these are missing, we go boom */
	if ( ( room = get_room_index( vnum ) ) == NULL || obj == NULL || victim == NULL )
	{
		bug( "Reward: found NULL in bounty.", 0 );
		return;
	}

	/* you need to either commit to fixing capitalised articles on mob->short in
	 * all the areas, or make it LOWER(victim->short_descr) in the 4th argument */
	switch ( obj->item_type )
	{
		case ITEM_WEAPON :
		sprintf( buf, "%s, everyone knows %s stole %s. Find the thief and get %s, bring it to me, and I will see you are handsomely rewarded!",
			ch->name, victim->short_descr, obj->short_descr, obj->short_descr );
			break;
		case ITEM_ARMOR :
		sprintf( buf, "%s has been reported lost, %s, but we know who took it. Find %s and get %s, bring it to me, and I will tell you who will pay a generous reward for its return.",
			capitalize(obj->short_descr), ch->name, victim->short_descr, obj->short_descr );
			break;
		case ITEM_CONTAINER :
		sprintf( buf, "%s is gone! %s, rumor among the %s is that %s filched it. Slay %s and get %s, bring it to me, and I'll see to it you are rewarded!",
			capitalize(obj->short_descr), ch->name, race_table[victim->race].name, victim->short_descr, victim->short_descr, obj->short_descr );
			break;
		default :
		sprintf( buf, "%s is gone! %s, just before %s died, our %s informant said %s filched it. Slay %s and get %s, bring it to me, and I'll see to it you are rewarded!",
			capitalize(obj->short_descr), ch->name, victim->sex == 1 ? "he" : "she", race_table[victim->race].name, victim->short_descr, victim->short_descr, obj->short_descr );
			break;
	}
	do_say( captain, buf );

	/* again, you'll probably need to edit a few areas - the Sewers has room names ending in periods */
	for( guard = ch->in_room->people; guard; guard = guard->next_in_room )
	{
		if( IS_NPC(guard) && !IS_AFFECTED(guard, AFF_CHARM)
		&&  !IS_SET(guard->act, ACT_BOUNTY) )
		break;
	}
	/* this is in case all the guards wander off while the door is open.
	 * you might want to set one guard 'sentinel', so he stays put. */
	if( guard == NULL )
	{
		guard = captain;
	}

	do_emote( guard, "points to a map." );

	one_argument( room->name, first );
	if( is_exact_name(first, "the a an") )
	{
		prep = "at ";
		where = decap(victim->in_room->name);
	}

	else if( is_exact_name(first, "on in at by near inside under within approaching standing climbing swimming sailing walking sliding floating close lost along going over near very atop beneath before below between" ) )
	{
		prep = "";
		where = decap(victim->in_room->name);
	}

	else
	{
		prep = "at ";
		where = victim->in_room->name;
	}

	sprintf( buf,  "I hear that %s was recently seen %s%s in %s.",
		 victim->short_descr, prep, where, room->area->name );
	do_say( guard, buf );

	ch->pcdata->rewarder   = 0;
	ch->pcdata->bounty_mob = victim->pIndexData->vnum;
	ch->pcdata->bounty_obj = obj->pIndexData->vnum;
	ch->pcdata->hunt_time  = (current_time + 1200);
	return;
}


/* the reward command structure eliminates the need for arguments
 * but do_command needs them */
void do_reward( CHAR_DATA *ch, char *argument )
{
	char 				buf[MAX_STRING_LENGTH];
	char 				arg[MAX_STRING_LENGTH];
	char const 			*art;
	CHAR_DATA 			*captain;
	CHAR_DATA 			*rewarder = NULL;
	OBJ_DATA			*obj = NULL;
	OBJ_DATA			*obj_next;
	ROOM_INDEX_DATA 	*room;
	int					reward = 0;
	int 				vnum = 0;
	bool obj_found		= FALSE;
	bool rewarder_found	= FALSE;

	/* Newbie's can't hunt bounties. */
	if (ch->level < 3 )
	{
		sprintf( buf, "Sorry, you must be level 3 to hunt for bounty.\n\r" );
		send_to_char( buf, ch );
		return;
	}

	/* Imms can't get bounties, either. */
	else if (ch->level > 101)
	{
		sprintf( buf, "Sorry, you're too old to hunt for bounty.\n\r" );
		send_to_char( buf, ch );
		return;
	}

	/* reset them if they've run out of time */
	if ( ch->pcdata->hunt_time < current_time )
	{
		reset_hunt(ch);
	}

	/* see if they're available for work */
	if ( ch->pcdata->recovery > current_time )
	{
		sprintf( buf, "You have %ld minute%s until you may request another bounty.\n\r",
		((ch->pcdata->recovery - current_time) / 60) + 1,
		(((ch->pcdata->recovery - current_time) / 60)) + 1 == 1 ? "" : "s" );
		send_to_char( buf, ch );
		return;
	}

	/* if they have been assigned a hunt, look for the object in
	 * the char's inventory */
	if ( ch->pcdata->bounty_obj > 0 )
	{
		obj_found = FALSE;
		for (obj = ch->carrying; obj != NULL; obj= obj_next)
		{
			obj_next = obj->next_content;

			if ( obj->pIndexData->vnum  ==  ch->pcdata->bounty_obj )
			{
				obj_found = TRUE;
				break;
			}
		}
	}

	/* if they have the object, check for the rewarder in the room */
	if ( obj_found  &&  (rewarder = find_rewarder(ch)) != NULL )
	{
   		reward = ( (ch->level + 50) / 3 ) + ( (ch->pcdata->hunt_time - current_time) / 20 ) +
				 ( UMAX((get_mob_index(ch->pcdata->bounty_mob)->level - ch->level), 0) * 10 );

		switch( number_range(1, 3) )
		{
			case 1:
			sprintf( buf, "Wonderful, %s! You have found %s that was stolen from me by %s! In appreciation, I grant you a bounty of %d.",
				ch->name, obj->pIndexData->short_descr,
				get_mob_index(ch->pcdata->bounty_mob)->short_descr, reward );
				break;
			case 2:
			sprintf( buf, "%s, you have found %s! This is marvelous! Here, I'll give you %d for a bounty.",
				ch->name, obj->pIndexData->short_descr, reward );
				break;
			case 3:
			sprintf( buf, "%s! %s, you have %s! To show my gratitude, you shall have a bounty of %d.",
				capitalize(obj->pIndexData->short_descr), ch->name, obj->pIndexData->short_descr, reward );
				break;
		}
		do_say( rewarder, buf );

		act( "$c takes $p from $N and gives $M the bounty.", rewarder, obj, ch, TO_NOTVICT );
		act( "$c takes $p from you and gives you the bounty.", rewarder, obj, ch, TO_VICT );
		act( "{c You have recieved a '{x{GQuest Point{x{c',{x {cawesome!{x{c.{x", rewarder, obj, ch, TO_VICT ); 

		extract_obj(obj);
		reset_hunt(ch);
		ch->exp  += reward;
		ch->qps ++;
		return;
	}

	/* see if the character is in the room with the ACT_BOUNTY mob */
	if ( ( captain = find_captain(ch) ) != NULL )
	{
		/* if the char has no bounty_obj set, give her one */
		if ( ch->pcdata->bounty_obj < 1 )
		{
			/* bounty() will almost always find a mob, so if you want a chance of
			 * failure, THIS is the place to put it - before we load the CPU. */
			bounty( ch, captain );
			return;
		}

		/* they have found the object, so we find someone to send them to */
		if ( obj_found )
		{
			/* first make sure it's their first time asking */
			if ( ch->pcdata->rewarder > 0 )
			{
				sprintf( buf, "%s, you have %s that belongs to %s. Now, get going! You have %ld minute%s left.",
					ch->name,
					get_obj_index(ch->pcdata->bounty_obj)->short_descr,
					get_mob_index(ch->pcdata->rewarder)->short_descr,
					(ch->pcdata->hunt_time - current_time) / 60 + 1,
					(((ch->pcdata->hunt_time - current_time ) / 60)) + 1 == 1 ? "" : "s" );
				do_say( captain, buf );
				return;
			}

			else
			{
				rewarder_found	= FALSE;
				/* start with a random number */
				for ( vnum = 1202; vnum < 30001; vnum = number_range(100, 30000) )
				{
					/*make it a random room */
					if ( ( room = get_room_index(vnum) ) != NULL )
					{
						for ( rewarder = room->people; rewarder != NULL; rewarder = rewarder->next_in_room )
						{
							if ( IS_NPC(rewarder)
							&&   ch->level + (ch->level / 3) + 3 >= rewarder->level
							&&   !IS_SET(rewarder->act, ACT_AGGRESSIVE) /* by popular demand! */
							&&   !IS_SET(rewarder->act, ACT_PRACTICE)  /* this takes care of school */
							&&   !IS_SET(rewarder->act, ACT_GAIN)     /* watch out for mobs in clan halls */
							&&   !IS_SET(rewarder->affected_by, AFF_CHARM) ) /* No children, no pets allowed */
							{
								ch->pcdata->rewarder = rewarder->pIndexData->vnum;
								ch->pcdata->hunt_time = (current_time + 1200);
								rewarder_found = TRUE;
								break;
							}
						}
					}
					if ( rewarder_found )
					break;
				}
			}

			if ( ( room = get_room_index(vnum) ) == NULL || rewarder == NULL )
			{
				bug( "Reward: found NULL in reward.", 0 );
				return;
			}

			one_argument( room->name, arg );
			if( is_exact_name(arg, "on in at by near inside under within approaching standing climbing flying sailing swimming falling close shipwrecked along going over near very beneath before way" ) )
			art = "";
			else art = "at ";

			sprintf( buf, "Well done, %s, you have found %s! Now, take it to %s %s%s in %s and ask for a fat reward.",
				ch->name,
				get_obj_index(ch->pcdata->bounty_obj)->short_descr,
				get_mob_index(ch->pcdata->rewarder)->short_descr,
				art,
				lowercase(room->name), room->area->name );
			do_say( captain, buf );

			return;
		}

		/* OK, no obj, let's give them a progress report */
		else
		{
			sprintf( buf, "%s, you are hunting for %s that was stolen by %s. You have %ld minute%s left.",
				ch->name,
				get_obj_index(ch->pcdata->bounty_obj)->short_descr,
				get_mob_index(ch->pcdata->bounty_mob)->short_descr,
				(ch->pcdata->hunt_time - current_time) / 60 + 1,
				(((ch->pcdata->hunt_time - current_time ) / 60)) + 1 == 1 ? "" : "s");
			do_say( captain, buf );
			return;
		}
	}

	/* not with the ACT_BOUNTY or REWARDER mob, so...  */

	if ( ch->pcdata->bounty_obj < 1 )
	{
		sprintf( buf, "You are not bounty hunting at the moment.\n\rSee the Cityguard Captain for a chance at a reward.\n\r" );
		send_to_char( buf, ch );
		return;
	}

	if ( obj_found )
	{
		/* if they have a rewarder, send a reminder */
		if ( ch->pcdata->rewarder != 0 )
		{
			sprintf( buf, "You have %ld minute%s left to return %s to %s!\n\r",
				(ch->pcdata->hunt_time - current_time) / 60 + 1,
				(((ch->pcdata->hunt_time - current_time ) / 60)) + 1 == 1 ? "" : "s",
				get_obj_index(ch->pcdata->bounty_obj)->short_descr,
				get_mob_index(ch->pcdata->rewarder)->short_descr );
			send_to_char( buf, ch );
			return;
		}

		/* if they've just found the object, send them back to the captain */
		else
		{
			sprintf( buf, "You have found %s with %ld minute%s left to go!\n\rReturn to the Captain for further intructions.\n\r",
				get_obj_index(ch->pcdata->bounty_obj)->short_descr,
				(ch->pcdata->hunt_time - current_time) / 60 + 1,
				(((ch->pcdata->hunt_time - current_time ) / 60)) + 1 == 1 ? "" : "s");
			send_to_char( buf, ch );
			return;
		}
	}

	/* if they're still hunting, give them a progress report */
	if ( ch->pcdata->bounty_obj > 0 )
	{
		sprintf( buf, "You are hunting for %s that was stolen by %s.\n\rYou have %ld minute%s left.\n\r",
			get_obj_index(ch->pcdata->bounty_obj)->short_descr,
			get_mob_index(ch->pcdata->bounty_mob)->short_descr,
			(ch->pcdata->hunt_time - current_time) / 60 + 1,
			(((ch->pcdata->hunt_time - current_time ) / 60)) + 1 == 1 ? "" : "s");
		send_to_char( buf, ch );
		return;
	}

	/* since we've tested all the possibilities, you should never see this message */
	else
	{
		bug( "Reward: A player passed ALL the tests. Error!", 0 );
		return;
	}

}
