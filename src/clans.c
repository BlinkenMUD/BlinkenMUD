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
#include "recycle.h"
#include "tables.h"
#include "lookup.h"

extern char                    boot_buf[MAX_STRING_LENGTH];
DECLARE_DO_FUN(	do_clanlist	);

CLN_DATA *cln_list;

const struct clan_type clan_table[MAX_CLAN] =
{
/*	name,		who entry,
	death-transfer room,	clan room entrance,
	clan pit vnum,
	independent,	pkill?		ext name

independent should be FALSE if is a real clan
pkill should be TRUE if pkilling is allowed

!!!DO NOT erase the first clan, all non-clanned players are
   placed in this first entry!!!

*/
    {"",		"",
	ROOM_VNUM_ALTAR,	ROOM_VNUM_ALTAR,
	OBJ_VNUM_PIT,
	TRUE,	FALSE,	"Unused" },

    {"loner",		" Loner  ",
	ROOM_VNUM_ALTAR,	ROOM_VNUM_ALTAR,
	OBJ_VNUM_PIT,
	TRUE,	TRUE,	"Unused" },

    {"devotion",		" {WD{De{Wv{Do{Wt{Di{Wo{Dn{x ",
	20050,		20051,
	20099,
	FALSE,	TRUE,	"The Immortals of Eternal Devotion"},

    {"empty_1",		" Unused ",
        ROOM_VNUM_ALTAR,        ROOM_VNUM_ALTAR,
        OBJ_VNUM_PIT,
        TRUE,   TRUE,   "Unused" },

    {"empty_2",		" Unused ",
        ROOM_VNUM_ALTAR,        ROOM_VNUM_ALTAR,
        OBJ_VNUM_PIT,
        TRUE,   TRUE,   "Unused" },

    {"empty_3",		" Unused ",
        ROOM_VNUM_ALTAR,        ROOM_VNUM_ALTAR,
        OBJ_VNUM_PIT,
        TRUE,   TRUE,   "Unused" },

    {"empty_4",		" Unused ",
        ROOM_VNUM_ALTAR,        ROOM_VNUM_ALTAR,
        OBJ_VNUM_PIT,
        TRUE,   TRUE,   "Unused" },

    {"empty_5",		" Unused ",
        ROOM_VNUM_ALTAR,        ROOM_VNUM_ALTAR,
        OBJ_VNUM_PIT,
        TRUE,   TRUE,   "Unused" },

    {"empty_6",		" Unused ",
        ROOM_VNUM_ALTAR,        ROOM_VNUM_ALTAR,
        OBJ_VNUM_PIT,
        TRUE,   TRUE,   "Unused" },

    {"empty_7",		" Unused ",
        ROOM_VNUM_ALTAR,        ROOM_VNUM_ALTAR,
        OBJ_VNUM_PIT,
        TRUE,   TRUE,   "Unused" },

    {"empty_8",		" Unused ",
        ROOM_VNUM_ALTAR,        ROOM_VNUM_ALTAR,
        OBJ_VNUM_PIT,
        TRUE,   TRUE,   "Unused" },

    {"empty_9",		" Unused ",
        ROOM_VNUM_ALTAR,        ROOM_VNUM_ALTAR,
        OBJ_VNUM_PIT,
        TRUE,   TRUE,   "Unused" },

    {"empty_10",	" Unused ",
        ROOM_VNUM_ALTAR,        ROOM_VNUM_ALTAR,
        OBJ_VNUM_PIT,
        TRUE,   TRUE,   "Unused" },

    {"empty_11",	" Unused ",
	ROOM_VNUM_ALTAR,	ROOM_VNUM_ALTAR,
	OBJ_VNUM_PIT,
	TRUE,	TRUE,	"Unused" },

    {"rot",		"  ROT   ",
	ROOM_VNUM_ALTAR,	ROOM_VNUM_ALTAR,
	OBJ_VNUM_PIT,
	TRUE,	FALSE,	"Unused" }
};

