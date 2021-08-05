#include <stdio.h>
#include <ctype.h>

#define CTRL_KEY       0x80
#define TERM_FUNC_KEY  0xA0
#define CURSOR_KEY     0xC0
#define CBM_FUNC_KEY   0xD0
#define SPECIAL_FUNC   0xE0
#define UNMAPPED_KEY   0xF0

char *ascii_table[128] =
  {
   "NUL", "SOH", "STX",  "ETX", "EOT", "ENQ", "ACK", "BEL", "BS", "HT", "LF", "VT", "FF", "CR", "SO", "SI",
   "DLE", "XON", "DC2", "XOFF", "DC4", "NAK", "SYN", "ETB", "CAN", "EM", "SUB", "ESC", "FS", "GS", "RS", "US",
   "SPACE", "EXCLAMATION", "DBL_QUOTE", "HASH", "DOLLAR", "PERCENT", "AMPERSAND", "QUOTE", "PAR_OPEN", "PAR_CLOSE", "STAR", "PLUS", "COMMA", "MINUS", "PERIOD", "SLASH",
   "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "COLON", "SEMICOLON", "LESS_THAN", "EQUAL", "GREATER_THAN", "QUESTION",
   "AT", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O",
   "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "BRACKET_OPEN", "BACKSLASH", "BRACKET_CLOSE", "CARET", "UNDERSCORE",
   "REV_QUOTE", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o",
   "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "CURLY_OPEN", "PIPE", "CURLY_CLOSE", "TILDE", "DEL",
   
  };


