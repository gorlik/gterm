;******************************************************************************
;*  GTERM - GGLABS TERMINAL                                                   *
;*  A fast 80 columns terminal for the C64                                    *
;*  Copyright 2019-2026 Gabriele Gorla                                        *
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

_SCR_BASE   = $6000
_COLOR_BASE = _SCR_BASE-$400
_SPR_DATA   = _COLOR_BASE-64

_SER_BUF    = $C800
_STR_BUF    = $C900
_ACIA_BASE  = $DE00

.segment "ZP_2" : zeropage
;;  ZP_2 range 0xF7-0xFE
_dst_addr:
	.res 2
_get:
	.res 1
_put:
	.res 1
_c1:	
	.res 1
_c2:
	.res 1
_ROW:
	.res 1
_COL:
	.res 1

.segment "ZEROPAGE" : zeropage
_src_addr:
t1:
	.res 1
t2:
	.res 1

_zptemp:
	.res 1

.exportzp	_dst_addr
.exportzp	_get
.exportzp	_put
.exportzp	_c1
.exportzp	_c2

.exportzp	t1
.exportzp	t2
.exportzp	_zptemp

.exportzp	_ROW
.exportzp	_COL
.exportzp	_src_addr

.export	_STR_BUF
.export	_SCR_BASE
.export	_COLOR_BASE
.export	_SPR_DATA
.export	_SER_BUF
.export	_ACIA_BASE
