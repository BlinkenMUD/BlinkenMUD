/**************************************************
 * Original Code By : Gothar
 * Rewriten by : Wiccaan
 * Addons : None
 * Removes : - Thief System
 *           - Share System
 *
 * Contact Gothar : mcco0055@algonquinc.on.ca
 * Contact Wiccaan : wiccaan@comcast.net
 *
 * Beings that this code has been rewriten
 * by Wiccaan, Gothar may not be able to
 * help with this.
 * Credit goes to Gothar beings that he wrote it.
 * Wiccaan just redid it.
 ***************************************************/


#include <sys/types.h>
#include <sys/time.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "recycle.h"

/*
 * These are the vnums for your bank.
 * You can use more then one.
 * Beings that there isnt a bank in your mud yet,
 * Im putting a random number instead.
*/

/* Be sure to change this and add the area to your mud */
#define ROOM_VNUM_BANK 3367

/* Declare procedures that will be used */
DECLARE_DO_FUN (do_help);

/* Now for the code */

/* do_account is your balace, you can change this to do_balance if you want.
 * if you do change it, make sure to change the tables in interp.c and .h.
*/
void
do_account (CHAR_DATA * ch, char *argument)
{
  long plat = 0;
  long gold = 0;
  long silver = 0;

  char buf[MAX_STRING_LENGTH];

  plat = ch->pcdata->plat_bank;
  gold = ch->pcdata->gold_bank;
  silver = ch->pcdata->silver_bank;

/* IMM's, pet's, and npc's dont need bank accounts so we will remove them */
  if ((IS_NPC (ch) || IS_SET (ch->act, ACT_PET)) || (IS_IMMORTAL (ch)))
    {
      send_to_char ("Only players need accounts.\n\r", ch);
      return;
    }

  if (ch->in_room != get_room_index (ROOM_VNUM_BANK))
    {
      send_to_char ("You must be at the bank!\n\r", ch);
    }
  else if (ch->in_room == get_room_index (ROOM_VNUM_BANK))
    {
      sprintf (buf,
	       "You have Platinum: %10ld Gold: %10ld Silver: %10ld in your account.\n\r",
	       plat, gold, silver);
      send_to_char (buf, ch);
    }
}

/* do_deposit is to put money in the bank */
void
do_deposit (CHAR_DATA * ch, char *argument)
{
  long amount = 0;
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];

/* Remove IMM's, pet's, and npc's again */
  if ((IS_NPC (ch) || IS_SET (ch->act, ACT_PET)) || (IS_IMMORTAL (ch)))
    {
      send_to_char ("Only players need a bank account.\n\r", ch);
      return;
    }

/* If not in bank let them know */
  if (ch->in_room != get_room_index (ROOM_VNUM_BANK))
    {
      send_to_char ("You must be at the bank!\n\r", ch);
      return;
    }
  else				/* If they are in the bank */
    {
      argument = one_argument (argument, arg1);
      argument = one_argument (argument, arg2);
      if (arg1[0] == '\0' || arg2[0] == '\0')
	{
	  send_to_char ("How much do you want to deposit?\n\r", ch);
	  send_to_char ("Commands are:\n\r", ch);
	  send_to_char ("Deposite <value> plat\n\r", ch);
	  send_to_char ("Deposite <value> gold\n\r", ch);
	  send_to_char ("Deposite <value> silver\n\r", ch);
	  return;
	}

      if (ch->in_room == get_room_index (ROOM_VNUM_BANK))
	{
	  if (is_number (arg1))
	    {
	      amount = atoi (arg1);

	      if (amount <= 0)
		{
		  send_to_char ("To deposit money, you must give money!\r\n",
				ch);
		  send_to_char ("For more help type 'help bank'.\r\n", ch);
		  return;
		}

/* If they are depositing plat */
	      if (!str_cmp (arg2, "plat"))
		{
		  if (ch->platinum < amount)
		    {
		      send_to_char ("You dont have that much platinum.\n\r",
				    ch);
		      return;
		    }
		  else
		    {
		      ch->pcdata->plat_bank += amount;
		      ch->platinum -= amount;
		      act ("$n has deposited some platinum into $s account.",
			   ch, NULL, NULL, TO_ROOM);
		      sprintf (buf,
			       "You have deposited %ld Platinum.\n\r   Account: %10ld.\n\r   You still hold %8ld platinum.\n\r",
			       amount, ch->pcdata->plat_bank, ch->platinum);
		      send_to_char (buf, ch);
		      return;
		    }
		}


/* If they are depositing gold */
	      if (!str_cmp (arg2, "gold"))
		{
		  if (ch->gold < amount)
		    {
		      send_to_char ("You dont have that much gold.\n\r", ch);
		      return;
		    }
		  else
		    {
		      ch->pcdata->gold_bank += amount;
		      ch->gold -= amount;
		      act ("$n has deposited some gold into $s account.", ch,
			   NULL, NULL, TO_ROOM);
		      sprintf (buf,
			       "You have deposited %ld Gold.\n\r   Account: %10ld.\n\r   You still hold %8ld gold.\n\r",
			       amount, ch->pcdata->gold_bank, ch->gold);
		      send_to_char (buf, ch);
		      return;
		    }
		}

/* If they are depositing silver */
	      if (!str_cmp (arg2, "silver"))
		{
		  if (ch->silver < amount)
		    {
		      send_to_char ("You dont have that much silver.\n\r",
				    ch);
		      return;
		    }
		  else
		    {
		      ch->pcdata->silver_bank += amount;
		      ch->silver -= amount;
		      act ("$n has deposited some silver into $s account.",
			   ch, NULL, NULL, TO_ROOM);
		      sprintf (buf,
			       "You have deposited %ld Silver.\n\r   Account: %10ld silver.\n\r   You still hold %8ld silver.\n\r",
			       amount, ch->pcdata->silver_bank, ch->silver);
		      send_to_char (buf, ch);
		      return;
		    }
		}
	    }
	}
    }
  return;
}

