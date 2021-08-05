;******************************************************************************
;*  GTERM - GGLABS TERMINAL                                                   *
;*  A fast 80 columns terminal for the C64                                    *
;*  Copyright 2019 Gabriele Gorla                                             *
;*                                                                            *
;*  This program is free software: you can redistribute it and/or modify      *
;*  it under the terms of the GNU General Public License as published by      *
;*  the Free Software Foundation, either version 3 of the License, or         *
;*  (at your option) any later version.                                       *
;*                                                                            *
;*  GMT is distributed in the hope that it will be useful,                    *
;*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
;*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
;*  GNU General Public License for more details.                              *
;*                                                                            *
;*  You should have received a copy of the GNU General Public License         *
;*  along with GTERM.  If not, see <http://www.gnu.org/licenses/>.            *
;*                                                                            *
;******************************************************************************
.setcpu		"6502"
.autoimport 	on
.importzp	sp

ACIA_DATA    = (_ACIA_BASE+0)
ACIA_STATUS  = (_ACIA_BASE+1)
ACIA_COMMAND = (_ACIA_BASE+2)
ACIA_CONTROL = (_ACIA_BASE+3)
ACIA_EXT     = (_ACIA_BASE+7)

; from C files
.import         _soft80_charset	
.import         _attr


.importzp       _line_addr
.importzp       _get
.importzp       _put
.importzp       _c1
.importzp       _c2
.importzp	_ROW
.importzp	_COL
.importzp 	_SRC_LINE

.import _SCR_BASE
.import _SER_BUF
.import _STR_BUF

