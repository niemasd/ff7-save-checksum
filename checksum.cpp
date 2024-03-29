/* Written by dziugo (http://forums.qhimm.com/index.php?topic=4211.msg60545#msg60545)
 * Modified by Niema Moshiri
 */
#include <iostream>
using namespace std;

// define constants
const long FF7_SAVE_GAME_SIZE = 0xFE55;
const long FF7_SAVE_GAME_FIRST_CHECKSUM = 0x09;
const long FF7_SAVE_GAME_SLOT_SIZE = 0x10F4;

// compute checksum
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

// fix the checksums of a given file
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
                cerr << "Memory allocation problem." << endl;
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
                cerr << "Couldn't open dest-file: " << file_name_write << endl;
             }
             free(data);
          }
       }else
       {
          cerr << "Couldn't open source-file: " << file_name_read << endl;
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
       cerr << "Not enough parameters." << endl << "Usage: " << argv[0] << " <input_save_file>.ff7 <ouput_save_file>.ff7" << endl;
    }
    if(!return_value)
    {
       cerr << "There were some errors." << endl;
    }
   return EXIT_SUCCESS;
}
