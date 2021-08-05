#include<stdio.h>

#define ESC 0x1B

char *start[] = {
	"[1;24r",
	"[2J",
	"[H",
	NULL
};
char *seq[] = {

	       //		"[?3l",
	"[H",
	"[3;21r",
	"[?6h",
	
	"[19;1HA",
	"[19;80Ha\n",

	"[18;80HaB",
	//	"[19;80HB\b b\n",
	"[19;80HB\by\n",
	
	"[19;80HC\b\b\t\tc",
	"[19;2H\bC\n",
	
	"[19;80H\n",
	"[18;1HD",
	"[18;80Hd",
	
	//
	"[19;1HE",
	"[19;80He\n",

	"[18;80HeF",
	"[19;80HF\b f\n",
	
	"[19;80HG\b\b\t\tg",
	"[19;2H\bG\n",
	
	"[19;80H\n",
	"[18;1HH",
	"[18;80Hh",

	NULL
};

int main()
{
  int i;

  for(i=0;start[i];i++) 
    printf("%c%s",0x1b,start[i]);
  
  for(i=1;i<24;i++)
    printf("    %03d **************************  %03d ************************ %03d\n",i,i,i);

  for(i=0;seq[i];i++) 
    printf("%c%s",0x1b,seq[i]);

  printf("\n");
}
