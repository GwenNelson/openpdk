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
#include "pleoarchive.h"
#include "resource_list.h"

void usage() {
     printf("urf_tool command archive [path]\n");
     printf("commands:\n");
     printf("\t l list contents\n");
     printf("\t x extract all files to path specified (current directory if none)\n");
     printf("\t c create archive with contents of path specified\n");
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

int main(int argc, char* argv[])
{
  if(argc<3) {
     usage();
     return 1;
  }
  char* command      = argv[1];
  char* archive_file = argv[2];
  printf("Listing archive file %s...\n",archive_file);

  pleo_archive_type urf;
  if(urf.read_archive_file(archive_file)<0) {
     fprintf(stderr,"Error reading!\n");
     return 1;
  }

  resource_type res;
  if(strncmp(command,"l",1)==0) {
     printf("%-30s\t\t%-30s\t\t%-30s\n","Name","Size","Type");
     DUMP_RES(sounds,PLEO_TOC_SOUND_SIGNATURE,"Sound")
     DUMP_RES(motions,PLEO_TOC_MTN_SIGNATURE,"Motion")
     DUMP_RES(commands,PLEO_TOC_COMMAND_SIGNATURE,"Command")
     DUMP_RES(scripts,"AMX","Script")
     DUMP_RES(properties,PLEO_TOC_PROPERTY_SIGNATURE,"Properties file")
     return 0;
  }
  if(strncmp(command,"x",1)==0) {
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
     
  }
}

