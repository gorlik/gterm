/******************************************************************************
 *  GTERM - GGLABS TERMINAL                                                   *
 *  A fast 80 columns terminal for the C64                                    *
 *  Copyright 2019-2022 Gabriele Gorla                                        *
 *                                                                            *
 *  This program is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU General Public License as published by      *
 *  the Free Software Foundation, either version 3 of the License, or         *
 *  (at your option) any later version.                                       *
 *                                                                            *
 *  GTERM is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 *  GNU General Public License for more details.                              *
 *                                                                            *
 *  You should have received a copy of the GNU General Public License         *
 *  along with GTERM.  If not, see <http://www.gnu.org/licenses/>.            *
 *                                                                            *
 ******************************************************************************/
#include <stdio.h>
#include <conio.h>
#include <c64.h>

#include "gterm.h"
#include "asc_ctype.h"
#include "ascii_define.h"

#define VERSION "0.19"

#define SPR_BASE   (COLOR_BASE+0x3f8)

#define ACIA_DATA    ACIA_BASE.data
#define ACIA_STATUS  ACIA_BASE.status
#define ACIA_COMMAND ACIA_BASE.command
#define ACIA_CONTROL ACIA_BASE.control
#define ACIA_EXT     ACIA_BASE.ext

#ifndef DEF_BAUD_IDX 
#define DEF_BAUD_IDX 3
#endif

unsigned char turbo232, baud_idx_max;

unsigned char esc_mode;  // vt100 state machine

unsigned char priv_mode[2];  // DEC private state
unsigned char attr;   // char attribute

unsigned char g_act;   // active char set (G0/G1)
unsigned char G[4];    // G0-G3 charset
unsigned char AM[2];   // active top and bottom margin
unsigned char tab[10]; // tab stops (80 bits)

// tx circular buffer
unsigned char send_buf[32];
unsigned char send_get, send_put;

unsigned char baud_index; // current baud rate

// storage for save/restore cursor
unsigned char col_save, row_save, attr_save;

unsigned char * const line[] = {
		       //			SCR_BASE+0,
			SCR_BASE+320*1,
			SCR_BASE+320*2,
			SCR_BASE+320*3,
			SCR_BASE+320*4,
			SCR_BASE+320*5,
			SCR_BASE+320*6,
			SCR_BASE+320*7,
			SCR_BASE+320*8,
			SCR_BASE+320*9,
			SCR_BASE+320*10,
			SCR_BASE+320*11,
			SCR_BASE+320*12,
			SCR_BASE+320*13,
			SCR_BASE+320*14,
			SCR_BASE+320*15,
			SCR_BASE+320*16,
			SCR_BASE+320*17,
			SCR_BASE+320*18,
			SCR_BASE+320*19,
			SCR_BASE+320*20,
			SCR_BASE+320*21,
			SCR_BASE+320*22,
			SCR_BASE+320*23,
			SCR_BASE+320*24,
};


const struct baud_table_t baud_table[] =
  {
   {   24, 0x18, 0x00 },
   {   96, 0x1C, 0x00 },
   {  192, 0x1E, 0x00 },
   {  384, 0x1F, 0x00 },
   {  576, 0x10, 0x02 | BT_EXT_TURBO },
   { 1152, 0x10, 0x01 | BT_EXT_TURBO | BT_EXT_POLL },
   //   { 2304, 0x10, 0x00 | BT_EXT_TURBO | BT_EXT_POLL },
  };

#define BAUD_TABLE_SIZE (sizeof(baud_table)/sizeof(struct baud_table_t))

void (*Scroll1)(void);
void (*InvScroll1)(void);

#ifdef __SDCC
static void memset8(unsigned char *a, unsigned char v, unsigned char count)
{
  while(count) {
    a[--count]=v;
  }
}
#else
#define memset8(addr, v, c) \
  __asm__("ldx #%b",c);	    \
  __asm__("lda #%b",v);		      \
  __asm__("l%v: sta %v-1,x",addr,addr);   \
  __asm__("dex"); \
  __asm__("bne l%v",addr);
#endif

unsigned char __fastcall__ recv_fifo_fill(void)
{
  return (put-get)&0xff;
}

unsigned char __fastcall__ send_fifo_space(void)
{
  return 0x1f-(send_put-send_get)&0x1f;
}

void Scroll(unsigned char l)
{
  for(ctemp1=l;ctemp1<AM[1];ctemp1++) {
    line_addr=line[ctemp1];
    SRC_LINE=line[ctemp1+1];
    CopyLine();
  }
  ClrLine(ctemp1);
  line_addr=line[ctemp1];
}

