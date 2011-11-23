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
 **************************************************************************/

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
#include <time.h>
#include "merc.h"
#include "recycle.h"
#include "tables.h"

/* command procedures needed */
DECLARE_DO_FUN (do_quit);
DECLARE_DO_FUN (do_announce);

/* RT code to delete yourself */

void
do_delet (CHAR_DATA * ch, char *argument)
{
  send_to_char ("You must type the full command to delete yourself.\n\r", ch);
}

void
do_delete (CHAR_DATA * ch, char *argument)
{
  char strsave[MAX_INPUT_LENGTH];

  if (IS_NPC (ch))
    return;

  if (ch->pcdata->confirm_delete)
    {
      if (argument[0] != '\0')
	{
	  send_to_char ("Delete status removed.\n\r", ch);
	  ch->pcdata->confirm_delete = FALSE;
	  return;
	}
      else
	{
	  if (is_clead (ch))
	    update_clanlist (ch, ch->clead, FALSE, TRUE);
	  if (is_clan (ch))
	    update_clanlist (ch, ch->clead, FALSE, FALSE);
	  sprintf (strsave, "%s%s", PLAYER_DIR, capitalize (ch->name));
	  wiznet ("$N turns $Mself into line noise.", ch, NULL, 0, 0, 0);
	  stop_fighting (ch, TRUE);
	  if (ch->level > HERO)
	    {
	      update_wizlist (ch, 1);
	    }
	  do_quit (ch, "");
	  unlink (strsave);
	  return;
	}
    }

  if (argument[0] != '\0')
    {
      send_to_char ("Just type delete. No argument.\n\r", ch);
      return;
    }

  send_to_char ("Type delete again to confirm this command.\n\r", ch);
  send_to_char ("WARNING: this command is irreversible.\n\r", ch);
  send_to_char ("Typing delete with an argument will undo delete status.\n\r",
		ch);
  ch->pcdata->confirm_delete = TRUE;
  wiznet ("$N is contemplating deletion.", ch, NULL, 0, 0, get_trust (ch));
}


/* RT code to display channel status */

void
do_channels (CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];

  /* lists all channels and their status */
  send_to_char ("   channel     status\n\r", ch);
  send_to_char ("{^---------------------{x\n\r", ch);

  send_to_char ("gossip         ", ch);
  if (!IS_SET (ch->comm, COMM_NOGOSSIP))
    send_to_char ("{2ON{x\n\r", ch);
  else
    send_to_char ("{1OF{xF\n\r", ch);

  send_to_char ("clan gossip    ", ch);
  if (!IS_SET (ch->comm, COMM_NOCGOSSIP))
    send_to_char ("{2ON{x\n\r", ch);
  else
    send_to_char ("{1OFF{x\n\r", ch);

  send_to_char ("quest gossip   ", ch);
  if (!IS_SET (ch->comm, COMM_NOCGOSSIP))
    send_to_char ("{2ON{x\n\r", ch);
  else
    send_to_char ("{1OFF{x\n\r", ch);

  send_to_char ("ooc            ", ch);
  if (!IS_SET (ch->comm, COMM_NOOOC))
    send_to_char ("{2ON{x\n\r", ch);
  else
    send_to_char ("{1OFF{x\n\r", ch);

  send_to_char ("music          ", ch);
  if (!IS_SET (ch->comm, COMM_NOMUSIC))
    send_to_char ("{2ON{x\n\r", ch);
  else
    send_to_char ("{1OFF{x\n\r", ch);

  send_to_char ("Q/A            ", ch);
  if (!IS_SET (ch->comm, COMM_NOASK))
    send_to_char ("{2ON{x\n\r", ch);
  else
    send_to_char ("{1OFF{x\n\r", ch);

  send_to_char ("Quote          ", ch);
  if (!IS_SET (ch->comm, COMM_NOQUOTE))
    send_to_char ("{2ON{x\n\r", ch);
  else
    send_to_char ("{1OFF{x\n\r", ch);

  send_to_char ("grats          ", ch);
  if (!IS_SET (ch->comm, COMM_NOGRATS))
    send_to_char ("{2ON{x\n\r", ch);
  else
    send_to_char ("{1OFF{x\n\r", ch);

  if (IS_IMMORTAL (ch))
    {
      send_to_char ("god channel    ", ch);
      if (!IS_SET (ch->comm, COMM_NOWIZ))
	send_to_char ("{2ON{x\n\r", ch);
      else
	send_to_char ("{1OFF{x\n\r", ch);
    }

  send_to_char ("shouts         ", ch);
  if (!IS_SET (ch->comm, COMM_SHOUTSOFF))
    send_to_char ("{2ON{x\n\r", ch);
  else
    send_to_char ("{1OFF{x\n\r", ch);

  send_to_char ("tells          ", ch);
  if (!IS_SET (ch->comm, COMM_DEAF))
    send_to_char ("{2ON{x\n\r", ch);
  else
    send_to_char ("{1OFF{x\n\r", ch);

  send_to_char ("quiet mode     ", ch);
  if (IS_SET (ch->comm, COMM_QUIET))
    send_to_char ("{2ON{x\n\r", ch);
  else
    send_to_char ("{1OFF{x\n\r", ch);

  if (IS_SET (ch->comm, COMM_AFK))
    send_to_char ("You are {3AFK{x.\n\r", ch);

  if (IS_SET (ch->comm, COMM_STORE))
    send_to_char ("You store tells during fights.\n\r", ch);

  if (IS_SET (ch->comm, COMM_SNOOP_PROOF))
    send_to_char ("You are immune to snooping.\n\r", ch);

  if (ch->lines != PAGELEN)
    {
      if (ch->lines)
	{
	  sprintf (buf, "You display %d lines of scroll.\n\r", ch->lines + 2);
	  send_to_char (buf, ch);
	}
      else
	send_to_char ("Scroll buffering is off.\n\r", ch);
    }

  if (ch->prompt != NULL)
    {
      sprintf (buf, "Your current prompt is: %s\n\r", ch->prompt);
      send_to_char (buf, ch);
    }

  if (IS_SET (ch->comm, COMM_NOSHOUT))
    send_to_char ("You cannot shout.\n\r", ch);

  if (IS_SET (ch->comm, COMM_NOTELL))
    send_to_char ("You cannot use tell.\n\r", ch);

  if (IS_SET (ch->comm, COMM_NOCHANNELS))
    send_to_char ("You cannot use channels.\n\r", ch);

  if (IS_SET (ch->comm, COMM_NOEMOTE))
    send_to_char ("You cannot show emotions.\n\r", ch);

}

/* RT deaf blocks out all shouts */

void
do_deaf (CHAR_DATA * ch, char *argument)
{

  if (IS_SET (ch->comm, COMM_DEAF))
    {
      send_to_char ("You can now hear {%tells{x again.\n\r", ch);
      REMOVE_BIT (ch->comm, COMM_DEAF);
    }
  else
    {
      send_to_char ("From now on, you won't hear {%tells{x.\n\r", ch);
      SET_BIT (ch->comm, COMM_DEAF);
    }
}

/* RT quiet blocks out all communication */

void
do_quiet (CHAR_DATA * ch, char *argument)
{
  if (IS_SET (ch->comm, COMM_QUIET))
    {
      send_to_char ("Quiet mode removed.\n\r", ch);
      REMOVE_BIT (ch->comm, COMM_QUIET);
    }
  else
    {
      send_to_char ("From now on, you will only hear says and emotes.\n\r",
		    ch);
      SET_BIT (ch->comm, COMM_QUIET);
    }
}

/* afk command */

void
do_afk (CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];

  if (IS_SET (ch->comm, COMM_AFK))
    {
      if (ch->tells)
	{
	  sprintf (buf,
		   "AFK mode removed.  You have {R%d{x tells waiting.\n\r",
		   ch->tells);
	  send_to_char (buf, ch);
	  send_to_char ("Type '{6replay{x' to see {%tells{x.\n\r", ch);
	}
      else
	{
	  send_to_char ("{3AFK{x mode removed.  You have no {%tells{x waiting.\n\r",
			ch);
	}
      REMOVE_BIT (ch->comm, COMM_AFK);
    }
  else
    {
      send_to_char ("You are now in {3AFK{x mode.\n\r", ch);
      SET_BIT (ch->comm, COMM_AFK);
    }
}

void
do_autostore (CHAR_DATA * ch, char *argument)
{
  if (IS_SET (ch->comm, COMM_STORE))
    {
      send_to_char ("You will no longer store {%tells{x during fights.\n\r", ch);
      REMOVE_BIT (ch->comm, COMM_STORE);
    }
  else
    {
      send_to_char ("You will now store {%tells{x during fights.\n\r", ch);
      SET_BIT (ch->comm, COMM_STORE);
    }
}

void
do_replay (CHAR_DATA * ch, char *argument)
{
  if (IS_NPC (ch))
    {
      send_to_char ("You can't replay.\n\r", ch);
      return;
    }

  if (buf_string (ch->pcdata->buffer)[0] == '\0')
    {
      send_to_char ("You have no {%tells{x to replay.\n\r", ch);
      return;
    }

  page_to_char (buf_string (ch->pcdata->buffer), ch);
  clear_buf (ch->pcdata->buffer);
  ch->tells = 0;
}

/* RT ooc rewritten in ROM style */
void
do_ooc (CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  int wtime;

  if (argument[0] == '\0')
    {
      if (IS_SET (ch->comm, COMM_NOOOC))
	{
	  send_to_char ("Ooc channel is now ON.\n\r", ch);
	  REMOVE_BIT (ch->comm, COMM_NOOOC);
	}
      else
	{
	  send_to_char ("Ooc channel is now OFF.\n\r", ch);
	  SET_BIT (ch->comm, COMM_NOOOC);
	}
    }
  else				/* ooc message sent, turn ooc on if it is off */
    {
      if (IS_SET (ch->comm, COMM_QUIET))
	{
	  send_to_char ("You must turn off quiet mode first.\n\r", ch);
	  return;
	}

      if (IS_SET (ch->comm, COMM_NOCHANNELS))
	{
	  send_to_char ("The gods have revoked your channel priviliges.\n\r",
			ch);
	  return;

	  REMOVE_BIT (ch->comm, COMM_NOOOC);
	}

      if ((ch->in_room->vnum == ROOM_VNUM_CORNER) && (!IS_IMMORTAL (ch)))
	{
	  send_to_char
	    ("Just keep your nose in the corner like a good little player.\n\r",
	     ch);
	  return;
	}

      sprintf (buf, "You OOC '{A%s{x'\n\r", argument);
      send_to_char (buf, ch);
      for (d = descriptor_list; d != NULL; d = d->next)
	{
	  CHAR_DATA *victim;
	  int pos;
	  bool found = FALSE;

	  victim = d->original ? d->original : d->character;

	  if (d->connected == CON_PLAYING &&
	      d->character != ch &&
	      !IS_SET (victim->comm, COMM_NOOOC) &&
	      !IS_SET (victim->comm, COMM_QUIET))
	    {
	      for (pos = 0; pos < MAX_FORGET; pos++)
		{
		  if (victim->pcdata->forget[pos] == NULL)
		    break;
		  if (!str_cmp (ch->name, victim->pcdata->forget[pos]))
		    found = TRUE;
		}
	      if (!found)
		{
		  act_new ("$n OOC '{A$t{x'",
			   ch, argument, d->character, TO_VICT, POS_DEAD);
		}
	    }
	}
    }
  wtime = UMAX (2, 9 - (ch->level));
  WAIT_STATE (ch, wtime);
}

