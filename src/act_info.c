/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/***************************************************************************
*	ROM 2.4 is copyright 1993-1995 Russ Taylor			   *
*	ROM has been brought to you by the ROM consortium		   *
*	    Russ Taylor (rtaylor@pacinfo.com)				   *
*	    Gabrielle Taylor (gtaylor@pacinfo.com)			   *
*	    Brian Moore (rom@rom.efn.org)				   *
*	By using this code, you have agreed to follow the terms of the	   *
*	ROM license, in the file Rom24/doc/rom.license			   *
***************************************************************************/

/*************************************************************************** 
*       ROT 1.4 is copyright 1996-1997 by Russ Walsh                       * 
*       By using this code, you have agreed to follow the terms of the     * 
*       ROT license, in the file doc/rot.license                           * 
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
#include <ctype.h>
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"

/* command procedures needed */
DECLARE_DO_FUN (do_exits);
DECLARE_DO_FUN (do_look);
DECLARE_DO_FUN (do_help);
DECLARE_DO_FUN (do_affects);
DECLARE_DO_FUN (do_play);
DECLARE_DO_FUN (do_inventory);
DECLARE_DO_FUN (do_recall);




char *const where_name[] = {
  "{G<{Cused as light{G>{x     ",
  "{G<{Cworn on finger{G>{x    ",
  "{G<{Cworn on finger{G>{x    ",
  "{G<{Cworn around neck{G>{x  ",
  "{G<{Cworn around neck{G>{x  ",
  "{G<{Cworn on torso{G>{x     ",
  "{G<{Cworn on head{G>{x      ",
  "{G<{Cworn on legs{G>{x      ",
  "{G<{Cworn on feet{G>{x      ",
  "{G<{Cworn on hands{G>{x     ",
  "{G<{Cworn on arms{G>{x      ",
  "{G<{Cworn as shield{G>{x    ",
  "{G<{Cworn about body{G>{x   ",
  "{G<{Cworn about waist{G>{x  ",
  "{G<{Cworn around wrist{G>{x ",
  "{G<{Cworn around wrist{G>{x ",
  "{G<{Cprimary wield{G>{x     ",
  "{G<{Cheld{G>{x              ",
  "{G<{Cfloating nearby{G>{x   ",
  "{G<{Csecondary wield{G>{x   ",
  "{G<{Cworn on face{G>{x      "
};

sh_int const where_order[] = {
  1, 2, 3, 4, 5,
  6, 20, 7, 8, 9,
  10, 11, 12, 13, 14,
  15, 16, 19, 17, 18,
  0
};


/* for do_count */
int max_on = 0;
bool is_pm = FALSE;


/*
 * Local functions.
 */
char *format_obj_to_char args ((OBJ_DATA * obj, CHAR_DATA * ch, bool fShort));
BUFFER *show_list_to_char args ((OBJ_DATA * list, CHAR_DATA * ch,
				 bool fShort, bool fShowNothing));
void show_char_to_char_0 args ((CHAR_DATA * victim, CHAR_DATA * ch));
void show_char_to_char_1 args ((CHAR_DATA * victim, CHAR_DATA * ch));
void show_char_to_char args ((CHAR_DATA * list, CHAR_DATA * ch));
bool check_blind args ((CHAR_DATA * ch));
void display_map (CHAR_DATA * ch);

char *
format_obj_to_char (OBJ_DATA * obj, CHAR_DATA * ch, bool fShort)
{
  static char buf[MAX_STRING_LENGTH];

  buf[0] = '\0';

  if ((fShort && (obj->short_descr == NULL || obj->short_descr[0] == '\0'))
      || (obj->description == NULL || obj->description[0] == '\0'))
    return buf;

  if (obj->pIndexData->vnum == ch->pcdata->bounty_obj)
    {
      if (ch->pcdata->hunt_time < current_time)
	reset_hunt (ch);
      else
	strcat (buf, "{D({rS{Rt{Wolen Au{Rr{ra{D){x ");
    }
  else
   if (IS_OBJ_STAT (obj, ITEM_SHARP))
    strcat (buf, "{C({DS{dh{w{War{Dp{C){x");

  if (!IS_SET (ch->comm, COMM_LONG))
    {
      strcat (buf, "{x[{y.{R.{B.{M.{Y.{W.{G.{C.{x]");
      if (IS_OBJ_STAT (obj, ITEM_INVIS))
	buf[5] = 'V';
      if (IS_AFFECTED (ch, AFF_DETECT_EVIL) && IS_OBJ_STAT (obj, ITEM_EVIL))
	buf[8] = 'E';
      if (IS_AFFECTED (ch, AFF_DETECT_GOOD) && IS_OBJ_STAT (obj, ITEM_BLESS))
	buf[11] = 'B';
      if (IS_AFFECTED (ch, AFF_DETECT_MAGIC) && IS_OBJ_STAT (obj, ITEM_MAGIC))
	buf[14] = 'M';
      if (IS_OBJ_STAT (obj, ITEM_GLOW))
	buf[17] = 'G';
      if (IS_OBJ_STAT (obj, ITEM_HUM))
	buf[20] = 'H';
      if (IS_OBJ_STAT (obj, ITEM_QUEST))
	buf[23] = 'Q';
      if (IS_OBJ_STAT (obj, ITEM_SHARP))
	buf[26] = 'S';
      if (!strcmp (buf, "{x[{y.{R.{B.{M.{Y.{W.{G.{C.{x]"))
	buf[0] = '\0';
    }
  else
    {
      if (IS_OBJ_STAT (obj, ITEM_INVIS))
	strcat (buf, "({yInvis{x)");
      if (IS_OBJ_STAT (obj, ITEM_DARK))
	strcat (buf, "({DHidden{x)");
      if (IS_AFFECTED (ch, AFF_DETECT_EVIL) && IS_OBJ_STAT (obj, ITEM_EVIL))
	strcat (buf, "({RRed Aura{x)");
      if (IS_AFFECTED (ch, AFF_DETECT_GOOD) && IS_OBJ_STAT (obj, ITEM_BLESS))
	strcat (buf, "({BBlue Aura{x)");
      if (IS_AFFECTED (ch, AFF_DETECT_MAGIC) && IS_OBJ_STAT (obj, ITEM_MAGIC))
	strcat (buf, "({yMagical{x)");
      if (IS_OBJ_STAT (obj, ITEM_GLOW))
	strcat (buf, "({YGlowing{x)");
      if (IS_OBJ_STAT (obj, ITEM_HUM))
	strcat (buf, "({yHumming{x)");
      if (IS_OBJ_STAT (obj, ITEM_QUEST))
	strcat (buf, "({GQuest{x)");
      if (IS_OBJ_STAT (obj, ITEM_SHARP))
	strcat (buf, "{C({DS{dh{w{War{Dp{C){x");
    }

  if (buf[0] != '\0')
    {
      strcat (buf, " ");
    }

  if (fShort)
    {
      if (obj->short_descr != NULL)
	strcat (buf, obj->short_descr);
    }
  else
    {
      if (obj->description != NULL)
	strcat (buf, obj->description);
    }
  if (strlen (buf) <= 0)
    strcat (buf, "This object has no description. Please inform the IMP.");

  return buf;
}



/*
 * Show a list to a character.
 * Can coalesce duplicated items.
 */
BUFFER *
show_list_to_char (OBJ_DATA * list, CHAR_DATA * ch, bool fShort,
		   bool fShowNothing)
{
  char buf[MAX_STRING_LENGTH];
  BUFFER *output;
  char **prgpstrShow;
  int *prgnShow;
  char *pstrShow;
  OBJ_DATA *obj;
  int nShow;
  int iShow;
  int count;
  bool fCombine;

  /*
   * Alloc space for output lines.
   */
  output = new_buf ();
  count = 0;
  for (obj = list; obj != NULL; obj = obj->next_content)
    count++;
  prgpstrShow = alloc_mem (count * sizeof (char *));
  prgnShow = alloc_mem (count * sizeof (int));
  nShow = 0;

  /*
   * Format the list of objects.
   */
  for (obj = list; obj != NULL; obj = obj->next_content)
    {
      if (obj->wear_loc == WEAR_NONE && can_see_obj (ch, obj))
	{
	  pstrShow = format_obj_to_char (obj, ch, fShort);

	  fCombine = FALSE;

	  if (IS_NPC (ch) || IS_SET (ch->comm, COMM_COMBINE))
	    {
	      /*
	       * Look for duplicates, case sensitive.
	       * Matches tend to be near end so run loop backwords.
	       */
	      for (iShow = nShow - 1; iShow >= 0; iShow--)
		{
		  if (!strcmp (prgpstrShow[iShow], pstrShow))
		    {
		      prgnShow[iShow]++;
		      fCombine = TRUE;
		      break;
		    }
		}
	    }

	  /*
	   * Couldn't combine, or didn't want to.
	   */
	  if (!fCombine)
	    {
	      prgpstrShow[nShow] = str_dup (pstrShow);
	      prgnShow[nShow] = 1;
	      nShow++;
	    }
	}
    }

  /*
   * Output the formatted list.
   */
  for (iShow = 0; iShow < nShow; iShow++)
    {
      if (prgpstrShow[iShow][0] == '\0')
	{
	  free_string (prgpstrShow[iShow]);
	  continue;
	}

      if (IS_NPC (ch) || IS_SET (ch->comm, COMM_COMBINE))
	{
	  if (prgnShow[iShow] != 1)
	    {
	      sprintf (buf, "(%2d) ", prgnShow[iShow]);
	      add_buf (output, buf);
	    }
	  else
	    {
	      add_buf (output, "     ");
	    }
	}
      add_buf (output, prgpstrShow[iShow]);
      add_buf (output, "\n\r");
      free_string (prgpstrShow[iShow]);
    }

  if (fShowNothing && nShow == 0)
    {
      if (IS_NPC (ch) || IS_SET (ch->comm, COMM_COMBINE))
	send_to_char ("     ", ch);
      send_to_char ("Nothing.\n\r", ch);
    }
  /*
   * Clean up.
   */
  free_mem (prgpstrShow, count * sizeof (char *));
  free_mem (prgnShow, count * sizeof (int));

  return output;
}



void
show_char_to_char_0 (CHAR_DATA * victim, CHAR_DATA * ch)
{
  char buf[MAX_STRING_LENGTH], message[MAX_STRING_LENGTH];

  buf[0] = '\0';

  if (!IS_SET (ch->comm, COMM_LONG))
    {
      strcat (buf, "{x[{y.{D.{c.{b.{w.{C.{r.{B.{R.{Y.{W.{G.{x]");
      if (IS_SHIELDED (victim, SHD_INVISIBLE))
	buf[5] = 'V';
      if (IS_AFFECTED (victim, AFF_HIDE))
	buf[8] = 'H';
      if (IS_AFFECTED (victim, AFF_CHARM))
	buf[11] = 'C';
      if (IS_AFFECTED (victim, AFF_PASS_DOOR))
	buf[14] = 'T';
      if (IS_AFFECTED (victim, AFF_FAERIE_FIRE))
	buf[17] = 'P';
      if (IS_SHIELDED (victim, SHD_ICE))
	buf[20] = 'I';
      if (IS_SHIELDED (victim, SHD_FIRE))
	buf[23] = 'F';
      if (IS_SHIELDED (victim, SHD_SHOCK))
	buf[26] = 'L';
      if (IS_EVIL (victim) && IS_AFFECTED (ch, AFF_DETECT_EVIL))
	buf[29] = 'E';
      if (IS_GOOD (victim) && IS_AFFECTED (ch, AFF_DETECT_GOOD))
	buf[32] = 'G';
      if (IS_SHIELDED (victim, SHD_SANCTUARY))
	buf[35] = 'S';
      if (victim->on_quest)
	buf[38] = 'Q';
      if (!strcmp (buf, "{x[{y.{D.{c.{b.{w.{C.{r.{B.{R.{Y.{W.{G.{x]"))
	buf[0] = '\0';
      if (IS_SET (victim->comm, COMM_AFK))
	strcat (buf, "[{yAFK{x]");
      if (victim->invis_level >= LEVEL_HERO)
	strcat (buf, "({WWizi{x)");
    }
  else
    {
      if (IS_SET (victim->comm, COMM_AFK))
	strcat (buf, "[{yAFK{x]");
      if (IS_SHIELDED (victim, SHD_INVISIBLE))
	strcat (buf, "({yInvis{x)");
      if (victim->invis_level >= LEVEL_HERO)
	strcat (buf, "({WWizi{x)");
      if (IS_AFFECTED (victim, AFF_HIDE))
	strcat (buf, "({DHide{x)");
      if (IS_AFFECTED (victim, AFF_CHARM))
	strcat (buf, "({cCharmed{x)");
      if (IS_AFFECTED (victim, AFF_PASS_DOOR))
	strcat (buf, "({bTranslucent{x)");
      if (IS_AFFECTED (victim, AFF_FAERIE_FIRE))
	strcat (buf, "({wPink Aura{x)");
      if (IS_SHIELDED (victim, SHD_ICE))
	strcat (buf, "({DGrey Aura{x)");
      if (IS_SHIELDED (victim, SHD_FIRE))
	strcat (buf, "({rOrange Aura{x)");
      if (IS_SHIELDED (victim, SHD_SHOCK))
	strcat (buf, "({BBlue Aura{x)");
      if (IS_EVIL (victim) && IS_AFFECTED (ch, AFF_DETECT_EVIL))
	strcat (buf, "({RRed Aura{x)");
      if (IS_GOOD (victim) && IS_AFFECTED (ch, AFF_DETECT_GOOD))
	strcat (buf, "({YGolden Aura{x)");
      if (IS_SHIELDED (victim, SHD_SANCTUARY))
	strcat (buf, "({WWhite Aura{x)");
      if (victim->on_quest)
	strcat (buf, "({GQuest{x)");
    }

  if (IS_NPC (victim) && ch->questmob > 0
      && victim->pIndexData->vnum == ch->questmob)
    strcat (buf, "[TARGET] ");
  if (!IS_NPC (victim) && IS_SET (victim->act, PLR_TWIT))
    strcat (buf, "({rTWIT{x)");
  if (buf[0] != '\0')
    {
      strcat (buf, " ");
    }
  if (victim->position == victim->start_pos && victim->long_descr[0] != '\0')
    {
      strcat (buf, victim->long_descr);
      send_to_char (buf, ch);
      return;
    }

  strcat (buf, PERS (victim, ch));
  if (!IS_NPC (victim) && !IS_SET (ch->comm, COMM_BRIEF)
      && victim->position == POS_STANDING && ch->on == NULL)
    strcat (buf, victim->pcdata->title);

  switch (victim->position)
    {
    case POS_DEAD:
      strcat (buf, " is DEAD!!");
      break;
    case POS_MORTAL:
      strcat (buf, " is mortally wounded.");
      break;
    case POS_INCAP:
      strcat (buf, " is incapacitated.");
      break;
    case POS_STUNNED:
      strcat (buf, " is lying here stunned.");
      break;
    case POS_SLEEPING:
      if (victim->on != NULL)
	{
	  if (IS_SET (victim->on->value[2], SLEEP_AT))
	    {
	      sprintf (message, " is sleeping at %s.",
		       victim->on->short_descr);
	      strcat (buf, message);
	    }
	  else if (IS_SET (victim->on->value[2], SLEEP_ON))
	    {
	      sprintf (message, " is sleeping on %s.",
		       victim->on->short_descr);
	      strcat (buf, message);
	    }
	  else
	    {
	      sprintf (message, " is sleeping in %s.",
		       victim->on->short_descr);
	      strcat (buf, message);
	    }
	}
      else
	strcat (buf, " is sleeping here.");
      break;
    case POS_RESTING:
      if (victim->on != NULL)
	{
	  if (IS_SET (victim->on->value[2], REST_AT))
	    {
	      sprintf (message, " is resting at %s.",
		       victim->on->short_descr);
	      strcat (buf, message);
	    }
	  else if (IS_SET (victim->on->value[2], REST_ON))
	    {
	      sprintf (message, " is resting on %s.",
		       victim->on->short_descr);
	      strcat (buf, message);
	    }
	  else
	    {
	      sprintf (message, " is resting in %s.",
		       victim->on->short_descr);
	      strcat (buf, message);
	    }
	}
      else
	strcat (buf, " is resting here.");
      break;
    case POS_SITTING:
      if (victim->on != NULL)
	{
	  if (IS_SET (victim->on->value[2], SIT_AT))
	    {
	      sprintf (message, " is sitting at %s.",
		       victim->on->short_descr);
	      strcat (buf, message);
	    }
	  else if (IS_SET (victim->on->value[2], SIT_ON))
	    {
	      sprintf (message, " is sitting on %s.",
		       victim->on->short_descr);
	      strcat (buf, message);
	    }
	  else
	    {
	      sprintf (message, " is sitting in %s.",
		       victim->on->short_descr);
	      strcat (buf, message);
	    }
	}
      else
	strcat (buf, " is sitting here.");
      break;
    case POS_STANDING:
      if (victim->on != NULL)
	{
	  if (IS_SET (victim->on->value[2], STAND_AT))
	    {
	      sprintf (message, " is standing at %s.",
		       victim->on->short_descr);
	      strcat (buf, message);
	    }
	  else if (IS_SET (victim->on->value[2], STAND_ON))
	    {
	      sprintf (message, " is standing on %s.",
		       victim->on->short_descr);
	      strcat (buf, message);
	    }
	  else
	    {
	      sprintf (message, " is standing in %s.",
		       victim->on->short_descr);
	      strcat (buf, message);
	    }
	}
      else
	strcat (buf, " is here.");
      break;
    case POS_FIGHTING:
      strcat (buf, " is here, fighting ");
      if (victim->fighting == NULL)
	strcat (buf, "thin air??");
      else if (victim->fighting == ch)
	strcat (buf, "YOU!");
      else if (victim->in_room == victim->fighting->in_room)
	{
	  strcat (buf, PERS (victim->fighting, ch));
	  strcat (buf, ".");
	}
      else
	strcat (buf, "someone who left??");
      break;
    }

  strcat (buf, "\n\r");
  buf[0] = UPPER (buf[0]);
  send_to_char (buf, ch);
  return;
}