void ScrollRegion()
{
  Scroll(AM[0]);
  /*  for(ctemp1=AM[0];ctemp1<AM[1];ctemp1++) {
    line_addr=line[ctemp1];
    SRC_LINE=line[ctemp1+1];
    CopyLine();
  }
  ClrLine(ctemp1);
  line_addr=line[ctemp1];*/
}

void InvScroll(unsigned char l)
{
  for(ctemp1=AM[1];ctemp1>l;ctemp1--) {
    line_addr=line[ctemp1];
    SRC_LINE=line[ctemp1-1];
    CopyLine();
  }
  ClrLine(ctemp1);
  line_addr=line[ctemp1];
}

void InvScrollRegion()
{
  InvScroll(AM[0]);
  /*  for(ctemp1=AM[1];ctemp1>AM[0];ctemp1--) {
    line_addr=line[ctemp1];
    SRC_LINE=line[ctemp1-1];
    CopyLine();
  }
  ClrLine(ctemp1);
  line_addr=line[ctemp1];*/
}


void set_pos()
{
  if(COL>79) {
    if(priv_mode[0]&PRIV_DECAWM) {
      ++ROW;
      COL=0;
    } else {
      COL=79;
    }
    if(priv_mode[0]&PRIV_DECOM) {
      if(ROW>AM[1]) {
	ROW=AM[1];
	Scroll1();
      }
    } else {
      if(ROW>23) {
	ROW=23;
	Scroll1();
      }
    }
  }
  line_addr=POS(ROW,COL);
}

void set_cursor()
{
  itemp=(COL<<2)+24;
  VIC.spr0_x   = ctemp1;
  VIC.spr_hi_x = ctemp2;
  VIC.spr0_y   = (ROW<<3)+64;
}

void ser_set()
{
  ACIA_CONTROL=baud_table[baud_index].ctrl;
  ACIA_EXT=baud_table[baud_index].ext;
  
  if(baud_table[baud_index].ext & BT_EXT_POLL) {
    // no parity, no echo, no tx irq, no rts, no rx irq, en dtr
    ACIA_COMMAND=0x03;
  } else {
    // no parity, no echo, no tx irq, en rts, en rx irq, en dtr
    ACIA_COMMAND=0x09;
  }
}

void ser_init()
{
  unsigned char j;

  // FIXME: the routine detects turbo232 when no ACIA card is present
  
  get=0;
  put=0;
  send_get=0;
  send_put=0;
  
  ACIA_CONTROL=0;
  
  if(ACIA_EXT) {
    turbo232=1;
    baud_idx_max=BAUD_TABLE_SIZE-1;
  } else {
    turbo232=0;
    for(j=BAUD_TABLE_SIZE-1;j>0;j--)
      if(!(baud_table[j].ext&BT_EXT_TURBO)) break;
    baud_idx_max=j;
  }

  // FIXME: need to check that baud index is valid for the acia
  baud_index=DEF_BAUD_IDX;
  ser_set();
}

void ser_send()
{
  unsigned char st;
  st=ACIA_STATUS;
  if(st&0x10) {
    ACIA_DATA=send_buf[send_get];
    send_get++;
    send_get&=0x1F;
  }
}

void send_str(unsigned char *str)
{
  // fixme: check for buffer full
  while(*str) {
    send_buf[send_put]=conv_table[*str];
    str++;
    send_put++;
    send_put&=0x1F;
  }
}

void ser_put_raw(unsigned char c)
{
  // fixme: check for buffer full
  send_buf[send_put]=c;
  send_put++;
  send_put&=0x1F;
}

void ser_put_char(unsigned char c)
{
  ser_put_raw(conv_table[c]);
}


void send_cursor(unsigned char c)
{
  ser_put_raw(ASCII_ESC);
  
  if(priv_mode[0]&0x01) // DECCKM cursor key mode
    ser_put_raw(ASCII_O);
  else
    ser_put_raw(ASCII_BRACKET_OPEN);

  ser_put_raw(ASCII_A+c);
}

static unsigned char F_conv[] = {
   ASCII_P, ASCII_Q, ASCII_R, ASCII_S, ASCII_t, ASCII_u, ASCII_v, ASCII_l,
   ASCII_w, ASCII_x, 0,       0,       0,       0,       0,       0, // need to fix F9-F16
};


void send_func(unsigned char c)
{
  if(c<10) {
    ser_put_raw(ASCII_ESC);
    ser_put_raw(ASCII_O);
    ser_put_raw(F_conv[c]);
  }
}

