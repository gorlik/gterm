/******************************************************************************
 *  GTERM - GGLABS TERMINAL                                                   *
 *  A fast 80 columns terminal for the C64                                    *
 *  Copyright 2019 Gabriele Gorla                                             *
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
#include <c64.h>

#include "gterm.h"
#include "asc_ctype.h"

#define EP_MAX 8
unsigned char e[EP_MAX];
unsigned char ep;

unsigned char g_save;
unsigned char G_s[2];

unsigned char g_set;

//#define VT102

#ifdef ESC_DEBUG
void debug_char(unsigned char w)
{
    attr|=ATTR_INVERSE;
    c1=w;
    PutChar80();
    attr&=~ATTR_INVERSE;
}
#endif

void parse_control()
{
  switch(c1) {
  case 0x00: // NULL
  case 0x07: // BELL
    break;

    //   case 0x7F: // DEL
  case 0x08: // backspace
#ifndef ESC_DEBUG
    if(COL) {
      if(COL<79) --COL;
      else COL=78;
      set_pos();
    }
#else
    debug_char('b');
#endif    
    break;
    
  case 0x09: // HTAB
    if(COL<78) {
      for(ctemp1=COL+1;ctemp1<79;ctemp1++) {
	if(tab[ctemp1>>3]&(1<<(ctemp1&0x7))) break;
      }
      COL=ctemp1;
    } else COL=79;
#ifdef TAB_DEBUG
    sprintf(STR_BUF,"TAB %02d",COL);
    PutTitleAt(34);
    sprintf(STR_BUF,"T %02x %02x %02x %02x %02x %02x ",tab[0],tab[1],
	    tab[2],tab[3],tab[4],tab[5]);
    PutTitleAt(48);
#endif
    set_pos();
    break;
    
  case 0x0A: // Line Feed
  case 0x0B: // VTAB
  case 0x0C: // Form Feed
    #ifndef ESC_DEBUG
    //    sprintf(STR_BUF,"LF %02d %02d L %02d %02d",ROW,COL,AM[0],AM[1]);
    //    PutTitleAt(40);
    if(ROW>=AM[1]) Scroll1();
    else ++ROW;
    if(priv_mode[0]&ANSI_LNM) COL=0;
    set_pos();
    #else
        debug_char('n');
    #endif
    break;
    
  case 0x0D: // CR
    COL=0;
    set_pos();
    break;
  case 0x0E: // shift out - use G1 char set
    g_act=1;
    break;
  case 0x0F: // shift in - use G0 char set
    g_act=0;
    break;
  case 0x1B: // ESCAPE
#ifndef ESC_DEBUG
    esc_mode=1;
//    sprintf(STR_BUF,"S %02X %02x %02x P %02d %02d",attr,priv_mode[0],priv_mode[1],ROW,COL);
//    PutTitleAt(34);
#else
    sprintf(STR_BUF,"S %02X %02x %02x P %02d %02d",attr,priv_mode[0],priv_mode[1],ROW,COL);
    PutTitleAt(34);
    debug_char('^');
#endif
    break;
  default:
#ifdef DEBUG
    sprintf(STR_BUF,"control %02x          ",c1);
    PutTitleAt(48);
#endif
    break;
  }
}

void escape_mode_imm()
{
  esc_mode=0; 
  switch(c1) {
  case 0x5B:  // '['
    esc_mode=2; 
    for(ctemp1=0;ctemp1<EP_MAX;ctemp1++) e[ctemp1]=0;
    ep=0;
    break;
  case '7': // DECSC save cursor
    col_save=COL;
    row_save=ROW;
    attr_save=attr;
    g_save=g_act;
    G_s[0]=G[0];
    G_s[1]=G[1];
    break;
  case '8': // DECRC restore cursor
    COL=col_save;
    ROW=row_save;
    attr=attr_save;
    g_act=g_save;
    G[0]=G_s[0];
    G[1]=G_s[1];
    //  if(ROW<AM[0]) ROW=AM[0];
    //  if(ROW>AM[1]) ROW=AM[1];
    break;
  case 0x18: // CAN
  case 0x1A: // SUB
    // cancel escape sequence
    break;
  case 0x23: // DEC private sequences
    esc_mode=3;
    break;
  case 0x28: // '('
  case 0x29: // ')'
  case 0x2A: // '*'
  case 0x2B: // '+' 
    // character set select commands   
    esc_mode=5;
    g_set=c1-0x28;
    break;
  case 0x3D: // DECKPAM  keypad application mode
  case 0x3E: // DECKPNM  kepypad numeric mode
    break;
    
  case 0x44: // 'D' IND index
    //    sprintf(STR_BUF,"ID %02d %02d L %02d %02d",ROW,COL,AM[0],AM[1]);
    //    PutTitleAt(48);
    if(ROW>=AM[1]) Scroll1();
    else ++ROW;
    break;
  case 0x45: // 'E' NEL next line
    //    sprintf(STR_BUF,"NL %02d %02d L %02d %02d",ROW,COL,AM[0],AM[1]);
    //    PutTitleAt(48);
    COL=0;
    if(ROW>=AM[1]) Scroll1();
    else ++ROW;
    break;
  case 0x48: // 'H' tab set
    if(COL<80)
      tab[COL>>3]|=(1<<(COL&0x7));
#ifdef TAB_DEBUG
    sprintf(STR_BUF,"T %02x %02x %02x %02x %02x %02x ",tab[0],tab[1],
	    tab[2],tab[3],tab[4],tab[5]);
    PutTitleAt(48);
#endif
    break;
  case 0x4D: // 'M' RI reverse index
    //        sprintf(STR_BUF,"RI %02d %02d L %02d %02d",ROW,COL,AM[0],AM[1]);
    //        PutTitleAt(40);
    if(ROW>AM[0]) --ROW;
    else {
      InvScroll1();
    }
    break;
#ifdef VT102
  case 0x4E: // 'N' SS2 Single shift 2
  case 0x4F: // 'O' SS2 Single shift 2
    break;
#endif
  default:
    // unrecognized immediate escape
#ifdef DEBUG
    sprintf(STR_BUF,"esc %02x               ",c1);
    PutTitleAt(48);
#endif
    
    if(!isprint(c1)) {
      parse_control();
      esc_mode=1;
    }
    break;
  }
}

#ifdef VT102
void delete_char(unsigned char c)
{
  unsigned char *src;
  unsigned char *dst;
  unsigned char i;

  if(c&1) {
    for(i=COL;i<80-c;i++) {
      dst=POS(ROW,i);
      src=POS(ROW,i+c);
      if(i&1) {
	// ODD
	dst[0]=dst[0]&0xF0|(src[0]>>4)&0x0F;
	dst[1]=dst[1]&0xF0|(src[1]>>4)&0x0F;
	dst[2]=dst[2]&0xF0|(src[2]>>4)&0x0F;
	dst[3]=dst[3]&0xF0|(src[3]>>4)&0x0F;
	dst[4]=dst[4]&0xF0|(src[4]>>4)&0x0F;
	dst[5]=dst[5]&0xF0|(src[5]>>4)&0x0F;
	dst[6]=dst[6]&0xF0|(src[6]>>4)&0x0F;
	dst[7]=dst[7]&0xF0|(src[7]>>4)&0x0F;
      } else {
	// EVEN
	dst[0]=(src[0]<<4)&0xF0;
	dst[1]=(src[1]<<4)&0xF0;
	dst[2]=(src[2]<<4)&0xF0;
	dst[3]=(src[3]<<4)&0xF0;
	dst[4]=(src[4]<<4)&0xF0;
	dst[5]=(src[5]<<4)&0xF0;
	dst[6]=(src[6]<<4)&0xF0;
	dst[7]=(src[7]<<4)&0xF0;
      }
    }
  } else {
    for(i=COL;i<80-c;i++) {
      dst=POS(ROW,i);
      src=POS(ROW,i+c);
      if(i&1) {
	// ODD
	dst[0]=dst[0]&0xF0|(src[0])&0x0F;
	dst[1]=dst[1]&0xF0|(src[1])&0x0F;
	dst[2]=dst[2]&0xF0|(src[2])&0x0F;
	dst[3]=dst[3]&0xF0|(src[3])&0x0F;
	dst[4]=dst[4]&0xF0|(src[4])&0x0F;
	dst[5]=dst[5]&0xF0|(src[5])&0x0F;
	dst[6]=dst[6]&0xF0|(src[6])&0x0F;
	dst[7]=dst[7]&0xF0|(src[7])&0x0F;
      } else {
	// EVEN
	dst[0]=(src[0])&0xF0;
	dst[1]=(src[1])&0xF0;
	dst[2]=(src[2])&0xF0;
	dst[3]=(src[3])&0xF0;
	dst[4]=(src[4])&0xF0;
	dst[5]=(src[5])&0xF0;
	dst[6]=(src[6])&0xF0;
	dst[7]=(src[7])&0xF0;
      }
    }
  }
}
#endif

void escape_mode_csi()
{
  unsigned char i;
  
    // parse '[' commands
    if(isdigit(c1)) e[ep]=e[ep]*10+c1-0x30; // 0x30 is '0'
    else if(c1==0x3B) ep++; // ';' separator
    else if(c1==0x3F) esc_mode=4; // "[?" sequence
    else if(isprint(c1)) {
      switch(c1) {
      case 0x41: // 'A' CUU cursor up
	e[0]=e[0]?e[0]:1;
	if(ROW>e[0]) ROW-=e[0];
	  else ROW=0;
	if(ROW<AM[0]) ROW=AM[0];
	break;
	
      case 0x42: // 'B' CUD cursor down
	ROW+=e[0]?e[0]:1;
	if(ROW>AM[1]) ROW=AM[1]; // prevent scrolling
	break;
	
      case 0x43: // 'C' CUF cursor forward
	COL+=e[0]?e[0]:1;
	if(COL>79) COL=79; // prevent scrolling
	break;
	
      case 0x44: // 'D' CUB cursor backwards
	e[0]=e[0]?e[0]:1;
	if(COL>e[0]) COL-=e[0];
	  else COL=0;
	break;

#ifdef ANSI
      case 0x45: // 'E' cursor next line
	ROW+=e[0]?e[0]:1;
	COL=0;
    	break;
	
      case 0x46: // 'F' cursor prev line
	ROW-=e[0]?e[0]:1;
	COL=0;
	break;
	
      case 0x47: // 'G'  cursor absolute col
	COL=e[0]?e[0]-1:0;
	break;
#endif
	
      case 0x66: // 'f' HVP cursor absolute pos
      case 0x48: // 'H' CUP
	//	sprintf(STR_BUF,"H %02d %02d %02d %02d %02x",e[0],e[1],AM[0],AM[1],priv_mode[0]);
	//	PutTitleAt(60);

	if(priv_mode[0]&PRIV_DECOM) {
          // if origin mode is enabled limit the cursor position
	  ROW=AM[0]+(e[0]?e[0]-1:0);
	  if(ROW>AM[1]) ROW=AM[1];
	} else {
	  ROW=(e[0]?e[0]-1:0);
	  if(ROW>23) ROW=23;
	}
	
	COL=e[1]?e[1]-1:0;
	break;

      case 0x4A: // 'J' ED erase display
	//	line_addr=POS(ROW,COL);
	switch(e[0]) {
	case 1: // erase lines to cursor position
	  for(i=0;i<ROW;i++)
	    ClrLine(i);
	  line_addr=line[ROW];
	  ClrTo();
	  break;
	case 2: // erase all screen
	  ClrScr();
	  // FIXME: is erase screen limited by the active area?
	  // Cursor position should not change
	  break;
	default: // erase from cursor
	  line_addr=POS(ROW,COL);
	  ClrFrom();
	  for(i=ROW+1;i<24;i++)
	    ClrLine(i);
	  break;
	}
	line_addr=POS(ROW,COL);
	break;
	
      case 0x4B: // 'K' EL erase line
	switch(e[0]) {
	case 1: // erase line to cursor
	  line_addr=line[ROW];
	  ClrTo();
	  break;
	case 2: // erase all line
	  ClrLine(ROW);
	  break;
	default: // erase line from cursor
	  line_addr=POS(ROW,COL);
	  ClrFrom();
	  break;
	}
	line_addr=POS(ROW,COL);
//	set_cursor();
	break;
#ifdef VT102
      case 0x4c: // 'L' - IL Insert Line
	if(ROW>=AM[0] && ROW<=AM[1]) {
	  InvScroll(ROW);
	  ClrLine(ROW);
	  for(i=1;i<e[0];i++) InvScroll(ROW);
	}
	break;
      case 0x4d: // 'M' - DL Delete Line
	if(ROW>=AM[0] && ROW<=AM[1]) {
	  Scroll(ROW);
	  ClrLine(AM[1]);
	  for(i=1;i<e[0];i++) Scroll(ROW);
	}
	break;	
      case 0x50: // 'P' - DCH Delete Character
	e[0]=e[0]?e[0]:1;
	delete_char(e[0]);
	ctemp=COL;
	COL=80-e[0];
	line_addr=POS(ROW,80-e[0]);
	ClrFrom();
	COL=ctemp;
	line_addr=POS(ROW,COL);
	//	for(i=1;i<e[0];i++) delete_char(1);
	break;	
#endif

	
#ifdef ANSI	
      case 0x53: // 'S'
	if(e[0]==0) ++e[0];
	for(i=0;i<e[0];i++)
	  Scroll1();
	break;

      case 0x58: // 'X'
	if(e[0]==0) ++e[0];
	ctemp=COL;
	c1=' ';
	for(i=0;i<e[0];i++)
	  PutChar80();
	COL=ctemp;
	break;
	
      case 0x60:
	COL+=e[0]?e[0]-1:0;
	break;
	
      case 0x61:
	COL+=e[0]?e[0]:1;
	break;
#endif
	
      case 0x63: // 'c' DA device attributes
	if(e[0]==0) {
	  send_str("\x1b[?1;0c"); // VT100 with no options
	}
	break;

#ifdef ANSI
      case 0x64: // 'd'
	ROW=e[0]?e[0]-1:0;
	break;

      case 0x65: // 'e'
	ROW+=e[0]?e[0]:1;
	break;
#endif
      case 0x67: // 'g' TBC tab clear
	if(e[0]==3) {
	for(ctemp1=0;ctemp1<10;++ctemp1)
	  tab[ctemp1]=0;
	} else {
	  if((e[0]==0) && (COL<80))
	    tab[COL>>3]&=~(1<<(COL&0x7));
	}
	break;

      case 0x68: // 'h' SM set mode
      case 0x6C: // 'l' RM reset mode
	// should check for e[0]==20
	if(c1==0x68) priv_mode[0]|=ANSI_LNM;
	else priv_mode[0]&=~ANSI_LNM;
	break;
	
      case 0x6D: // 'm' SGR set graphics rendition
	for(i=0;i<ep+1;i++) {
	  if(e[i]==0) attr=0;
          if(e[i]==4) attr|=ATTR_UNDERLINE;
	  if(e[i]==7) attr|=ATTR_INVERSE;
	  if(e[i]==27) attr&=~ATTR_INVERSE;
	}
	//	sprintf(STR_BUF,"SRG p%02d",ep+1);
	//	PutTitleAt(36);
	break;
	
      case 0x6E: // 'n' DSR device status report
	if(e[0]==5) { // report status
	  send_str("\x1b[0n"); // status ready
	} else if(e[0]==6) { // CPR report active position
	  if(priv_mode[0]&PRIV_DECOM) 
	    sprintf(STR_BUF,"\x1b[%d;%dR",ROW+1-AM[0],COL+1);
	  else
	    sprintf(STR_BUF,"\x1b[%d;%dR",ROW+1,COL+1);
	  send_str(STR_BUF);
	}
	break;

      case 0x72: // 'r' DECSTBM set top and bottom margins
	AM[0]=e[0]?e[0]-1:0;
	AM[1]=e[1]?e[1]-1:23;
	COL=0;
	ROW=(priv_mode[0]&PRIV_DECOM)?AM[0]:0;
	  
	line_addr=line[ROW];
	if((AM[0]==0) && (AM[1]==23)) {
	  Scroll1=FS_Scroll1;
	  InvScroll1=FS_InvScroll1; 
	} else {
	  Scroll1=ScrollRegion;
	  InvScroll1=InvScrollRegion; 
	}
#ifdef DEBUG
	sprintf(STR_BUF,"%02d %02d %c",AM[0],AM[1],(priv_mode[0]&PRIV_DECOM)?'e':'d');
	PutTitleAt(64);
#endif
	break;

#ifdef ANSI
      case 0x73: // 's' - save cusor pos
	col_save=COL;
	row_save=ROW;
	break;

      case 0x74: // 't'
	if(e[0]==1) attr=1;
	break;

      case 0x75: // 'u' - restore cursor pos
	COL=col_save;
	ROW=row_save;
	break;
#endif
#if 0
      case 0x78: // 'x' - DECREPTPARM
	// VT100 only - removed in VT102
	break;
#endif	
      default:
#ifdef DEBUG
	sprintf(STR_BUF,"esc [ %02x[%d] %d,%d %d %d    ",c1,ep,e[0],e[1],e[2],e[3]);
	PutTitleAt(48);
#endif
	break;
      }
      //      set_cursor();
      esc_mode=0;
    } else parse_control(); 
}

void escape_mode_hash()
{
  static unsigned char x;
  
  // parse the "#" DEC private sequences
  // should parse for sequence '8' - fill screen with 'E'
  switch(c1) {
  case '8':
    for(x=0;x<80;x++) STR_BUF[x]=conv_table['E'];
    for(x=0;x<24;x++) {
      COL=0;
      line_addr=line[x];
      PutLine();
    }
    break;
  default:
    break;
  }
  esc_mode=0;
}

void escape_mode_priv()
{
  unsigned char i;
  
  // parse the "[?" sequences
  if(isdigit(c1)) e[ep]=e[ep]*10+c1-0x30; // 0x30 is '0'
  else if(c1==0x3B) ep++; // ';' separator 
  else if(isprint(c1)) {
    switch(c1) {
    case 0x68: // 'h' set param
    case 0x6C: // 'l' reset param
      for(i=0;i<ep+1;i++) {
	switch(e[i]) {
	case 1: // DECCKM 
	  if(c1==0x68) priv_mode[0]|=PRIV_DECCKM;
	  else priv_mode[0]&=~PRIV_DECCKM;
	  break;
	  //	  case 2: // DECANM ANSI/VT52 mode
	  //	    break;
	case 3: // DECCOLM screen columns
	  // 132 column - not implemented
	  if(c1==0x68) priv_mode[1]|=PRIV_DECCOLM;
	  else priv_mode[1]&=~PRIV_DECCOLM;
	  // standard VT100 docs say screen should be cleared on mode switch
	  ClrScr();
	  ROW=0; COL=0;
	  line_addr=line[0];
	  break;
	case 4: // DECSCLM smooth scroll
	  if(c1==0x68) priv_mode[1]|=PRIV_DECSCLM;
	  else priv_mode[1]&=~PRIV_DECSCLM;
	  break;
	case 5: // DECSCNM screen mode
	  if(c1==0x68) {
	    SetColor(0x01);
	    VIC.spr0_color=0x00;
	    priv_mode[1]|=PRIV_DECSCNM;
	  } else {
	    SetColor(0x10);
	    VIC.spr0_color=0x01;
	    priv_mode[1]&=~PRIV_DECSCNM;
	  }
	  break;
	case 6: // DECOM origin
	  if(c1==0x68) {
	    priv_mode[0]|=PRIV_DECOM;
	    ROW=AM[0];
	  } else {
	    priv_mode[0]&=~PRIV_DECOM;
	    ROW=0;
	  }
	  COL=0;
	  line_addr=line[ROW];
#ifdef DEBUG
	  sprintf(STR_BUF,"%02d %02d %c ",AM[0],AM[1],(priv_mode[0]&PRIV_DECOM)?'E':'D');
	  PutTitleAt(64);
#endif
	  break;
	case 7: // DECAWM 
	  if(c1==0x68) priv_mode[0]|=PRIV_DECAWM;
	  else priv_mode[0]&=~PRIV_DECAWM;
	  break;
	case 8: // DECARM auto repeat mode
	  if(c1==0x68) priv_mode[1]|=PRIV_DECARM;
	  else priv_mode[1]&=~PRIV_DECARM;
	  break;
	case 9: // DECINLM interlace
	  if(c1==0x68) priv_mode[1]|=PRIV_DECINLM;
	  else priv_mode[1]&=~PRIV_DECINLM;
	  break;
	  //	  case 20: // LNM line feed mode
	  //	    break;
	default:
#ifdef DEBUG
	  if(c1==0x68) sprintf(STR_BUF,"set?   %02x [%d]        ",e[i],ep);
	  else sprintf(STR_BUF,"reset? %02x [%d]         ",e[i],ep);
	  PutTitleAt(48);
#endif
	  break;
	}
      }
      break;
    default:
#ifdef DEBUG
      sprintf(STR_BUF,"esc [? %02x[%d] %d,%d      ",c1,ep,e[0],e[1]);
      PutTitleAt(48);
#endif
      break;
    }
    esc_mode=0;
  }
}

void escape_mode_scs()
{
    switch(c1) {
    case 0x30: // '0'
      G[g_set]=CHAR_GRP;
      break;
    case 0x31: // '1'
      G[g_set]=CHAR_ALT;
      break;
    case 0x32: // '2'
      G[g_set]=CHAR_ALT_GR;
      break;
    case 0x41: // 'A'
      G[g_set]=CHAR_UK;
      break;
    case 0x42: // 'B'
      G[g_set]=CHAR_US;
      break;      
      default:
#ifdef DEBUG
	sprintf(STR_BUF,"SCS G%d %c %02X                  ",g_set,c1,c1);
	PutTitleAt(48);
#endif
	break;
    }
    esc_mode=0;  
}