void
show_char_to_char_1 (CHAR_DATA * victim, CHAR_DATA * ch)
{
  char buf[MAX_STRING_LENGTH];
  OBJ_DATA *obj;
  BUFFER *output;
  BUFFER *outlist;
  int iWear;
  int oWear;
  int percent;
  bool found;

  if (can_see (victim, ch) && get_trust (victim) >= ch->ghost_level)
    {
      if (ch == victim)
	act ("$n looks at $mself.", ch, NULL, NULL, TO_ROOM);
      else
	{
	  act ("$n looks at you.", ch, NULL, victim, TO_VICT);
	  act ("$n looks at $N.", ch, NULL, victim, TO_NOTVICT);
	}
    }

  output = new_buf ();
  if (victim->description[0] != '\0')
    {
      sprintf (buf, "{C%s{x", victim->description);
    }
  else
    {
      sprintf (buf, "{CYou see nothing special about %s{x\n\r", victim->name);
    }

  add_buf (output, buf);

  if (victim->max_hit > 0)
    percent = (100 * victim->hit) / victim->max_hit;
  else
    percent = -1;

  buf[0] = '\0';
  strcpy (buf, PERS (victim, ch));

  if (percent >= 100)
    strcat (buf, " {fis in excellent condition.{x\n\r");
  else if (percent >= 90)
    strcat (buf, " {fhas a few scratches.{x\n\r");
  else if (percent >= 75)
    strcat (buf, " {fhas some small wounds and bruises.{x\n\r");
  else if (percent >= 50)
    strcat (buf, " {fhas quite a few wounds.{x\n\r");
  else if (percent >= 30)
    strcat (buf, " {fhas some big nasty wounds and scratches.{x\n\r");
  else if (percent >= 15)
    strcat (buf, " {flooks pretty hurt.{x\n\r");
  else if (percent >= 0)
    strcat (buf, " {fis in awful condition.{x\n\r");
  else
    strcat (buf, " {fis bleeding to death.{x\n\r");

  buf[0] = UPPER (buf[0]);
  add_buf (output, buf);

  if (IS_SHIELDED (victim, SHD_ICE))
    {
      sprintf (buf, "%s is surrounded by an {Cicy{x shield.\n\r",
	       PERS (victim, ch));
      buf[0] = UPPER (buf[0]);
      add_buf (output, buf);
    }
  if (IS_SHIELDED (victim, SHD_FIRE))
    {
      sprintf (buf, "%s is surrounded by a {Rfiery{x shield.\n\r",
	       PERS (victim, ch));
      buf[0] = UPPER (buf[0]);
      add_buf (output, buf);
    }
  if (IS_SHIELDED (victim, SHD_SHOCK))
    {
      sprintf (buf, "%s is surrounded by a {Bcrackling{x shield.\n\r",
	       PERS (victim, ch));
      buf[0] = UPPER (buf[0]);
      add_buf (output, buf);
    }

  found = FALSE;
  for (oWear = 0; oWear < MAX_WEAR; oWear++)
    {
      iWear = where_order[oWear];
      if ((obj = get_eq_char (victim, iWear)) != NULL
	  && can_see_obj (ch, obj))
	{
	  if (!found)
	    {
	      sprintf (buf, "\n\r");
	      add_buf (output, buf);
	      sprintf (buf, "{G%s is using:{x\n\r", victim->name);
	      add_buf (output, buf);
	      found = TRUE;
	    }
	  sprintf (buf, "%s%s\n\r", where_name[iWear],
		   format_obj_to_char (obj, ch, TRUE));
	  add_buf (output, buf);
	}
    }

  if (victim != ch
      && !IS_NPC (ch)
      && number_percent () < get_skill (ch, gsn_peek)
      && IS_SET (ch->act, PLR_AUTOPEEK))
    {
      sprintf (buf, "\n\r{GYou peek at the inventory:{x\n\r");
      add_buf (output, buf);
      check_improve (ch, gsn_peek, TRUE, 4);
      outlist = show_list_to_char (victim->carrying, ch, TRUE, TRUE);
      add_buf (output, buf_string (outlist));
      free_buf (outlist);
    }
  page_to_char (buf_string (output), ch);
  free_buf (output);
  return;
}



void
show_char_to_char (CHAR_DATA * list, CHAR_DATA * ch)
{
  CHAR_DATA *rch;

  for (rch = list; rch != NULL; rch = rch->next_in_room)
    {
      if (rch == ch)
	continue;

      if (get_trust (ch) < rch->invis_level)
	continue;

      if (get_trust (ch) < rch->ghost_level)
	continue;

      if (can_see (ch, rch))
	{
	  show_char_to_char_0 (rch, ch);
	}
      else if (room_is_dark (ch->in_room) && IS_AFFECTED (rch, AFF_INFRARED))
	{
	  send_to_char ("You see {Rglowing red{x eyes watching YOU!\n\r", ch);
	}
    }

  return;
}

void
do_peek (CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  BUFFER *output;
  BUFFER *outlist;
  CHAR_DATA *victim;

  one_argument (argument, arg);

  if (IS_NPC (ch))
    return;

  if (arg[0] == '\0')
    {
      send_to_char ("Peek at who?\n\r", ch);
      return;
    }

  if ((victim = get_char_room (ch, arg)) == NULL)
    {
      send_to_char ("They aren't here.\n\r", ch);
      return;
    }

  if (victim == ch)
    {
      do_inventory (ch, "");
      return;
    }

  if (can_see (victim, ch) && get_trust (victim) >= ch->ghost_level)
    {
      act ("$n peers intently at you.", ch, NULL, victim, TO_VICT);
      act ("$n peers intently at $N.", ch, NULL, victim, TO_NOTVICT);
    }

  output = new_buf ();

  if (number_percent () < get_skill (ch, gsn_peek))
    {
      sprintf (buf, "\n\r{GYou peek at the inventory:{x\n\r");
      add_buf (output, buf);
      check_improve (ch, gsn_peek, TRUE, 4);
      outlist = show_list_to_char (victim->carrying, ch, TRUE, TRUE);
      add_buf (output, buf_string (outlist));
      free_buf (outlist);
    }
  else
    {
      sprintf (buf, "{RYou fail to see anything.{x\n\r");
      add_buf (output, buf);
      check_improve (ch, gsn_peek, FALSE, 2);
    }
  page_to_char (buf_string (output), ch);
  free_buf (output);
  return;
}

bool
check_blind (CHAR_DATA * ch)
{

  if (!IS_NPC (ch) && IS_SET (ch->act, PLR_HOLYLIGHT))
    return TRUE;

  if (IS_AFFECTED (ch, AFF_BLIND))
    {
      send_to_char ("You can't see a thing!\n\r", ch);
      return FALSE;
    }

  return TRUE;
}

/* changes your scroll */
void
do_scroll (CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  char buf[100];
  int lines;

  one_argument (argument, arg);

  if (arg[0] == '\0')
    {
      if (ch->lines == 0)
	send_to_char ("You do not page long messages.\n\r", ch);
      else
	{
	  sprintf (buf, "You currently display %d lines per page.\n\r",
		   ch->lines + 2);
	  send_to_char (buf, ch);
	}
      return;
    }

  if (!is_number (arg))
    {
      send_to_char ("You must provide a number.\n\r", ch);
      return;
    }

  lines = atoi (arg);

  if (lines == 0)
    {
      send_to_char ("Paging disabled.\n\r", ch);
      ch->lines = 0;
      return;
    }

  if (lines < 10 || lines > 100)
    {
      send_to_char ("You must provide a reasonable number.\n\r", ch);
      return;
    }

  sprintf (buf, "Scroll set to %d lines.\n\r", lines);
  send_to_char (buf, ch);
  ch->lines = lines - 2;
}

/* RT does socials */
void
do_socials (CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  int iSocial;
  int col;

  col = 0;

  for (iSocial = 0; social_table[iSocial].name[0] != '\0'; iSocial++)
    {
      sprintf (buf, "%-12s", social_table[iSocial].name);
      send_to_char (buf, ch);
      if (++col % 6 == 0)
	send_to_char ("\n\r", ch);
    }

  if (col % 6 != 0)
    send_to_char ("\n\r", ch);
  return;
}




/* RT Commands to replace news, motd, imotd, etc from ROM */

void
do_motd (CHAR_DATA * ch, char *argument)
{
  do_help (ch, "motd");
}

void
do_imotd (CHAR_DATA * ch, char *argument)
{
  do_help (ch, "imotd");
}

void
do_rules (CHAR_DATA * ch, char *argument)
{
  do_help (ch, "rules");
}

void
do_story (CHAR_DATA * ch, char *argument)
{
  do_help (ch, "story");
}

/* RT this following section holds all the auto commands from ROM, as well as
   replacements for config */

void
do_autolist (CHAR_DATA * ch, char *argument)
{
  /* lists most player flags */
  if (IS_NPC (ch))
    return;

  send_to_char ("   action     status\n\r", ch);
  send_to_char ("---------------------\n\r", ch);

  send_to_char ("autoassist     ", ch);
  if (IS_SET (ch->act, PLR_AUTOASSIST))
    send_to_char ("ON\n\r", ch);
  else
    send_to_char ("OFF\n\r", ch);

  send_to_char ("autoexit       ", ch);
  if (IS_SET (ch->act, PLR_AUTOEXIT))
    send_to_char ("ON\n\r", ch);
  else
    send_to_char ("OFF\n\r", ch);

  send_to_char ("autogold       ", ch);
  if (IS_SET (ch->act, PLR_AUTOGOLD))
    send_to_char ("ON\n\r", ch);
  else
    send_to_char ("OFF\n\r", ch);

  send_to_char ("autoloot       ", ch);
  if (IS_SET (ch->act, PLR_AUTOLOOT))
    send_to_char ("ON\n\r", ch);
  else
    send_to_char ("OFF\n\r", ch);

  send_to_char ("autosac        ", ch);
  if (IS_SET (ch->act, PLR_AUTOSAC))
    send_to_char ("ON\n\r", ch);
  else
    send_to_char ("OFF\n\r", ch);

  send_to_char ("autosplit      ", ch);
  if (IS_SET (ch->act, PLR_AUTOSPLIT))
    send_to_char ("ON\n\r", ch);
  else
    send_to_char ("OFF\n\r", ch);

  send_to_char ("autopeek       ", ch);
  if (IS_SET (ch->act, PLR_AUTOPEEK))
    send_to_char ("ON\n\r", ch);
  else
    send_to_char ("OFF\n\r", ch);

  send_to_char ("autostore      ", ch);
  if (IS_SET (ch->comm, COMM_STORE))
    send_to_char ("ON\n\r", ch);
  else
    send_to_char ("OFF\n\r", ch);

  send_to_char ("compact mode   ", ch);
  if (IS_SET (ch->comm, COMM_COMPACT))
    send_to_char ("ON\n\r", ch);
  else
    send_to_char ("OFF\n\r", ch);

  send_to_char ("prompt         ", ch);
  if (IS_SET (ch->comm, COMM_PROMPT))
    send_to_char ("ON\n\r", ch);
  else
    send_to_char ("OFF\n\r", ch);

  send_to_char ("combine items  ", ch);
  if (IS_SET (ch->comm, COMM_COMBINE))
    send_to_char ("ON\n\r", ch);
  else
    send_to_char ("OFF\n\r", ch);

  send_to_char ("long flags     ", ch);
  if (IS_SET (ch->comm, COMM_LONG))
    send_to_char ("ON\n\r", ch);
  else
    send_to_char ("OFF\n\r", ch);

  if (!IS_SET (ch->act, PLR_CANLOOT))
    send_to_char ("Your corpse is safe from thieves.\n\r", ch);
  else
    send_to_char ("Your corpse may be looted.\n\r", ch);

  if (IS_SET (ch->act, PLR_NOSUMMON))
    send_to_char ("You cannot be summoned.\n\r", ch);
  else
    send_to_char ("You can be summoned.\n\r", ch);

  if (IS_SET (ch->act, PLR_NOFOLLOW))
    send_to_char ("You do not welcome followers.\n\r", ch);
  else
    send_to_char ("You accept followers.\n\r", ch);
}



