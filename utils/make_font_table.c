#include <stdio.h>

unsigned char buffer[2048];

int main(int argc, char *argv[])
{
  int i,j;
  FILE *f;
 
  if(argc!=2) {
    printf("usage: make_table filename\n");
    return 0;
  }

  f=fopen(argv[1],"rb");
  fread(buffer, 2048, 1, f);
  fclose(f);

  printf("/* soft80_font.c */\n\n");


  printf("#pragma rodata-name (\"TABLES\")\n\n");

  printf("const unsigned char soft80_charset[2048] = {\n");

  for(j=0;j<8;j++) {
    for(i=0;i<128;i++) {
      //      if(i%16==0) printf(".byte ");
      printf("0x%02X, ",buffer[i*8+j]);
      if(i%16==15) printf("\n");
      //      else printf(", ");
    }
  }

  for(j=0;j<8;j++) {
    for(i=0;i<128;i++) {
      //      if(i%16==0) printf(".byte ");
      printf("0x%02X, ",buffer[i*8+j]>>4);
      if(i%16==15) printf("\n");
      //      else printf(", ");
    }
  }

  printf("};\n");

  return 0;
}
