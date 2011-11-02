/* The following code is based on ILAB OLC by Jason Dinkel */
/* Mobprogram code by Lordrom for Nevermore Mud */

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "olc.h"
#include "recycle.h"

#define MPEDIT( fun )           bool fun(CHAR_DATA *ch, char*argument)
#define EDIT_MPCODE(Ch, Code)   ( Code = (MPROG_CODE*)Ch->desc->pEdit )


const struct olc_cmd_type mpedit_table[] =
{
/*	{	command		function	}, */

	{	"commands",	show_commands	},
	{	"create",	mpedit_create	},
	{	"code",		mpedit_code	},
	{	"show",		mpedit_show	},
	{	"?",		show_help	},

	{	NULL,		0		}
};

void mpedit( CHAR_DATA *ch, char *argument)
{
    MPROG_CODE *pMcode;
    char arg[MAX_INPUT_LENGTH];
    char command[MAX_INPUT_LENGTH];
    int cmd;

    smash_tilde(argument);
    strcpy(arg, argument);
    argument = one_argument( argument, command);

    EDIT_MPCODE(ch, pMcode);
    if (ch->pcdata->security < 7)
    {
        send_to_char("MPEdit: Insufficient security to modify code\n\r",ch);
        edit_done(ch);
        return;
    }

    if (command[0] == '\0')
    {
        mpedit_show(ch, argument);
        return;
    }
    if (!str_cmp(command, "done") )
    {
        edit_done(ch);
        return;
    }

    for (cmd = 0; mpedit_table[cmd].name != NULL; cmd++)
    {
        if (!str_prefix(command, mpedit_table[cmd].name) )
        {
           (*mpedit_table[cmd].olc_fun) (ch, argument);
           return;
        }
    }

    interpret(ch, arg);
    return;
}

void do_mpedit(CHAR_DATA *ch, char *argument)
{
    MPROG_CODE *pMcode;
    char command[MAX_INPUT_LENGTH];

    if ( IS_NPC(ch) || ch->pcdata->security < 7 )
    {
    	send_to_char( "MPEdit: Insufficient security to modify code.\n\r", ch );
    	return;
    }

    argument = one_argument( argument, command);
    if(is_number(command) )
    {
       if (! (pMcode = get_mprog_index( atoi(command) ) ) )
       {
           send_to_char("MPEdit: That vnum does not exist.\n\r",ch);
           return;
       }
       ch->desc->pEdit=(void *)pMcode;
       ch->desc->editor= ED_MPCODE;
       return;
    }

    if (!str_cmp(command, "create" ) )
    {
        if (argument[0] == '\0')
        {
           send_to_char("Syntax: edit code create [vnum]\n\r",ch);
           return;
        }

        if (mpedit_create(ch, argument) )
                ch->desc->editor = ED_MPCODE;
    }

    return;
}

MPEDIT (mpedit_create)
{
    MPROG_CODE *pMcode;
    int value;

    value = atoi(argument);
    if (argument[0] == '\0' || value == 0)
    {
        send_to_char("Syntax: mpedit create [vnum]\n\r",ch);
        return FALSE;
    }

    if (!IS_NPC(ch) && ch->pcdata->security < 7)
    {
        send_to_char("MPEdit: Insuficiente seguridad para crear MobProgs.\n\r", ch);
        return FALSE;
    }

    if (get_mprog_index(value) )
    {
        send_to_char("MPEdit: Code vnum already exists.\n\r",ch);
        return FALSE;
    }

    if (!get_vnum_area( value ))
    {
    	send_to_char("MPEdit: Vnum no asignado a ningun Area.\n\r", ch);
    	return FALSE;
    }

    pMcode                        = new_mpcode();
    pMcode->vnum                  = value;
    pMcode->next                  = mprog_list;
    mprog_list                    = pMcode;
    ch->desc->pEdit               = (void *)pMcode;

    send_to_char("MobProgram Code Created.\n\r",ch);
    return TRUE;
}

MPEDIT(mpedit_show)
{
    MPROG_CODE *pMcode;
    char buf[MAX_STRING_LENGTH];


    EDIT_MPCODE(ch,pMcode);

    sprintf(buf,
           "Vnum:       [%d]\n\r"
           "Code:\n\r%s\n\r",
           pMcode->vnum, pMcode->code);
    send_to_char(buf, ch);

    return FALSE;
}

MPEDIT( mpedit_code)
{
    MPROG_CODE *pMcode;
    EDIT_MPCODE(ch, pMcode);

    if (argument[0] =='\0')
    {
       string_append(ch, &pMcode->code);
       return TRUE;
    }

    send_to_char(" Syntax: code\n\r",ch);
    return FALSE;
}

void do_mplist( CHAR_DATA *ch, char *argument )
{
    int count;
    MPROG_CODE *mprg;
    char buf[MAX_STRING_LENGTH];
    BUFFER *buffer;
    buffer=new_buf();

    for (count =1, mprg= mprog_list; mprg !=NULL; mprg = mprg->next)
    {
      sprintf(buf, "[%3d] %5d\n\r", count, mprg->vnum );
      add_buf(buffer, buf);
      count++;
    }
    page_to_char(buf_string(buffer), ch);
    free_buf(buffer);
    return;
}
