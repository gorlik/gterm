/* asc_ctype.h */
/* selected ctype.h macros for ASCII */

#ifndef ASC_CTYPE_H
#define ASC_CTYPE_H

extern const unsigned char isxx[256];

#define isalpha(x)    (isxx[x]&0x06)
#define islower(x)    (isxx[x]&0x04)
#define isupper(x)    (isxx[x]&0x02)
#define isdigit(x)    (isxx[x]&0x01)
#define isxdigit(x)   (isxx[x]&0x08)
#define isalphanum(x) (isxx[x]&0x07)
#define isprint(x)    (isxx[x]&0x80)

#define isdecgfx(x)   (isxx[x]&0x40)

#endif