/* 
 * Lore written by Rahl (Daniel Anderson).
 * Can use on object anywhere in the world, but you also get less info than 
 * identify 
 */
void
do_lore (CHAR_DATA * ch, char *argument)
{
  OBJ_DATA *obj;
  char arg[MAX_STRING_LENGTH];
  char buf[MAX_STRING_LENGTH];

  one_argument (argument, arg);

  obj = get_obj_world (ch, arg);

  /* 
   * <blush> oops. Dunno how I forgot this the first time around
   * -Rahl
   */

  if (obj == NULL)
    {
      sprintf (buf, "You've never heard of a %s.\n\r", arg);
      send_to_char (buf, ch);
      return;
    }

  if (get_skill (ch, gsn_lore) == 0)
    {
      send_to_char ("You don't know anything about it.\n\r", ch);
      return;
    }

  if (arg[0] == '\0')
    {
      send_to_char ("What do you want information on?\n\r", ch);
      return;
    }

  if (number_percent () < get_skill (ch, gsn_lore))
    {
      sprintf (buf, "'%s' is type %s, extra flags %s.\n\rLevel %d.\n\r",
	       obj->name, item_type_name (obj),
	       extra_bit_name (obj->extra_flags), obj->level);
      send_to_char (buf, ch);

      check_improve (ch, gsn_lore, TRUE, 1);
    }
  else
    {
      send_to_char ("You can't remember a thing about it.\n\r", ch);
      check_improve (ch, gsn_lore, FALSE, 1);
    }

  return;
}

void
do_autoall (CHAR_DATA * ch, char *argument)
{
  if (IS_NPC (ch))
    return;

  if (!IS_SET (ch->act, PLR_AUTOEXIT))
    {
      send_to_char ("You will now assist when needed.\n\r", ch);
      SET_BIT (ch->act, PLR_AUTOASSIST);
      send_to_char ("Exits will now be displayed.\n\r", ch);
      SET_BIT (ch->act, PLR_AUTOEXIT);
      send_to_char ("Automatic gold looting set.\n\r", ch);
      SET_BIT (ch->act, PLR_AUTOGOLD);
      send_to_char ("Automatic corpse looting set.\n\r", ch);
      SET_BIT (ch->act, PLR_AUTOLOOT);
      send_to_char ("Automatic corpse sacrificing set.\n\r", ch);
      SET_BIT (ch->act, PLR_AUTOSAC);
      send_to_char ("Automatic gold splitting set.\n\r", ch);
      SET_BIT (ch->act, PLR_AUTOSPLIT);
      send_to_char ("Automatic peek set.\n\r", ch);
      SET_BIT (ch->act, PLR_AUTOPEEK);
    }
  else
    {
      send_to_char ("Exits will no longer be displayed.\n\r", ch);
      REMOVE_BIT (ch->act, PLR_AUTOEXIT);
      send_to_char ("Autogold removed.\n\r", ch);
      REMOVE_BIT (ch->act, PLR_AUTOGOLD);
      send_to_char ("Autolooting removed.\n\r", ch);
      REMOVE_BIT (ch->act, PLR_AUTOLOOT);
      send_to_char ("Autosacrificing removed.\n\r", ch);
      REMOVE_BIT (ch->act, PLR_AUTOSAC);
      send_to_char ("Autosplitting removed.\n\r", ch);
      REMOVE_BIT (ch->act, PLR_AUTOSPLIT);
      send_to_char ("Autopeek removed.\n\r", ch);
      REMOVE_BIT (ch->act, PLR_AUTOPEEK);
    }
}



void
do_autoassist (CHAR_DATA * ch, char *argument)
{
  if (IS_NPC (ch))
    return;

  if (IS_SET (ch->act, PLR_AUTOASSIST))
    {
      send_to_char ("Autoassist removed.\n\r", ch);
      REMOVE_BIT (ch->act, PLR_AUTOASSIST);
    }
  else
    {
      send_to_char ("You will now assist when needed.\n\r", ch);
      SET_BIT (ch->act, PLR_AUTOASSIST);
    }
}

void
do_autoexit (CHAR_DATA * ch, char *argument)
{
  if (IS_NPC (ch))
    return;

  if (IS_SET (ch->act, PLR_AUTOEXIT))
    {
      send_to_char ("Exits will no longer be displayed.\n\r", ch);
      REMOVE_BIT (ch->act, PLR_AUTOEXIT);
    }
  else
    {
      send_to_char ("Exits will now be displayed.\n\r", ch);
      SET_BIT (ch->act, PLR_AUTOEXIT);
    }
}

void
do_autogold (CHAR_DATA * ch, char *argument)
{
  if (IS_NPC (ch))
    return;

  if (IS_SET (ch->act, PLR_AUTOGOLD))
    {
      send_to_char ("Autogold removed.\n\r", ch);
      REMOVE_BIT (ch->act, PLR_AUTOGOLD);
    }
  else
    {
      send_to_char ("Automatic gold looting set.\n\r", ch);
      SET_BIT (ch->act, PLR_AUTOGOLD);
    }
}

void
do_autoloot (CHAR_DATA * ch, char *argument)
{
  if (IS_NPC (ch))
    return;

  if (IS_SET (ch->act, PLR_AUTOLOOT))
    {
      send_to_char ("Autolooting removed.\n\r", ch);
      REMOVE_BIT (ch->act, PLR_AUTOLOOT);
    }
  else
    {
      send_to_char ("Automatic corpse looting set.\n\r", ch);
      SET_BIT (ch->act, PLR_AUTOLOOT);
    }
}

void
do_autosac (CHAR_DATA * ch, char *argument)
{
  if (IS_NPC (ch))
    return;

  if (IS_SET (ch->act, PLR_AUTOSAC))
    {
      send_to_char ("Autosacrificing removed.\n\r", ch);
      REMOVE_BIT (ch->act, PLR_AUTOSAC);
    }
  else
    {
      send_to_char ("Automatic corpse sacrificing set.\n\r", ch);
      SET_BIT (ch->act, PLR_AUTOSAC);
    }
}

void
do_autosplit (CHAR_DATA * ch, char *argument)
{
  if (IS_NPC (ch))
    return;

  if (IS_SET (ch->act, PLR_AUTOSPLIT))
    {
      send_to_char ("Autosplitting removed.\n\r", ch);
      REMOVE_BIT (ch->act, PLR_AUTOSPLIT);
    }
  else
    {
      send_to_char ("Automatic gold splitting set.\n\r", ch);
      SET_BIT (ch->act, PLR_AUTOSPLIT);
    }
}

void
do_autopeek (CHAR_DATA * ch, char *argument)
{
  if (IS_NPC (ch))
    return;

  if (IS_SET (ch->act, PLR_AUTOPEEK))
    {
      send_to_char ("Autopeek removed.\n\r", ch);
      REMOVE_BIT (ch->act, PLR_AUTOPEEK);
    }
  else
    {
      send_to_char ("Automatic peek set.\n\r", ch);
      SET_BIT (ch->act, PLR_AUTOPEEK);
    }
}

void
do_brief (CHAR_DATA * ch, char *argument)
{
  if (IS_SET (ch->comm, COMM_BRIEF))
    {
      send_to_char ("Full descriptions activated.\n\r", ch);
      REMOVE_BIT (ch->comm, COMM_BRIEF);
    }
  else
    {
      send_to_char ("Short descriptions activated.\n\r", ch);
      SET_BIT (ch->comm, COMM_BRIEF);
    }
}

void
do_compact (CHAR_DATA * ch, char *argument)
{
  if (IS_SET (ch->comm, COMM_COMPACT))
    {
      send_to_char ("Compact mode removed.\n\r", ch);
      REMOVE_BIT (ch->comm, COMM_COMPACT);
    }
  else
    {
      send_to_char ("Compact mode set.\n\r", ch);
      SET_BIT (ch->comm, COMM_COMPACT);
    }
}

void
do_long (CHAR_DATA * ch, char *argument)
{
  if (!IS_SET (ch->comm, COMM_LONG))
    {
      send_to_char ("Long flags activated.\n\r", ch);
      SET_BIT (ch->comm, COMM_LONG);
    }
  else
    {
      send_to_char ("Short flags activated.\n\r", ch);
      REMOVE_BIT (ch->comm, COMM_LONG);
    }
}

void
do_show (CHAR_DATA * ch, char *argument)
{
  if (IS_SET (ch->comm, COMM_SHOW_AFFECTS))
    {
      send_to_char ("Affects will no longer be shown in score.\n\r", ch);
      REMOVE_BIT (ch->comm, COMM_SHOW_AFFECTS);
    }
  else
    {
      send_to_char ("Affects will now be shown in score.\n\r", ch);
      SET_BIT (ch->comm, COMM_SHOW_AFFECTS);
    }
}

void
do_prompt (CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];

  if (argument[0] == '\0')
    {
      if (IS_SET (ch->comm, COMM_PROMPT))
	{
	  send_to_char ("You will no longer see prompts.\n\r", ch);
	  REMOVE_BIT (ch->comm, COMM_PROMPT);
	}
      else
	{
	  send_to_char ("You will now see prompts.\n\r", ch);
	  SET_BIT (ch->comm, COMM_PROMPT);
	}
      return;
    }

  if (!strcmp (argument, "all"))
    strcpy (buf, "<%hhp %mm %vmv> ");
  else
    {
      if (strlen (argument) > 50)
	argument[50] = '\0';
      strcpy (buf, argument);
      smash_tilde (buf);
      if (str_suffix ("%c", buf))
	strcat (buf, "{x ");

    }

  free_string (ch->prompt);
  ch->prompt = str_dup (buf);
  sprintf (buf, "Prompt set to %s\n\r", ch->prompt);
  send_to_char (buf, ch);
  return;
}

void
do_combine (CHAR_DATA * ch, char *argument)
{
  if (IS_SET (ch->comm, COMM_COMBINE))
    {
      send_to_char ("Long inventory selected.\n\r", ch);
      REMOVE_BIT (ch->comm, COMM_COMBINE);
    }
  else
    {
      send_to_char ("Combined inventory selected.\n\r", ch);
      SET_BIT (ch->comm, COMM_COMBINE);
    }
}

void
do_noloot (CHAR_DATA * ch, char *argument)
{
  if (IS_NPC (ch))
    return;

  if (IS_SET (ch->act, PLR_CANLOOT))
    {
      send_to_char ("Your corpse is now safe from thieves.\n\r", ch);
      REMOVE_BIT (ch->act, PLR_CANLOOT);
    }
  else
    {
      send_to_char ("Your corpse may now be looted.\n\r", ch);
      SET_BIT (ch->act, PLR_CANLOOT);
    }
}

void
do_nofollow (CHAR_DATA * ch, char *argument)
{
  if (IS_NPC (ch) || IS_AFFECTED (ch, AFF_CHARM))
    return;
  if (IS_SET (ch->act, PLR_NOFOLLOW))
    {
      send_to_char ("You now accept followers.\n\r", ch);
      REMOVE_BIT (ch->act, PLR_NOFOLLOW);
    }
  else
    {
      send_to_char ("You no longer accept followers.\n\r", ch);
      SET_BIT (ch->act, PLR_NOFOLLOW);
      die_follower (ch);
    }
}

void
do_nosummon (CHAR_DATA * ch, char *argument)
{
  if (IS_NPC (ch))
    {
      if (IS_SET (ch->imm_flags, IMM_SUMMON))
	{
	  send_to_char ("You are no longer immune to summon.\n\r", ch);
	  REMOVE_BIT (ch->imm_flags, IMM_SUMMON);
	}
      else
	{
	  send_to_char ("You are now immune to summoning.\n\r", ch);
	  SET_BIT (ch->imm_flags, IMM_SUMMON);
	}
    }
  else
    {
      if (IS_SET (ch->act, PLR_NOSUMMON))
	{
	  send_to_char ("You are no longer immune to summon.\n\r", ch);
	  REMOVE_BIT (ch->act, PLR_NOSUMMON);
	}
      else
	{
	  send_to_char ("You are now immune to summoning.\n\r", ch);
	  SET_BIT (ch->act, PLR_NOSUMMON);
	}
    }
}

void
do_notran (CHAR_DATA * ch, char *argument)
{
  if (IS_NPC (ch))
    {
      return;
    }
  else
    {
      if (IS_SET (ch->act, PLR_NOTRAN))
	{
	  send_to_char ("You are no longer immune to transport.\n\r", ch);
	  REMOVE_BIT (ch->act, PLR_NOTRAN);
	}
      else
	{
	  send_to_char ("You are now immune to transport.\n\r", ch);
	  SET_BIT (ch->act, PLR_NOTRAN);
	}
    }
}