/* RT chat replaced with ROM gossip */
void
do_gossip (CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  int wtime;

  if (argument[0] == '\0')
    {
      if (IS_SET (ch->comm, COMM_NOGOSSIP))
	{
	  send_to_char ("Gossip channel is now ON.\n\r", ch);
	  REMOVE_BIT (ch->comm, COMM_NOGOSSIP);
	}
      else
	{
	  send_to_char ("Gossip channel is now OFF.\n\r", ch);
	  SET_BIT (ch->comm, COMM_NOGOSSIP);
	}
    }
  else				/* gossip message sent, turn gossip on if it isn't already */
    {
      if (IS_SET (ch->comm, COMM_QUIET))
	{
	  send_to_char ("You must turn off quiet mode first.\n\r", ch);
	  return;
	}

      if (IS_SET (ch->comm, COMM_NOCHANNELS))
	{
	  send_to_char ("The gods have revoked your channel priviliges.\n\r",
			ch);
	  return;
	}

      if ((ch->in_room->vnum == ROOM_VNUM_CORNER) && (!IS_IMMORTAL (ch)))
	{
	  send_to_char
	    ("Just keep your nose in the corner like a good little player.\n\r",
	     ch);
	  return;
	}

      REMOVE_BIT (ch->comm, COMM_NOGOSSIP);

      sprintf (buf, "You {bgo{Bs{Ws{Bi{bp{x '{m%s{x'\n\r", argument);
      send_to_char (buf, ch);
      for (d = descriptor_list; d != NULL; d = d->next)
	{
	  CHAR_DATA *victim;
	  int pos;
	  bool found = FALSE;

	  victim = d->original ? d->original : d->character;

	  if (d->connected == CON_PLAYING &&
	      d->character != ch &&
	      !IS_SET (victim->comm, COMM_NOGOSSIP) &&
	      !IS_SET (victim->comm, COMM_QUIET))
	    {
	      for (pos = 0; pos < MAX_FORGET; pos++)
		{
		  if (victim->pcdata->forget[pos] == NULL)
		    break;
		  if (!str_cmp (ch->name, victim->pcdata->forget[pos]))
		    found = TRUE;
		}
	      if (!found)
		{
		  act_new ("{W({bgo{Bs{Ws{Bi{bp{W) {B$n{x '{m$t{x'",
			   ch, argument, d->character, TO_VICT, POS_SLEEPING);
		}
	    }
	}
    }
  wtime = UMAX (2, 9 - (ch->level));
  WAIT_STATE (ch, wtime);
}


void
do_qgossip (CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  int wtime;

  if (!ch->on_quest && !IS_IMMORTAL (ch))
    {
      send_to_char ("You are not on a quest!\n\r", ch);
      return;
    }
  if (argument[0] == '\0')
    {
      if (IS_SET (ch->comm, COMM_NOQGOSSIP))
	{
	  send_to_char ("Quest gossip channel is now ON.\n\r", ch);
	  REMOVE_BIT (ch->comm, COMM_NOQGOSSIP);
	}
      else
	{
	  send_to_char ("Quest gossip channel is now OFF.\n\r", ch);
	  SET_BIT (ch->comm, COMM_NOQGOSSIP);
	}
    }
  else				/* qgossip message sent, turn qgossip on if it isn't already */
    {
      if (IS_SET (ch->comm, COMM_QUIET))
	{
	  send_to_char ("You must turn off quiet mode first.\n\r", ch);
	  return;
	}

      if (IS_SET (ch->comm, COMM_NOCHANNELS))
	{
	  send_to_char ("The gods have revoked your channel priviliges.\n\r",
			ch);
	  return;
	}

      if ((ch->in_room->vnum == ROOM_VNUM_CORNER) && (!IS_IMMORTAL (ch)))
	{
	  send_to_char
	    ("Just keep your nose in the corner like a good little player.\n\r",
	     ch);
	  return;
	}

      REMOVE_BIT (ch->comm, COMM_NOQGOSSIP);

      sprintf (buf, "You qgossip '{l%s{x'\n\r", argument);
      send_to_char (buf, ch);
      for (d = descriptor_list; d != NULL; d = d->next)
	{
	  CHAR_DATA *victim;
	  int pos;
	  bool found = FALSE;

	  victim = d->original ? d->original : d->character;

	  if (d->connected == CON_PLAYING &&
	      d->character != ch &&
	      !IS_SET (victim->comm, COMM_NOQGOSSIP) &&
	      !IS_SET (victim->comm, COMM_QUIET) &&
	      ((victim->on_quest) || IS_IMMORTAL (victim)))
	    {
	      for (pos = 0; pos < MAX_FORGET; pos++)
		{
		  if (victim->pcdata->forget[pos] == NULL)
		    break;
		  if (!str_cmp (ch->name, victim->pcdata->forget[pos]))
		    found = TRUE;
		}
	      if (!found)
		{
		  act_new ("$n qgossips '{l$t{x'",
			   ch, argument, d->character, TO_VICT, POS_SLEEPING);
		}
	    }
	}
    }
  wtime = UMAX (2, 9 - (ch->level));
  WAIT_STATE (ch, wtime);
}


void
do_grats (CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  //    char bufy[MAX_STRING_LENGTH];
  //    char bufz[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  int wtime;

  buf[0] = '\0';
  //bufy[0] = '\0';
  //bufz[0] = '\0';
  if (argument[0] == '\0')
    {
      if (IS_SET (ch->comm, COMM_NOGRATS))
	{
	  send_to_char ("Grats channel is now ON.\n\r", ch);
	  REMOVE_BIT (ch->comm, COMM_NOGRATS);
	}
      else
	{
	  send_to_char ("Grats channel is now OFF.\n\r", ch);
	  SET_BIT (ch->comm, COMM_NOGRATS);
	}
    }
  else				/* grats message sent, turn grats on if it isn't already */
    {
      if (IS_SET (ch->comm, COMM_QUIET))
	{
	  send_to_char ("You must turn off quiet mode first.\n\r", ch);
	  return;
	}

      if (IS_SET (ch->comm, COMM_NOCHANNELS))
	{
	  send_to_char ("The gods have revoked your channel priviliges.\n\r",
			ch);
	  return;
	}

      if ((ch->in_room->vnum == ROOM_VNUM_CORNER) && (!IS_IMMORTAL (ch)))
	{
	  send_to_char
	    ("Just keep your nose in the corner like a good little player.\n\r",
	     ch);
	  return;
	}

      REMOVE_BIT (ch->comm, COMM_NOGRATS);

      sprintf (buf, "You grats '{J%s{x'\n\r", argument);
      send_to_char (buf, ch);
      for (d = descriptor_list; d != NULL; d = d->next)
	{
	  CHAR_DATA *victim;
	  int pos;
	  bool found = FALSE;

	  victim = d->original ? d->original : d->character;

	  if (d->connected == CON_PLAYING &&
	      d->character != ch &&
	      !IS_SET (victim->comm, COMM_NOGRATS) &&
	      !IS_SET (victim->comm, COMM_QUIET))
	    {
	      for (pos = 0; pos < MAX_FORGET; pos++)
		{
		  if (victim->pcdata->forget[pos] == NULL)
		    break;
		  if (!str_cmp (ch->name, victim->pcdata->forget[pos]))
		    found = TRUE;
		}
	      if (!found)
		{
		  act_new ("$n grats '{J$t{x'",
			   ch, argument, d->character, TO_VICT, POS_SLEEPING);
		}
	    }
	}
    }
  wtime = UMAX (2, 9 - (ch->level));
  WAIT_STATE (ch, wtime);
}

void
do_quote (CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  int wtime;

  if (argument[0] == '\0')
    {
      if (IS_SET (ch->comm, COMM_NOQUOTE))
	{
	  send_to_char ("Quote channel is now ON.\n\r", ch);
	  REMOVE_BIT (ch->comm, COMM_NOQUOTE);
	}
      else
	{
	  send_to_char ("Quote channel is now OFF.\n\r", ch);
	  SET_BIT (ch->comm, COMM_NOQUOTE);
	}
    }
  else				/* quote message sent, turn quote on if it isn't already */
    {
      if (IS_SET (ch->comm, COMM_QUIET))
	{
	  send_to_char ("You must turn off quiet mode first.\n\r", ch);
	  return;
	}

      if (IS_SET (ch->comm, COMM_NOCHANNELS))
	{
	  send_to_char ("The gods have revoked your channel priviliges.\n\r",
			ch);
	  return;
	}

      if ((ch->in_room->vnum == ROOM_VNUM_CORNER) && (!IS_IMMORTAL (ch)))
	{
	  send_to_char
	    ("Just keep your nose in the corner like a good little player.\n\r",
	     ch);
	  return;
	}

      REMOVE_BIT (ch->comm, COMM_NOQUOTE);

      sprintf (buf, "You quote '{Q%s{x'\n\r", argument);
      send_to_char (buf, ch);
      for (d = descriptor_list; d != NULL; d = d->next)
	{
	  CHAR_DATA *victim;
	  int pos;
	  bool found = FALSE;

	  victim = d->original ? d->original : d->character;

	  if (d->connected == CON_PLAYING &&
	      d->character != ch &&
	      !IS_SET (victim->comm, COMM_NOQUOTE) &&
	      !IS_SET (victim->comm, COMM_QUIET))
	    {
	      for (pos = 0; pos < MAX_FORGET; pos++)
		{
		  if (victim->pcdata->forget[pos] == NULL)
		    break;
		  if (!str_cmp (ch->name, victim->pcdata->forget[pos]))
		    found = TRUE;
		}
	      if (!found)
		{
		  act_new ("$n quotes '{Q$t{x'",
			   ch, argument, d->character, TO_VICT, POS_SLEEPING);
		}
	    }
	}
    }
  wtime = UMAX (2, 9 - (ch->level));
  WAIT_STATE (ch, wtime);
}

void
social_channel (const char *format, CHAR_DATA * ch, const void *arg2,
		int type)
{
  static char *const he_she[] = { "it", "he", "she" };
  static char *const him_her[] = { "it", "him", "her" };
  static char *const his_her[] = { "its", "his", "her" };

  CHAR_DATA *to;
  CHAR_DATA *vch = (CHAR_DATA *) arg2;
  const char *str;
  char *i;
  char *point;
  char *i2;
  char fixed[MAX_STRING_LENGTH];
  char buf[MAX_STRING_LENGTH];
  bool fColour = FALSE;

  if ((ch->in_room->vnum == ROOM_VNUM_CORNER) && (!IS_IMMORTAL (ch)))
    {
      send_to_char
	("Just keep your nose in the corner like a good little player.\n\r",
	 ch);
      return;
    }

  if (IS_SET (ch->comm, COMM_QUIET))
    {
      send_to_char ("You must turn off quiet mode first.\n\r", ch);
      return;
    }

  if (IS_SET (ch->comm, COMM_NOCHANNELS))
    {
      send_to_char ("The gods have revoked your channel priviliges.\n\r", ch);
      return;
    }

  if (!format || !*format)
    return;

  if (!ch || !ch->desc || !ch->in_room)
    return;

  to = char_list;
  if (type == TO_VICT)
    {
      if (!vch)
	{
	  bug ("Act: null vch with TO_VICT.", 0);
	  return;
	}

      if (!vch->in_room)
	return;

      to = vch;
    }
  if (type == TO_CHAR)
    {
      if (ch == NULL)
	{
	  return;
	}
      to = ch;
    }

  for (; to; to = to->next)
    {
      if (!to->desc || to->position < POS_RESTING || !to->in_room)
	continue;

      if (type == TO_CHAR && to != ch)
	continue;
      if (type == TO_VICT && (to != vch || to == ch))
	continue;
      if (type == TO_ROOM && to == ch)
	continue;
      if (type == TO_NOTVICT && (to == ch || to == vch))
	continue;
      if (IS_SET (to->comm, COMM_NOSOCIAL) || IS_SET (to->comm, COMM_QUIET))
	continue;
      point = buf;
      sprintf (buf, "[*SOC*] ");
      point += 8;
      str = format;
      while (*str)
	{
	  if (*str != '$' && *str != '{')
	    {
	      *point++ = *str++;
	      continue;
	    }

	  i = NULL;
	  switch (*str)
	    {
	    case '$':
	      fColour = TRUE;
	      ++str;
	      i = " <@@@> ";
	      if (!arg2 && *str >= 'A' && *str <= 'Z' && *str != 'G')
		{
		  bug ("Act: missing arg2 for code %d.", *str);
		  i = " <@@@> ";
		}
	      else
		{
		  switch (*str)
		    {
		    default:
		      bug ("Act: bad code %d.", *str);
		      i = " <@@@> ";
		      break;

		    case 'T':
		      i = (char *) arg2;
		      break;

		    case 'n':
		      i = PERS (ch, to);
		      break;

		    case 'N':
		      i = PERS (vch, to);
		      break;

		    case 'e':
		      i = he_she[URANGE (0, ch->sex, 2)];
		      break;

		    case 'E':
		      i = he_she[URANGE (0, vch->sex, 2)];
		      break;

		    case 'm':
		      i = him_her[URANGE (0, ch->sex, 2)];
		      break;

		    case 'M':
		      i = him_her[URANGE (0, vch->sex, 2)];
		      break;

		    case 's':
		      i = his_her[URANGE (0, ch->sex, 2)];
		      break;

		    case 'S':
		      i = his_her[URANGE (0, vch->sex, 2)];
		      break;


		    }
		}
	      break;

	    case '{':
	      fColour = FALSE;
	      ++str;
	      i = NULL;
	      if (IS_SET (to->act, PLR_COLOUR))
		{
		  i = colour (*str, to);
		}
	      break;

	    default:
	      fColour = FALSE;
	      *point++ = *str++;
	      break;
	    }

	  ++str;
	  if (fColour && i)
	    {
	      fixed[0] = '\0';
	      i2 = fixed;

	      if (IS_SET (to->act, PLR_COLOUR))
		{
		  for (i2 = fixed; *i; i++)
		    {
		      if (*i == '{')
			{
			  i++;
			  strcat (fixed, colour (*i, to));
			  for (i2 = fixed; *i2; i2++)
			    ;
			  if (*i == '\0')
			    break;
			  continue;
			}
		      *i2 = *i;
		      *++i2 = '\0';
		    }
		  *i2 = '\0';
		  i = &fixed[0];
		}
	      else
		{
		  for (i2 = fixed; *i; i++)
		    {
		      if (*i == '{')
			{
			  i++;
			  if (*i != '{')
			    {
			      continue;
			    }
			}
		      *i2 = *i;
		      *++i2 = '\0';
		    }
		  *i2 = '\0';
		  i = &fixed[0];
		}
	    }


	  if (i)
	    {
	      while ((*point = *i) != '\0')
		{
		  ++point;
		  ++i;
		}
	    }
	}

      *point++ = '\n';
      *point++ = '\r';
      *point = '\0';
      buf[0] = UPPER (buf[0]);
      send_to_char ("{W", to);
      if (to->desc)
	write_to_buffer (to->desc, buf, point - buf);
      send_to_char ("{x", to);
    }
  return;
}

