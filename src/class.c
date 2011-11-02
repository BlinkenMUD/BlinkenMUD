/* Online setting of skill/spell levels, 
 * (c) 1996 Erwin S. Andreasen <erwin@pip.dknet.dk>
 *
 */

#include "include.h" /* This includes all the usual include files */

/* 

  Class table levels loading/saving
  
*/

/* Save this class */
void save_class (int num)
{
	FILE *fp;
	char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
	int lev, i;
	
	sprintf (buf, "%sclasses/%s", DATA_DIR, class_table[num].name);
	
	if (!(fp = fopen (buf, "w")))
	{
                sprintf(buf2,"Could not open file %s in order to save.", buf );
                bug (buf2, 0);
		return;
	}
	
	
	for (lev = 0; lev < MAX_LEVEL; lev++)
		for (i = 0; i < MAX_SKILL; i++)
		{
			if (!skill_table[i].name || !skill_table[i].name[0])
				continue;
				
			if (skill_table[i].skill_level[num] == lev)
				fprintf (fp, "%d %s\n", lev, skill_table[i].name);
		}
	
	fprintf (fp, "-1"); /* EOF -1 */
	fclose (fp);
}



void save_classes()
{
	int i;
	
	for (i = 0; i < MAX_CLASS; i++)
		save_class (i);
}


/* Load a class */
void load_class (int num)
{
	char buf[MAX_STRING_LENGTH],buf2[MAX_STRING_LENGTH];
	int level,n;
	FILE *fp;
	
	sprintf (buf, "%sclasses/%s", DATA_DIR, class_table[num].name);
	
	if (!(fp = fopen (buf, "r")))
	{
                sprintf(buf2,"Could not open file %s in order to save.", buf );
                bug (buf2, 0);
                return;
	}
	
	fscanf (fp, "%d", &level);
	
	while (level != -1)
	{
		fscanf (fp, " %[^\n]\n", buf); /* read name of skill into buf */
		
		n = skill_lookup (buf); /* find index */
		
		if (n == -1)
		{
			char buf2[200];
			sprintf (buf2, "Class %s: unknown spell %s", class_table[num].name, buf);
			bug (buf2, 0);
		}
		else
			skill_table[n].skill_level[num] = level;

		fscanf (fp, "%d", &level);
	}
	
	fclose (fp);
}
	
void load_classes ()
{
	int i,j;

	for (i = 0; i < MAX_CLASS; i++)
	{
		for (j = 0; j < MAX_SKILL; j++)
			skill_table[j].skill_level[i] = MAX_LEVEL;
		
		load_class (i);
	}
}



void do_modskill (CHAR_DATA *ch ,char * argument)
{
	char class_name[MIL], skill_name[MIL];
	int sn, level, class_no;
        char buf[MSL], lvl[MSL];

        
	
	argument = one_argument (argument, class_name);
	argument = one_argument (argument, skill_name);
	
	if (!argument[0])
	{
		send_to_char ("Syntax is: SKILL <class> <skill> <level>.\n\r",ch);
		return;
	}
	
	level = atoi (argument);
	
	if (!is_number(argument) || level < 0 || level > MAX_LEVEL)
	{
                strcpy( buf, "Level range is from 0 to 310.\n\r" );
                send_to_char ( buf, ch ); 
  		return;
	}
	
	
	if ( (sn = skill_lookup (skill_name)) == -1)
	{
		strcpy ( buf, "There is no such spell/skill as " );
                strcat ( buf, skill_name );
                strcat ( buf, ".\n\r" );
	        send_to_char (buf, ch );
		return;
	}
	
	for (class_no = 0; class_no < MAX_CLASS; class_no++)
		if (!str_cmp(class_name, class_table[class_no].who_name))
			break;
	
	if (class_no == MAX_CLASS)
	{
                strcpy ( buf, "No class named '" );
                strcat ( buf, class_name );
                strcat ( buf, "' exists. Use the 3-letter WHO names" );
                strcat ( buf, "(Psi, Mag etc.)\n\r" );
                send_to_char (buf, ch);
		return;
	}
	
	skill_table[sn].skill_level[class_no] = level;
	
	 strcpy ( buf, "OK, ");
         strcat ( buf, class_table[class_no].name );
         strcat ( buf, "'s will now gain " );
         strcat ( buf, skill_table[sn].name );
         strcat ( buf, " at level " );
         sprintf (lvl, "%d", level );
         strcat ( buf, lvl );
         strcat ( buf, level > 101 ? " (i.e. never)" : "");
         send_to_char (buf, ch );
	
	save_classes();
}