void
do_look (CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char arg3[MAX_INPUT_LENGTH];
  BUFFER *outlist;
  EXIT_DATA *pexit;
  CHAR_DATA *victim;
  OBJ_DATA *obj;
  char *pdesc;
  int door;
  int number, count;

  if (ch->desc == NULL)
    return;

  if (ch->position < POS_SLEEPING)
    {
      send_to_char ("You can't see anything but stars!\n\r", ch);
      return;
    }

  if (ch->position == POS_SLEEPING)
    {
      send_to_char ("You can't see anything, you're sleeping!\n\r", ch);
      return;
    }

  if (!check_blind (ch))
    return;

  if (!IS_NPC (ch)
      && !IS_SET (ch->act, PLR_HOLYLIGHT) && room_is_dark (ch->in_room))
    {
      send_to_char ("It is pitch black ... \n\r", ch);
      show_char_to_char (ch->in_room->people, ch);
      return;
    }

  argument = one_argument (argument, arg1);
  argument = one_argument (argument, arg2);
  number = number_argument (arg1, arg3);
  count = 0;

  if (arg1[0] == '\0' || !str_cmp (arg1, "auto"))
    {
      /*
       * 'look' or 'look auto'
       * Removed in favor of display_map
       *send_to_char( "{e", ch );
       *send_to_char( ch->in_room->name, ch );
       *send_to_char( "{x", ch );
       */

      display_map (ch);

      if (IS_IMMORTAL (ch)
	  && (IS_NPC (ch) || IS_SET (ch->act, PLR_HOLYLIGHT)))
	{
	  sprintf (buf, " [Room %d]", ch->in_room->vnum);
	  send_to_char (buf, ch);
	}

      send_to_char ("\n\r", ch);

      if (arg1[0] == '\0' || (!IS_NPC (ch) && !IS_SET (ch->comm, COMM_BRIEF)))
	{
	  send_to_char ("  ", ch);
	  /* ( send_to_char( ch->in_room->description, ch ); removed for minimap */
	  if (ch->in_room->vnum == chain)
	    {
	      send_to_char
		("A huge black iron chain as thick as a tree trunk is drifting above the ground\n\r",
		 ch);
	      send_to_char ("here.\n\r", ch);
	    }
	}

      if (!IS_NPC (ch) && IS_SET (ch->act, PLR_AUTOEXIT))
	{
	  send_to_char ("\n\r", ch);
	  do_exits (ch, "auto");
	}

      outlist = show_list_to_char (ch->in_room->contents, ch, FALSE, FALSE);
      page_to_char (buf_string (outlist), ch);
      free_buf (outlist);
      show_char_to_char (ch->in_room->people, ch);
      return;
    }

  if (!str_cmp (arg1, "i") || !str_cmp (arg1, "in") || !str_cmp (arg1, "on"))
    {
      /* 'look in' */
      if (arg2[0] == '\0')
	{
	  send_to_char ("Look in what?\n\r", ch);
	  return;
	}

      if ((obj = get_obj_here (ch, arg2)) == NULL)
	{
	  send_to_char ("You do not see that here.\n\r", ch);
	  return;
	}

      switch (obj->item_type)
	{
	default:
	  send_to_char ("That is not a container.\n\r", ch);
	  break;

	case ITEM_DRINK_CON:
	  if (obj->value[1] <= 0)
	    {
	      send_to_char ("It is empty.\n\r", ch);
	      break;
	    }

	  sprintf (buf, "It's %sfilled with  a %s liquid.\n\r",
		   obj->value[1] < obj->value[0] / 4
		   ? "less than half-" :
		   obj->value[1] < 3 * obj->value[0] / 4
		   ? "about half-" : "more than half-",
		   liq_table[obj->value[2]].liq_color);

	  send_to_char (buf, ch);
	  break;

	case ITEM_CONTAINER:
	case ITEM_PIT:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	  if (IS_SET (obj->value[1], CONT_CLOSED))
	    {
	      send_to_char ("It is closed.\n\r", ch);
	      break;
	    }

	  act ("$p holds:", ch, obj, NULL, TO_CHAR);
	  outlist = show_list_to_char (obj->contains, ch, TRUE, TRUE);
	  page_to_char (buf_string (outlist), ch);
	  free_buf (outlist);
	  break;
	}
      return;
    }

  if ((victim = get_char_room (ch, arg1)) != NULL)
    {
      show_char_to_char_1 (victim, ch);
      return;
    }

  for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
    {
      if (can_see_obj (ch, obj))
	{			/* player can see object */
	  pdesc = get_extra_descr (arg3, obj->extra_descr);
	  if (pdesc != NULL)
	    {
	      if (++count == number)
		{
		  send_to_char (pdesc, ch);
		  return;
		}
	      else
		continue;
	    }
	  pdesc = get_extra_descr (arg3, obj->pIndexData->extra_descr);
	  if (pdesc != NULL)
	    {
	      if (++count == number)
		{
		  send_to_char (pdesc, ch);
		  return;
		}
	      else
		continue;
	    }
	  if (is_name (arg3, obj->name))
	    if (++count == number)
	      {
		send_to_char (obj->description, ch);
		send_to_char ("\n\r", ch);
		return;
	      }
	}
    }

  for (obj = ch->in_room->contents; obj != NULL; obj = obj->next_content)
    {
      if (can_see_obj (ch, obj))
	{
	  pdesc = get_extra_descr (arg3, obj->extra_descr);
	  if (pdesc != NULL)
	    if (++count == number)
	      {
		send_to_char (pdesc, ch);
		return;
	      }

	  pdesc = get_extra_descr (arg3, obj->pIndexData->extra_descr);
	  if (pdesc != NULL)
	    if (++count == number)
	      {
		send_to_char (pdesc, ch);
		return;
	      }
	  if (is_name (arg3, obj->name))
	    if (++count == number)
	      {
		send_to_char (obj->description, ch);
		send_to_char ("\n\r", ch);
		return;
	      }
	}
    }

  pdesc = get_extra_descr (arg3, ch->in_room->extra_descr);
  if (pdesc != NULL)
    {
      if (++count == number)
	{
	  send_to_char (pdesc, ch);
	  return;
	}
    }

  if (count > 0 && count != number)
    {
      if (count == 1)
	sprintf (buf, "You only see one %s here.\n\r", arg3);
      else
	sprintf (buf, "You only see %d of those here.\n\r", count);

      send_to_char (buf, ch);
      return;
    }

  if (!str_cmp (arg1, "n") || !str_cmp (arg1, "north"))
    door = 0;
  else if (!str_cmp (arg1, "e") || !str_cmp (arg1, "east"))
    door = 1;
  else if (!str_cmp (arg1, "s") || !str_cmp (arg1, "south"))
    door = 2;
  else if (!str_cmp (arg1, "w") || !str_cmp (arg1, "west"))
    door = 3;
  else if (!str_cmp (arg1, "u") || !str_cmp (arg1, "up"))
    door = 4;
  else if (!str_cmp (arg1, "d") || !str_cmp (arg1, "down"))
    door = 5;
  else
    {
      send_to_char ("You do not see that here.\n\r", ch);
      return;
    }

  /* 'look direction' */
  if ((ch->alignment < 0) && (pexit = ch->in_room->exit[door + 6]) != NULL)
    door += 6;
  if ((pexit = ch->in_room->exit[door]) == NULL)
    {
      send_to_char ("Nothing special there.\n\r", ch);
      return;
    }

  if (pexit->description != NULL && pexit->description[0] != '\0')
    send_to_char (pexit->description, ch);
  else
    send_to_char ("Nothing special there.\n\r", ch);

  if (pexit->keyword != NULL
      && pexit->keyword[0] != '\0' && pexit->keyword[0] != ' ')
    {
      if (IS_SET (pexit->exit_info, EX_CLOSED))
	{
	  act ("The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR);
	}
      else if (IS_SET (pexit->exit_info, EX_ISDOOR))
	{
	  act ("The $d is open.", ch, NULL, pexit->keyword, TO_CHAR);
	}
    }

  return;
}

/* RT added back for the hell of it */
void
do_read (CHAR_DATA * ch, char *argument)
{
  do_look (ch, argument);
}

void
do_examine (CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;

  one_argument (argument, arg);

  if (arg[0] == '\0')
    {
      send_to_char ("Examine what?\n\r", ch);
      return;
    }

  do_look (ch, arg);

  if ((obj = get_obj_here (ch, arg)) != NULL)
    {
      switch (obj->item_type)
	{
	default:
	  break;

	case ITEM_JUKEBOX:
	  do_play (ch, "list");
	  break;

	case ITEM_MONEY:
	  if (obj->value[0] == 0)
	    {
	      if (obj->value[1] == 0)
		sprintf (buf, "Odd...there's no coins in the pile.\n\r");
	      else if (obj->value[1] == 1)
		sprintf (buf, "Wow. One gold coin.\n\r");
	      else
		sprintf (buf, "There are %d gold coins in the pile.\n\r",
			 obj->value[1]);
	    }
	  else if (obj->value[1] == 0)
	    {
	      if (obj->value[0] == 1)
		sprintf (buf, "Wow. One silver coin.\n\r");
	      else
		sprintf (buf, "There are %d silver coins in the pile.\n\r",
			 obj->value[0]);
	    }
	  else
	    sprintf (buf,
		     "There are %d gold and %d silver coins in the pile.\n\r",
		     obj->value[1], obj->value[0]);
	  send_to_char (buf, ch);
	  break;

	case ITEM_DRINK_CON:
	case ITEM_CONTAINER:
	case ITEM_PIT:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	  sprintf (buf, "in %s", argument);
	  do_look (ch, buf);
	}
    }

  return;
}



/*
 * Thanks to Zrin for auto-exit part.
 */
void
do_exits (CHAR_DATA * ch, char *argument)
{
  extern char *const dir_name[];
  char buf[MAX_STRING_LENGTH];
  EXIT_DATA *pexit;
  bool found;
  bool round;
  bool fAuto;
  int door;
  int outlet;

  fAuto = !str_cmp (argument, "auto");

  if (!check_blind (ch))
    return;

  if (fAuto)
    sprintf (buf, "[Exits:");
  else if (IS_IMMORTAL (ch))
    sprintf (buf, "Obvious exits from room %d:\n\r", ch->in_room->vnum);
  else
    sprintf (buf, "Obvious exits:\n\r");

  found = FALSE;
  for (door = 0; door < 6; door++)
    {
      round = FALSE;
      outlet = door;
      if ((ch->alignment < 0)
	  && (pexit = ch->in_room->exit[door + 6]) != NULL)
	outlet += 6;
      if ((pexit = ch->in_room->exit[outlet]) != NULL
	  && pexit->u1.to_room != NULL
	  && can_see_room (ch, pexit->u1.to_room)
	  && !IS_SET (pexit->exit_info, EX_CLOSED))
	{
	  found = TRUE;
	  round = TRUE;
	  if (fAuto)
	    {
	      strcat (buf, " ");
	      strcat (buf, dir_name[outlet]);
	    }
	  else
	    {
	      sprintf (buf + strlen (buf), "%-5s - %s",
		       capitalize (dir_name[outlet]),
		       room_is_dark (pexit->u1.to_room)
		       ? "Too dark to tell" : pexit->u1.to_room->name);
	      if (IS_IMMORTAL (ch))
		sprintf (buf + strlen (buf),
			 " (room %d)\n\r", pexit->u1.to_room->vnum);
	      else
		sprintf (buf + strlen (buf), "\n\r");
	    }
	}
      if (!round)
	{
	  OBJ_DATA *portal;
	  ROOM_INDEX_DATA *to_room;

	  portal = get_obj_exit (dir_name[door], ch->in_room->contents);
	  if (portal != NULL)
	    {
	      found = TRUE;
	      round = TRUE;
	      if (fAuto)
		{
		  strcat (buf, " ");
		  strcat (buf, dir_name[door]);
		}
	      else
		{
		  to_room = get_room_index (portal->value[0]);
		  sprintf (buf + strlen (buf), "%-5s - %s",
			   capitalize (dir_name[door]),
			   room_is_dark (to_room)
			   ? "Too dark to tell" : to_room->name);
		  if (IS_IMMORTAL (ch))
		    sprintf (buf + strlen (buf),
			     " (room %d)\n\r", to_room->vnum);
		  else
		    sprintf (buf + strlen (buf), "\n\r");
		}
	    }
	}
    }

  if (!found)
    strcat (buf, fAuto ? " none" : "None.\n\r");

  if (fAuto)
    strcat (buf, "]\n\r");

  send_to_char (buf, ch);
  return;
}


/* This command was added to desplay the map/description to the character */
void
do_map (CHAR_DATA * ch, char *argument)
{
  if (ch->desc == NULL)
    return;

  if (ch->position < POS_SLEEPING)
    {
      send_to_char ("You can't see anything but stars!\n\r", ch);
      return;
    }

  if (ch->position == POS_SLEEPING)
    {
      send_to_char ("You can't see anything, you're sleeping!\n\r", ch);
      return;
    }

  if (!check_blind (ch))
    return;

  if (!IS_NPC (ch)
      && !IS_SET (ch->act, PLR_HOLYLIGHT) && room_is_dark (ch->in_room))
    {
      send_to_char ("It is pitch black ... \n\r", ch);
      show_char_to_char (ch->in_room->people, ch);
      return;
    }
  display_map (ch);

  return;
}



void
do_worth (CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];

  if (IS_NPC (ch))
    {
      sprintf (buf, "You have %ld platinum, %ld gold and %ld silver.\n\r",
	       ch->platinum, ch->gold, ch->silver);
      send_to_char (buf, ch);
      return;
    }

  sprintf (buf,
	   "You have %ld platinum, %ld gold, %ld silver,\n\rand %ld experience (%ld exp to level).\n\r",
	   ch->platinum, ch->gold, ch->silver, ch->exp,
	   (ch->level + 1) * exp_per_level (ch,
					    ch->pcdata->points) - ch->exp);

  send_to_char (buf, ch);

  return;
}