void do_clead( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg1 );

    if ( arg1[0] == '\0' )
    {
        send_to_char( "Syntax: clead <char>\n\r",ch);
        return;
    }
    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
        send_to_char( "They aren't playing.\n\r", ch );
        return;
    }
    
    if (!is_clan(victim))
    {
        send_to_char("This person is not in a clan.\n\r",ch);
        return;
    }
 
    if (clan_table[victim->clan].independent)
    {
        sprintf(buf,"This person is a %s.\n\r",clan_table[victim->clan].name);
        send_to_char(buf,ch);
	return;
    }

    if (is_clead(victim))
    {
	sprintf(buf,"They are no longer leader of clan %s.\n\r",
	    capitalize(clan_table[victim->clan].name));
	send_to_char(buf,ch);
	sprintf(buf,"You are no longer leader of clan %s.\n\r",
	    capitalize(clan_table[victim->clan].name));
        send_to_char(buf,victim);
	update_clanlist(victim, victim->clan, FALSE, TRUE);
	victim->clead = 0;
    }
    else
    {
        sprintf(buf,"They are now leader of clan %s.\n\r",
            capitalize(clan_table[victim->clan].name));
        send_to_char(buf,ch);
        sprintf(buf,"You are now leader of clan %s.\n\r",
            capitalize(clan_table[victim->clan].name));
        send_to_char(buf,victim); 
	update_clanlist(victim, victim->clan, TRUE, TRUE);
	victim->clead = victim->clan;
    }
}


void do_guild( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int clan;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
        send_to_char( "Syntax: guild <char> <cln name>\n\r",ch);
        return;
    }
    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL
    || (victim->level > ch->level && victim->level == MAX_LEVEL))
    {
        send_to_char( "They aren't playing.\n\r", ch );
        return;
    }
    
    if (!str_prefix(arg2,"none"))
    {
	send_to_char("They are now clanless.\n\r",ch);
	send_to_char("You are now a member of no clan!\n\r",victim);
	if (is_clead(victim))
	    update_clanlist(victim, victim->clead, FALSE, TRUE);
	if (is_clan(victim))
	    update_clanlist(victim, victim->clan, FALSE, FALSE);
	victim->clan = 0;
	victim->clead = 0;
	return;
    }

    if ((clan = clan_lookup(arg2)) == 0)
    {
	send_to_char("No such clan exists.\n\r",ch);
	return;
    }

    if (IS_SET(victim->act, PLR_NOCLAN) && clan_table[clan].pkill )
    {
	send_to_char("This player is banned from pkill clans.\n\r",ch);
	return;
    }

    if (clan_table[clan].independent)
    {
	sprintf(buf,"They are now a %s.\n\r",clan_table[clan].name);
	send_to_char(buf,ch);
	sprintf(buf,"You are now a %s.\n\r",clan_table[clan].name);
	send_to_char(buf,victim);
    }
    else
    {
	sprintf(buf,"They are now a member of clan %s.\n\r",
	    capitalize(clan_table[clan].name));
	send_to_char(buf,ch);
	sprintf(buf,"You are now a member of clan %s.\n\r",
	    capitalize(clan_table[clan].name));
        send_to_char(buf,victim);
    }

    if (is_clead(victim))
    {
	update_clanlist(victim, victim->clead, FALSE, TRUE);
	victim->clead = 0;
    }
    if (is_clan(victim))
    {
	update_clanlist(victim, victim->clan, FALSE, FALSE);
	victim->clan = 0;
    }
    update_clanlist(victim, clan, TRUE, FALSE);
    victim->clan = clan;
}