void
do_social (CHAR_DATA * ch, char *argument)
{
  CHAR_DATA *victim;
  int cmd;
  char arg[MAX_INPUT_LENGTH];
  char command[MAX_INPUT_LENGTH];
  bool found = FALSE;

  if (*argument == '\0')
    {
      if (IS_SET (ch->comm, COMM_NOSOCIAL))
	{
	  REMOVE_BIT (ch->comm, COMM_NOSOCIAL);
	  send_to_char ("Social channel is now ON.\n\r", ch);
	}
      else
	{
	  SET_BIT (ch->comm, COMM_NOSOCIAL);
	  send_to_char ("Social channel is now OFF.\n\r", ch);
	}
      return;
    }

  if (IS_SET (ch->comm, COMM_NOSOCIAL))
    {
      send_to_char ("Turn on the social channel first.\n\r", ch);
      return;
    }
  if (IS_SET (ch->comm, COMM_QUIET))
    {
      send_to_char ("Turn off quiet first.\n\r", ch);
      return;
    }

  if ((ch->in_room->vnum == ROOM_VNUM_CORNER) && (!IS_IMMORTAL (ch)))
    {
      send_to_char
	("Just keep your nose in the corner like a good little player.\n\r",
	 ch);
      return;
    }
  argument = one_argument (argument, command);

  for (cmd = 0; social_table[cmd].name[0] != '\0'; cmd++)
    {
      if (command[0] == social_table[cmd].name[0]
	  && !str_prefix (command, social_table[cmd].name))
	{
	  found = TRUE;
	  break;
	}
    }
  if (found == FALSE)
    {
      send_to_char ("You wanna social what to who?!?!\n\r", ch);
      return;
    }

  one_argument (argument, arg);
  victim = NULL;
  if (arg[0] == '\0')
    {
      social_channel (social_table[cmd].others_no_arg, ch, victim, TO_ROOM);
      social_channel (social_table[cmd].char_no_arg, ch, victim, TO_CHAR);
    }
  else if ((victim = get_char_world (ch, arg)) == NULL)
    {
      send_to_char ("They aren't here.\n\r", ch);
    }
  else if (victim == ch)
    {
      social_channel (social_table[cmd].others_auto, ch, victim, TO_ROOM);
      social_channel (social_table[cmd].char_auto, ch, victim, TO_CHAR);
    }
  else
    {
      social_channel (social_table[cmd].others_found, ch, victim, TO_NOTVICT);
      social_channel (social_table[cmd].char_found, ch, victim, TO_CHAR);
      social_channel (social_table[cmd].vict_found, ch, victim, TO_VICT);
    }

  return;
}

void
do_gmote (CHAR_DATA * ch, char *argument)
{
  if (IS_SET (ch->comm, COMM_QUIET))
    {
      send_to_char ("Turn on the social channel first.\n\r", ch);
      return;
    }
  if (IS_SET (ch->comm, COMM_QUIET))
    {
      send_to_char ("Turn off quiet first.\n\r", ch);
      return;
    }
  if (!IS_NPC (ch) && IS_SET (ch->comm, COMM_NOEMOTE))
    {
      send_to_char ("You can't show your emotions.\n\r", ch);
      return;
    }

  if (IS_SET (ch->comm, COMM_NOCHANNELS))
    {
      send_to_char ("The gods have revoked your channel priviliges.\n\r", ch);
      return;
    }

  if ((ch->in_room->vnum == ROOM_VNUM_CORNER) && (!IS_IMMORTAL (ch)))
    {
      send_to_char
	("Just keep your nose in the corner like a good little player.\n\r",
	 ch);
      return;
    }

  if (argument[0] == '\0')
    {
      send_to_char ("Gmote what?\n\r", ch);
      return;
    }

/*    if ( ch->pcdata->condition[COND_DRUNK] > 0 )
       { argument = makedrunk (ch, argument); } */
  social_channel ("{B$n $T{x", ch, argument, TO_ROOM);
  social_channel ("{B$n $T{x", ch, argument, TO_CHAR);
  return;

}

/* RT ask channel */
void
do_ask (CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  int wtime;

  if (argument[0] == '\0')
    {
      if (IS_SET (ch->comm, COMM_NOASK))
	{
	  send_to_char ("Q/A channel is now ON.\n\r", ch);
	  REMOVE_BIT (ch->comm, COMM_NOASK);
	}
      else
	{
	  send_to_char ("Q/A channel is now OFF.\n\r", ch);
	  SET_BIT (ch->comm, COMM_NOASK);
	}
    }
  else				/* ask sent, turn Q/A on if it isn't already */
    {
      if (IS_SET (ch->comm, COMM_QUIET))
	{
	  send_to_char ("You must turn off quiet mode first.\n\r", ch);
	  return;
	}

      if (IS_SET (ch->comm, COMM_NOCHANNELS))
	{
	  send_to_char ("The gods have revoked your channel priviliges.\n\r",
			ch);
	  return;
	}

      if ((ch->in_room->vnum == ROOM_VNUM_CORNER) && (!IS_IMMORTAL (ch)))
	{
	  send_to_char
	    ("Just keep your nose in the corner like a good little player.\n\r",
	     ch);
	  return;
	}

      REMOVE_BIT (ch->comm, COMM_NOASK);

      sprintf (buf, "You ask '{P%s{x'\n\r", argument);
      send_to_char (buf, ch);
      for (d = descriptor_list; d != NULL; d = d->next)
	{
	  CHAR_DATA *victim;
	  int pos;
	  bool found = FALSE;

	  victim = d->original ? d->original : d->character;

	  if (d->connected == CON_PLAYING &&
	      d->character != ch &&
	      !IS_SET (victim->comm, COMM_NOASK) &&
	      !IS_SET (victim->comm, COMM_QUIET))
	    {
	      for (pos = 0; pos < MAX_FORGET; pos++)
		{
		  if (victim->pcdata->forget[pos] == NULL)
		    break;
		  if (!str_cmp (ch->name, victim->pcdata->forget[pos]))
		    found = TRUE;
		}
	      if (!found)
		{
		  act_new ("$n asks '{P$t{x'",
			   ch, argument, d->character, TO_VICT, POS_SLEEPING);
		}
	    }
	}
    }
  wtime = UMAX (2, 9 - (ch->level));
  WAIT_STATE (ch, wtime);
}

/* RT answer channel - uses same line as asks */
void
do_answer (CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  int wtime;

  if (argument[0] == '\0')
    {
      if (IS_SET (ch->comm, COMM_NOASK))
	{
	  send_to_char ("Q/A channel is now ON.\n\r", ch);
	  REMOVE_BIT (ch->comm, COMM_NOASK);
	}
      else
	{
	  send_to_char ("Q/A channel is now OFF.\n\r", ch);
	  SET_BIT (ch->comm, COMM_NOASK);
	}
    }
  else				/* answer sent, turn Q/A on if it isn't already */
    {
      if (IS_SET (ch->comm, COMM_QUIET))
	{
	  send_to_char ("You must turn off quiet mode first.\n\r", ch);
	  return;
	}

      if (IS_SET (ch->comm, COMM_NOCHANNELS))
	{
	  send_to_char ("The gods have revoked your channel priviliges.\n\r",
			ch);
	  return;
	}

      if ((ch->in_room->vnum == ROOM_VNUM_CORNER) && (!IS_IMMORTAL (ch)))
	{
	  send_to_char
	    ("Just keep your nose in the corner like a good little player.\n\r",
	     ch);
	  return;
	}

      REMOVE_BIT (ch->comm, COMM_NOASK);

      sprintf (buf, "You answer '{P%s{x'\n\r", argument);
      send_to_char (buf, ch);
      for (d = descriptor_list; d != NULL; d = d->next)
	{
	  CHAR_DATA *victim;
	  int pos;
	  bool found = FALSE;

	  victim = d->original ? d->original : d->character;

	  if (d->connected == CON_PLAYING &&
	      d->character != ch &&
	      !IS_SET (victim->comm, COMM_NOASK) &&
	      !IS_SET (victim->comm, COMM_QUIET))
	    {
	      for (pos = 0; pos < MAX_FORGET; pos++)
		{
		  if (victim->pcdata->forget[pos] == NULL)
		    break;
		  if (!str_cmp (ch->name, victim->pcdata->forget[pos]))
		    found = TRUE;
		}
	      if (!found)
		{
		  act_new ("$n answers '{P$t{x'",
			   ch, argument, d->character, TO_VICT, POS_SLEEPING);
		}
	    }
	}
    }
  wtime = UMAX (2, 9 - (ch->level));
  WAIT_STATE (ch, wtime);
}

