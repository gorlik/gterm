;******************************************************************************
;*  GTERM - GGLABS TERMINAL                                                   *
;*  A fast 80 columns terminal for the C64                                    *
;*  Copyright 2026 Gabriele Gorla                                             *
;*                                                                            *
;*  This program is free software: you can redistribute it and/or modify      *
;*  it under the terms of the GNU General Public License as published by      *
;*  the Free Software Foundation, either version 3 of the License, or         *
;*  (at your option) any later version.                                       *
;*                                                                            *
;*  GTERM is distributed in the hope that it will be useful,                  *
;*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
;*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
;*  GNU General Public License for more details.                              *
;*                                                                            *
;*  You should have received a copy of the GNU General Public License         *
;*  along with GTERM.  If not, see <http://www.gnu.org/licenses/>.            *
;*                                                                            *
;******************************************************************************

_SCR_BASE   = 0x6000
_COLOR_BASE = _SCR_BASE-0x400
_SPR_DATA   = _COLOR_BASE-64

_SER_BUF    = 0xC800
_STR_BUF    = 0xC900
_ACIA_BASE  = 0xDE00

.area ZP
_dst_addr:
	.ds 2
_get:
	.ds 1
_put:
	.ds 1
_c1:
	.ds 1
_c2:
	.ds 1
_ROW:
	.ds 1
_COL:
	.ds 1
_src_addr:
t1:
	.ds 1
t2:
	.ds 1

_zptemp:
	.ds 1

.globl	_dst_addr
.globl	_get
.globl	_put
.globl	_c1
.globl	_c2

.globl	t1
.globl	t2
.globl	_zptemp

.globl	_ROW
.globl	_COL
.globl	_src_addr

.globl	_STR_BUF
.globl	_SCR_BASE
.globl	_COLOR_BASE
.globl	_SPR_DATA
.globl	_SER_BUF
.globl	_ACIA_BASE