void do_member( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg1 );

    if (IS_NPC(ch))
    {
	return;
    }

    if (!is_clead(ch))
    {
	if (is_clan(ch))
	{
	    send_to_char( "You are not a clan leader.\n\r",ch);
	    return;
	}
	if (!ch->invited)
	{
	    send_to_char( "You have not been invited to join a clan.\n\r",ch);
	    return;
	}
	if (!str_cmp(arg1, "accept"))
	{
	    sprintf(buf,"{RYou are now a member of clan {x[{%s%s{x]\n\r",
		clan_table[ch->invited].pkill ? "B" : "M",
		clan_table[ch->invited].who_name);
	    send_to_char(buf,ch);
	    ch->clan = ch->invited;
	    update_clanlist(ch, ch->invited, TRUE, FALSE);
	    ch->invited = 0;
	    return;
	}
	if (!str_cmp(arg1, "deny"))
	{
	    send_to_char("You turn down the invitation.\n\r", ch);
	    ch->invited = 0;
	    return;
	}
	send_to_char( "Syntax: member <accept|deny>\n\r",ch);
	return;
    }

    if ( arg1[0] == '\0' )
    {
        send_to_char( "Syntax: member <char>\n\r",ch);
        return;
    }
    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
        send_to_char( "They aren't playing.\n\r", ch );
        return;
    }
    if (IS_NPC(victim))
    {
	send_to_char( "NPC's cannot join clans.\n\r", ch);
	return;
    }

    if (IS_SET(victim->act, PLR_NOCLAN) && clan_table[ch->clan].pkill) 
    {
	send_to_char( "This player is banned from pkill clans.\n\r", ch);
	return;
    }
    if ( victim == ch )
    {
	send_to_char( "You're stuck...only a god can help you now!\n\r",ch);
	return;
    }
    if (is_clan(victim) && !is_same_clan(ch,victim))
    {
        send_to_char("They are in another clan already.\n\r",ch);
        return;
    }
    if (is_clan(victim))
    {
	if (is_clead(victim)) {
	    send_to_char( "You can't kick out another clan leader.\n\r",ch);
	    return;
	}
        send_to_char("They are now clanless.\n\r",ch);
        send_to_char("Your clan leader has kicked you out!\n\r",victim);
	update_clanlist(victim, victim->clan, FALSE, FALSE);
        victim->clan = 0;
        return;
    }
    if (victim->invited)
    {
	send_to_char( "They have already been invited to join a clan.\n\r",ch);
	return;
    }
    if ( (victim->class < MAX_CLASS/2) && ( victim->level < 25 || victim->level > 70 ) )
    {
	send_to_char( "They must be between levels 25 -> 70.\n\r",ch);
	return;
    }
    if ( (victim->class >= MAX_CLASS/2) && ( victim->level < 15 || victim->level > 70 ) )
    {
	send_to_char( "They must be between levels 15 -> 70.\n\r",ch);
	return;
    }
    sprintf(buf,"%s has been invited to join your clan.\n\r",victim->name);
    send_to_char(buf,ch);
    sprintf(buf,"{RYou have been invited to join clan {x[{%s%s{x]\n\r",
	clan_table[ch->clan].pkill ? "B" : "M",
	clan_table[ch->clan].who_name);
    send_to_char(buf,victim);
    send_to_char("{YUse {Gmember accept{Y to join this clan,{x\n\r", victim);
    send_to_char("{Yor {Gmember deny{Y to turn down the invitation.{x\n\r",victim);
    victim->invited = ch->clan;
}

