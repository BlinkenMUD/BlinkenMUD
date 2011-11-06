/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,	   *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *									   *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael	   *
 *  Chastain, Michael Quan, and Mitchell Tse.				   *
 *									   *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc	   *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.						   *
 *									   *
 *  Much time and thought has gone into this software and you are	   *
 *  benefitting.  We hope that you share your changes too.  What goes	   *
 *  around, comes around.						   *
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

/* externs */
extern char str_empty[1];
extern int mobile_count;

/* stuff for providing a crash-proof buffer */

#define MAX_BUF		65536
#define MAX_BUF_LIST 	12
#define BASE_BUF 	1024

/* valid states */
#define BUFFER_SAFE	0
#define BUFFER_OVERFLOW	1
#define BUFFER_FREED 	2

/* note recycling */
#define ND NOTE_DATA
ND *new_note (void);
void free_note (NOTE_DATA * note);
#undef ND

/* ban data recycling */
#define BD BAN_DATA
BD *new_ban (void);
void free_ban (BAN_DATA * ban);
#undef BD

/* wizlist data recycling */
#define WD WIZ_DATA
WD *new_wiz (void);
void free_wiz (WIZ_DATA * ban);
#undef WD

/* clan data recycling */
#define CL CLN_DATA
CL *new_cln (void);
#undef CL

/* clan data recycling */
#define MB MBR_DATA
MB *new_mbr (void);
void free_mbr (MBR_DATA * mbr);
#undef MB

/* descriptor recycling */
#define DD DESCRIPTOR_DATA
DD *new_descriptor (void);
void free_descriptor (DESCRIPTOR_DATA * d);
#undef DD

/* char gen data recycling */
#define GD GEN_DATA
GD *new_gen_data (void);
void free_gen_data (GEN_DATA * gen);
#undef GD

/* extra descr recycling */
#define ED EXTRA_DESCR_DATA
ED *new_extra_descr (void);
void free_extra_descr (EXTRA_DESCR_DATA * ed);
#undef ED

/* affect recycling */
#define AD AFFECT_DATA
AD *new_affect (void);
void free_affect (AFFECT_DATA * af);
#undef AD

/* object recycling */
#define OD OBJ_DATA
OD *new_obj (void);
void free_obj (OBJ_DATA * obj);
#undef OD

/* character recyling */
#define CD CHAR_DATA
#define PD PC_DATA
CD *new_char (void);
void free_char (CHAR_DATA * ch);
PD *new_pcdata (void);
void free_pcdata (PC_DATA * pcdata);
#undef PD
#undef CD


/* mob id and memory procedures */
#define MD MEM_DATA
long get_pc_id (void);
long get_mob_id (void);
MD *new_mem_data (void);
void free_mem_data (MEM_DATA * memory);
MD *find_memory (MEM_DATA * memory, long id);
#undef MD

/* buffer procedures */

BUFFER *new_buf (void);
BUFFER *new_buf_size (int size);
void free_buf (BUFFER * buffer);
bool add_buf (BUFFER * buffer, char *string);
void clear_buf (BUFFER * buffer);
char *buf_string (BUFFER * buffer);