unsigned char conv_table[0xc0] =
  {
   /*    mapped code        function       petscii      key press   alt key press */   
   CTRL_KEY + '@' - '@', /* CTRL + @         0x00       CTRL + @                  */
   CTRL_KEY + 'A' - '@', /* CTRL + A         0x01       CTRL + A                  */
   CTRL_KEY + 'B' - '@', /* CTRL + B         0x02       CTRL + B                  */
   0x09,                 /* TAB              0x03       CTRL + C      RUN STOP    */
   CTRL_KEY + 'D' - '@', /* CTRL + D         0x04       CTRL + D                  */
   TERM_FUNC_KEY + 1,    /* F2               0x05       CTRL + E      CTRL + 2  */
   CTRL_KEY + 'F' - '@', /* CTRL + F         0x06       CTRL + F      CTRL + LEFT_ARROW   */ 
   CTRL_KEY + 'G' - '@', /* CTRL + G         0x07       CTRL + G                  */
   CTRL_KEY + 'H' - '@', /* CTRL + H         0x08       CTRL + H                  */
   CTRL_KEY + 'I' - '@', /* CTRL + I         0x09       CTRL + I                  */
   CTRL_KEY + 'J' - '@', /* CTRL + J         0x0a       CTRL + J                  */
   CTRL_KEY + 'K' - '@', /* CTRL + K         0x0b       CTRL + K                  */
   CTRL_KEY + 'L' - '@', /* CTRL + I         0x0c       CTRL + L                  */
   CTRL_KEY + 'M' - '@', /* CTRL + M         0x0d       CTRL + M      RETURN    */
   CTRL_KEY + 'N' - '@', /* CTRL + N         0x0e       CTRL + N                  */
   CTRL_KEY + 'O' - '@', /* CTRL + O         0x0f       CTRL + O                  */
   CTRL_KEY + 'P' - '@', /* CTRL + P         0x10       CTRL + P                  */
   CURSOR_KEY + 'B'-'A', /* CURSOR DOWN      0x11       CTRL + Q      CURSOR DOWN          */
   TERM_FUNC_KEY + 8,    /* F9               0x12       CTRL + R      CTRL + 9    */
   0xff,                 /* NO_MAP           0x13       CTRL + S      HOME        */
   0x7f,                 /* CTRL + T         0x14       CTRL + T      DELETE      */
   CTRL_KEY + 'U' - '@', /* CTRL + U         0x15       CTRL + U                  */
   CTRL_KEY + 'V' - '@', /* CTRL + V         0x16       CTRL + V                  */
   CTRL_KEY + 'W' - '@', /* CTRL + W         0x17       CTRL + W                  */
   CTRL_KEY + 'X' - '@', /* CTRL + X         0x18       CTRL + X                  */
   CTRL_KEY + 'Y' - '@', /* CTRL + Y         0x19       CTRL + Y                  */
   CTRL_KEY + 'Z' - '@', /* CTRL + Z         0x1a       CTRL + Z                  */
   0x1b,                 /* ESC              0x1b       CTRL + [                  */
   TERM_FUNC_KEY + 2,    /* F3               0x1c       CTRL + POUND  CTRL + 3    */
   CURSOR_KEY + 'C'-'A', /* CURSOR RIGHT     0x1d       CTRL + ]      CURSOR RIGHT           */
   TERM_FUNC_KEY + 5,    /* F6               0x1e       CTRL + ^      CTRL + 6    */
   TERM_FUNC_KEY + 6,    /* F7               0x1f                     CTRL + 7    */

   ' ',                  /* SPACE            0x20       SPACE                     */
   '!',                  /* !                0x21       !                         */
   '"',                  /* "                0x22       "                         */
   '#',                  /* #                0x23       #                         */
   '$',                  /* $                0x24       $                         */
   '%',                  /* %                0x25       %                         */
   '&',                  /* &                0x26       &                         */
   '\'',                 /* '                0x27       '                         */
   '(',                  /* (                0x28       (                         */
   ')',                  /* )                0x29       )                         */
   '*',                  /* *                0x2a       *                         */
   '+',                  /* +                0x2b       +                         */
   ',',                  /* ,                0x2c       ,                         */
   '-',                  /* -                0x2d       -                         */
   '.',                  /* .                0x2e       .                         */
   '/',                  /* /                0x2f       /                         */

   '0',                  /* 0                0x30       0                         */
   '1',                  /* 1                0x31       1                         */
   '2',                  /* 2                0x32       2                         */
   '3',                  /* 3                0x33       3                         */
   '4',                  /* 4                0x34       4                         */
   '5',                  /* 5                0x35       5                         */
   '6',                  /* 6                0x36       6                         */
   '7',                  /* 7                0x37       7                         */
   '8',                  /* 8                0x38       8                         */
   '9',                  /* 9                0x39       9                         */
   ':',                  /* :                0x3a       SPACE */
   ';',                  /* ;                0x3b       SPACE */
   '<',                  /* <                0x3c       SPACE */
   '=',                  /* =                0x3d       SPACE */
   '>',                  /* >                0x3e       SPACE */
   '?',                  /* ?                0x3f       SPACE */
   
   '@',                  /* @                0x40       SPACE */
   'a',                  /* a                0x41       SPACE */
   'b',                  /* b                0x42       SPACE */
   'c',                  /* c                0x43       SPACE */
   'd',                  /* d                0x44       SPACE */
   'e',                  /* e                0x45       SPACE */
   'f',                  /* f                0x46       SPACE */
   'g',                  /* g                0x47       SPACE */
   'h',                  /* h                0x48       SPACE */
   'i',                  /* i                0x49       SPACE */
   'j',                  /* j                0x4a       SPACE */
   'k',                  /* k                0x4b       SPACE */
   'l',                  /* l                0x4c       SPACE */
   'm',                  /* m                0x4d       SPACE */
   'n',                  /* n                0x4e       SPACE */
   'o',                  /* o                0x4f       SPACE */
   
   'p',                  /* p                0x50       SPACE */
   'q',                  /* q                0x51       SPACE */
   'r',                  /* r                0x52       SPACE */
   's',                  /* s                0x53       SPACE */
   't',                  /* t                0x54       SPACE */
   'u',                  /* u                0x55       SPACE */
   'v',                  /* v                0x56       SPACE */
   'w',                  /* w                0x57       SPACE */
   'x',                  /* x                0x58       SPACE */
   'y',                  /* y                0x59       SPACE */
   'z',                  /* x                0x5a       SPACE */
   '[',                  /* [                0x5b       [ */
   '\\',                 /* \                0x5c       POUND*/
   ']',                  /* ]                0x5d       ] */
   '^',                  /* .                0x5e       ^ */
   0x1b,                 /* ESC              0x5f       LEFT ARROW */
   
   '}',                  /* }                0x60       SHIFT + '*' */
   'A',                  /* A                0x61       SPACE */
   'B',                  /* B                0x62       SPACE */
   'C',                  /* C                0x63       SPACE */
   'D',                  /* D                0x64       SPACE */
   'E',                  /* E                0x65       SPACE */
   'F',                  /* F                0x66       SPACE */
   'G',                  /* G                0x67       SPACE */
   'H',                  /* H                0x68       SPACE */
   'I',                  /* I                0x69       SPACE */
   'J',                  /* J                0x6a       SPACE */
   'K',                  /* K                0x6b       SPACE */
   'L',                  /* L                0x6c       SPACE */
   'M',                  /* M                0x6d       SPACE */
   'N',                  /* N                0x6e       SPACE */
   'O',                  /* O                0x6f       SPACE */
   
   'P',                  /* P                0x70       SPACE */
   'Q',                  /* Q                0x71       SPACE */
   'R',                  /* R                0x72       SPACE */
   'S',                  /* S                0x73       SPACE */
   'T',                  /* T                0x74       SPACE */
   'U',                  /* U                0x75       SPACE */
   'V',                  /* V                0x76       SPACE */
   'W',                  /* W                0x77       SPACE */
   'X',                  /* X                0x78       SPACE */
   'Y',                  /* Y                0x79       SPACE */
   'Z',                  /* Z                0x7a       SPACE          */
   '|',                  /* |                0x7b       SHIFT + '+'    */  
   0xff,                 /* NO_MAP           0x7c       CBM + '-'      */
   '_',                  /* _                0x7d       SHIFT + '-'    */
   '~',                  /* ~                0x7e       PI             */
   0xff,                 /* NO_MAP           0x7f       CBM + '*'      */

   0xff,                 /* NO_MAP           0x80       NO_KEY         */
   TERM_FUNC_KEY + 10,   /* F11              0x81       CBM + 1        */
   0xff,                 /* NO_MAP           0x82       NO_KEY         */
   0xff,                 /* NO_MAP           0x83       SHIFT + "RUN STOP"    */
   0xff,                 /* NO_MAP           0x84       NO_KEY         */
   CBM_FUNC_KEY + 0,     /* CBM_F1           0x85       F1             */
   CBM_FUNC_KEY + 2,     /* CBM_F3           0x86       F3             */
   CBM_FUNC_KEY + 4,     /* CBM_F5           0x87       F5             */
   CBM_FUNC_KEY + 6,     /* CBM_F7           0x88       F7             */
   CBM_FUNC_KEY + 1,     /* CBM_F2           0x89       F2             */
   CBM_FUNC_KEY + 3,     /* CBM_F4           0x8a       F4             */
   CBM_FUNC_KEY + 5,     /* CBM_F6           0x8b       F6             */
   CBM_FUNC_KEY + 7,     /* CBM_F8           0x8c       F8             */
   0xff,                 /* NO_MAP           0x8d       SHIFT + RETURN */
   0xff,                 /* NO_MAP           0x8e       NO_KEY         */
   0xff,                 /* NO_MAP           0x8f       NO_KEY         */

   TERM_FUNC_KEY + 0,    /* F1               0x90       CTRL + 1       */
   CURSOR_KEY + 'A'-'A', /* CURSOR UP        0x91       CURSOR UP      */
   TERM_FUNC_KEY + 9,    /* F10              0x92       CTRL + 0       */
   0xff,                 /* NO_MAP           0x93       CLR SCREEN     */
   0xff,                 /* NO_MAP           0x94       INSERT         */
   TERM_FUNC_KEY + 11,   /* F12              0x95       CBM + 2        */
   TERM_FUNC_KEY + 12,   /* F13              0x96       CBM + 3        */
   TERM_FUNC_KEY + 13,   /* F14              0x97       CBM + 4        */
   TERM_FUNC_KEY + 14,   /* F15              0x98       CBM + 5        */
   TERM_FUNC_KEY + 15,   /* F16              0x99       CBM + 6        */
   0xff,                 /* NO_MAP           0x9a       CBM + 7        */
   0xff,                 /* NO_MAP           0x9b       CBM + 8        */
   CBM_FUNC_KEY + 4,     /* F5               0x9c       CTRL + 5       */
   CURSOR_KEY + 'D'-'A', /* CURSOR LEFT      0x9d       CURSOR LEFT    */
   TERM_FUNC_KEY + 7,    /* F7               0x9e       CTRL + 8       */
   TERM_FUNC_KEY + 3,    /* F3               0x9f       CTRL + 4       */

   0xff,                 /* NO_MAP           0xa0       SHIFT + SPACE  */
   'K' - '@',            /* CTRL + K         0xa1       CBM + K        */
   'I' - '@',            /* CTRL + I         0xa2       CBM + I        */
   'T' - '@',            /* CTRL + T         0xa3       CBM + T        */
   '@' - '@',            /* CTRL + @         0xa4       CBM + @        */
   'G' - '@',            /* CTRL + G         0xa5       CBM + G        */
   0xff,                 /* NO_MAP           0xa6       CBM + '+'      */
   'M' - '@',            /* CTRL + M         0xa7       CBM + M        */
   0xff     ,            /* NO_MAP           0xa8       CBM + POUND    */
   '`',                  /* `                0xa9       SHIFT + POUND  */
   'N' - '@',            /* CTRL + N         0xaa       CBM + N        */
   'Q' - '@',            /* CTRL + Q         0xab       CBM + Q        */
   'D' - '@',            /* CTRL + D         0xac       CBM + D        */
   'Z' - '@',            /* CTRL + Z         0xad       CBM + Z        */
   'S' - '@',            /* CTRL + S         0xae       CBM + S        */
   'P' - '@',            /* CTRL + P         0xaf       CBM + P        */
   
   'A' - '@',            /* CTRL + A         0xb0       CBM + A        */
   'E' - '@',            /* CTRL + E         0xb1       CBM + E        */
   'R' - '@',            /* CTRL + R         0xb2       CBM + R        */
   'W' - '@',            /* CTRL + W         0xb3       CBM + W        */
   'H' - '@',            /* CTRL + H         0xb4       CBM + H        */
   'J' - '@',            /* CTRL + J         0xb5       CBM + J        */
   'L' - '@',            /* CTRL + L         0xb6       CBM + L        */
   'Y' - '@',            /* CTRL + Y         0xb7       CBM + Y        */
   'U' - '@',            /* CTRL + U         0xb8       CBM + U        */
   'O' - '@',            /* CTRL + O         0xb9       CBM + O        */
   '{',                  /* {                0xba       SHIFT + @      */
   'F' - '@',            /* CTRL + F         0xbb       CBM + F        */
   'C' - '@',            /* CTRL + C         0xbc       CBM + C        */
   'X' - '@',            /* CTRL + X         0xbd       CBM + X        */
   'V' - '@',            /* CTRL + V         0xbe       CBM + V        */
   'B' - '@',            /* CTRL + B         0xbf       CBM + B        */
  };