void conv_str(unsigned char *str)
{
  while(*str) {
    *str=conv_table[*str];
    str++;
  }
}


unsigned char * line_save;
unsigned char loc_csave;

void PutTitleAt(unsigned char c)
{
#ifndef __SDCC
  conv_str(STR_BUF);
#endif
  loc_csave=COL;
  line_save=line_addr;
  COL=c;
  line_addr=SCR_BASE+((c>>1)<<3);
  PutLine();
  line_addr=line_save;
  COL=loc_csave;
}

void PutLineAt(unsigned char r, unsigned char c)
{
#ifndef __SDCC
  conv_str(STR_BUF);
#endif
  loc_csave=COL;
  line_save=line_addr;
  COL=c;
  line_addr=POS(r,COL);
  PutLine();
  line_addr=line_save;
  COL=loc_csave;
}


unsigned char * line_temp;
unsigned char ctemp;

void title()
{
  sprintf(STR_BUF,"GGLABS GTERM V"VERSION" - %4d00 8N1  ",baud_table[baud_index].baud);
  PutTitleAt(0);
}


void Bitmap_En()
{
  VIC.ctrl1=0x3B; // enable bitmap, no extended color, no blank, 25 rows, ypos=3
  //  CIA2.pra=0x00;  // selects VIC page 0xC000-0xFFFF
  CIA2.pra=0x2;  // selects VIC page 0x4000-0x7FFF
  
  //  VIC.addr=0x68;  
  VIC.addr=0x78;  // color ram at base +0x1c00, bitmap at base + 0x2000
  VIC.ctrl2=0xC8; // no multicolor, 40 cols, xpos=0
}

void Bitmap_Dis()
{
  VIC.ctrl1=0x1B; // disable bitmap, no extended color, no blank, 25 rows, ypos=3
  CIA2.pra=0x03;  // selects vic page 0x0000-0x3fff
  VIC.addr=0x15;  // screen base at 0x0400, char def at $0x1400
}

void Bitmap_Setup()
{
  line_addr=SCR_BASE;
  ClrLineAddr();
  ClrScr();

  // clear color RAM
  SetColor(0x01);

  // white on red title
  memset8(COLOR_BASE,0x12,40);

  // cursor sprite
  memset8(SPR_DATA,0,64);
  
  SPR_DATA[0]=0xf0;

  VIC.spr0_color=0x00;
  SPR_BASE[0]=111;  
}

const void (* const esc_func[7])(void) =
  {
   NULL,
   escape_mode_imm,
   escape_mode_csi,
   escape_mode_hash,
   escape_mode_priv,
   escape_mode_scs
  };

