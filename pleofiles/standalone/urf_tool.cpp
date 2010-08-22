/*
 * Copyright (c) 2010 John of dogsbodynet.com
 *                    Gareth Nelson
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include "pleoarchive.h"
#include "resource_list.h"

void usage() {
     printf("urf_tool command archive [path]\n");
     printf("commands:\n");
     printf("\t l list contents\n");
     printf("\t x extract all files to path specified (current directory if none)\n");
     printf("\t c create archive with contents of path specified\n");
}

bool endswith(char* s1, char* s2) {
     int s1_len = strlen(s1);
     int s2_len = strlen(s2);
     s1 += (s1_len-s2_len);
     if (strncmp(s1,s2,s2_len)==0) return true;
     return false;
}

#define CHUNK 1024

/* Read the contents of a file into a buffer.  Return the size of the file 
 * and set buf to point to a buffer allocated with malloc that contains  
 * the file contents.
 */
int read_file(char* filename, char **buf) 
{
  FILE* fp = fopen(filename,"rb");
  int n, np,r;
  char *b, *b2;

  n = CHUNK;
  np = n;
  b = (char*)malloc(sizeof(char)*n);
  
  while ((r = fread(b, sizeof(char), CHUNK, fp)) > 0) {
    n += r;
    if (np - n < CHUNK) { 
      np *= 2;                      // buffer is too small, the next read could overflow!
      b2 = (char*)malloc(np*sizeof(char));
      memcpy(b2, b, n * sizeof(char));
      free(b);
      b = b2;
    }
  }
  *buf = b;
  fclose(fp);
  return n;
}

// strip off file extensions
char* splitstr(char* s1, char* s2) {
    char* retval=strtok(s1,s2);
    retval[strlen(retval)-1]=(char)0;
    return retval;
}

#define DUMP_RES(__RES_TYPE__,__RES_EXTENSION__,__READABLE_NAME__) \
        if(urf.m_##__RES_TYPE__.m_count==0) { \
           printf("no %ss\n", __READABLE_NAME__); \
        } else { \
           for(int i=0; i<urf.m_##__RES_TYPE__.m_count; i++) { \
               res = urf.m_##__RES_TYPE__.m_resource_list[i]; \
               printf("%-30s\t\t%-30d\t\t",res.m_element_name,res.m_element_size); \
               printf("%s/%s\n",__RES_EXTENSION__,__READABLE_NAME__); \
           } \
        }

#define EXTRACT_RES(__RES_TYPE__,__RES_EXTENSION__,__READABLE_NAME__) \
        mkdir(#__RES_TYPE__,(mode_t)0777); \
        if(urf.m_##__RES_TYPE__.m_count==0) { \
           printf("no %s\n", __READABLE_NAME__); \
        } else { \
          urf.m_##__RES_TYPE__.write_all_element_files((char*)#__RES_TYPE__,(char*)__RES_EXTENSION__); \
        }

#define IMPORT_RES(__RES_TYPE__,__RES_EXTENSION__) \
        chdir(#__RES_TYPE__); \
        while (cur_ent != NULL) { \
           cur_ent = readdir(dir);\
           if(cur_ent != NULL) {\
             if(endswith(cur_ent->d_name,__RES_EXTENSION__)) { \
                printf("%s\n",splitstr(cur_ent->d_name,__RES_EXTENSION__)); \ 
                cur_res = (resource_type*)malloc(sizeof(resource_type)); \
                strncpy(__RES_EXTENSION__,cur_res->m_signature,4);\
                cur_res->m_element_name = cur_ent->d_name; \
                cur_res->m_element_size = read_file(cur_ent->d_name,cur_res->m_element); \
             } \
           }\ 
        }\
        closedir(dir); 


#define LOAD_URF \
        if(urf.read_archive_file(archive_file)<0) { \
           fprintf(stderr,"Error reading!\n"); \
           return 1; \
        }

int main(int argc, char* argv[])
{
  endswith((char*)"blameow",(char*)"meow");
  if(argc<3) {
     usage();
     return 1;
  }
  char* command      = argv[1];
  char* archive_file = argv[2];

  pleo_archive_type urf;
  resource_type res;

  if(strncmp(command,"l",1)==0) {
     LOAD_URF
     printf("%-30s\t\t%-30s\t\t%-30s\n","Name","Size","Type");
     DUMP_RES(sounds,PLEO_TOC_SOUND_SIGNATURE,"Sound")
     DUMP_RES(motions,PLEO_TOC_MTN_SIGNATURE,"Motion")
     DUMP_RES(commands,PLEO_TOC_COMMAND_SIGNATURE,"Command")
     DUMP_RES(scripts,"AMX","Script")
     DUMP_RES(properties,PLEO_TOC_PROPERTY_SIGNATURE,"Propertie")
     return 0;
  }
  if(strncmp(command,"x",1)==0) {
     LOAD_URF
     char *dest_path;
     if(argc==4) {
        dest_path = argv[3];
     } else {
        dest_path = (char*)malloc(sizeof(char*)*1024);
        if(getcwd(dest_path,1024)==NULL) {
           fprintf(stderr,"Path too long!\n");
           return 1;
        }
     }
     if(chdir(dest_path)==-1) {
        fprintf(stderr,"Could not change CWD - does the destination path exist?\n");
        return 1;
     }
     printf("Extracting %s into %s...\n",archive_file,dest_path);
     EXTRACT_RES(sounds,PLEO_TOC_SOUND_SIGNATURE,"Sound")
     EXTRACT_RES(motions,PLEO_TOC_MTN_SIGNATURE,"Motion")
     EXTRACT_RES(commands,PLEO_TOC_COMMAND_SIGNATURE,"Command")
     EXTRACT_RES(scripts,"AMX","Script")
     EXTRACT_RES(properties,PLEO_TOC_PROPERTY_SIGNATURE,"Propertie")
  }
  if(strncmp(command,"c",1)==0) {
     char *dest_path;
     if(argc<4) {
        usage();
        return 1;
     }
     dest_path = argv[3];
     if(chdir(dest_path)==-1) {
        fprintf(stderr,"Could not change CWD - does the destination path exist?\n");
        return 1;
     }
     urf.init_archive();
     DIR* dir;
     struct dirent* cur_ent;
     resource_type* cur_res;
     
     IMPORT_RES(sounds,PLEO_TOC_SOUND_SIGNATURE)
     IMPORT_RES(motions,PLEO_TOC_MTN_SIGNATURE)
  }
}

