#include <stdio.h>
#include <stdlib.h>

using namespace std;

const long FF7_SAVE_GAME_SIZE = 0xFE55;
const long FF7_SAVE_GAME_FIRST_CHECKSUM = 0x09;
const long FF7_SAVE_GAME_SLOT_SIZE = 0x10F4;

int ff7_checksum( void* qw )
{
   int i = 0, t, d;
   long r = 0xFFFF, len = 4336;
   long pbit = 0x8000;
   char* b=(char*)qw;

   while( len-- ) {
      t = b[i++];
      r ^= t << 8;
      for(d=0;d<8;d++) {
         if( r & pbit )
            r = ( r << 1 ) ^ 0x1021;
         else
            r <<= 1;
      }
      r &= ( 1 << 16 ) - 1;
   }
   return (r^0xFFFF)&0xFFFF;
}

int ff7_writechecksums(char *file_name_read, char *file_name_write)
{
    FILE * file;
    void * data;
    long file_size;
    
    if(file_name_read)
    {
       file = fopen(file_name_read, "rb");
       if(file)
       {
          fseek(file, 0, SEEK_END);
          file_size = ftell(file);
          if(file_size == FF7_SAVE_GAME_SIZE)
          {
             rewind(file);
             data = (void*) malloc(file_size);
             if(!data)
             {
                printf("Memory allocation problem.\0");
                return 0;
             }
             fread(data, 1, file_size, file);
             fclose(file);

             for(int i=0, checksum=0; i<15; i++)
             {
                char * data_pointer = ((char*)data + FF7_SAVE_GAME_FIRST_CHECKSUM + \
                   FF7_SAVE_GAME_SLOT_SIZE*i + 0x04);
                checksum = ff7_checksum(data_pointer);
                data_pointer -= 4;
                data_pointer[0] = (char)((checksum)&0xFF);
                data_pointer[1] = (char)((checksum>>8)&0xFF);
                data_pointer[2] = (char)((checksum>>16)&0xFF);
                data_pointer[3] = (char)((checksum>>24)&0xFF);
             }
             if(!file_name_write)
                file_name_write = file_name_read;
             file = fopen(file_name_write, "wb");
             if(file)
             {
                fwrite(data, 1, file_size, file);
                fclose(file);
                free(data);
                return 1;
             }else
             {
                printf("Couldn't open dest-file: %s", file_name_write);
             }
             free(data);
          }
       }else
       {
          printf("Couldn't open source-file: %s", file_name_read);
       }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    int return_value = 0;
    if(argc > 1)
    {
       if(argc == 2)
          return_value = ff7_writechecksums(argv[1], NULL);
       else
          return_value = ff7_writechecksums(argv[1], argv[2]);
    }else
    {
       printf("Not enough parameters.\nUsage: checksum.exe srcfile.ff7 dstfile.ff7\n\n\0");
    }
    if(!return_value)
    {
       printf("There were some errors.\0");
    }
   return EXIT_SUCCESS;
}