void
do_score (CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  BUFFER *output;
  int i;

  output = new_buf ();
  sprintf (buf,
	   "{xYou are {G%s{x%s{x\n\r",
	   ch->name, IS_NPC (ch) ? ", the mobile." : ch->pcdata->title);
  add_buf (output, buf);

  sprintf (buf,
	   "{xLevel {B%d{x,  {B%d{x years old.\n\r", ch->level, get_age (ch));
  add_buf (output, buf);

  if (get_trust (ch) != ch->level)
    {
      sprintf (buf, "{xYou are trusted at level {B%d{x.\n\r", get_trust (ch));
      add_buf (output, buf);
    }

  sprintf (buf, "{xRace: {M%s{x  Sex: {M%s{x  Class:  {M%s{x\n\r",
	   race_table[ch->race].name,
	   ch->sex == 0 ? "sexless" : ch->sex == 1 ? "male" : "female",
	   IS_NPC (ch) ? "mobile" : class_table[ch->class].name);
  add_buf (output, buf);


  sprintf (buf,
	   "{xYou have {G%d{x/{B%d{x hit, {G%d{x/{B%d{x mana, {G%d{x/{B%d{x movement.\n\r",
	   ch->hit, ch->max_hit,
	   ch->mana, ch->max_mana, ch->move, ch->max_move);
  add_buf (output, buf);

  sprintf (buf,
	   "{xYou have {B%d{x practices and {B%d{x training sessions.\n\r",
	   ch->practice, ch->train);
  add_buf (output, buf);

  sprintf (buf,
	   "{xYou are carrying {G%d{x/{B%d{x items with weight {G%ld{x/{B%d{x pounds.\n\r",
	   ch->carry_number, can_carry_n (ch),
	   get_carry_weight (ch) / 10, can_carry_w (ch) / 10);
  add_buf (output, buf);

  sprintf (buf,
	   "{xStr: {R%d{x({r%d{x)  Int: {R%d{x({r%d{x)  Wis: {R%d{x({r%d{x)  Dex: {R%d{x({r%d{x)  Con: {R%d{x({r%d{x)\n\r",
	   ch->perm_stat[STAT_STR],
	   get_curr_stat (ch, STAT_STR),
	   ch->perm_stat[STAT_INT],
	   get_curr_stat (ch, STAT_INT),
	   ch->perm_stat[STAT_WIS],
	   get_curr_stat (ch, STAT_WIS),
	   ch->perm_stat[STAT_DEX],
	   get_curr_stat (ch, STAT_DEX),
	   ch->perm_stat[STAT_CON], get_curr_stat (ch, STAT_CON));
  add_buf (output, buf);

  sprintf (buf,
	   "{xYou have {Y%ld{x platinum, {Y%ld{x gold and {Y%ld{x silver coins.\n\r",
	   ch->platinum, ch->gold, ch->silver);
  add_buf (output, buf);

  if (!IS_NPC (ch) && ch->level == LEVEL_HERO)
    {
      sprintf (buf, "{xYou have scored {C%ld exp{x.\n\r", ch->exp);
      add_buf (output, buf);
    }
  else if (!IS_NPC (ch) && ch->level < LEVEL_HERO)
    {
      sprintf (buf,
	       "{xYou have scored {C%ld exp{x. You need {C%ld exp{x to level.\n\r",
	       ch->exp,
	       ((ch->level + 1) * exp_per_level (ch,
						 ch->pcdata->points) -
		ch->exp));
      add_buf (output, buf);
    }

  if (ch->wimpy)
    {
      sprintf (buf, "Wimpy set to %d hit points.\n\r", ch->wimpy);
      add_buf (output, buf);
    }

  if (!IS_NPC (ch) && ch->pcdata->condition[COND_DRUNK] > 10)
    {
      sprintf (buf, "{yYou are drunk.{x\n\r");
      add_buf (output, buf);
    }
  if (!IS_NPC (ch) && ch->pcdata->condition[COND_THIRST] == 0)
    {
      sprintf (buf, "{yYou are thirsty.{x\n\r");
      add_buf (output, buf);
    }
  if (!IS_NPC (ch) && ch->pcdata->condition[COND_HUNGER] == 0)
    {
      sprintf (buf, "{yYou are hungry.{x\n\r");
      add_buf (output, buf);
    }

  switch (ch->position)
    {
    case POS_DEAD:
      sprintf (buf, "{RYou are DEAD!!{x\n\r");
      add_buf (output, buf);
      break;
    case POS_MORTAL:
      sprintf (buf, "{RYou are mortally wounded.{x\n\r");
      add_buf (output, buf);
      break;
    case POS_INCAP:
      sprintf (buf, "{RYou are incapacitated.{x\n\r");
      add_buf (output, buf);
      break;
    case POS_STUNNED:
      sprintf (buf, "{RYou are stunned.{x\n\r");
      add_buf (output, buf);
      break;
    case POS_SLEEPING:
      sprintf (buf, "{BYou are sleeping.{x\n\r");
      add_buf (output, buf);
      break;
    case POS_RESTING:
      sprintf (buf, "{BYou are resting.{x\n\r");
      add_buf (output, buf);
      break;
    case POS_STANDING:
      sprintf (buf, "{BYou are standing.{x\n\r");
      add_buf (output, buf);
      break;
    case POS_FIGHTING:
      sprintf (buf, "{RYou are fighting.{x\n\r");
      add_buf (output, buf);
      break;
    }


  /* print AC values */
  if (ch->level >= 25)
    {
      sprintf (buf,
	       "{xArmor: pierce: {G%d{x  bash: {G%d{x  slash: {G%d{x  magic: {G%d{x\n\r",
	       GET_AC (ch, AC_PIERCE), GET_AC (ch, AC_BASH), GET_AC (ch,
								     AC_SLASH),
	       GET_AC (ch, AC_EXOTIC));
      add_buf (output, buf);
    }

  for (i = 0; i < 4; i++)
    {
      char *temp;

      switch (i)
	{
	case (AC_PIERCE):
	  temp = "piercing";
	  break;
	case (AC_BASH):
	  temp = "bashing";
	  break;
	case (AC_SLASH):
	  temp = "slashing";
	  break;
	case (AC_EXOTIC):
	  temp = "magic";
	  break;
	default:
	  temp = "error";
	  break;
	}

      sprintf (buf, "{xYou are ");
      add_buf (output, buf);

      if (GET_AC (ch, i) >= 101)
	sprintf (buf, "{Rhopelessly vulnerable{x to %s.\n\r", temp);
      else if (GET_AC (ch, i) >= 80)
	sprintf (buf, "{Rdefenseless{x against %s.\n\r", temp);
      else if (GET_AC (ch, i) >= 60)
	sprintf (buf, "{Rbarely protected{x from %s.\n\r", temp);
      else if (GET_AC (ch, i) >= 40)
	sprintf (buf, "{yslightly armored{x against %s.\n\r", temp);
      else if (GET_AC (ch, i) >= 20)
	sprintf (buf, "{ysomewhat armored{x against %s.\n\r", temp);
      else if (GET_AC (ch, i) >= 0)
	sprintf (buf, "{yarmored{x against %s.\n\r", temp);
      else if (GET_AC (ch, i) >= -20)
	sprintf (buf, "{ywell-armored{x against %s.\n\r", temp);
      else if (GET_AC (ch, i) >= -40)
	sprintf (buf, "{yvery well-armored{x against %s.\n\r", temp);
      else if (GET_AC (ch, i) >= -60)
	sprintf (buf, "{Bheavily armored{x against %s.\n\r", temp);
      else if (GET_AC (ch, i) >= -80)
	sprintf (buf, "{Bsuperbly armored{x against %s.\n\r", temp);
      else if (GET_AC (ch, i) >= -100)
	sprintf (buf, "{Balmost invulnerable{x to %s.\n\r", temp);
      else
	sprintf (buf, "{Wdivinely armored{x against %s.\n\r", temp);

      add_buf (output, buf);
    }


  /* RT wizinvis and holy light */
  if (IS_IMMORTAL (ch))
    {
      sprintf (buf, "Holy Light: ");
      add_buf (output, buf);
      if (IS_SET (ch->act, PLR_HOLYLIGHT))
	sprintf (buf, "on");
      else
	sprintf (buf, "off");

      add_buf (output, buf);

      if (ch->invis_level)
	{
	  sprintf (buf, "  Invisible: level %d", ch->invis_level);
	  add_buf (output, buf);
	}

      if (ch->incog_level)
	{
	  sprintf (buf, "  Incognito: level %d", ch->incog_level);
	  add_buf (output, buf);
	}
      sprintf (buf, "\n\r");
      add_buf (output, buf);
    }

  if (ch->level >= 15)
    {
      sprintf (buf, "{xHitroll: {G%d{x  Damroll: {G%d{x.\n\r",
	       GET_HITROLL (ch), GET_DAMROLL (ch));
      add_buf (output, buf);
    }

  if (ch->level >= 10)
    {
      sprintf (buf, "{xAlignment: {B%d{x.  ", ch->alignment);
      add_buf (output, buf);
    }

  sprintf (buf, "{xYou are ");
  add_buf (output, buf);
  if (ch->alignment > 900)
    sprintf (buf, "{Wangelic{x.\n\r");
  else if (ch->alignment > 700)
    sprintf (buf, "{Wsaintly{x.\n\r");
  else if (ch->alignment > 350)
    sprintf (buf, "{wgood{x.\n\r");
  else if (ch->alignment > 100)
    sprintf (buf, "kind.\n\r");
  else if (ch->alignment > -100)
    sprintf (buf, "neutral.\n\r");
  else if (ch->alignment > -350)
    sprintf (buf, "mean.\n\r");
  else if (ch->alignment > -700)
    sprintf (buf, "{revil{x.\n\r");
  else if (ch->alignment > -900)
    sprintf (buf, "{Rdemonic{x.\n\r");
  else
    sprintf (buf, "{Rsatanic{x.\n\r");

  add_buf (output, buf);

  if (ch->qps)
    {
      if (ch->qps == 1)
	sprintf (buf, "{xYou have {M%d{x quest point.\n\r", ch->qps);
      else
	sprintf (buf, "{xYou have {M%d{x quest points.\n\r", ch->qps);
      add_buf (output, buf);
    }
  if (ch->pcdata->incarnations)
    {
      sprintf (buf, "{xYou have remorted {M%d{x time(s)\n\r",
	       ch->pcdata->incarnations);
      add_buf (output, buf);
    }

  if (ch->invited)
    {
      sprintf (buf, "{RYou have been invited to join clan {x[{%s%s{x]\n\r",
	       clan_table[ch->invited].pkill ? "B" : "M",
	       clan_table[ch->invited].who_name);
      add_buf (output, buf);
    }
  page_to_char (buf_string (output), ch);
  free_buf (output);

  if (IS_SET (ch->comm, COMM_SHOW_AFFECTS))
    do_affects (ch, "");
}

void
do_affects (CHAR_DATA * ch, char *argument)
{
  AFFECT_DATA *paf, *paf_last = NULL;
  char buf[MAX_STRING_LENGTH];
  BUFFER *output;

  output = new_buf ();
  if (ch->affected != NULL)
    {
      sprintf (buf, "You are affected by the following spells:\n\r");
      add_buf (output, buf);
      for (paf = ch->affected; paf != NULL; paf = paf->next)
	{
	  if (paf_last != NULL && paf->type == paf_last->type)
	    if (ch->level >= 20)
	      sprintf (buf, "                      ");
	    else
	      continue;
	  else
	    sprintf (buf, "Spell: %-15s", skill_table[paf->type].name);

	  add_buf (output, buf);

	  if (ch->level >= 20)
	    {
	      sprintf (buf,
		       ": modifies %s by %d ",
		       affect_loc_name (paf->location), paf->modifier);
	      add_buf (output, buf);
	      if (paf->duration == -1)
		sprintf (buf, "permanently");
	      else
		sprintf (buf, "for %d hours", paf->duration);
	      add_buf (output, buf);
	    }

	  sprintf (buf, "\n\r");
	  add_buf (output, buf);
	  paf_last = paf;
	}
    }
  else
    {
      sprintf (buf, "You are not affected by any spells.\n\r");
      add_buf (output, buf);
    }
  page_to_char (buf_string (output), ch);
  free_buf (output);
  return;
}



char * const day_name[] = {
  "the Moon", "the Bull", "Deception", "Thunder", "Freedom",
  "the Great Gods", "the Sun"
};

char * const month_name[] = {
  "Winter", "the Winter Wolf", "the Frost Giant", "the Old Forces",
  "the Grand Struggle", "the Spring", "Nature", "Futility", "the Dragon",
  "the Sun", "the Heat", "the Battle", "the Dark Shades", "the Shadows",
  "the Long Shadows", "the Ancient Darkness", "the Great Evil"
};

void
do_time (CHAR_DATA * ch, char *argument)
{
  extern char str_boot_time[];
  char buf[MAX_STRING_LENGTH];
  char *suf;
  int day;

  day = time_info.day + 1;

  if (day > 4 && day < 20)
    suf = "th";
  else if (day % 10 == 1)
    suf = "st";
  else if (day % 10 == 2)
    suf = "nd";
  else if (day % 10 == 3)
    suf = "rd";
  else
    suf = "th";

  sprintf (buf,
	   "It is %d o'clock %s, Day of %s, %d%s the Month of %s.\n\r",
	   (time_info.hour % 12 == 0) ? 12 : time_info.hour % 12,
	   time_info.hour >= 12 ? "pm" : "am",
	   day_name[day % 7], day, suf, month_name[time_info.month]);
  send_to_char (buf, ch);
  sprintf (buf, "ROT started up at %s\n\rThe system time is %s\n\r",
	   str_boot_time, (char *) ctime (&current_time));
  send_to_char (buf, ch);
  return;
}



void
do_weather (CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];

  static char *const sky_look[4] = {
    "cloudless",
    "cloudy",
    "rainy",
    "lit by flashes of lightning"
  };

  if (!IS_OUTSIDE (ch))
    {
      send_to_char ("You can't see the weather indoors.\n\r", ch);
      return;
    }

  sprintf (buf, "The sky is %s and %s.\n\r",
	   sky_look[weather_info.sky],
	   weather_info.change >= 0
	   ? "a warm southerly breeze blows" : "a cold northern gust blows");
  send_to_char (buf, ch);
  return;
}



void
do_help (CHAR_DATA * ch, char *argument)
{
  HELP_DATA *pHelp;
  char argall[MAX_INPUT_LENGTH], argone[MAX_INPUT_LENGTH];

  if (argument[0] == '\0')
    argument = "summary";

  /* this parts handles help a b so that it returns help 'a b' */
  argall[0] = '\0';
  while (argument[0] != '\0')
    {
      argument = one_argument (argument, argone);
      if (argall[0] != '\0')
	strcat (argall, " ");
      strcat (argall, argone);
    }

  for (pHelp = help_first; pHelp != NULL; pHelp = pHelp->next)
    {
      if (pHelp->level > get_trust (ch))
	continue;

      if (is_name (argall, pHelp->keyword))
	{
	  if (pHelp->level >= 0 && str_cmp (argall, "imotd"))
	    {
	      send_to_char (pHelp->keyword, ch);
	      send_to_char ("\n\r", ch);
	    }

	  /*
	   * Strip leading '.' to allow initial blanks.
	   */
	  if (pHelp->text[0] == '.')
	    page_to_char (pHelp->text + 1, ch);
	  else
	    page_to_char (pHelp->text, ch);
	  return;
	}
    }

  send_to_char ("No help on that word.\n\r", ch);
  return;
}


