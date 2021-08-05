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

#ifndef GTERM_H
#define GTERM_H

// zero page variables
#define itemp_ptr    0xA3
#define itemp        (*((unsigned int *)  itemp_ptr))
#define ctemp1       (*((unsigned char *) itemp_ptr))
#define ctemp2       (*((unsigned char *) (itemp_ptr+1) ))

#define POS(r,c)     (line[r]+((c>>1)<<3))

// priv_mode defines
#define PRIV_DECCKM  0x01  // cursor key
#define PRIV_DECSCNM 0x10  // screen mode (norm/inv)
#define PRIV_DECOM   0x20  // origin mode
#define PRIV_DECAWM  0x40  // wrap mode
#define ANSI_LNM     0x80  // line feed mode

#define PRIV_DECINLM 0x01 // interlace
#define PRIV_DECSCLM 0x10 // scroll mode (jump/smooth)
#define PRIV_DECCOLM 0x40 // screen columns (80/132 col)
#define PRIV_DECARM  0x80 // auto repeat mode

// char sets
#define CHAR_US     0x00
#define CHAR_UK     0x01
#define CHAR_GRP    0x80
#define CHAR_ALT    0x02
#define CHAR_ALT_GR 0x82

// baud table r2 flags
#define BT_EXT_TURBO  0x40
#define BT_EXT_POLL   0x80

#define ATTR_INVERSE   0x01
#define ATTR_UNDERLINE 0x02

struct baud_table_t {
  int baud;
  unsigned char ctrl;
  unsigned char ext;
};

struct acia_t {
  unsigned char data;
  unsigned char status;
  unsigned char command;
  unsigned char control;
  unsigned char pad1;
  unsigned char pad2;
  unsigned char pad3;
  unsigned char ext;
};

/*
struct term_cfg_t {
  unsigned char priv_mode[2];
  unsigned char G[2];
  unsigned char g_set;
  
};
*/

// escape functions
void no_escape(void);
void escape_mode_imm(void);
void escape_mode_csi(void);
void escape_mode_hash(void);
void escape_mode_priv(void);
void escape_mode_scs(void);

void parse_control(void);

// other prototypes
void PutTitleAt(unsigned char c);
void send_str(unsigned char *str);

void ScrollRegion(void);
void InvScrollRegion(void);
void Scroll(unsigned char l);
void InvScroll(unsigned char l);
extern void (*Scroll1)(void);
extern void (*InvScroll1)(void);

void set_pos(void);

// ASM function prototypes
void NMI(void);
void __fastcall__ ser_recv(void);

void __fastcall__ ClrScr(void);
void __fastcall__ SetColor(unsigned char c);
void __fastcall__ ClrLine(unsigned char l);
void __fastcall__ ClrLineAddr(void);
//void __fastcall__ ClrLastLine(void);
//void __fastcall__ ClrFirstLine(void);
void __fastcall__ FS_Scroll1(void);
void __fastcall__ FS_InvScroll1(void);
void __fastcall__ CopyLine(void);
//void __fastcall__ Scroll4(void);
//void __fastcall__ Scroll8(void);
void __fastcall__ PutLine(void);

void __fastcall__ PutChar80(void);
void __fastcall__ Put2Char80(void);
void __fastcall__ ClrTo(void);
void __fastcall__ ClrFrom(void);

// variables from ASM
extern unsigned char * SRC_LINE;
#pragma zpsym("SRC_LINE")
extern unsigned char * line_addr;
#pragma zpsym("line_addr")
extern unsigned char ROW, COL;
#pragma zpsym("ROW")
#pragma zpsym("COL")
extern unsigned char get,put;
#pragma zpsym("get")
#pragma zpsym("put")
extern unsigned char c1,c2;
#pragma zpsym("c1")
#pragma zpsym("c2")

extern unsigned char STR_BUF[];  // temp string buffer
extern unsigned char SER_BUF[];  // rx buffer
extern unsigned char SCR_BASE[]; // screen base
extern unsigned char COLOR_BASE[]; // color base
extern unsigned char SPR_DATA[];   // cursor sprite

extern volatile struct acia_t ACIA_BASE;

extern const unsigned char soft80_charset[]; // soft80 font
extern const unsigned char conv_table[256];  // petscii to ascii
extern unsigned char * const line[];           // precomputed line address

// global variables
extern unsigned char esc_mode; // vt100 state machine

extern unsigned char priv_mode[2];  // DEC private state
extern unsigned char attr;          // char attribute

extern unsigned char g_act; // active char set (G0/G1)
extern unsigned char G[4];  // G0-G3 charset
extern unsigned char AM[2]; // active top and bottom margin
extern unsigned char tab[10]; // tab stops (80 bits)

// storage for save/restore cursor
extern unsigned char col_save, row_save, attr_save; 


extern unsigned char * line_temp;
extern unsigned char ctemp;


#define memset8(addr, v, c) \
  __asm__("ldx #%b",c);	    \
  __asm__("lda #%b",v);		      \
  __asm__("l%v: sta %v-1,x",addr,addr);   \
  __asm__("dex"); \
  __asm__("bne l%v",addr);

#endif
