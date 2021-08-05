#include <stdio.h>
#include <ctype.h>

int main()
{
  int i;
  unsigned char c;
  
  printf("const unsigned char isxx[256] = {\n");

  for(i=0;i<256;i++) {
    c=0;
    c|=isdigit(i)?1:0;
    c|=isupper(i)?2:0;
    c|=islower(i)?4:0;
    c|=isxdigit(i)?8:0;

    // generate isprint()
    c|=isprint(i)?0x80:0;

    // generate isdecgfx
    if( (i>0x5E) && (i<0x7F) ) c|=0x40;

    
    printf("0x%02X", c);
    if((i+1)&0x0f) printf(", ");
    else printf(",\n");
  }
  printf("};\n");

  return 0;
}