/* whois command */
void
do_whois (CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  BUFFER *output;
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  char clandat[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  bool found = FALSE;

  one_argument (argument, arg);

  if (arg[0] == '\0')
    {
      send_to_char ("You must provide a name.\n\r", ch);
      return;
    }

  output = new_buf ();

  for (d = descriptor_list; d != NULL; d = d->next)
    {
      CHAR_DATA *wch;
      char class[MAX_STRING_LENGTH];

      if (d->connected != CON_PLAYING || !can_see (ch, d->character))
	continue;

      wch = (d->original != NULL) ? d->original : d->character;

      if (!can_see (ch, wch))
	continue;

      if (!str_prefix (arg, wch->name))
	{
	  found = TRUE;

	  /* work out the printing */
	  sprintf (buf2, "%3d", wch->level);
	  switch (wch->level)
	    {
	    case MAX_LEVEL - 0:
	      sprintf (buf2, "{GIMP{x");
	      break;
	    case MAX_LEVEL - 1:
	      sprintf (buf2, "{GCRE{x");
	      break;
	    case MAX_LEVEL - 2:
	      sprintf (buf2, "{GSUP{x");
	      break;
	    case MAX_LEVEL - 3:
	      sprintf (buf2, "{GDEI{x");
	      break;
	    case MAX_LEVEL - 4:
	      sprintf (buf2, "{GGOD{x");
	      break;
	    case MAX_LEVEL - 5:
	      sprintf (buf2, "{GIMM{x");
	      break;
	    case MAX_LEVEL - 6:
	      sprintf (buf2, "{GDEM{x");
	      break;
	    case MAX_LEVEL - 7:
	      sprintf (buf2, "{CKNI{x");
	      break;
	    case MAX_LEVEL - 8:
	      sprintf (buf2, "{CSQU{x");
	      break;
	    case MAX_LEVEL - 9:
	      sprintf (buf2, "{BHRO{x");
	      break;
	    }

	  if (wch->class < MAX_CLASS / 2)
	    {
	      sprintf (class, "{R%c{r%c%c{x",
		       class_table[wch->class].who_name[0],
		       class_table[wch->class].who_name[1],
		       class_table[wch->class].who_name[2]);
	    }
	  else
	    {
	      sprintf (class, "{B%c{b%c%c{x",
		       class_table[wch->class].who_name[0],
		       class_table[wch->class].who_name[1],
		       class_table[wch->class].who_name[2]);
	    }

	  /* a little formatting */
	  if (!is_clan (wch))
	    {
	      sprintf (clandat, "%s", clan_table[wch->clan].who_name);
	    }
	  else
	    {
	      if (clan_table[wch->clan].independent)
		{
		  sprintf (clandat, "[{R%s{x] ",
			   clan_table[wch->clan].who_name);
		}
	      else if (is_clead (wch) && is_pkill (wch))
		{
		  sprintf (clandat, "[{B%s{x] ",
			   clan_table[wch->clan].who_name);
		}
	      else if (is_clead (wch) && !is_pkill (wch))
		{
		  sprintf (clandat, "[{M%s{x] ",
			   clan_table[wch->clan].who_name);
		}
	      else if (is_pkill (wch))
		{
		  sprintf (clandat, "[{b%s{x] ",
			   clan_table[wch->clan].who_name);
		}
	      else
		{
		  sprintf (clandat, "[{m%s{x] ",
			   clan_table[wch->clan].who_name);
		}
	    }
	  sprintf (buf, "[%s %-6s %s %s] %s%s%s%s%s%s%s%s\n\r",
		   buf2,
		   wch->race < MAX_PC_RACE ? pc_race_table[wch->race].who_name
		   : "     ",
		   class,
		   wch->sex == 0 ? "{6N{x" : wch->sex ==
		   1 ? "{4M{x" : "{mF{x", ((wch->ghost_level >= LEVEL_HERO)
					   && (ch->level >=
					       wch->level)) ? "(Ghost) " : "",
		   wch->incog_level >= LEVEL_HERO ? "(Incog) " : "",
		   wch->invis_level >= LEVEL_HERO ? "(Wizi) " : "", clandat,
		   IS_SET (wch->comm, COMM_AFK) ? "[{yAFK{x] " : "",
		   IS_SET (wch->act, PLR_TWIT) ? "({RTWIT{x) " : "",
		   wch->name, IS_NPC (wch) ? "" : wch->pcdata->title);
	  add_buf (output, buf);
	}
    }

  if (!found)
    {
      send_to_char ("No one of that name is playing.\n\r", ch);
      return;
    }

  page_to_char (buf_string (output), ch);
  free_buf (output);
}


/*
 * New 'who' command originally by Alander of Rivers of Mud.
 */
void
do_who (CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  char clandat[MAX_STRING_LENGTH];
  char questdat[MAX_STRING_LENGTH];
  BUFFER *output;
  BUFFER *outputimm;
  DESCRIPTOR_DATA *d;
  int iClass;
  int iRace;
  int iClan;
  int iLevelLower;
  int iLevelUpper;
  int nNumber;
  int nMatch;
  bool rgfClass[MAX_CLASS];
  bool rgfRace[MAX_PC_RACE];
  bool rgfClan[MAX_CLAN];
  bool fClassRestrict = FALSE;
  bool fClanRestrict = FALSE;
  bool fClan = FALSE;
  bool fRaceRestrict = FALSE;
  bool fImmortalOnly = FALSE;
  int count;
  int countimm;
  int hour;

  /*
   * Set default arguments.
   */
  iLevelLower = 0;
  iLevelUpper = MAX_LEVEL;
  for (iClass = 0; iClass < MAX_CLASS; iClass++)
    rgfClass[iClass] = FALSE;
  for (iRace = 0; iRace < MAX_PC_RACE; iRace++)
    rgfRace[iRace] = FALSE;
  for (iClan = 0; iClan < MAX_CLAN; iClan++)
    rgfClan[iClan] = FALSE;

  /*
   * Parse arguments.
   */
  count = 0;
  countimm = 0;
  nNumber = 0;
  for (;;)
    {
      char arg[MAX_STRING_LENGTH];

      argument = one_argument (argument, arg);
      if (arg[0] == '\0')
	break;

      if (is_number (arg))
	{
	  switch (++nNumber)
	    {
	    case 1:
	      iLevelLower = atoi (arg);
	      break;
	    case 2:
	      iLevelUpper = atoi (arg);
	      break;
	    default:
	      send_to_char ("Only two level numbers allowed.\n\r", ch);
	      return;
	    }
	}
      else
	{

	  /*
	   * Look for classes to turn on.
	   */
	  if (!str_prefix (arg, "immortals"))
	    {
	      fImmortalOnly = TRUE;
	    }
	  else
	    {
	      iClass = class_lookup (arg);
	      if (iClass == -1)
		{
		  iRace = race_lookup (arg);

		  if (iRace == 0 || iRace >= MAX_PC_RACE)
		    {
		      if (!str_prefix (arg, "clan"))
			fClan = TRUE;
		      else
			{
			  iClan = clan_lookup (arg);
			  if (iClan)
			    {
			      fClanRestrict = TRUE;
			      rgfClan[iClan] = TRUE;
			    }
			  else
			    {
			      send_to_char
				("That's not a valid race, class, or clan.\n\r",
				 ch);
			      return;
			    }
			}
		    }
		  else
		    {
		      fRaceRestrict = TRUE;
		      rgfRace[iRace] = TRUE;
		    }
		}
	      else
		{
		  fClassRestrict = TRUE;
		  rgfClass[iClass] = TRUE;
		}
	    }
	}
    }

  /*
   * Now show matching chars.
   */
  nMatch = 0;
  buf[0] = '\0';
  output = new_buf ();
  outputimm = new_buf ();
  add_buf (outputimm, "{xVisible {GImmortals{x:\n\r");
  add_buf (output, "{xVisible {GMortals{x:\n\r");
  for (d = descriptor_list; d != NULL; d = d->next)
    {
      CHAR_DATA *wch;
      char class[MAX_STRING_LENGTH];

      /*
       * Check for match against restrictions.
       * Don't use trust as that exposes trusted mortals.
       */
      if (d->connected != CON_PLAYING || !can_see (ch, d->character))
	continue;

      wch = (d->original != NULL) ? d->original : d->character;

      if (!can_see (ch, wch) && IS_SHIELDED (wch, SHD_INVISIBLE))
	continue;

      if (!can_see (ch, wch) && wch->level >= SQUIRE
	  && ch->level < wch->level)
	continue;

      if (!IS_IMMORTAL (wch))
	continue;

      if (wch->level < iLevelLower
	  || wch->level > iLevelUpper
	  || (fImmortalOnly && wch->level < LEVEL_IMMORTAL)
	  || (fClassRestrict && !rgfClass[wch->class])
	  || (fRaceRestrict && !rgfRace[wch->race])
	  || (fClan && !is_clan (wch))
	  || (fClanRestrict && !rgfClan[wch->clan]))
	continue;

      countimm++;

      nMatch++;

      /*
       * Figure out what to print for class.
       */
      sprintf (buf2, "%3d", wch->level);
      switch (wch->level)
	{
	default:
	  break;
	  {
	case MAX_LEVEL - 0:
	    sprintf (buf2, "{GIMP{x");
	    break;
	case MAX_LEVEL - 1:
	    sprintf (buf2, "{GCRE{x");
	    break;
	case MAX_LEVEL - 2:
	    sprintf (buf2, "{GSUP{x");
	    break;
	case MAX_LEVEL - 3:
	    sprintf (buf2, "{GDEI{x");
	    break;
	case MAX_LEVEL - 4:
	    sprintf (buf2, "{GGOD{x");
	    break;
	case MAX_LEVEL - 5:
	    sprintf (buf2, "{GIMM{x");
	    break;
	case MAX_LEVEL - 6:
	    sprintf (buf2, "{GDEM{x");
	    break;
	case MAX_LEVEL - 7:
	    sprintf (buf2, "{CKNI{x");
	    break;
	case MAX_LEVEL - 8:
	    sprintf (buf2, "{CSQU{x");
	    break;
	  }
	}

      if (wch->class < MAX_CLASS / 2)
	{
	  sprintf (class, "{R%c{r%c%c{x",
		   class_table[wch->class].who_name[0],
		   class_table[wch->class].who_name[1],
		   class_table[wch->class].who_name[2]);

	}
      else
	{
	  sprintf (class, "{B%c{b%c%c{x",
		   class_table[wch->class].who_name[0],
		   class_table[wch->class].who_name[1],
		   class_table[wch->class].who_name[2]);
	}

      /*
       * Format it up.
       */
      if (!wch->on_quest)
	{
	  questdat[0] = '\0';
	}
      else
	{
	  sprintf (questdat, "[{GQ{x] ");
	}
      if (!is_clan (wch))
	{
	  sprintf (clandat, "%s", clan_table[wch->clan].who_name);
	}
      else
	{
	  if (clan_table[wch->clan].independent)
	    {
	      sprintf (clandat, "[{R%s{x] ", clan_table[wch->clan].who_name);
	    }
	  else if (is_clead (wch) && is_pkill (wch))
	    {
	      sprintf (clandat, "[{B%s{x] ", clan_table[wch->clan].who_name);
	    }
	  else if (is_clead (wch) && !is_pkill (wch))
	    {
	      sprintf (clandat, "[{M%s{x] ", clan_table[wch->clan].who_name);
	    }
	  else if (is_pkill (wch))
	    {
	      sprintf (clandat, "[{b%s{x] ", clan_table[wch->clan].who_name);
	    }
	  else
	    {
	      sprintf (clandat, "[{m%s{x] ", clan_table[wch->clan].who_name);
	    }
	}
      if (wch->pcdata->who_descr[0] != '\0')
	{
	  sprintf (buf, "[%s %s] %s%s%s%s%s%s%s%s%s\n\r",
		   buf2,
		   wch->pcdata->who_descr,
		   questdat,
		   clandat,
		   ((wch->ghost_level >= LEVEL_HERO)
		    && (ch->level >= wch->level)) ? "(Ghost) " : "",
		   wch->incog_level >= LEVEL_HERO ? "(Incog) " : "",
		   wch->invis_level >= LEVEL_HERO ? "(Wizi) " : "",
		   IS_SET (wch->comm, COMM_AFK) ? "[{yAFK{x] " : "",
		   IS_SET (wch->act, PLR_TWIT) ? "({RTWIT{x) " : "",
		   wch->name, IS_NPC (wch) ? "" : wch->pcdata->title);
	}
      else
	{
	  sprintf (buf, "[%s %-6s %s] %s%s%s%s%s%s%s%s%s\n\r",
		   buf2,
		   wch->race < MAX_PC_RACE ? pc_race_table[wch->race].who_name
		   : "     ",
		   class,
		   questdat,
		   clandat,
		   ((wch->ghost_level >= LEVEL_HERO)
		    && (ch->level >= wch->level)) ? "(Ghost) " : "",
		   wch->incog_level >= LEVEL_HERO ? "(Incog) " : "",
		   wch->invis_level >= LEVEL_HERO ? "(Wizi) " : "",
		   IS_SET (wch->comm, COMM_AFK) ? "[{yAFK{x] " : "",
		   IS_SET (wch->act, PLR_TWIT) ? "({RTWIT{x) " : "",
		   wch->name, IS_NPC (wch) ? "" : wch->pcdata->title);
	}
      add_buf (outputimm, buf);
    }
  add_buf (outputimm, "\n\r");
  buf[0] = '\0';
  for (d = descriptor_list; d != NULL; d = d->next)
    {
      CHAR_DATA *wch;
      char class[MAX_STRING_LENGTH];

      /*
       * Check for match against restrictions.
       * Don't use trust as that exposes trusted mortals.
       */
      if (d->connected != CON_PLAYING || !can_see (ch, d->character))
	continue;

      wch = (d->original != NULL) ? d->original : d->character;

      if (!can_see (ch, wch) && IS_SHIELDED (wch, SHD_INVISIBLE))
	continue;

      if (!can_see (ch, wch) && wch->level >= SQUIRE
	  && ch->level < wch->level)
	continue;

      if (IS_IMMORTAL (wch))
	continue;

      count++;

      if ((wch->in_room->vnum == ROOM_VNUM_CORNER) && (!IS_IMMORTAL (ch)))
	continue;

      if (wch->level < iLevelLower
	  || wch->level > iLevelUpper
	  || (fImmortalOnly && wch->level < LEVEL_IMMORTAL)
	  || (fClassRestrict && !rgfClass[wch->class])
	  || (fRaceRestrict && !rgfRace[wch->race])
	  || (fClan && !is_clan (wch))
	  || (fClanRestrict && !rgfClan[wch->clan]))
	continue;

      nMatch++;

      /*
       * Figure out what to print for class.
       */
      sprintf (buf2, "%3d", wch->level);
      switch (wch->level)
	{
	default:
	  break;
	  {
	case MAX_LEVEL - 9:
	    sprintf (buf2, "{BHRO{x");
	    break;
	  }
	}

      if (wch->class < MAX_CLASS / 2)
	{
	  sprintf (class, "{R%c{r%c%c{x",
		   class_table[wch->class].who_name[0],
		   class_table[wch->class].who_name[1],
		   class_table[wch->class].who_name[2]);
	}
      else
	{
	  sprintf (class, "{B%c{b%c%c{x",
		   class_table[wch->class].who_name[0],
		   class_table[wch->class].who_name[1],
		   class_table[wch->class].who_name[2]);
	}

      /*
       * Format it up.
       */
      if (!wch->on_quest)
	{
	  questdat[0] = '\0';
	}
      else
	{
	  sprintf (questdat, "[{GQ{x] ");
	}
      if (!is_clan (wch))
	{
	  sprintf (clandat, "%s", clan_table[wch->clan].who_name);
	}
      else
	{
	  if (clan_table[wch->clan].independent)
	    {
	      sprintf (clandat, "[{R%s{x] ", clan_table[wch->clan].who_name);
	    }
	  else if (is_clead (wch) && is_pkill (wch))
	    {
	      sprintf (clandat, "[{B%s{x] ", clan_table[wch->clan].who_name);
	    }
	  else if (is_clead (wch) && !is_pkill (wch))
	    {
	      sprintf (clandat, "[{M%s{x] ", clan_table[wch->clan].who_name);
	    }
	  else if (is_pkill (wch))
	    {
	      sprintf (clandat, "[{b%s{x] ", clan_table[wch->clan].who_name);
	    }
	  else
	    {
	      sprintf (clandat, "[{m%s{x] ", clan_table[wch->clan].who_name);
	    }
	}
      if (wch->pcdata->who_descr[0] != '\0')
	{
	  sprintf (buf, "[%s %s] %s%s%s%s%s%s%s%s%s\n\r",
		   buf2,
		   wch->pcdata->who_descr,
		   questdat,
		   clandat,
		   ((wch->ghost_level >= LEVEL_HERO)
		    && (ch->level >= wch->level)) ? "(Ghost) " : "",
		   wch->incog_level >= LEVEL_HERO ? "(Incog) " : "",
		   wch->invis_level >= LEVEL_HERO ? "(Wizi) " : "",
		   IS_SET (wch->comm, COMM_AFK) ? "[{yAFK{x] " : "",
		   IS_SET (wch->act, PLR_TWIT) ? "({RTWIT{x) " : "",
		   wch->name, IS_NPC (wch) ? "" : wch->pcdata->title);
	}
      else
	{
	  sprintf (buf, "[%s %-6s %s] %s%s%s%s%s%s%s%s%s\n\r",
		   buf2,
		   wch->race < MAX_PC_RACE ? pc_race_table[wch->race].who_name
		   : "     ",
		   class,
		   questdat,
		   clandat,
		   ((wch->ghost_level >= LEVEL_HERO)
		    && (ch->level >= wch->level)) ? "(Ghost) " : "",
		   wch->incog_level >= LEVEL_HERO ? "(Incog) " : "",
		   wch->invis_level >= LEVEL_HERO ? "(Wizi) " : "",
		   IS_SET (wch->comm, COMM_AFK) ? "[{yAFK{x] " : "",
		   IS_SET (wch->act, PLR_TWIT) ? "({RTWIT{x) " : "",
		   wch->name, IS_NPC (wch) ? "" : wch->pcdata->title);
	}
      add_buf (output, buf);
    }

  count += countimm;
  hour = localtime(&current_time)->tm_hour;
  if (hour < 12)
    {
      if (is_pm)
	{
	  is_pm = FALSE;
	  max_on = 0;
	  expire_notes ();
	  randomize_entrances (ROOM_VNUM_CLANS);
	}
    }
  else
    {
      is_pm = TRUE;
    }
  max_on = UMAX (count, max_on);
  if (global_quest)
    {
      sprintf (buf2, "\n\r{GThe global quest flag is on.{x");
      add_buf (output, buf2);
    }
  if (nMatch != count)
    {
      sprintf (buf2, "\n\r{BMatches found: {W%d{x\n\r", nMatch);
      add_buf (output, buf2);
    }
  else
    {
      sprintf (buf2,
	       "\n\r{BPlayers found: {W%d   {BMost on today: {W%d{x\n\r",
	       count, max_on);
      add_buf (output, buf2);
    }
  if (countimm >= 1)
    {
      add_buf (outputimm, buf_string (output));
      page_to_char (buf_string (outputimm), ch);
    }
  else
    {
      page_to_char (buf_string (output), ch);
    }
  free_buf (output);
  free_buf (outputimm);
  return;
}

void
do_count (CHAR_DATA * ch, char *argument)
{
  int count;
  int hour;
  DESCRIPTOR_DATA *d;
  char buf[MAX_STRING_LENGTH];

  count = 0;
  hour = localtime (&current_time)->tm_hour;
  if (hour < 12)
    {
      if (is_pm)
	{
	  is_pm = FALSE;
	  max_on = 0;
	  expire_notes ();
	  randomize_entrances (ROOM_VNUM_CLANS);
	}
    }
  else
    {
      is_pm = TRUE;
    }

  for (d = descriptor_list; d != NULL; d = d->next)
    if (d->connected == CON_PLAYING && can_see (ch, d->character))
      count++;

  max_on = UMAX (count, max_on);

  if (max_on == count)
    sprintf (buf,
	     "{BThere are {W%d {Bcharacters on, the most so far today.{x\n\r",
	     count);
  else
    sprintf (buf,
	     "{BThere are {W%d {Bcharacters on, the most on today was {W%d{x.\n\r",
	     count, max_on);

  send_to_char (buf, ch);
}

void
do_inventory (CHAR_DATA * ch, char *argument)
{
  BUFFER *outlist;

  send_to_char ("You are carrying:\n\r", ch);
  outlist = show_list_to_char (ch->carrying, ch, TRUE, TRUE);
  page_to_char (buf_string (outlist), ch);
  free_buf (outlist);
  return;
}



void
do_equipment (CHAR_DATA * ch, char *argument)
{
  OBJ_DATA *obj;
  int iWear;
  //    bool found;
  send_to_char ("{wYou are using:{x\n\r", ch);
  //found = FALSE;
  for (iWear = 0; iWear < MAX_WEAR; iWear++)
    {
      if ((obj = get_eq_char (ch, iWear)) == NULL)
	{
	  send_to_char ("{w", ch);
	  send_to_char (where_name[iWear], ch);
	  send_to_char ("     ---\r\n", ch);
	  continue;
	}
      send_to_char ("{Y", ch);
      send_to_char (where_name[iWear], ch);
      send_to_char ("{W", ch);
      if (can_see_obj (ch, obj))
	{
	  send_to_char ("{W", ch);
	  send_to_char (format_obj_to_char (obj, ch, TRUE), ch);
	  send_to_char ("{x\n\r", ch);
	}
      else
	{
	  send_to_char ("{Wsomething.\n\r", ch);
	}
      //        found = TRUE;
    }

  send_to_char ("{x", ch);

  return;
}


void
do_compare (CHAR_DATA * ch, char *argument)
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  OBJ_DATA *obj1;
  OBJ_DATA *obj2;
  int value1;
  int value2;
  char *msg;

  argument = one_argument (argument, arg1);
  argument = one_argument (argument, arg2);
  if (arg1[0] == '\0')
    {
      send_to_char ("Compare what to what?\n\r", ch);
      return;
    }

  if ((obj1 = get_obj_carry (ch, arg1)) == NULL)
    {
      send_to_char ("You do not have that item.\n\r", ch);
      return;
    }

  if (arg2[0] == '\0')
    {
      for (obj2 = ch->carrying; obj2 != NULL; obj2 = obj2->next_content)
	{
	  if (obj2->wear_loc != WEAR_NONE
	      && can_see_obj (ch, obj2)
	      && obj1->item_type == obj2->item_type
	      && (obj1->wear_flags & obj2->wear_flags & ~ITEM_TAKE) != 0)
	    break;
	}

      if (obj2 == NULL)
	{
	  send_to_char ("You aren't wearing anything comparable.\n\r", ch);
	  return;
	}
    }

  else if ((obj2 = get_obj_carry (ch, arg2)) == NULL)
    {
      send_to_char ("You do not have that item.\n\r", ch);
      return;
    }

  msg = NULL;
  value1 = 0;
  value2 = 0;

  if (obj1 == obj2)
    {
      msg = "You compare $p to itself.  It looks about the same.";
    }
  else if (obj1->item_type != obj2->item_type)
    {
      msg = "You can't compare $p and $P.";
    }
  else
    {
      switch (obj1->item_type)
	{
	default:
	  msg = "You can't compare $p and $P.";
	  break;

	case ITEM_ARMOR:
	  if (obj1->clan)
	    {
	      value1 = apply_ac (ch->level, obj1, WEAR_HOLD, 0);
	      value1 += apply_ac (ch->level, obj1, WEAR_HOLD, 1);
	      value1 += apply_ac (ch->level, obj1, WEAR_HOLD, 2);
	    }
	  else
	    {
	      value1 = obj1->value[0] + obj1->value[1] + obj1->value[2];
	    }
	  if (obj2->clan)
	    {
	      value2 = apply_ac (ch->level, obj2, WEAR_HOLD, 0);
	      value2 += apply_ac (ch->level, obj2, WEAR_HOLD, 1);
	      value2 += apply_ac (ch->level, obj2, WEAR_HOLD, 2);
	    }
	  else
	    {
	      value2 = obj2->value[0] + obj2->value[1] + obj2->value[2];
	    }
	  break;

	case ITEM_WEAPON:
	  if (obj1->clan)
	    {
	      value1 = 4 * (ch->level / 3);
	    }
	  else
	    {
	      if (obj1->pIndexData->new_format)
		value1 = (1 + obj1->value[2]) * obj1->value[1];
	      else
		value1 = obj1->value[1] + obj1->value[2];
	    }
	  if (obj2->clan)
	    {
	      value2 = 4 * (ch->level / 3);
	    }
	  else
	    {
	      if (obj2->pIndexData->new_format)
		value2 = (1 + obj2->value[2]) * obj2->value[1];
	      else
		value2 = obj2->value[1] + obj2->value[2];
	    }
	  break;
	}
    }

  if (msg == NULL)
    {
      if (value1 == value2)
	msg = "$p and $P look about the same.";
      else if (value1 > value2)
	msg = "$p looks better than $P.";
      else
	msg = "$p looks worse than $P.";
    }

  act (msg, ch, obj1, obj2, TO_CHAR);
  return;
}