/* RT music channel */
void
do_music (CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  int wtime;

  if (argument[0] == '\0')
    {
      if (IS_SET (ch->comm, COMM_NOMUSIC))
	{
	  send_to_char ("Music channel is now ON.\n\r", ch);
	  REMOVE_BIT (ch->comm, COMM_NOMUSIC);
	}
      else
	{
	  send_to_char ("Music channel is now OFF.\n\r", ch);
	  SET_BIT (ch->comm, COMM_NOMUSIC);
	}
    }
  else				/* music sent, turn music on if it isn't already */
    {
      if (IS_SET (ch->comm, COMM_QUIET))
	{
	  send_to_char ("You must turn off quiet mode first.\n\r", ch);
	  return;
	}

      if (IS_SET (ch->comm, COMM_NOCHANNELS))
	{
	  send_to_char ("The gods have revoked your channel priviliges.\n\r",
			ch);
	  return;
	}

      if ((ch->in_room->vnum == ROOM_VNUM_CORNER) && (!IS_IMMORTAL (ch)))
	{
	  send_to_char
	    ("Just keep your nose in the corner like a good little player.\n\r",
	     ch);
	  return;
	}

      REMOVE_BIT (ch->comm, COMM_NOMUSIC);

      sprintf (buf, "You MUSIC: '{N%s{x'\n\r", argument);
      send_to_char (buf, ch);
      sprintf (buf, "$n MUSIC: '{N%s{x'", argument);
      for (d = descriptor_list; d != NULL; d = d->next)
	{
	  CHAR_DATA *victim;
	  int pos;
	  bool found = FALSE;

	  victim = d->original ? d->original : d->character;

	  if (d->connected == CON_PLAYING &&
	      d->character != ch &&
	      !IS_SET (victim->comm, COMM_NOMUSIC) &&
	      !IS_SET (victim->comm, COMM_QUIET))
	    {
	      for (pos = 0; pos < MAX_FORGET; pos++)
		{
		  if (victim->pcdata->forget[pos] == NULL)
		    break;
		  if (!str_cmp (ch->name, victim->pcdata->forget[pos]))
		    found = TRUE;
		}
	      if (!found)
		{
		  act_new ("$n MUSIC: '{N$t{x'",
			   ch, argument, d->character, TO_VICT, POS_SLEEPING);
		}
	    }
	}
    }
  wtime = UMAX (2, 9 - (ch->level));
  WAIT_STATE (ch, wtime);
}

void
do_announce (CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;

  if (argument[0] == '\0')
    {
      if (IS_SET (ch->comm, COMM_NOANNOUNCE))
	{
	  send_to_char ("Info channel is now {2ON{x\n\r", ch);
	  REMOVE_BIT (ch->comm, COMM_NOANNOUNCE);
	}
      else
	{
	  send_to_char ("Info channel is now {1OFF{x\n\r", ch);
	  SET_BIT (ch->comm, COMM_NOANNOUNCE);
	}
      return;
    }

  REMOVE_BIT (ch->comm, COMM_NOANNOUNCE);

  sprintf (buf, "{D[{RINFO{D] {C$n {L%s{x", argument);
  act_new ("{D[{RINFO{D] {C$n {L$t{x", ch, argument, NULL, TO_CHAR, POS_DEAD);
  for (d = descriptor_list; d != NULL; d = d->next)
    {
      if (d->connected == CON_PLAYING &&
	  d->character != ch && !IS_SET (d->character->comm, COMM_NOANNOUNCE))
	{
	  act_new ("{D[{RINFO{D] {C$n {L$t{x", ch, argument, d->character,
		   TO_VICT, POS_DEAD);
	}
    }

  return;
}

void
do_immtalk (CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;

  if (argument[0] == '\0')
    {
      if (IS_SET (ch->comm, COMM_NOWIZ))
	{
	  send_to_char ("Immortal channel is now {2ON{x\n\r", ch);
	  REMOVE_BIT (ch->comm, COMM_NOWIZ);
	}
      else
	{
	  send_to_char ("Immortal channel is now {1OFF{x\n\r", ch);
	  SET_BIT (ch->comm, COMM_NOWIZ);
	}
      return;
    }

  REMOVE_BIT (ch->comm, COMM_NOWIZ);

  sprintf (buf, "{M[ {r$n {M] {C%s{x", argument);
  act_new ("{M[ {r$n {M] {C$t{x", ch, argument, NULL, TO_CHAR, POS_DEAD);
  for (d = descriptor_list; d != NULL; d = d->next)
    {
      if (d->connected == CON_PLAYING &&
	  IS_IMMORTAL (d->character) &&
	  !IS_SET (d->character->comm, COMM_NOWIZ))
	{
	  act_new ("{M[ {r$n {M] {C$t{x", ch, argument, d->character, TO_VICT,
		   POS_DEAD);
	}
    }

  return;
}


void
do_say (CHAR_DATA * ch, char *argument)
{
  if (argument[0] == '\0')
    {
      send_to_char ("Say what?\n\r", ch);
      return;
    }

  act ("{Y $c {rs{Ra{ry{Rs{x '{W$T{x'", ch, NULL, argument, TO_ROOM);
  act ("You {rs{Ra{ry{x '{R$T{x'", ch, NULL, argument, TO_CHAR);

  if (!IS_NPC (ch))
    {
      CHAR_DATA *mob, *mob_next;
      for (mob = ch->in_room->people; mob != NULL; mob = mob_next)
	{
	  mob_next = mob->next_in_room;
	  if (IS_NPC (mob) && HAS_TRIGGER (mob, TRIG_SPEECH)
	      && mob->position == mob->pIndexData->default_pos)
	    mp_act_trigger (argument, mob, ch, NULL, NULL, TRIG_SPEECH);
	}
    }
  return;
}



void
do_shout (CHAR_DATA * ch, char *argument)
{
  DESCRIPTOR_DATA *d;

  if (argument[0] == '\0')
    {
      if (IS_SET (ch->comm, COMM_SHOUTSOFF))
	{
	  send_to_char ("You can hear shouts again.\n\r", ch);
	  REMOVE_BIT (ch->comm, COMM_SHOUTSOFF);
	}
      else
	{
	  send_to_char ("You will no longer hear shouts.\n\r", ch);
	  SET_BIT (ch->comm, COMM_SHOUTSOFF);
	}
      return;
    }

  if (IS_SET (ch->comm, COMM_NOSHOUT))
    {
      send_to_char ("You can't shout.\n\r", ch);
      return;
    }

  if ((ch->in_room->vnum == ROOM_VNUM_CORNER) && (!IS_IMMORTAL (ch)))
    {
      send_to_char
	("Just keep your nose in the corner like a good little player.\n\r",
	 ch);
      return;
    }

  REMOVE_BIT (ch->comm, COMM_SHOUTSOFF);

  WAIT_STATE (ch, 12);

  act ("You shout '{T$T{x'", ch, NULL, argument, TO_CHAR);
  for (d = descriptor_list; d != NULL; d = d->next)
    {
      CHAR_DATA *victim;
      int pos;
      bool found = FALSE;

      victim = d->original ? d->original : d->character;

      if (d->connected == CON_PLAYING &&
	  d->character != ch &&
	  !IS_SET (victim->comm, COMM_SHOUTSOFF) &&
	  !IS_SET (victim->comm, COMM_QUIET))
	{
	  for (pos = 0; pos < MAX_FORGET; pos++)
	    {
	      if (victim->pcdata->forget[pos] == NULL)
		break;
	      if (!str_cmp (ch->name, victim->pcdata->forget[pos]))
		found = TRUE;
	    }
	  if (!found)
	    {
	      act ("$n shouts '{T$t{x'", ch, argument, d->character, TO_VICT);
	    }
	}
    }

  return;
}



void
do_tell (CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  int pos;
  bool found = FALSE;
  int wtime;

  if (IS_SET (ch->comm, COMM_NOTELL) || IS_SET (ch->comm, COMM_DEAF))
    {
      send_to_char ("Your message didn't get through.\n\r", ch);
      return;
    }

  if (IS_SET (ch->comm, COMM_QUIET))
    {
      send_to_char ("You must turn off quiet mode first.\n\r", ch);
      return;
    }

  if (IS_SET (ch->comm, COMM_DEAF))
    {
      send_to_char ("You must turn off deaf mode first.\n\r", ch);
      return;
    }

  if ((ch->in_room->vnum == ROOM_VNUM_CORNER) && (!IS_IMMORTAL (ch)))
    {
      send_to_char
	("Just keep your nose in the corner like a good little player.\n\r",
	 ch);
      return;
    }

  argument = one_argument (argument, arg);

  if (arg[0] == '\0' || argument[0] == '\0')
    {
      send_to_char ("Tell whom what?\n\r", ch);
      return;
    }

  /*
   * Can tell to PC's anywhere, but NPC's only in same room.
   * -- Furey
   */
  if ((victim = get_char_world (ch, arg)) == NULL
      || (IS_NPC (victim) && victim->in_room != ch->in_room))
    {
      send_to_char ("They aren't here.\n\r", ch);
      return;
    }

  if (victim->desc == NULL && !IS_NPC (victim))
    {
      act ("$N seems to have misplaced $S link...try again later.",
	   ch, NULL, victim, TO_CHAR);
      sprintf (buf, "%s tells you '{U%s{x'\n\r", PERS (ch, victim), argument);
      buf[0] = UPPER (buf[0]);
      add_buf (victim->pcdata->buffer, buf);
      victim->tells++;
      return;
    }

  if (!IS_NPC (victim))
    {
      for (pos = 0; pos < MAX_FORGET; pos++)
	{
	  if (victim->pcdata->forget[pos] == NULL)
	    break;
	  if (!str_cmp (ch->name, victim->pcdata->forget[pos]))
	    found = TRUE;
	}
    }
  if (found)
    {
      act ("$N doesn't seem to be listening to you.", ch, NULL, victim,
	   TO_CHAR);
      return;
    }

/* Removed..Vengeance */
/*
    if ( !(IS_IMMORTAL(ch) && ch->level > LEVEL_IMMORTAL) && !IS_AWAKE(victim) )
    {
	act( "$E can't hear you.", ch, 0, victim, TO_CHAR );
	return;
    }
*/

  if ((IS_SET (victim->comm, COMM_QUIET) || IS_SET (victim->comm, COMM_DEAF))
      && !IS_IMMORTAL (ch))
    {
      act ("$E is not receiving tells.", ch, 0, victim, TO_CHAR);
      return;
    }

  if (IS_SET (victim->comm, COMM_AFK))
    {
      if (IS_NPC (victim))
	{
	  act ("$E is AFK, and not receiving tells.", ch, NULL, victim,
	       TO_CHAR);
	  return;
	}

      act ("$E is AFK, but your tell will go through when $E returns.",
	   ch, NULL, victim, TO_CHAR);
      sprintf (buf, "%s tells you '{U%s{x'\n\r", PERS (ch, victim), argument);
      buf[0] = UPPER (buf[0]);
      add_buf (victim->pcdata->buffer, buf);
      victim->tells++;
      return;
    }

  if (IS_SET (victim->comm, COMM_STORE) && (victim->fighting != NULL))
    {
      if (IS_NPC (victim))
	{
	  act ("$E is fighting, and not receiving tells.", ch, NULL, victim,
	       TO_CHAR);
	  return;
	}

      act ("$E is fighting, but your tell will go through when $E finishes.",
	   ch, NULL, victim, TO_CHAR);
      sprintf (buf, "%s tells you '{U%s{x'\n\r", PERS (ch, victim), argument);
      buf[0] = UPPER (buf[0]);
      add_buf (victim->pcdata->buffer, buf);
      victim->tells++;
      return;
    }

  act ("{MYou tell $N '{W$t{x'", ch, argument, victim, TO_CHAR);
  act_new ("{M$n tells you '{W$t{x'", ch, argument, victim, TO_VICT,
	   POS_DEAD);
  victim->reply = ch;

  if (!IS_NPC (ch) && IS_NPC (victim) && HAS_TRIGGER (victim, TRIG_SPEECH))
    mp_act_trigger (argument, victim, ch, NULL, NULL, TRIG_SPEECH);

  wtime = UMAX (2, 9 - (ch->level));
  WAIT_STATE (ch, wtime);

  return;
}