void do_cgossip( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int wtime;
 
    if (!is_clan(ch) && !IS_IMMORTAL(ch))
    {
        send_to_char("You are not in a clan!\n\r",ch);
	return;
    }
    if (argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOCGOSSIP))
      {
        send_to_char("Clan gossip channel is now ON.\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_NOCGOSSIP);
      }
      else
      {
        send_to_char("Clan gossip channel is now OFF.\n\r",ch);
        SET_BIT(ch->comm,COMM_NOCGOSSIP);
      }
    }
    else  /* cgossip message sent, turn cgossip on if it isn't already */
    {
        if (IS_SET(ch->comm,COMM_QUIET))
        {
          send_to_char("You must turn off quiet mode first.\n\r",ch);
          return;
        }
 
        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
          send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
          return;
       	}

        if ((!str_infix(" fuck ", argument))
        ||  (!str_prefix("fuck ", argument))
        ||  (!str_suffix(" fuck", argument))
        ||  (!str_suffix(" fuck.", argument))
        ||  (!str_suffix(" fuck!", argument))
        ||  (!str_cmp("fuck", argument))
        ||  (!str_cmp("fuck.", argument))
        ||  (!str_cmp("fuck!", argument))
        ||  (!str_cmp("shit", argument))
        ||  (!str_cmp("shit.", argument))
        ||  (!str_cmp("shit!", argument))
        ||  (!str_infix(" shit ", argument))
        ||  (!str_prefix("shit ", argument))
        ||  (!str_suffix(" shit", argument))
        ||  (!str_suffix(" shit.", argument))
        ||  (!str_suffix(" shit!", argument)))
        { 
            if (!IS_IMMORTAL(ch)) {
                send_to_char("Don't let the IMM's catch you talking like that!\n\r",ch);
                return; 
            } 
        } 


      REMOVE_BIT(ch->comm,COMM_NOCGOSSIP);
 
      sprintf( buf, "You cgossip '{E%s{x'\n\r", argument );
      send_to_char( buf, ch );
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
        CHAR_DATA *victim;
	int pos;
	bool found = FALSE;
 
        victim = d->original ? d->original : d->character;
 
        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
             !IS_SET(victim->comm,COMM_NOCGOSSIP) &&
             !IS_SET(victim->comm,COMM_QUIET) &&
	     (is_clan(victim) || IS_IMMORTAL(victim)))
        {
	    for (pos = 0; pos < MAX_FORGET; pos++)
	    {
		if (victim->pcdata->forget[pos] == NULL)
		    break;
		if (!str_cmp(ch->name,victim->pcdata->forget[pos]))
		    found = TRUE;
	    }
	    if (!found)
	    {
		act_new( "$n cgossips '{E$t{x'", 
		    ch,argument, d->character, TO_VICT,POS_SLEEPING );
	    }
        }
      }
    }
    wtime = UMAX(2, 9-(ch->level));
    WAIT_STATE( ch, wtime );
}

void do_clantalk( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;

    if (!str_prefix(argument,"list") && argument[0] != '\0')
    {
	argument = one_argument(argument,arg);
	do_clanlist(ch,argument);
	return;
    }

    if (!is_clan(ch) || clan_table[ch->clan].independent)
    {
	send_to_char("You aren't in a clan.\n\r",ch);
	return;
    }
    if ( argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOCLAN))
      {
        send_to_char("Clan channel is now ON\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_NOCLAN);
      }
      else
      {
        send_to_char("Clan channel is now OFF\n\r",ch);
        SET_BIT(ch->comm,COMM_NOCLAN);
      }
      return;
    }

        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
         send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
          return;
        }

        REMOVE_BIT(ch->comm,COMM_NOCLAN);

      sprintf( buf, "You clan '{F%s{x'\n\r", argument );
      send_to_char( buf, ch );
      sprintf( buf, "$n clans '{F%s{x'", argument );
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	int pos;
	bool found = FALSE;

        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
	     is_same_clan(ch,d->character) &&
             !IS_SET(d->character->comm,COMM_NOCLAN) &&
	     !IS_SET(d->character->comm,COMM_QUIET) )
        {
	    for (pos = 0; pos < MAX_FORGET; pos++)
	    {
		if (d->character->pcdata->forget[pos] == NULL)
		    break;
		if (!str_cmp(ch->name,d->character->pcdata->forget[pos]))
		    found = TRUE;
	    }
	    if (!found)
	    {
		act_new("$n clans '{F$t{x'",ch,argument,d->character,TO_VICT,POS_DEAD);
	    }
        }
    }

    return;
}