void
do_credits (CHAR_DATA * ch, char *argument)
{
  do_help (ch, "diku");
  return;
}



void
do_where (CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  DESCRIPTOR_DATA *d;
  bool found;

  one_argument (argument, arg);

  if (arg[0] == '\0')
    {
      send_to_char ("Players near you:\n\r", ch);
      found = FALSE;
      for (d = descriptor_list; d; d = d->next)
	{
	  if (d->connected == CON_PLAYING
	      && (victim = d->character) != NULL
	      && !IS_NPC (victim)
	      && victim->in_room != NULL
	      && !IS_SET (victim->in_room->room_flags, ROOM_NOWHERE)
	      && (is_room_owner (ch, victim->in_room)
		  || !room_is_private (ch, victim->in_room))
	      && victim->in_room->area == ch->in_room->area
	      && get_trust (ch) >= victim->ghost_level
	      && can_see (ch, victim))
	    {
	      found = TRUE;
	      sprintf (buf, "%-28s %s\n\r",
		       victim->name, victim->in_room->name);
	      send_to_char (buf, ch);
	    }
	}
      if (!found)
	send_to_char ("None\n\r", ch);
    }
  else
    {
      found = FALSE;
      for (victim = char_list; victim != NULL; victim = victim->next)
	{
	  if (victim->in_room != NULL
	      && victim->in_room->area == ch->in_room->area
	      && !IS_AFFECTED (victim, AFF_HIDE)
	      && !IS_AFFECTED (victim, AFF_SNEAK)
	      && get_trust (ch) >= victim->ghost_level
	      && can_see (ch, victim) && is_name (arg, victim->name))
	    {
	      found = TRUE;
	      sprintf (buf, "%-28s %s\n\r",
		       PERS (victim, ch), victim->in_room->name);
	      send_to_char (buf, ch);
	      break;
	    }
	}
      if (!found)
	act ("You didn't find any $T.", ch, NULL, arg, TO_CHAR);
    }

  return;
}