void
do_reply (CHAR_DATA * ch, char *argument)
{
  CHAR_DATA *victim;
  char buf[MAX_STRING_LENGTH];
  int pos;
  bool found = FALSE;
  int wtime;

  if (IS_SET (ch->comm, COMM_NOTELL))
    {
      send_to_char ("Your message didn't get through.\n\r", ch);
      return;
    }

  if ((ch->in_room->vnum == ROOM_VNUM_CORNER) && (!IS_IMMORTAL (ch)))
    {
      send_to_char
	("Just keep your nose in the corner like a good little player.\n\r",
	 ch);
      return;
    }

  if ((victim = ch->reply) == NULL)
    {
      send_to_char ("They aren't here.\n\r", ch);
      return;
    }

  if (victim->desc == NULL && !IS_NPC (victim))
    {
      act ("$N seems to have misplaced $S link...try again later.",
	   ch, NULL, victim, TO_CHAR);
      sprintf (buf, "%s tells you '{U%s{x'\n\r", PERS (ch, victim), argument);
      buf[0] = UPPER (buf[0]);
      add_buf (victim->pcdata->buffer, buf);
      victim->tells++;
      return;
    }

  if (!IS_NPC (victim))
    {
      for (pos = 0; pos < MAX_FORGET; pos++)
	{
	  if (victim->pcdata->forget[pos] == NULL)
	    break;
	  if (!str_cmp (ch->name, victim->pcdata->forget[pos]))
	    found = TRUE;
	}
    }
  if (found)
    {
      act ("$N doesn't seem to be listening to you.", ch, NULL, victim,
	   TO_CHAR);
      return;
    }

/* Removed...Vengeance */
/*
    if ( !IS_IMMORTAL(ch) && !IS_AWAKE(victim) )
    {
	act( "$E can't hear you.", ch, 0, victim, TO_CHAR );
	return;
    }
*/

  if ((IS_SET (victim->comm, COMM_QUIET) || IS_SET (victim->comm, COMM_DEAF))
      && !IS_IMMORTAL (ch) && !IS_IMMORTAL (victim))
    {
      act_new ("$E is not receiving tells.", ch, 0, victim, TO_CHAR,
	       POS_DEAD);
      return;
    }

/* Removed....Vengeance */
/*
    if (!IS_IMMORTAL(victim) && !IS_AWAKE(ch))
    {
	send_to_char( "In your dreams, or what?\n\r", ch );
	return;
    }
*/

  if (IS_SET (victim->comm, COMM_AFK))
    {
      if (IS_NPC (victim))
	{
	  act_new ("$E is AFK, and not receiving tells.",
		   ch, NULL, victim, TO_CHAR, POS_DEAD);
	  return;
	}

      act_new ("$E is AFK, but your tell will go through when $E returns.",
	       ch, NULL, victim, TO_CHAR, POS_DEAD);
      sprintf (buf, "%s tells you '{U%s{x'\n\r", PERS (ch, victim), argument);
      buf[0] = UPPER (buf[0]);
      add_buf (victim->pcdata->buffer, buf);
      victim->tells++;
      return;
    }

  act_new ("{RYou reply to $N{x '{W$t{x'", ch, argument, victim, TO_CHAR,
	   POS_DEAD);
  act_new ("{R$n replys to you{x '{W$t{x'", ch, argument, victim, TO_VICT,
	   POS_DEAD);
  victim->reply = ch;

  wtime = UMAX (2, 9 - (ch->level));
  WAIT_STATE (ch, wtime);

  return;
}



void
do_yell (CHAR_DATA * ch, char *argument)
{
  DESCRIPTOR_DATA *d;

  if (IS_SET (ch->comm, COMM_NOSHOUT))
    {
      send_to_char ("You can't yell.\n\r", ch);
      return;
    }

  if (argument[0] == '\0')
    {
      send_to_char ("Yell what?\n\r", ch);
      return;
    }

  if ((ch->in_room->vnum == ROOM_VNUM_CORNER) && (!IS_IMMORTAL (ch)))
    {
      send_to_char
	("Just keep your nose in the corner like a good little player.\n\r",
	 ch);
      return;
    }

  act ("You yell '{T$t{x'", ch, argument, NULL, TO_CHAR);
  for (d = descriptor_list; d != NULL; d = d->next)
    {
      int pos;
      bool found = FALSE;

      if (d->connected == CON_PLAYING
	  && d->character != ch
	  && d->character->in_room != NULL
	  && d->character->in_room->area == ch->in_room->area
	  && !IS_SET (d->character->comm, COMM_QUIET))
	{
	  for (pos = 0; pos < MAX_FORGET; pos++)
	    {
	      if (d->character->pcdata->forget[pos] == NULL)
		break;
	      if (!str_cmp (ch->name, d->character->pcdata->forget[pos]))
		found = TRUE;
	    }
	  if (!found)
	    {
	      act ("$n yells '{T$t{x'", ch, argument, d->character, TO_VICT);
	    }
	}
    }

  return;
}


void
do_emote (CHAR_DATA * ch, char *argument)
{
  if (!IS_NPC (ch) && IS_SET (ch->comm, COMM_NOEMOTE))
    {
      send_to_char ("You can't show your emotions.\n\r", ch);
      return;
    }

  if (argument[0] == '\0')
    {
      send_to_char ("Emote what?\n\r", ch);
      return;
    }

  MOBtrigger = FALSE;
  act ("{B$c $T{x", ch, NULL, argument, TO_ROOM);
  act ("{B$c $T{x", ch, NULL, argument, TO_CHAR);
  MOBtrigger = TRUE;
  return;
}


void
do_pmote (CHAR_DATA * ch, char *argument)
{
  CHAR_DATA *vch;
  char *letter, *name;
  char last[MAX_INPUT_LENGTH], temp[MAX_STRING_LENGTH];
  size_t matches = 0;

  if (!IS_NPC (ch) && IS_SET (ch->comm, COMM_NOEMOTE))
    {
      send_to_char ("You can't show your emotions.\n\r", ch);
      return;
    }

  if (argument[0] == '\0')
    {
      send_to_char ("Emote what?\n\r", ch);
      return;
    }

  act ("{c$n $t{x", ch, argument, NULL, TO_CHAR);

  for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
      if (vch->desc == NULL || vch == ch)
	continue;

      if ((letter = strstr (argument, vch->name)) == NULL)
	{
	  MOBtrigger = FALSE;
	  act ("{c$N $t{x", vch, argument, ch, TO_CHAR);
	  MOBtrigger = TRUE;
	  continue;
	}

      strcpy (temp, argument);
      temp[strlen (argument) - strlen (letter)] = '\0';
      last[0] = '\0';
      name = vch->name;

      for (; *letter != '\0'; letter++)
	{
	  if (*letter == '\'' && matches == strlen (vch->name))
	    {
	      strcat (temp, "r");
	      continue;
	    }

	  if (*letter == 's' && matches == strlen (vch->name))
	    {
	      matches = 0;
	      continue;
	    }

	  if (matches == strlen (vch->name))
	    {
	      matches = 0;
	    }

	  if (*letter == *name)
	    {
	      matches++;
	      name++;
	      if (matches == strlen (vch->name))
		{
		  strcat (temp, "you");
		  last[0] = '\0';
		  name = vch->name;
		  continue;
		}
	      strncat (last, letter, 1);
	      continue;
	    }

	  matches = 0;
	  strcat (temp, last);
	  strncat (temp, letter, 1);
	  last[0] = '\0';
	  name = vch->name;
	}

      MOBtrigger = FALSE;
      act ("{c$N $t{x", vch, temp, ch, TO_CHAR);
      MOBtrigger = TRUE;
    }

  return;
}


/*
 * All the posing stuff.
 */
struct pose_table_type
{
  char *message[2 * MAX_CLASS];
};