void do_clanlist( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    BUFFER *output;
    int e, members;
    MBR_DATA *pmbr;
    CLN_DATA *pcln;

    output = new_buf();
    sprintf(buf,"{x\n\r{B* = Pkill Clans    {M = Non-Pkill Clans{x\n\r");
    add_buf(output,buf);
    for ( e = 0; e < MAX_CLAN; e++)
    {
	if (str_cmp(clan_table[e].exname, "Unused"))
	{
	    sprintf(buf,"{x-------------------------------------");
	    add_buf(output,buf);
	    sprintf(buf,"--------------------------------------\n\r");
	    add_buf(output,buf);
	    members = 0;
	    for (pcln = cln_list; pcln != NULL; pcln = pcln->next)
	    {
		if (pcln->clan == e)
		{
		    members = pcln->members;
		}
	    }
	    if (IS_IMMORTAL(ch))
	    {
		sprintf( buf, "{xName: {Y%s{x",
		    clan_table[e].name );
		for ( ; ; )
		{
		    if (strlen(buf) > 29)
			break;

		    strcat(buf, " ");
		}
		add_buf(output,buf);
		sprintf( buf, "{xVnum: {Y%d{x\n\r",
		    clan_table[e].hall );
		add_buf(output,buf);
	    }
	    if (clan_table[e].pkill)
		sprintf( buf, "*[{B%s{x]\tMembers: {G%d{x\n\rDesc: {c%s{x\n\r",
		    clan_table[e].who_name,
		    members,
		    clan_table[e].exname );
	    else
		sprintf( buf, " [{M%s{x]\tMembers: {G%d{x\n\rDesc: {c%s{x\n\r",
		    clan_table[e].who_name,
		    members,
		    clan_table[e].exname );
	    add_buf(output,buf);
	    sprintf(buf,"Leaders:{R");
	    add_buf(output,buf);
	    for (pcln = cln_list; pcln != NULL; pcln = pcln->next)
	    {
		if (pcln->clan == e)
		{
		    for (pmbr = pcln->list;pmbr != NULL;pmbr = pmbr->next)
		    {
			sprintf(buf, "  %s", pmbr->name);
			add_buf(output,buf);
		    }
		}
	    }
	    sprintf(buf, "{x\n\r");
	    add_buf(output,buf);
	}
    }
    sprintf(buf,"{x----------------------------------------");
    add_buf(output,buf);
    sprintf(buf,"-----------------------------------\n\r\n\r");
    add_buf(output,buf);
    page_to_char(buf_string(output),ch);
    free_buf(output);
    return;
}

void do_noclan( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH],buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
 
    one_argument( argument, arg );
 
    if ( arg[0] == '\0' )
    {
        send_to_char( "Noclan whom?\n\r", ch );
        return;
    }
       
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }
       
    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }
       
    if ( get_trust( victim ) >= get_trust( ch ) )
    {
        send_to_char( "You failed.\n\r", ch );
        return;
    }
       
    if ( IS_SET(victim->act, PLR_NOCLAN) )
    {
        REMOVE_BIT(victim->act, PLR_NOCLAN);
        send_to_char( "NOCLAN removed.\n\r", ch );
        sprintf(buf,"$N allows %s to join pkill clans.",victim->name);
        wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    }
    else
    {
        SET_BIT(victim->act, PLR_NOCLAN);
	if (is_clead(victim))
	    update_clanlist(victim, victim->clead, FALSE, TRUE);
	if (is_clan(victim))
	    update_clanlist(victim, victim->clan, FALSE, FALSE);
	victim->clan = 0;
	victim->clead = 0;
        send_to_char( "NOCLAN set.\n\r", ch );
        sprintf(buf,"$N forbids %s to join pkill clans.",victim->name);
        wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    }
       
    save_char_obj( victim );
 
    return;
}

