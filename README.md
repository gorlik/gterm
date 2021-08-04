# gterm
GTERM is VT100 terminal emulator for the Commodore 64. GTERM supports the GLINK232T/Turbo232 and the GLINK232/swiftlink UART cartridges. It features a custom monochrome software 80 columns display and supports serial speeds up to 115200 bauds on a stock c64.

#Design Goals

    Support for the GGLABS GLINK232 and GLINK232T cartridges
    compact code that can fit in a standard 16K cartridge
    reasonably large subset of VT100 features to be useful
    optimized for speed (mixed C and assembly) 

#VT100 Emulation

GTERM VT100 emulation aims to be as complete as possible in order to be useful. A few features are not implemented because of the hardware limitations or the rare use of the features. The known missing features are listed below:

    Missing 132 column support (the C64 hardware is not capable of a legible 132 column display)
    Missing support for double wide and double height characters. This could be implemented but I have not seen any software that uses this feature. (MATE term also seems to not implement this feature)
    VT52 emulation (possible with some state machine rework)
    Request Terminal Parameters Feature (planned for a future version)
    bold is ignored (not possible with 3 pixel wide characters)
    blink is also ignored (possible but requires a character attributes buffer which is not implemented by design)
    UK charset is not implemented (planned for a future version) 

The GTERM VT100 emulation has been extensively tested using vttest 2.7.
All 80 columns test pass without any issues, 132 columns tests produce incorrect screens.

#Terminal Settings

Currently GTERM supports only 8N1. Several baud rates are available 2400,9600,19200,38400,57600 and 115200.
The function keys F1 (baud up) and F3 (baud down) can be used to change the baud rate.

#Keyboard Mapping

The differences between the Commodore keyboard and the VT100 keyboard makes necessary to translate some keys. The following table has a summary of the mappings:

VT100 	Commodore 	Notes
CTRL 	Commodore Key 	Easier to reach and similar location as a PC keyboard
F1-F10 	CTRL + number 	Can't use the Commdore function keys as most linux software expects 10 of them
ESC 	← 	 
TAB 	RUN STOP 	 
\ 	£ 	 
{ 	SHIFT + '@' 	 
¦ 	SHIFT + '+' 	 
} 	SHIFT + '*' 	 
_ 	SHIFT + '-' 	 
~ 	π 	 
` 	SHIFT + '£' 	 

#Soft-80 Implementation

To maximize speed the GTERM soft-80 implementation is monochrome only. There is no ASCII screen buffer. The character set was drawn using vchar64 exported as a bin file and then reordered with a custom utility to improve the character drawing performance.
A few important functions have special case implementation for common cases. I.e. character drawing in pairs, scrolling and clearing the entire screen.