const struct pose_table_type pose_table[] = {
  {
   {
    "You sizzle with {Be{bn{Be{br{Bg{by{x.",
    "$n sizzles with {Be{bn{Be{br{Bg{by{x.",
    "You feel very {Wholy{x.",
    "$n looks very {Wholy{x.",
    "You perform a small card trick.",
    "$n performs a small card trick.",
    "You show your bulging muscles.",
    "$n shows $s pitiful muscles.",
    "You nonchalantly turn wine into water.",
    "$n nonchalantly turns wine into water.",
    "... 98, 99, 100 ... you do pushups.",
    "..1....mmph!.. 2 .. $n does pushups.",
    "You deftly steal everyone's weapon.",
    "$n deftly steals your weapon."}
   },

  {
   {
    "You turn into a butterfly, then return to your normal shape.",
    "$n turns into a butterfly, then returns to $s normal shape.",
    "You nonchalantly turn wine into water.",
    "$n nonchalantly turns wine into water.",
    "You wiggle your ears alternately.",
    "$n wiggles $s ears alternately.",
    "You crack nuts between your fingers.",
    "$n cracks nuts between $s fingers.",
    "A {Whalo{x appears over your head.",
    "A {Whalo{x appears over $n's head.",
    "Deep in prayer, you levitate.",
    "Deep in prayer, $n levitates.",
    "You steal the underwear off every person in the room.",
    "Your underwear is gone!  $n stole it!"}
   },

  {
   {
    "{BBlue sparks{x fly from your fingers.",
    "{BBlue sparks{x fly from $n's fingers.",
    "A {Whalo{x appears over your head.",
    "A {Whalo{x appears over $n's head.",
    "You nimbly tie yourself into a knot.",
    "$n nimbly ties $mself into a knot.",
    "You grizzle your teeth and look mean.",
    "$n grizzles $s teeth and looks mean.",
    "Crunch, crunch -- you munch a bottle.",
    "Crunch, crunch -- $n munches a bottle.",
    "You juggle with daggers, apples, and eyeballs.",
    "$n juggles with daggers, apples, and eyeballs.",
    "Little {rred{x lights dance in your {z{Re{xy{z{Re{xs.",
    "Little {rred{x lights dance in $n's {z{Re{xy{z{Re{xs."}
   },

  {
   {
    "Little {rred{x lights dance in your {z{Re{xy{z{Re{xs.",
    "Little {rred{x lights dance in $n's {z{Re{xy{z{Re{xs.",
    "You recite words of wisdom.",
    "$n recites words of wisdom.",
    "You juggle with daggers, apples, and eyeballs.",
    "$n juggles with daggers, apples, and eyeballs.",
    "You hit your head, and your eyes roll.",
    "$n hits $s head, and $s eyes roll.",
    "You nimbly tie yourself into a knot.",
    "$n nimbly ties $mself into a knot.",
    "A spot light hits you.",
    "A spot light hits $n.",
    "You count the money in everyone's pockets.",
    "Check your money, $n is counting it."}
   },

  {
   {
    "A slimy {ggreen monster{x appears before you and bows.",
    "A slimy {ggreen monster{x appears before $n and bows.",
    "Deep in prayer, you levitate.",
    "Deep in prayer, $n levitates.",
    "You steal the underwear off every person in the room.",
    "Your underwear is gone!  $n stole it!",
    "Crunch, crunch -- you munch a bottle.",
    "Crunch, crunch -- $n munches a bottle.",
    "A fire elemental singes your hair.",
    "A fire elemental singes $n's hair.",
    "The sun pierces through the clouds to illuminate you.",
    "The sun pierces through the clouds to illuminate $n.",
    "Your eyes dance with greed.",
    "$n's eyes dance with greed."}
   },

  {
   {
    "You turn everybody into a little pink elephant.",
    "You are turned into a little pink elephant by $n.",
    "An {Wangel{x consults you.",
    "An {Wangel{x consults $n.",
    "The dice roll ... and you win again.",
    "The dice roll ... and $n craps out.",
    "... 98, 99, 100 ... you do pushups.",
    "..1....mmph!.. 2 .. $n does pushups.",
    "Smoke and fumes leak from your nostrils.",
    "Smoke and fumes leak from $n's nostrils.",
    "You balance a pocket knife on your tongue.",
    "$n balances a pocket knife on your tongue.",
    "Watch your feet, you are juggling granite boulders.",
    "Watch your feet, $n is juggling granite boulders."}
   },

  {
   {
    "A small ball of light dances on your fingertips.",
    "A small ball of light dances on $n's fingertips.",
    "Your body {Yglows with an unearthly light{x.",
    "$n's body {Yglows with an unearthly light{x.",
    "You count the money in everyone's pockets.",
    "Check your money, $n is counting it.",
    "Arnold Schwarzenegger admires your physique.",
    "PeeWee Herman laughs at $n's physique.",
    "Everyone levitates as you pray.",
    "You levitate as $n prays.",
    "You produce a coin from everyone's ear.",
    "$n produces a coin from your ear.",
    "Oomph!  You squeeze water out of a granite boulder.",
    "Oomph!  $n squeezes water out of a granite boulder."}
   },

  {
   {
    "Smoke and fumes leak from your nostrils.",
    "Smoke and fumes leak from $n's nostrils.",
    "A spot light hits you.",
    "A spot light hits $n.",
    "You balance a pocket knife on your tongue.",
    "$n balances a pocket knife on your tongue.",
    "Watch your feet, you are juggling granite boulders.",
    "Watch your feet, $n is juggling granite boulders.",
    "A {Ccool breeze{x refreshes you.",
    "A {Ccool breeze{x refreshes $n.",
    "You step behind your {Dshadow{x.",
    "$n steps behind $s {Dshadow{x.",
    "Arnold Schwarzenegger admires your physique.",
    "PeeWee Herman laughs at $n's physique."}
   },

  {
   {
    "The light flickers as you rap in magical languages.",
    "The light flickers as $n raps in magical languages.",
    "Everyone levitates as you pray.",
    "You levitate as $n prays.",
    "You produce a coin from everyone's ear.",
    "$n produces a coin from your ear.",
    "Oomph!  You squeeze water out of a granite boulder.",
    "Oomph!  $n squeezes water out of a granite boulder.",
    "The sky changes {Rc{Co{Gl{Bo{Yr{x to match your eyes.",
    "The sky changes {Rc{Co{Gl{Bo{Yr{x to match $n's eyes.",
    "The ocean parts before you.",
    "The ocean parts before $n.",
    "Your head disappears.",
    "$n's head disappears. {D(I was tired of looking at it anyway){x"}
   },

  {
   {
    "Your head disappears.",
    "$n's head disappears. {D(I was tired of looking at it anyway){x",
    "A {Ccool breeze{x refreshes you.",
    "A {Ccool breeze{x refreshes $n.",
    "You step behind your {Dshadow{x.",
    "$n steps behind $s {Dshadow{x.",
    "You pick your teeth with a spear.",
    "$n picks $s teeth with a spear.",
    "The stones dance to your command.",
    "The stones dance to $n's command.",
    "A thunder cloud kneels to you.",
    "A thunder cloud kneels to $n.",
    "The Grey Mouser buys you a beer.",
    "The Grey Mouser buys $n a beer."}
   },

  {
   {
    "A fire elemental singes your hair.",
    "A fire elemental singes $n's hair.",
    "The sun pierces through the clouds to illuminate you.",
    "The sun pierces through the clouds to illuminate $n.",
    "Your eyes dance with greed.",
    "$n's eyes dance with greed.",
    "Everyone is swept off their foot by your hug.",
    "You are swept off your feet by $n's hug.",
    "Your head disappears.",
    "$n's head disappears. {D(I was tired of looking at it anyway){x",
    "A {Ccool breeze{x refreshes you.",
    "A {Ccool breeze{x refreshes $n.",
    "You step behind your {Dshadow{x.",
    "$n steps behind $s {Dshadow{x."}
   },

  {
   {
    "The sky changes {Rc{Co{Gl{Bo{Yr{x to match your eyes.",
    "The sky changes {Rc{Co{Gl{Bo{Yr{x to match $n's eyes.",
    "The ocean parts before you.",
    "The ocean parts before $n.",
    "You deftly steal everyone's weapon.",
    "$n deftly steals your weapon.",
    "Your karate chop splits a tree.",
    "$n's karate chop breaks $s hand.",
    "A thunder cloud kneels to you.",
    "A thunder cloud kneels to $n.",
    "The Grey Mouser buys you a beer.",
    "The Grey Mouser buys $n a beer.",
    "A strap of your armor breaks over your mighty thews.",
    "A strap of $n's armor breaks over $s rolls of fat."}
   },

  {
   {
    "The stones dance to your command.",
    "The stones dance to $n's command.",
    "A thunder cloud kneels to you.",
    "A thunder cloud kneels to $n.",
    "The Grey Mouser buys you a beer.",
    "The Grey Mouser buys $n a beer.",
    "A strap of your armor breaks over your mighty thews.",
    "A strap of $n's armor breaks over $s rolls of fat.",
    "The heavens and grass change colour as you smile.",
    "The heavens and grass change colour as $n smiles.",
    "The Burning Man speaks to you.",
    "The Burning Man speaks to $n.",
    "Everyone's pocket explodes with your fireworks.",
    "Your pocket explodes with $n's fireworks."}
   },

  {
   {
    "The heavens and grass change colour as you smile.",
    "The heavens and grass change colour as $n smiles.",
    "The Burning Man speaks to you.",
    "The Burning Man speaks to $n.",
    "Everyone's pocket explodes with your fireworks.",
    "Your pocket explodes with $n's fireworks.",
    "A boulder cracks at your frown.",
    "A boulder cracks at $n's frown.",
    "Everyone's clothes are transparent, and you are laughing.",
    "Your clothes are transparent, and $n is laughing.",
    "An eye in a pyramid w{zi{xnks at you.",
    "An eye in a pyramid w{zi{xnks at $n.",
    "Everyone discovers your dagger a centimeter from their eye.",
    "You discover $n's dagger a centimeter from your eye."}
   },

  {
   {
    "Everyone's clothes are transparent, and you are laughing.",
    "Your clothes are transparent, and $n is laughing.",
    "An eye in a pyramid w{zi{xnks at you.",
    "An eye in a pyramid w{zi{xnks at $n.",
    "Everyone discovers your dagger a centimeter from their eye.",
    "You discover $n's dagger a centimeter from your eye.",
    "Mercenaries arrive to do your bidding.",
    "Mercenaries arrive to do $n's bidding.",
    "A black hole swallows you.",
    "A black hole swallows $n.",
    "Valentine Michael Smith offers you a glass of water.",
    "Valentine Michael Smith offers $n a glass of water.",
    "Where did you go?",
    "Where did $n go?"}
   },

  {
   {
    "A black hole swallows you.",
    "A black hole swallows $n.",
    "Valentine Michael Smith offers you a glass of water.",
    "Valentine Michael Smith offers $n a glass of water.",
    "Where did you go?",
    "Where did $n go?",
    "Four matched Percherons bring in your chariot.",
    "Four matched Percherons bring in $n's chariot.",
    "The world shimmers in time with your whistling.",
    "The world shimmers in time with $n's whistling.",
    "The great god $G gives you a staff.",
    "The great god $G gives $n a staff.",
    "Everyone's clothes are transparent, and you are laughing.",
    "Your clothes are transparent, and $n is laughing."}
   },

  {
   {
    "The world shimmers in time with your whistling.",
    "The world shimmers in time with $n's whistling.",
    "The great god $G gives you a staff.",
    "The great god $G gives $n a staff.",
    "Click.",
    "Click.",
    "Atlas asks you to relieve him.",
    "Atlas asks $n to relieve him.",
    "Four matched Percherons bring in your chariot.",
    "Four matched Percherons bring in $n's chariot.",
    "Mercenaries arrive to do your bidding.",
    "Mercenaries arrive to do $n's bidding.",
    "A boulder cracks at your frown.",
    "A boulder cracks at $n's frown."}
   }
};



void
do_pose (CHAR_DATA * ch, char *argument)
{
  int level;
  int pose;

  if (IS_NPC (ch))
    return;

  level = UMIN (ch->level, (int)(sizeof (pose_table) / sizeof (pose_table[0])) - 1);
  pose = number_range (0, level);

  if (ch->class < MAX_CLASS / 2)
    {
      act (pose_table[pose].message[2 * ch->class + 0], ch, NULL, NULL,
	   TO_CHAR);
      act (pose_table[pose].message[2 * ch->class + 1], ch, NULL, NULL,
	   TO_ROOM);
    }
  else
    {
      act (pose_table[pose].message[2 * (ch->class - (MAX_CLASS / 2)) + 0],
	   ch, NULL, NULL, TO_CHAR);
      act (pose_table[pose].message[2 * (ch->class - (MAX_CLASS / 2)) + 1],
	   ch, NULL, NULL, TO_ROOM);
    }
  return;
}



void
do_bug (CHAR_DATA * ch, char *argument)
{
  append_file (ch, BUG_FILE, argument);
  send_to_char ("Bug logged.\n\r", ch);
  return;
}

void
do_typo (CHAR_DATA * ch, char *argument)
{
  append_file (ch, TYPO_FILE, argument);
  send_to_char ("Typo logged.\n\r", ch);
  return;
}

void
do_rent (CHAR_DATA * ch, char *argument)
{
  send_to_char ("There is no rent here.  Just save and quit.\n\r", ch);
  return;
}


void
do_qui (CHAR_DATA * ch, char *argument)
{
  send_to_char ("If you want to QUIT, you have to spell it out.\n\r", ch);
  return;
}



void
do_quit (CHAR_DATA * ch, char *argument)
{
  DESCRIPTOR_DATA *d, *d_next;
  int id;

  if (IS_NPC (ch))
    return;

  if (ch->position == POS_FIGHTING)
    {
      send_to_char ("No way! You are fighting.\n\r", ch);
      return;
    }

  if (ch->position < POS_STUNNED)
    {
      send_to_char ("You're not DEAD yet.\n\r", ch);
      return;
    }
  send_to_char ("{rReality is attacking you!{x\n\r", ch);
  send_to_char ("{cYou hit Reality hard!{x\n\r", ch);
  send_to_char ("{BReality's truth does UNSPEAKABLE things to you!{x\n\r\n\r",
		ch);
  WAIT_STATE (ch, 25 * PULSE_VIOLENCE);
  act ("$n has left the game.", ch, NULL, NULL, TO_ROOM);
  sprintf (log_buf, "%s has quit.", ch->name);
  log_string (log_buf);
  wiznet ("$N rejoins the real world.", ch, NULL, WIZ_LOGINS, 0,
	  get_trust (ch));

  /*
   * After extract_char the ch is no longer valid!
   */
  save_char_obj (ch);
  id = ch->id;
  d = ch->desc;
  extract_char (ch, TRUE);
  if (d != NULL)
    close_socket (d);

  /* toast evil cheating bastards */
  for (d = descriptor_list; d != NULL; d = d_next)
    {
      CHAR_DATA *tch;

      d_next = d->next;
      tch = d->original ? d->original : d->character;
      if (tch && tch->id == id)
	{
	  extract_char (tch, TRUE);
	  close_socket (d);
	}
    }

  return;
}

void
force_quit (CHAR_DATA * ch, char *argument)
{
  DESCRIPTOR_DATA *d, *d_next;
  int id;
  int lose;

  if (IS_NPC (ch))
    return;

  if (ch->level >= LEVEL_IMMORTAL)
    return;


  if (ch->position == POS_FIGHTING)
    {
      lose = (ch->desc != NULL) ? 25 : 50;
      gain_exp (ch, 0 - lose);
      stop_fighting (ch, TRUE);
    }

  if (ch->position < POS_STUNNED)
    {
      lose = (ch->desc != NULL) ? 50 : 100;
      gain_exp (ch, 0 - lose);
    }
  send_to_char ("{rReality is attacking you!{x\n\r", ch);
  send_to_char ("{cYou hit Reality hard!{x\n\r", ch);
  send_to_char ("{BReality's truth does UNSPEAKABLE things to you!{x\n\r\n\r",
		ch);
  WAIT_STATE (ch, 25 * PULSE_VIOLENCE);
  act ("$n has left the game.", ch, NULL, NULL, TO_ROOM);
  sprintf (log_buf, "%s has quit.", ch->name);
  log_string (log_buf);
  wiznet ("$N rejoins the real world.", ch, NULL, WIZ_LOGINS, 0,
	  get_trust (ch));

  /*
   * After extract_char the ch is no longer valid!
   */
  save_char_obj (ch);
  id = ch->id;
  d = ch->desc;
  extract_char (ch, TRUE);
  if (d != NULL)
    close_socket (d);

  /* toast evil cheating bastards */
  for (d = descriptor_list; d != NULL; d = d_next)
    {
      CHAR_DATA *tch;

      d_next = d->next;
      tch = d->original ? d->original : d->character;
      if (tch && tch->id == id)
	{
	  extract_char (tch, TRUE);
	  close_socket (d);
	}
    }

  return;
}


void
do_save (CHAR_DATA * ch, char *argument)
{
  if (IS_NPC (ch))
    return;

  save_char_obj (ch);
  send_to_char ("Saving. Remember that BlinkenMuD has automatic saving.\n\r",
		ch);
  WAIT_STATE (ch, 4 * PULSE_VIOLENCE);
  return;
}