void
do_track (CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  EXIT_DATA *pexit;
  ROOM_INDEX_DATA *in_room;
  sh_int track_vnum;
  int door, move, chance, track;

  one_argument (argument, arg);

  if ((chance = get_skill (ch, gsn_track)) == 0)
    {
      send_to_char ("You don't know how to track.\n\r", ch);
      return;
    }

  if (arg[0] == '\0')
    {
      send_to_char ("Track whom?\n\r", ch);
      return;
    }

  in_room = ch->in_room;
  track_vnum = in_room->vnum;
  move = movement_loss[UMIN (SECT_MAX - 1, in_room->sector_type)];
  if (ch->move < move)
    {
      send_to_char ("You are too exhausted.\n\r", ch);
      return;
    }

  if (number_percent () < (100 - chance))
    {
      sprintf (buf, "You can find no recent tracks for %s.\n\r", arg);
      send_to_char (buf, ch);
      check_improve (ch, gsn_track, FALSE, 1);
      WAIT_STATE (ch, 1);
      ch->move -= move / 2;
      return;
    }

  for (victim = char_list; victim != NULL; victim = victim->next)
    {
      if (victim->in_room != NULL
	  && !IS_NPC (victim)
	  && can_see (ch, victim) && is_name (arg, victim->name))
	{
	  if (victim->in_room->vnum == track_vnum)
	    {
	      act ("The tracks end right under $S feet.", ch, NULL, victim,
		   TO_CHAR);
	      return;
	    }
	  for (track = 0; track < MAX_TRACK; track++)
	    {
	      if (victim->track_from[track] == track_vnum)
		{
		  for (door = 0; door < 12; door++)
		    {
		      if ((pexit = in_room->exit[door]) != NULL)
			{
			  if (pexit->u1.to_room->vnum ==
			      victim->track_to[track])
			    {
			      sprintf (buf,
				       "Some tracks lead off to the %s.\n\r",
				       dir_name[door]);
			      send_to_char (buf, ch);
			      check_improve (ch, gsn_track, TRUE, 1);
			      WAIT_STATE (ch, 1);
			      ch->move -= move;
			      return;
			    }
			}
		    }
		  act ("$N seems to have vanished here.", ch, NULL, victim,
		       TO_CHAR);
		  check_improve (ch, gsn_track, TRUE, 1);
		  WAIT_STATE (ch, 1);
		  ch->move -= move;
		  return;
		}
	    }
	  act ("You can find no recent tracks for $N.", ch, NULL, victim,
	       TO_CHAR);
	  check_improve (ch, gsn_track, FALSE, 1);
	  WAIT_STATE (ch, 1);
	  ch->move -= move / 2;
	  return;
	}
    }
  for (victim = char_list; victim != NULL; victim = victim->next)
    {
      if ((victim->in_room != NULL)
	  && IS_NPC (victim)
	  && can_see (ch, victim)
	  && (victim->in_room->area == ch->in_room->area)
	  && is_name (arg, victim->name))
	{
	  if (victim->in_room->vnum == track_vnum)
	    {
	      act ("The tracks end right under $S feet.", ch, NULL, victim,
		   TO_CHAR);
	      return;
	    }
	  for (track = 0; track < MAX_TRACK; track++)
	    {
	      if (victim->track_from[track] == track_vnum)
		{
		  for (door = 0; door < 12; door++)
		    {
		      if ((pexit = in_room->exit[door]) != NULL)
			{
			  if (pexit->u1.to_room->vnum ==
			      victim->track_to[track])
			    {
			      sprintf (buf,
				       "Some tracks lead off to the %s.\n\r",
				       dir_name[door]);
			      send_to_char (buf, ch);
			      check_improve (ch, gsn_track, TRUE, 1);
			      WAIT_STATE (ch, 1);
			      ch->move -= move;
			      return;
			    }
			}
		    }
		  act ("$N seems to have vanished here.", ch, NULL, victim,
		       TO_CHAR);
		  check_improve (ch, gsn_track, TRUE, 1);
		  WAIT_STATE (ch, 1);
		  ch->move -= move;
		  return;
		}
	    }
	}
    }
  sprintf (buf, "You can find no recent tracks for %s.\n\r", arg);
  send_to_char (buf, ch);
  check_improve (ch, gsn_track, FALSE, 1);
  WAIT_STATE (ch, 1);
  ch->move -= move / 2;
  return;
}



void
do_consider (CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  char *msg;
  int diff, vac, cac;

  one_argument (argument, arg);

  if (arg[0] == '\0')
    {
      send_to_char ("Consider killing whom?\n\r", ch);
      return;
    }

  if ((victim = get_char_room (ch, arg)) == NULL)
    {
      send_to_char ("They're not here.\n\r", ch);
      return;
    }

  if (is_safe (ch, victim))
    {
      send_to_char ("Don't even think about it.\n\r", ch);
      return;
    }

  diff = ((victim->hit / 50) - (ch->hit / 50));
  vac =
    -(GET_AC (victim, AC_PIERCE) + GET_AC (victim, AC_BASH) +
      GET_AC (victim, AC_SLASH) + GET_AC (victim, AC_EXOTIC));
  cac =
    -(GET_AC (ch, AC_PIERCE) + GET_AC (ch, AC_BASH) + GET_AC (ch, AC_SLASH) +
      GET_AC (ch, AC_EXOTIC));
  diff += (vac - cac);
  diff += (GET_DAMROLL (victim) - GET_DAMROLL (ch));
  diff += (GET_HITROLL (victim) - GET_HITROLL (ch));
  diff += (get_curr_stat (victim, STAT_STR) - get_curr_stat (ch, STAT_STR));

  if (diff <= -110)
    msg = "You can kill $N naked and weaponless.";
  else if (diff <= -70)
    msg = "$N is no match for you.";
  else if (diff <= -20)
    msg = "$N looks like an easy kill.";
  else if (diff <= 20)
    msg = "The perfect match!";
  else if (diff <= 70)
    msg = "$N says '{aDo you feel lucky, punk?{x'.";
  else if (diff <= 110)
    msg = "$N laughs at you mercilessly.";
  else
    msg = "Death will thank you for your gift.";

  act (msg, ch, NULL, victim, TO_CHAR);
  return;
}

void
set_title (CHAR_DATA * ch, char *title)
{
  char buf[MAX_STRING_LENGTH];

  if (IS_NPC (ch))
    {
      bug ("Set_title: NPC.", 0);
      return;
    }

  if (title[0] != '.' && title[0] != ',' && title[0] != '!'
      && title[0] != '?')
    {
      buf[0] = ' ';
      strcpy (buf + 1, title);
    }
  else
    {
      strcpy (buf, title);
    }

  free_string (ch->pcdata->title);
  ch->pcdata->title = str_dup (buf);
  return;
}



void
do_title (CHAR_DATA * ch, char *argument)
{
  int value;

  if (IS_NPC (ch))
    return;

  if (IS_SET (ch->act, PLR_NOTITLE))
    return;

  if ((ch->in_room->vnum == ROOM_VNUM_CORNER) && (!IS_IMMORTAL (ch)))
    {
      send_to_char
	("Just keep your nose in the corner like a good little player.\n\r",
	 ch);
      return;
    }

  if (argument[0] == '\0')
    {
      send_to_char ("Change your title to what?\n\r", ch);
      return;
    }

  if (strlen (argument) > 45)
    {
      argument[45] = '{';
      argument[46] = 'x';
      argument[47] = '\0';
    }
  else
    {
      value = strlen (argument);
      argument[value] = '{';
      argument[value + 1] = 'x';
      argument[value + 2] = '\0';
    }
  smash_tilde (argument);
  set_title (ch, argument);
  send_to_char ("Ok.\n\r", ch);
}



void
do_description (CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];

  if (argument[0] != '\0')
    {
      buf[0] = '\0';
      smash_tilde (argument);

      if (argument[0] == '-')
	{
	  int len;
	  bool found = FALSE;

	  if (ch->description == NULL || ch->description[0] == '\0')
	    {
	      send_to_char ("No lines left to remove.\n\r", ch);
	      return;
	    }

	  strcpy (buf, ch->description);

	  for (len = strlen (buf); len > 0; len--)
	    {
	      if (buf[len] == '\r')
		{
		  if (!found)	/* back it up */
		    {
		      if (len > 0)
			len--;
		      found = TRUE;
		    }
		  else		/* found the second one */
		    {
		      buf[len + 1] = '\0';
		      free_string (ch->description);
		      ch->description = str_dup (buf);
		      send_to_char ("Your description is:\n\r", ch);
		      send_to_char (ch->description ? ch->description :
				    "(None).\n\r", ch);
		      return;
		    }
		}
	    }
	  buf[0] = '\0';
	  free_string (ch->description);
	  ch->description = str_dup (buf);
	  send_to_char ("Description cleared.\n\r", ch);
	  return;
	}
      if (argument[0] == '+')
	{
	  if (ch->description != NULL)
	    strcat (buf, ch->description);
	  argument++;
	  while (isspace (*argument))
	    argument++;
	}

      if (strlen (buf) + strlen (argument) >= MAX_STRING_LENGTH - 2)
	{
	  send_to_char ("Description too long.\n\r", ch);
	  return;
	}

      strcat (buf, argument);
      strcat (buf, "\n\r");
      free_string (ch->description);
      ch->description = str_dup (buf);
    }

  send_to_char ("Your description is:\n\r", ch);
  send_to_char (ch->description ? ch->description : "(None).\n\r", ch);
  return;
}



void
do_report (CHAR_DATA * ch, char *argument)
{
  char buf[MAX_INPUT_LENGTH];

  sprintf (buf,
	   "You say '{SI have %d/%d hp %d/%d mana %d/%d mv %ld xp.{x'\n\r",
	   ch->hit, ch->max_hit,
	   ch->mana, ch->max_mana, ch->move, ch->max_move, ch->exp);

  send_to_char (buf, ch);

  sprintf (buf, "$n says '{SI have %d/%d hp %d/%d mana %d/%d mv %ld xp.{x'",
	   ch->hit, ch->max_hit,
	   ch->mana, ch->max_mana, ch->move, ch->max_move, ch->exp);

  act (buf, ch, NULL, NULL, TO_ROOM);

  return;
}



void
do_practice (CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  int sn;

  if (IS_NPC (ch))
    return;

  if (argument[0] == '\0')
    {
      int col;

      col = 0;
      for (sn = 0; sn < MAX_SKILL; sn++)
	{
	  if (skill_table[sn].name == NULL)
	    break;
	  if (ch->level < skill_table[sn].skill_level[ch->class]
	      || ch->pcdata->learned[sn] < 1 /* skill is not known */ )
	    continue;

	  sprintf (buf, "%-18s %3d%%  ",
		   skill_table[sn].name, ch->pcdata->learned[sn]);
	  send_to_char (buf, ch);
	  if (++col % 3 == 0)
	    send_to_char ("\n\r", ch);
	}

      if (col % 3 != 0)
	send_to_char ("\n\r", ch);

      sprintf (buf, "You have %d practice sessions left.\n\r", ch->practice);
      send_to_char (buf, ch);
    }
  else
    {
      CHAR_DATA *mob;
      int adept;

      if (!IS_AWAKE (ch))
	{
	  send_to_char ("In your dreams, or what?\n\r", ch);
	  return;
	}

      for (mob = ch->in_room->people; mob != NULL; mob = mob->next_in_room)
	{
	  if (IS_NPC (mob) && IS_SET (mob->act, ACT_PRACTICE))
	    break;
	}

      if (mob == NULL)
	{
	  send_to_char ("You can't do that here.\n\r", ch);
	  return;
	}

      if (ch->practice <= 0)
	{
	  send_to_char ("You have no practice sessions left.\n\r", ch);
	  return;
	}

      if ((sn = find_spell (ch, argument)) < 0 || (!IS_NPC (ch) && (ch->level < skill_table[sn].skill_level[ch->class] || ch->pcdata->learned[sn] < 1	/* skill is not known */
								    ||
								    skill_table
								    [sn].
								    rating
								    [ch->
								     class] ==
								    0)))
	{
	  send_to_char ("You can't practice that.\n\r", ch);
	  return;
	}

      adept = IS_NPC (ch) ? 100 : class_table[ch->class].skill_adept;

      if (ch->pcdata->learned[sn] >= adept)
	{
	  sprintf (buf, "You are already learned at %s.\n\r",
		   skill_table[sn].name);
	  send_to_char (buf, ch);
	}
      else
	{
	  ch->practice--;
	  ch->pcdata->learned[sn] +=
	    int_app[get_curr_stat (ch, STAT_INT)].learn /
	    skill_table[sn].rating[ch->class];
	  if (ch->pcdata->learned[sn] < adept)
	    {
	      act ("You practice $T.",
		   ch, NULL, skill_table[sn].name, TO_CHAR);
	      act ("$n practices $T.",
		   ch, NULL, skill_table[sn].name, TO_ROOM);
	    }
	  else
	    {
	      ch->pcdata->learned[sn] = adept;
	      act ("You are now learned at $T.",
		   ch, NULL, skill_table[sn].name, TO_CHAR);
	      act ("$n is now learned at $T.",
		   ch, NULL, skill_table[sn].name, TO_ROOM);
	    }
	}
    }
  return;
}



/*
 * 'Wimpy' originally by Dionysos.
 */
void
do_wimpy (CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  int wimpy;

  one_argument (argument, arg);

  if (arg[0] == '\0')
    wimpy = ch->max_hit / 5;
  else
    wimpy = atoi (arg);

  if (wimpy < 0)
    {
      send_to_char ("Your courage exceeds your wisdom.\n\r", ch);
      return;
    }

  if (wimpy > ch->max_hit / 2)
    {
      send_to_char ("Such cowardice ill becomes you.\n\r", ch);
      return;
    }

  ch->wimpy = wimpy;
  sprintf (buf, "Wimpy set to %d hit points.\n\r", wimpy);
  send_to_char (buf, ch);
  return;
}



void
do_password (CHAR_DATA * ch, char *argument)
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char *pArg;
  char *pwdnew;
  char *p;
  char cEnd;

  if (IS_NPC (ch))
    return;

  /*
   * Can't use one_argument here because it smashes case.
   * So we just steal all its code.  Bleagh.
   */
  pArg = arg1;
  while (isspace (*argument))
    argument++;

  cEnd = ' ';
  if (*argument == '\'' || *argument == '"')
    cEnd = *argument++;

  while (*argument != '\0')
    {
      if (*argument == cEnd)
	{
	  argument++;
	  break;
	}
      *pArg++ = *argument++;
    }
  *pArg = '\0';

  pArg = arg2;
  while (isspace (*argument))
    argument++;

  cEnd = ' ';
  if (*argument == '\'' || *argument == '"')
    cEnd = *argument++;

  while (*argument != '\0')
    {
      if (*argument == cEnd)
	{
	  argument++;
	  break;
	}
      *pArg++ = *argument++;
    }
  *pArg = '\0';

  if (arg1[0] == '\0' || arg2[0] == '\0')
    {
      send_to_char ("Syntax: password <old> <new>.\n\r", ch);
      return;
    }

  if (strcmp (crypt (arg1, ch->pcdata->pwd), ch->pcdata->pwd))
    {
      WAIT_STATE (ch, 40);
      send_to_char ("Wrong password.  Wait 10 seconds.\n\r", ch);
      return;
    }

    if ( strlen(arg2) < 5 )
    {
	send_to_char(
	    "New password must be at least five characters long.\n\r", ch );
	return;
    }

  /*
   * No tilde allowed because of player file format.
   */
  pwdnew = crypt (arg2, ch->name);
  for (p = pwdnew; *p != '\0'; p++)
    {
      if (*p == '~')
	{
	  send_to_char ("New password not acceptable, try again.\n\r", ch);
	  return;
	}
    }

  free_string (ch->pcdata->pwd);
  ch->pcdata->pwd = str_dup (pwdnew);
  save_char_obj (ch);
  send_to_char ("Ok.\n\r", ch);
  return;
}
