#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>


#define A		  	1
#define B			2
#define C			4
#define D			8
#define E			16
#define F			32
#define G			64
#define H			128

#define I			256
#define J			512
#define K		        1024
#define L		 	2048
#define M			4096
#define N		 	8192
#define O			16384
#define P			32768

#define Q			65536
#define R			131072
#define S			262144
#define T			524288
#define U			1048576
#define V			2097152
#define W			4194304
#define X			8388608

#define Y			16777216
#define Z			33554432
#define aa			67108864	/* doubled due to conflicts */
#define bb			134217728
#define cc			268435456
#define dd			536870912
#define ee 		       1073741824
#define ff		       2147483648
#define gg		       4294967296

#define ROOM_DARK		(A)
#define ROOM_NO_MOB		(C)
#define ROOM_INDOORS		(D)

#define ROOM_TELEPORT           (G)

#define ROOM_PRIVATE		(J)
#define ROOM_SAFE		(K)
#define ROOM_SOLITARY		(L)
#define ROOM_PET_SHOP		(M)
#define ROOM_NO_RECALL		(N)
#define ROOM_IMP_ONLY		(O)
#define ROOM_GODS_ONLY		(P)
#define ROOM_HEROES_ONLY	(Q)
#define ROOM_NEWBIES_ONLY	(R)
#define ROOM_LAW		(S)
#define ROOM_NOWHERE		(T)
#define ROOM_LOCKED		(X)

#define FLAGS A|C|D|J|K|L|M|N|O|P|Q|R|S|T|X

#define BUF_SIZE 1024

char buf[BUF_SIZE];

void loop_room(void);
void read_room(void);

int main(void)
{
  fprintf(stderr, "DEBUG: ENTERING MAIN\n");
  while(fgets(buf, BUF_SIZE, stdin) != NULL)
    {
      printf("%s",buf);
      if(strstr(buf,"#ROOMS") != NULL)
	loop_room();
    }
}

void loop_room(void)
{
  fprintf(stderr, "DEBUG: ENTERING LOOP_ROOM\n");
  for(;;)
    {
      if(fgets(buf, BUF_SIZE, stdin) == NULL)
	{
	  exit(EXIT_FAILURE);
	}
      printf("%s",buf);
      if(buf[0] == '#')
	{
	  if(buf[1] == '0')
	    return;
	  
	  read_room();
	}
    }
}

/*
  ....
  #ROOM
  #VNUM
   blabla~
   blabla~
   int int-flags int
   ...
  #VNUM
   bla~
   bla~
   int int-flags int

*/

void read_room(void)
{
  int32_t a, b, c;
  const char * str = buf;
  char * i1;
  char * i2;

  fprintf(stderr, "DEBUG: ENTERING READ_ROOM\n");
  
  for(;;)
    {
      if(fgets(buf, BUF_SIZE, stdin) == NULL)
	{
	  exit(EXIT_FAILURE);
	}
      printf("%s",buf);
      if(strstr(buf, "~"))
	break;
    }
  
  for(;;)
    {
      if(fgets(buf, BUF_SIZE, stdin) == NULL)
	{
	  exit(EXIT_FAILURE);
	}
      printf("%s",buf);
      if(strstr(buf, "~"))
	break;
    }

  if(fgets(buf, BUF_SIZE, stdin) == NULL)
    {
      exit(EXIT_FAILURE);
    }

  i1 = strstr( str, " ");
  i1++;
  i2 = strstr(i1, " ");

  a = atoi(buf);
  b = atoi(i1);
  c = atoi(i2);

  b = b && FLAGS;

  printf("%d %d %d\n", a,b,c);
}
  