void
do_follow (CHAR_DATA * ch, char *argument)
{
/* RT changed to allow unlimited following and follow the NOFOLLOW rules */
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  one_argument (argument, arg);

  if (arg[0] == '\0')
    {
      send_to_char ("Follow whom?\n\r", ch);
      return;
    }

  if ((victim = get_char_room (ch, arg)) == NULL)
    {
      send_to_char ("They aren't here.\n\r", ch);
      return;
    }

  if (IS_AFFECTED (ch, AFF_CHARM) && ch->master != NULL)
    {
      act ("But you'd rather follow $N!", ch, NULL, ch->master, TO_CHAR);
      return;
    }

  if (victim == ch)
    {
      if (ch->master == NULL)
	{
	  send_to_char ("You already follow yourself.\n\r", ch);
	  return;
	}
      stop_follower (ch);
      return;
    }

  if (!IS_NPC (victim) && IS_SET (victim->act, PLR_NOFOLLOW)
      && !IS_IMMORTAL (ch))
    {
      act ("$N doesn't seem to want any followers.\n\r",
	   ch, NULL, victim, TO_CHAR);
      return;
    }

  REMOVE_BIT (ch->act, PLR_NOFOLLOW);

  if (ch->master != NULL)
    stop_follower (ch);

  add_follower (ch, victim);
  return;
}


void
add_follower (CHAR_DATA * ch, CHAR_DATA * master)
{
  if (ch->master != NULL)
    {
      bug ("Add_follower: non-null master.", 0);
      return;
    }

  ch->master = master;
  ch->leader = NULL;

  if (can_see (master, ch))
    act ("$n now follows you.", ch, NULL, master, TO_VICT);

  act ("You now follow $N.", ch, NULL, master, TO_CHAR);

  return;
}

void
stop_follower (CHAR_DATA * ch)
{
  if (ch->master == NULL)
    {
      bug ("Stop_follower: null master.", 0);
      return;
    }

  if (IS_AFFECTED (ch, AFF_CHARM))
    {
      REMOVE_BIT (ch->affected_by, AFF_CHARM);
      affect_strip (ch, gsn_charm_person);
    }
  if (IS_NPC (ch) && IS_SET (ch->act, ACT_PET))
    {
      REMOVE_BIT (ch->act, ACT_PET);
      ch->timer = 0;
      ch->hastimer = TRUE;
    }

  if (can_see (ch->master, ch) && ch->in_room != NULL)
    {
      act ("$n stops following you.", ch, NULL, ch->master, TO_VICT);
      act ("You stop following $N.", ch, NULL, ch->master, TO_CHAR);
    }
  if (ch->master->pet == ch)
    ch->master->pet = NULL;

  ch->master = NULL;
  ch->leader = NULL;
  return;
}

/* nukes charmed monsters and pets */
void
nuke_pets (CHAR_DATA * ch)
{
  CHAR_DATA *pet;

  if ((pet = ch->pet) != NULL)
    {
      stop_follower (pet);
      if (pet->in_room != NULL)
	act ("$N slowly fades away.", ch, NULL, pet, TO_NOTVICT);
      extract_char (pet, TRUE);
    }
  ch->pet = NULL;

  return;
}



void
die_follower (CHAR_DATA * ch)
{
  CHAR_DATA *fch;

  if (ch->master != NULL)
    {
      if (ch->master->pet == ch)
	ch->master->pet = NULL;
      stop_follower (ch);
    }

  ch->leader = NULL;

  for (fch = char_list; fch != NULL; fch = fch->next)
    {
      if (fch->master == ch)
	stop_follower (fch);
      if (fch->leader == ch)
	fch->leader = fch;
    }

  return;
}



void
do_order (CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  CHAR_DATA *och;
  CHAR_DATA *och_next;
  bool found;
  bool fAll;

  argument = one_argument (argument, arg);
  one_argument (argument, arg2);

  if (!str_cmp (arg2, "delete") || !str_cmp (arg2, "mob"))
    {
      send_to_char ("That will NOT be done.\n\r", ch);
      return;
    }

  if (arg[0] == '\0' || argument[0] == '\0')
    {
      send_to_char ("Order whom to do what?\n\r", ch);
      return;
    }

  if (IS_AFFECTED (ch, AFF_CHARM))
    {
      send_to_char ("You feel like taking, not giving, orders.\n\r", ch);
      return;
    }

  if (!str_cmp (arg, "all"))
    {
      fAll = TRUE;
      victim = NULL;
    }
  else
    {
      fAll = FALSE;
      if ((victim = get_char_room (ch, arg)) == NULL)
	{
	  send_to_char ("They aren't here.\n\r", ch);
	  return;
	}

      if (victim == ch)
	{
	  send_to_char ("Aye aye, right away!\n\r", ch);
	  return;
	}

      if (!IS_AFFECTED (victim, AFF_CHARM) || victim->master != ch
	  || (IS_IMMORTAL (victim) && victim->trust >= ch->trust))
	{
	  send_to_char ("Do it yourself!\n\r", ch);
	  return;
	}
    }

  found = FALSE;
  for (och = ch->in_room->people; och != NULL; och = och_next)
    {
      och_next = och->next_in_room;

      if (IS_AFFECTED (och, AFF_CHARM)
	  && och->master == ch && (fAll || och == victim))
	{
	  found = TRUE;
	  sprintf (buf, "$n orders you to '%s'.", argument);
	  act (buf, ch, NULL, och, TO_VICT);
	  interpret (och, argument);
	}
    }

  if (found)
    {
      WAIT_STATE (ch, PULSE_VIOLENCE);
      send_to_char ("Ok.\n\r", ch);
    }
  else
    send_to_char ("You have no followers here.\n\r", ch);
  return;
}

void
do_group (CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  one_argument (argument, arg);

  if (arg[0] == '\0')
    {
      CHAR_DATA *gch;
      CHAR_DATA *leader;

      leader = (ch->leader != NULL) ? ch->leader : ch;
      sprintf (buf, "%s's group:\n\r", PERS (leader, ch));
      send_to_char (buf, ch);

      for (gch = char_list; gch != NULL; gch = gch->next)
	{
	  if (is_same_group (gch, ch))
	    {
	      sprintf (buf,
		       "[%2d %s] %-16s %4d/%4d hp %4d/%4d mana %4d/%4d mv %5ld xp\n\r",
		       gch->level,
		       IS_NPC (gch) ? "Mob" : class_table[gch->class].
		       who_name, capitalize (PERS (gch, ch)), gch->hit,
		       gch->max_hit, gch->mana, gch->max_mana, gch->move,
		       gch->max_move, gch->exp);
	      send_to_char (buf, ch);
	    }
	}
      return;
    }

  if ((victim = get_char_room (ch, arg)) == NULL)
    {
      send_to_char ("They aren't here.\n\r", ch);
      return;
    }

  if (ch->master != NULL || (ch->leader != NULL && ch->leader != ch))
    {
      send_to_char ("But you are following someone else!\n\r", ch);
      return;
    }

  if (victim->master != ch && ch != victim)
    {
      act_new ("$N isn't following you.", ch, NULL, victim, TO_CHAR,
	       POS_SLEEPING);
      return;
    }

  if (IS_AFFECTED (victim, AFF_CHARM))
    {
      send_to_char ("You can't remove charmed mobs from your group.\n\r", ch);
      return;
    }

  if (IS_AFFECTED (ch, AFF_CHARM))
    {
      act ("You like your master too much to leave $m!", ch, NULL, victim,
	   TO_VICT);
      return;
    }

  if (victim->level - ch->level > 14)
    {
      send_to_char ("They are to high of a level for your group.\n\r", ch);
      return;
    }

  if (victim->level - ch->level < -14)
    {
      send_to_char ("They are to low of a level for your group.\n\r", ch);
      return;
    }

  if (is_same_group (victim, ch) && ch != victim)
    {
      victim->leader = NULL;
      act ("$n removes $N from $s group.", ch, NULL, victim, TO_NOTVICT);
      act ("$n removes you from $s group.", ch, NULL, victim, TO_VICT);
      act ("You remove $N from your group.", ch, NULL, victim, TO_CHAR);
      return;
    }

  victim->leader = ch;
  act ("$N joins $n's group.", ch, NULL, victim, TO_NOTVICT);
  act ("You join $n's group.", ch, NULL, victim, TO_VICT);
  act ("$N joins your group.", ch, NULL, victim, TO_CHAR);
  return;
}



/*
 * 'Split' originally by Gnort, God of Chaos.
 */
void
do_split (CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char arg3[MAX_INPUT_LENGTH];
  CHAR_DATA *gch;
  int members;
  int amount_platinum = 0, amount_gold = 0, amount_silver = 0;
  int share_platinum, share_gold, share_silver;
  int extra_platinum, extra_gold, extra_silver;

  argument = one_argument (argument, arg1);
  argument = one_argument (argument, arg2);
  one_argument (argument, arg3);

  if (arg1[0] == '\0')
    {
      send_to_char ("Split how much?\n\r", ch);
      return;
    }

  amount_silver = atoi (arg1);

  if (arg2[0] != '\0')
    amount_gold = atoi (arg2);

  if (arg3[0] != '\0')
    amount_platinum = atoi (arg3);

  if (amount_platinum < 0 || amount_gold < 0 || amount_silver < 0)
    {
      send_to_char ("Your group wouldn't like that.\n\r", ch);
      return;
    }

  if (amount_platinum == 0 && amount_gold == 0 && amount_silver == 0)
    {
      send_to_char ("You hand out zero coins, but no one notices.\n\r", ch);
      return;
    }

  if ((ch->silver + (ch->gold * 100) + (ch->platinum * 10000))
      < (amount_silver + (amount_gold * 100) + (amount_platinum * 10000)))
    {
      send_to_char ("You don't have that much to split.\n\r", ch);
      return;
    }

  members = 0;
  for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room)
    {
      if (is_same_group (gch, ch) && !IS_AFFECTED (gch, AFF_CHARM))
	members++;
    }

  if (members < 2)
    {
      send_to_char ("Just keep it all.\n\r", ch);
      return;
    }

  share_platinum = amount_platinum / members;
  extra_platinum = amount_platinum % members;

  amount_gold += (extra_platinum * 100);
  share_gold = amount_gold / members;
  extra_gold = amount_gold % members;

  amount_silver += (extra_gold * 100);
  share_silver = amount_silver / members;
  extra_silver = amount_silver % members;

  if (share_platinum == 0 && share_gold == 0 && share_silver == 0)
    {
      send_to_char ("Don't even bother, cheapskate.\n\r", ch);
      return;
    }

  deduct_cost (ch, amount_platinum - extra_platinum, VALUE_PLATINUM);
  add_cost (ch, share_platinum, VALUE_PLATINUM);
  deduct_cost (ch, amount_gold - extra_gold, VALUE_GOLD);
  add_cost (ch, share_gold, VALUE_GOLD);
  deduct_cost (ch, amount_silver, VALUE_SILVER);
  add_cost (ch, share_silver + extra_silver, VALUE_SILVER);

  if (share_platinum > 0)
    {
      sprintf (buf,
	       "You split %d platinum coins. Your share is %d platinum.\n\r",
	       amount_platinum - extra_platinum, share_platinum);
      send_to_char (buf, ch);
    }
  if (share_gold > 0)
    {
      sprintf (buf,
	       "You split %d gold coins. Your share is %d gold.\n\r",
	       amount_gold - extra_gold, share_gold);
      send_to_char (buf, ch);
    }
  if (share_silver > 0)
    {
      sprintf (buf,
	       "You split %d silver coins. Your share is %d silver.\n\r",
	       amount_silver, share_silver + extra_silver);
      send_to_char (buf, ch);
    }

  if (share_gold == 0 && share_silver == 0)
    {
      sprintf (buf, "$n splits %d platinum coins. Your share is %d platinum.",
	       amount_platinum - extra_platinum, share_platinum);
    }
  else if (share_platinum == 0 && share_silver == 0)
    {
      sprintf (buf, "$n splits %d gold coins. Your share is %d gold.",
	       amount_gold - extra_gold, share_gold);
    }
  else if (share_platinum == 0 && share_gold == 0)
    {
      sprintf (buf, "$n splits %d silver coins. Your share is %d silver.",
	       amount_silver, share_silver);
    }
  else if (share_silver == 0)
    {
      sprintf (buf,
	       "$n splits %d platinum and %d gold coins. giving you %d platinum and %d gold.\n\r",
	       amount_platinum - extra_platinum, amount_gold - extra_gold,
	       share_platinum, share_gold);
    }
  else if (share_gold == 0)
    {
      sprintf (buf,
	       "$n splits %d platinum and %d silver coins. giving you %d platinum and %d silver.\n\r",
	       amount_platinum - extra_platinum, amount_silver,
	       share_platinum, share_silver);
    }
  else if (share_platinum == 0)
    {
      sprintf (buf,
	       "$n splits %d gold and %d silver coins. giving you %d gold and %d silver.\n\r",
	       amount_gold - extra_gold, amount_silver,
	       share_gold, share_silver);
    }
  else
    {
      sprintf (buf,
	       "$n splits %d platinum, %d gold and %d silver coins. giving you %d platinum, %d gold and %d silver.\n\r",
	       amount_platinum - extra_platinum, amount_gold - extra_gold,
	       amount_silver, share_platinum, share_gold, share_silver);
    }

  for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room)
    {
      if (gch != ch && is_same_group (gch, ch)
	  && !IS_AFFECTED (gch, AFF_CHARM))
	{
	  act (buf, ch, NULL, gch, TO_VICT);
	  add_cost (gch, share_platinum, VALUE_PLATINUM);
	  add_cost (gch, share_gold, VALUE_GOLD);
	  add_cost (gch, share_silver, VALUE_SILVER);
	}
    }

  return;
}