void save_clanlist(int clannum)
{
    char buf[MAX_STRING_LENGTH];
    CLN_DATA *pcln;
    MBR_DATA *pmbr;
    FILE *fp;
    bool found;

    if (!str_cmp(clan_table[clannum].exname, "Unused"))
    {
	return;
    }
    sprintf(buf,"%s.cln", clan_table[clannum].name);
    fclose( fpReserve ); 
    if ( ( fp = fopen( buf, "w" ) ) == NULL )
    {
        perror( buf );
    }
    found = FALSE;
    for (pcln = cln_list; pcln != NULL; pcln = pcln->next)
    {
	if (pcln->clan == clannum)
	{
	    found = TRUE;
	    fprintf(fp,"%d\n",pcln->members);
	    for (pmbr = pcln->list; pmbr != NULL; pmbr = pmbr->next)
	    {
		fprintf(fp,"%s\n",pmbr->name);
	    }
	}
    }
    if (!found)
    {
	fprintf(fp,"0\n");
    }
    fclose(fp);
    fpReserve = fopen( NULL_FILE, "r" );
}

void load_clanlist(void)
{
    char buf[MAX_STRING_LENGTH];
    FILE *fp;
    MBR_DATA *mbr_last;
    int clannum;
 
    strcat(boot_buf,"- the Powe");
    for(clannum = 0; clannum < MAX_CLAN; clannum++)
    {
	if (str_cmp(clan_table[clannum].exname, "Unused"))
	{
	    CLN_DATA *pcln;
	    pcln = new_cln();
	    pcln->clan = clannum;
	    pcln->name = str_dup(clan_table[clannum].name);
	    sprintf(buf,"%s.cln", clan_table[clannum].name);
	    if ( ( fp = fopen( buf, "r" ) ) == NULL )
	    {
		pcln->members = 0;
 	    } else
	    {
		pcln->members = fread_number(fp);
		fread_to_eol(fp);
		mbr_last = NULL;
		for ( ; ; )
		{
		    MBR_DATA *pmbr;
		    if ( feof(fp) )
		    {
			break;
		    }
 
		    pmbr = new_mbr();
 
		    pmbr->name = str_dup(fread_word(fp));
		    fread_to_eol(fp);

		    if (pcln->list == NULL)
			pcln->list = pmbr;
		    else
			mbr_last->next = pmbr;
		    mbr_last = pmbr;
		}
		fclose( fp );
	    }
	    pcln->next = cln_list;
	    cln_list = pcln;
	}
    }
    strcat(boot_buf,"rs that Be.");
    return;
}

void update_clanlist(CHAR_DATA *ch, int clannum, bool add, bool clead)
{
    MBR_DATA *prev;
    MBR_DATA *curr;
    CLN_DATA *pcln;

    if (IS_NPC(ch))
    {
	return;
    }
    for (pcln = cln_list; pcln != NULL; pcln = pcln->next)
    {
	if (pcln->clan == clannum)
	{
	    if (clead)
	    {
		if (!add)
		{
		    prev = NULL;
		    for ( curr = pcln->list; curr != NULL; prev = curr, curr = curr->next )
		    {
			if ( !str_cmp( ch->name, curr->name ) )
			{
			    if ( prev == NULL )
				pcln->list = pcln->list->next;
			    else
				prev->next = curr->next;
	
			    free_mbr(curr);
			    save_clanlist(clannum);
			}
		    }
		    return;
		} else
		{
		    curr = new_mbr();
		    curr->name = str_dup(ch->name);
		    curr->next = pcln->list;
		    pcln->list = curr;
		    save_clanlist(clannum);
		    return;
		}
	    }
	    if (add)
		pcln->members++;
	    else
		pcln->members--;
	    if (pcln->members < 0)
		pcln->members = 0;
	    save_clanlist(clannum);
	}
    }
    return;
}
