/*


Along with the skill and the following code a flag needs to be added 
also sharp weapons should do more dammage and dull over time . 


This is a snippet I wrote for rom 2.4. It is a the sharpen skill, that
adds the sharp flag to weapons. Could you please add this Rom snippet
area.

Thanks,
Steve Kynaston

This is a skill that add's the sharp flags to weapons. This skill should
be given to warroirs, and
fighters.

in act_obj.c
*/

/* Moyis and Almighty */
/* Sharpen Weapon Skill For Fighters*/

void do_sharpen(CHAR_DATA *ch, char *argument)
{
    OBJ_DATA *obj;
    AFFECT_DATA af;
    int percent,skill;
char arg[MAX_INPUT_LENGTH];
argument = one_argument(argument, arg);

    /* find out what */
    if (argument == '\0')
    {
        send_to_char("Sharpen what weapon?\n\r",ch);
         return;
{
    obj =  get_obj_list(ch,argument,ch->carrying);

    if (obj== NULL)
    {
        send_to_char("You don't have that item.\n\r",ch);
        return;
    }

    obj =  get_obj_list(ch,argument,ch->carrying);

    if (obj== NULL)
    {
        send_to_char("You don't have that item.\n\r",ch);
        return;
    }

    if ((skill = get_skill(ch,gsn_sharpen)) < 1)
    {
        send_to_char("You have no idea how to sharpen weapons.\n\r",ch);
        return;
    }


    if (obj->item_type == ITEM_WEAPON)
    {
        if (IS_WEAPON_STAT(obj,WEAPON_SHARP))
        {
            act("$p has already been sharpened.",ch,obj,NULL,TO_CHAR);
            return;
       }

        percent = number_percent();
        if (percent < skill)
        {

            af.where     = TO_WEAPON;
            af.type      = gsn_sharpen;
            af.level     = ch->level * percent / 100;
            af.duration  = -1;
            af.location  = 0;
            af.modifier  = 0;
            af.bitvector = WEAPON_SHARP;
            affect_to_obj(obj,&af);

            act("$n pulls out a piece of stone and begins sharpening $p.",ch,ob
j,NULL,TO_ROOM);
            act("You sharpen $p.",ch,obj,NULL,TO_CHAR);
            check_improve(ch,gsn_sharpen,TRUE,3);
            WAIT_STATE(ch,skill_table[gsn_sharpen].beats);
            return;
        }
     }
send_to_char("You can only sharpen weapons.\n\r",ch);
return;
}
}

/*
this should go into your const.c

{
        "sharpen",              { 93, 93, 93, 22 },     { 0, 0, 0, 6 },
        spell_null,             TAR_IGNORE,             POS_RESTING,
        &gsn_sharpen,           SLOT(0),        0,      36,
        "",                     "!Sharpen!",            ""
    },


make sure you modify your db.c, merc.h, interp.c, and interp.h
Thank you,
Moyis

moyis@mud.op.net
almighty@mud.op.net

Eternal Flames: mud.op.net 5000
*/



/*
 =============================================================================
/   ______ _______ ____   _____   ___ __    _ ______    ____  ____   _____   /
\  |  ____|__   __|  _ \ / ____\ / _ \| \  / |  ____|  / __ \|  _ \ / ____\  \
/  | |__     | |  | |_| | |     | |_| | |\/| | |___   | |  | | |_| | |       /
/  | ___|    | |  | ___/| |   __|  _  | |  | | ____|  | |  | |  __/| |   ___ \
\  | |       | |  | |   | |___| | | | | |  | | |____  | |__| | |\ \| |___| | /
/  |_|       |_|  |_|  o \_____/|_| |_|_|  |_|______|o \____/|_| \_|\_____/  \
\                                                                            /
 ============================================================================

------------------------------------------------------------------------------
ftp://ftp.game.org/pub/mud      FTP.GAME.ORG      http://www.game.org/ftpsite/
------------------------------------------------------------------------------

  This file came from FTP.GAME.ORG, the ultimate source for MUD resources.

------------------------------------------------------------------------------

*/