void
do_gtell (CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *gch;

  if (argument[0] == '\0')
    {
      send_to_char ("Tell your group what?\n\r", ch);
      return;
    }

  if (IS_SET (ch->comm, COMM_NOTELL))
    {
      send_to_char ("Your message didn't get through!\n\r", ch);
      return;
    }

  if ((ch->in_room->vnum == ROOM_VNUM_CORNER) && (!IS_IMMORTAL (ch)))
    {
      send_to_char
	("Just keep your nose in the corner like a good little player.\n\r",
	 ch);
      return;
    }

  /*
   * Note use of send_to_char, so gtell works on sleepers.
   */
  sprintf (buf, "%s tells the group '{K%s{x'\n\r", ch->name, argument);
  for (gch = char_list; gch != NULL; gch = gch->next)
    {
      if (is_same_group (gch, ch))
	send_to_char (buf, gch);
    }

  return;
}



/*
 * It is very important that this be an equivalence relation:
 * (1) A ~ A
 * (2) if A ~ B then B ~ A
 * (3) if A ~ B  and B ~ C, then A ~ C
 */
bool
is_same_group (CHAR_DATA * ach, CHAR_DATA * bch)
{
  if (ach == NULL || bch == NULL)
    return FALSE;

  if (ach->leader != NULL)
    ach = ach->leader;
  if (bch->leader != NULL)
    bch = bch->leader;
  return ach == bch;
}

/*
 * Colour setting and unsetting, way cool, Lope Oct '94
 */
void
do_colour (CHAR_DATA * ch, char *argument)
{
  char arg[MAX_STRING_LENGTH];
  int ccolor;

  argument = one_argument (argument, arg);

  if (!*arg)
    {
      if (!IS_SET (ch->act, PLR_COLOUR))
	{
	  SET_BIT (ch->act, PLR_COLOUR);
	  send_to_char ("{bC{ro{yl{co{mu{gr{x is now {rON{x, Way Cool!\n\r",
			ch);
	}
      else
	{
	  send_to_char_bw ("Colour is now OFF, <sigh>\n\r", ch);
	  REMOVE_BIT (ch->act, PLR_COLOUR);
	}
    }
  else if (!str_prefix (arg, "list"))
    {
      send_to_char ("\n\rColors:\n\r", ch);
      send_to_char ("     0 - Reset           9 - Bright Red\n\r", ch);
      send_to_char ("     1 - Red            10 - Bright Green\n\r", ch);
      send_to_char ("     2 - Green          11 - Yellow\n\r", ch);
      send_to_char ("     3 - Brown          12 - Bright Blue\n\r", ch);
      send_to_char ("     4 - Blue           13 - Bright Magenta\n\r", ch);
      send_to_char ("     5 - Magenta        14 - Bright Cyan\n\r", ch);
      send_to_char ("     6 - Cyan           15 - Bright White\n\r", ch);
      send_to_char ("     7 - White          16 - Black\n\r", ch);
      send_to_char ("     8 - Grey           17 - None\n\r", ch);
      send_to_char ("Channels:\n\r", ch);
      send_to_char ("     ooc        cgossip    clan\n\r", ch);
      send_to_char ("     gossip     grats      gtell\n\r", ch);
      send_to_char ("     immtalk    music      ask\n\r", ch);
      send_to_char ("     quote      say        shout\n\r", ch);
      send_to_char ("     tell       wiznet     mobsay\n\r", ch);
      send_to_char ("     room       condition  fight\n\r", ch);
      send_to_char ("     opponent   witness    disarm\n\r", ch);
      send_to_char ("     qgossip\n\r", ch);
      send_to_char ("For a more detailed list, see HELP COLORS\n\r", ch);
      send_to_char ("For a list of current settings, see HELP SETTINGS\n\r",
		    ch);
    }
  else if (!strcmp (arg, "0"))
    {
      ch->color = 0;
      send_to_char ("{xOK\n\r", ch);
    }
  else if (!strcmp (arg, "1"))
    {
      ch->color = 1;
      send_to_char ("{xOK\n\r", ch);
    }
  else if (!strcmp (arg, "2"))
    {
      ch->color = 2;
      send_to_char ("{xOK\n\r", ch);
    }
  else if (!strcmp (arg, "3"))
    {
      ch->color = 3;
      send_to_char ("{xOK\n\r", ch);
    }
  else if (!strcmp (arg, "4"))
    {
      ch->color = 4;
      send_to_char ("{xOK\n\r", ch);
    }
  else if (!strcmp (arg, "5"))
    {
      ch->color = 5;
      send_to_char ("{xOK\n\r", ch);
    }
  else if (!strcmp (arg, "6"))
    {
      ch->color = 6;
      send_to_char ("{xOK\n\r", ch);
    }
  else if (!strcmp (arg, "7"))
    {
      ch->color = 7;
      send_to_char ("{xOK\n\r", ch);
    }
  else if (!strcmp (arg, "8"))
    {
      ch->color = 8;
      send_to_char ("{xOK\n\r", ch);
    }
  else if (!strcmp (arg, "9"))
    {
      ch->color = 9;
      send_to_char ("{xOK\n\r", ch);
    }
  else if (!strcmp (arg, "10"))
    {
      ch->color = 10;
      send_to_char ("{xOK\n\r", ch);
    }
  else if (!strcmp (arg, "11"))
    {
      ch->color = 11;
      send_to_char ("{xOK\n\r", ch);
    }
  else if (!strcmp (arg, "12"))
    {
      ch->color = 12;
      send_to_char ("{xOK\n\r", ch);
    }
  else if (!strcmp (arg, "13"))
    {
      ch->color = 13;
      send_to_char ("{xOK\n\r", ch);
    }
  else if (!strcmp (arg, "14"))
    {
      ch->color = 14;
      send_to_char ("{xOK\n\r", ch);
    }
  else if (!strcmp (arg, "15"))
    {
      ch->color = 15;
      send_to_char ("{xOK\n\r", ch);
    }
  else if (!strcmp (arg, "16"))
    {
      ch->color = 16;
      send_to_char ("{xOK\n\r", ch);
    }
  else if (!strcmp (arg, "17"))
    {
      ch->color = 0;
      send_to_char ("{xOK\n\r", ch);
    }
  else if (argument[0] == '\0')
    {
      send_to_char ("Syntax: color {{list|#|<channel> #}\n\r", ch);
    }
  else if (!is_number (argument))
    {
      send_to_char ("Syntax: color {{list|#|<channel> #}\n\r", ch);
    }
  else
    {
      ccolor = atoi (argument);
      if (ccolor >= 18)
	{
	  send_to_char ("Color number must be 0-17\n\r", ch);
	  return;
	}
      if (!str_prefix (arg, "ooc"))
	{
	  ch->color_auc = ccolor;
	  send_to_char ("ooc channel set.\n\r", ch);
	}
      else if (!str_prefix (arg, "cgossip"))
	{
	  ch->color_cgo = ccolor;
	  send_to_char ("clan gossip channel set.\n\r", ch);
	}
      else if (!str_prefix (arg, "clan"))
	{
	  ch->color_cla = ccolor;
	  send_to_char ("clan talk channel set.\n\r", ch);
	}
      else if (!str_prefix (arg, "gossip"))
	{
	  ch->color_gos = ccolor;
	  send_to_char ("gossip channel set.\n\r", ch);
	}
      else if (!str_prefix (arg, "grats"))
	{
	  ch->color_gra = ccolor;
	  send_to_char ("grats channel set.\n\r", ch);
	}
      else if (!str_prefix (arg, "gtell"))
	{
	  ch->color_gte = ccolor;
	  send_to_char ("group tell channel set.\n\r", ch);
	}
      else if (!str_prefix (arg, "immtalk"))
	{
	  ch->color_imm = ccolor;
	  send_to_char ("immortal talk channel set.\n\r", ch);
	}
      else if (!str_prefix (arg, "music"))
	{
	  ch->color_mus = ccolor;
	  send_to_char ("music channel set.\n\r", ch);
	}
      else if (!str_prefix (arg, "ask"))
	{
	  ch->color_que = ccolor;
	  send_to_char ("question/answer channel set.\n\r", ch);
	}
      else if (!str_prefix (arg, "quote"))
	{
	  ch->color_quo = ccolor;
	  send_to_char ("quote channel set.\n\r", ch);
	}
      else if (!str_prefix (arg, "say"))
	{
	  ch->color_say = ccolor;
	  send_to_char ("say channel set.\n\r", ch);
	}
      else if (!str_prefix (arg, "shout"))
	{
	  ch->color_sho = ccolor;
	  send_to_char ("shout/yell channel set.\n\r", ch);
	}
      else if (!str_prefix (arg, "tell"))
	{
	  ch->color_tel = ccolor;
	  send_to_char ("tell/reply channel set.\n\r", ch);
	}
      else if (!str_prefix (arg, "wiznet"))
	{
	  ch->color_wiz = ccolor;
	  send_to_char ("wiznet channel set.\n\r", ch);
	}
      else if (!str_prefix (arg, "mobsay"))
	{
	  ch->color_mob = ccolor;
	  send_to_char ("mobile talk channel set.\n\r", ch);
	}
      else if (!str_prefix (arg, "room"))
	{
	  ch->color_roo = ccolor;
	  send_to_char ("room name display set.\n\r", ch);
	}
      else if (!str_prefix (arg, "condition"))
	{
	  ch->color_con = ccolor;
	  send_to_char ("character condition display set.\n\r", ch);
	}
      else if (!str_prefix (arg, "fight"))
	{
	  ch->color_fig = ccolor;
	  send_to_char ("your fight actions set.\n\r", ch);
	}
      else if (!str_prefix (arg, "opponent"))
	{
	  ch->color_opp = ccolor;
	  send_to_char ("opponents fight actions set.\n\r", ch);
	}
      else if (!str_prefix (arg, "disarm"))
	{
	  ch->color_dis = ccolor;
	  send_to_char ("disarm display set.\n\r", ch);
	}
      else if (!str_prefix (arg, "witness"))
	{
	  ch->color_wit = ccolor;
	  send_to_char ("witness fight actions set.\n\r", ch);
	}
      else if (!str_prefix (arg, "qgossip"))
	{
	  ch->color_qgo = ccolor;
	  send_to_char ("quest gossip channel set.\n\r", ch);
	}
      else
	{
	  send_to_char ("Syntax: color {{list|#|<channel> #}\n\r", ch);
	}

    }
  return;
}