int main(int argc, char *argv[])
{
  int i,t;
  //  unsigned char c,t;

  if(argc==2) {
    switch(argv[1][0]) {
    case 'h':
      printf("#define CTRL_KEY       0x%02X\n", CTRL_KEY);
      printf("#define TERM_FUNC_KEY  0x%02X\n", TERM_FUNC_KEY);
      printf("#define CURSOR_KEY     0x%02X\n", CURSOR_KEY);
      printf("#define CBM_FUNC_KEY   0x%02X\n", CBM_FUNC_KEY);
      printf("#define SPECIAL_FUNC   0x%02X\n", SPECIAL_FUNC);
      printf("#define UNMAPPED_KEY   0x%02X\n", UNMAPPED_KEY);
      printf("\n\n");
      
      for(i=0;i<128;i++) {
	printf("#define ASCII_%s 0x%02X\n", ascii_table[i],i);
      }
      break;
    case 'c':
      printf("/* char_tables.c */\n");
      printf("/* PETSCII to ASCII */\n\n");

      printf("#pragma rodata-name (\"TABLES\")\n\n");
      
      printf("const unsigned char conv_table[256] =\n  {\n");
      for(i=0;i<256;i++) {
	t=i;
	if(t>=192 && t<=223) t-=96;
	else if(t>=224 && t<=254) t-=64;
	else if(t==255) t=126;

	printf("0x%02x,",conv_table[t]);
	if((i+1)%16==0) printf("\n");
	  
      }
      printf("};\n");
      break;
    default:
      printf("parameter %c not recognized\n",argv[1][0]);
      break;
    }
  } else {
    printf("usage:\n");
    printf("%s  h : create include file\n",argv[0]);
    printf("%s  c : create c file\n",argv[0]);
  }
  
  //  printf("const unsigned char isxx[256] = {\n");


  //  printf("};\n");

  return 0;
}