void
do_withdraw (CHAR_DATA * ch, char *argument)
{
  long amount = 0;
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];

/* Remove IMM's, pet's, and npc's again */
  if ((IS_NPC (ch) || IS_SET (ch->act, ACT_PET)) || (IS_IMMORTAL (ch)))
    {
      send_to_char ("Only players need have a bank account.\n\r", ch);
      return;
    }

/* If not in bank let them know */
  if (ch->in_room != get_room_index (ROOM_VNUM_BANK))
    {
      send_to_char ("You must be at the bank!\n\r", ch);
    }
  else				/* If they are in the bank */
    {
      argument = one_argument (argument, arg1);
      argument = one_argument (argument, arg2);
      if (arg1[0] == '\0' || arg2[0] == '\0')
	{
	  send_to_char ("How much do you want to withdraw?\n\r", ch);
	  send_to_char ("Commands are:\n\r", ch);
	  send_to_char ("Withdraw <value> plat\n\r", ch);
	  send_to_char ("Withdraw <value> gold\n\r", ch);
	  send_to_char ("Withdraw <value> silver\n\r", ch);
	  return;
	}

      if (ch->in_room == get_room_index (ROOM_VNUM_BANK))
	{
	  if (is_number (arg1))
	    {
	      amount = atoi (arg1);

	      if (amount <= 0)
		{
		  send_to_char
		    ("To withdraw that amount, you have to have it in the bank first.\n\r",
		     ch);
		  send_to_char ("For more help type 'help bank'.\n\r", ch);
		  return;
		}

/* If they are withdrawing plat */
	      if (!str_cmp (arg2, "plat"))
		{
		  if (ch->pcdata->plat_bank < amount)
		    {
		      send_to_char
			("You dont have that much platinum in your account.\n\r",
			 ch);
		      return;
		    }
		  else
		    {
		      ch->pcdata->plat_bank -= amount;
		      ch->platinum += amount;
		      act ("$n withdraws platinum from $s account.\n\r", ch,
			   NULL, NULL, TO_ROOM);
		      sprintf (buf,
			       "You have withdrawn %ld Platinum.\n\r   Account: %10ld platunum.\n\r   You now hold: %8ld platinum.\n\r",
			       amount, ch->pcdata->plat_bank, ch->platinum);
		      send_to_char (buf, ch);
		      return;
		    }
		}

/* If they are withdrawing gold */
	      if (!str_cmp (arg2, "gold"))
		{
		  if (ch->pcdata->gold_bank < amount)
		    {
		      send_to_char
			("You dont have that much gold in your account.\n\r",
			 ch);
		      return;
		    }
		  else
		    {
		      ch->pcdata->gold_bank -= amount;
		      ch->gold += amount;
		      act ("$n withdraws gold from $s account.\n\r", ch, NULL,
			   NULL, TO_ROOM);
		      sprintf (buf,
			       "You have withdrawn %ld Gold.\n\r   Account: %10ld gold.\n\r   You now hold: %8ld gold.\n\r",
			       amount, ch->pcdata->gold_bank, ch->gold);
		      send_to_char (buf, ch);
		      return;
		    }
		}

/* If they are withdrawing silver */
	      if (!str_cmp (arg2, "silver"))
		{
		  if (ch->pcdata->silver_bank < amount)
		    {
		      send_to_char
			("You dont have that much silver in your account.\n\r",
			 ch);
		      return;
		    }
		  else
		    {
		      ch->pcdata->silver_bank -= amount;
		      ch->silver += amount;
		      act ("$n withdraws silver from $s account.\n\r", ch,
			   NULL, NULL, TO_ROOM);
		      sprintf (buf,
			       "You have withdrawn %ld Silver.\n\r   Account: %10ld silver.\n\r   You now hold: %8ld silver.\n\r",
			       amount, ch->pcdata->silver_bank, ch->silver);
		      send_to_char (buf, ch);
		      return;
		    }
		}
	    }
	}
    }
  return;
}