int main()
{
  unsigned char c, cc;

#ifdef DEBUG
  printf("GTERM starting\n");
  printf("bitmap at 0x%04X\n",SCR_BASE);
  printf("charset at 0x%04X\n",soft80_charset);
  printf("conv_table at 0x%04X\n",conv_table);
  printf("isxx table at 0x%04X\n",isxx);
  cgetc();
#endif
  
  Bitmap_Setup();
  Bitmap_En();
  
  ser_init();
  // setup the NMI vector
  *((unsigned int *)0x0318)=(unsigned int)NMI;

  title();

  // VT100 state init
  esc_mode=0;
  COL=0;
  ROW=0;
  attr=0;
  col_save=0;
  row_save=0;
  attr_save=0;
  priv_mode[0]=PRIV_DECAWM;
  priv_mode[1]=0;
  g_act=0;
  G[0]=CHAR_US;
  G[1]=CHAR_US;
  G[2]=CHAR_US;
  G[3]=CHAR_US;
  AM[0]=0;
  AM[1]=23;

  // default tab is every 8 columns
  memset8(tab,1,10);
  tab[0]=0;

  Scroll1=FS_Scroll1;
  InvScroll1=FS_InvScroll1;
#ifdef __SDCC
  sprintf(STR_BUF,"GGLABS Terminal V%s - %s (SDCC V%d.%d.%d) http://gglabs.us", VERSION, __DATE__,
	  __SDCC_VERSION_MAJOR, __SDCC_VERSION_MINOR, __SDCC_VERSION_PATCH );
#else
  sprintf(STR_BUF,"GGLABS Terminal V%s - %s (CC65 V2.%d) - http://gglabs.us", VERSION, __DATE__,
	  (__CC65__-0x200)>>4);
#endif
  PutLineAt(0,0);
  sprintf(STR_BUF,"A modern VT100 terminal emulator for the Commodore 64");
  PutLineAt(1,0);
  sprintf(STR_BUF,"Copyright (c) 2019-2022 Gabriele Gorla");
  PutLineAt(2,0);
  
  sprintf(STR_BUF,"This program is free software: you can redistribute it and/or modify it under");
  PutLineAt(4,0);
  sprintf(STR_BUF,"the terms of the GNU General Public License as published by the Free Software");
  PutLineAt(5,0);
  sprintf(STR_BUF,"Foundation, either License version 3, or (at your option) any later version.");
  PutLineAt(6,0);

  sprintf(STR_BUF,"ACIA Cartridge: %s",(turbo232)?"Turbo232":"Swiftlink");
  PutLineAt(8,0);

  ROW=10;
  set_cursor();
  VIC.spr_ena=0x01;


  for(;;) { // main loop
    
    //  if(put!=get) {
    if(recv_fifo_fill()) {

      if(!esc_mode) {
#if 1
	// process printable character in pairs
	//	if(((COL&1)==0) && (fifo_fill()>1)) {
	if((COL&1)==0) {
	  while(recv_fifo_fill()>1) {
	    c1=SER_BUF[get];
	    c2=SER_BUF[(get+1)&0xff];
	    if(isprint(c1)&&isprint(c2)) {
	      if(G[g_act]==CHAR_GRP) {
		c1-=isdecgfx(c1)?0x5F:0;
		c2-=isdecgfx(c2)?0x5F:0;
	      }
	      set_pos();
	      //	      attr|=ATTR_INVERSE;
	      Put2Char80();
	      //	      attr&=~ATTR_INVERSE;
	      get+=2;
	    } else break;
	  }
	}
#endif
	// odd position character, last character or control character
	if(recv_fifo_fill()) { 
	  c1=SER_BUF[get];
	  ++get;
	  if(isprint(c1)) {
	    if(G[g_act]==CHAR_GRP) {
	      c1-=isdecgfx(c1)?0x5F:0;
	    }
	    set_pos();
	    PutChar80();	    
	  } else {
	    parse_control();
	  }
	}
      } else {
	c1=SER_BUF[get];
	++get;
	(*esc_func[esc_mode])();
      }
      set_cursor(); // should be able to optimize
    } else {
      // input buffer empty
      if(baud_table[baud_index].ext & BT_EXT_POLL) {
	ser_recv();
      } else {
	// release RTS
	ACIA_COMMAND=0x09;
	*(char *)(SCR_BASE-0x400+38)=1;
      }
    } 

    if(kbhit()) {
      c=cgetc();
      cc=conv_table[c];
      
#ifdef DEBUG
      sprintf(STR_BUF,"%02x %02x %02x %02x ",c,cc,priv_mode[0],priv_mode[1]);
      PutTitleAt(36);
#endif
      
      if(cc<0x80) {
	// PETSCII character directly maps to ASCII
	ser_put_raw(cc);
      } else {
	// PETSCII character does not map to ASCII	
	switch(cc&0xf0) {
	case CTRL_KEY:       // CTRL + KEY : used for local control
	case CTRL_KEY+0x10:
	  if(cc==0x8d) {
	    ser_put_raw(0x0D);
	    if(priv_mode[0]&ANSI_LNM) {
	      ser_put_raw(0x0A);
	    }
	  }
	  break;

	case CURSOR_KEY:      // cursor keys
	  send_cursor(cc&0x0f);
	  break;
	case TERM_FUNC_KEY:  // terminal function keys
	case TERM_FUNC_KEY+0x10:
	  send_func(cc&0x0f);
	  break;
	case CBM_FUNC_KEY:   // CBM function keys
	  switch(cc&0x0f) {
	  case 0x00: // F1
	    if(baud_index<baud_idx_max) ++baud_index;
	    ser_set();
	    title();
	    break;
	  case 0x02: // F3
	    if(baud_index>0) --baud_index;
	    ser_set();
	    title();
	    break;
#ifdef DEBUG
	  case 0x07:
	    sprintf(STR_BUF,"                                        ");
	    PutTitleAt(48);
	    break;
#endif
	  default:
	    break;
	  }
	  break;
	case UNMAPPED_KEY:
	  break;	  
	default: // control	  
	  break;
	}	  
      }
    }
    
    if(send_get!=send_put) {
      // this is required to ensure escape sequences are sent uninterrupted
      // many programs (i.e. nano, emacs) will misinterpret the sequence if
      // all the characters are not back to back
      do {
	while(!(ACIA_STATUS&0x10)) {}; // no space Tx Register	
	ser_send();
      } while(send_get!=send_put);
    }
    //    if(c=='q') break;
  } // main loop
  
  Bitmap_Dis();
  
  return 0;
}
