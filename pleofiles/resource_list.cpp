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
#include <stdlib.h>
#include <stdio.h>
#include "resource_list.h"


//
// Constructor...
//
pleo_resource_type::pleo_resource_type()
{
  m_resource_list = NULL;
  init_resource();
}


void pleo_resource_type::init_resource()
{
  if (m_resource_list) {
    for (int i=0; i<m_count; i++) free_element(i);
    free (m_resource_list);
  }
  m_resource_list = NULL;
  m_max_count = 0;
  m_count = 0;
}


//
// Destructor...
//
pleo_resource_type::~pleo_resource_type()
{
  init_resource();
}


void pleo_resource_type::free_element(int index)
{
  if (m_resource_list[index].m_element) {
    free (m_resource_list[index].m_element); 
    m_resource_list[index].m_element=NULL;
  }
}


bool pleo_resource_type::set_element_count(int new_count)
{
  if (new_count>=m_max_count) {
    int new_max_count = new_count+64;
    resource_type *new_resource_list = (resource_type*)malloc(sizeof(resource_type)*new_max_count);
    if (new_resource_list == NULL) return false;
    memset (new_resource_list, 0, sizeof(resource_type)*new_max_count);
    if (m_resource_list != NULL) {
      memcpy (new_resource_list, m_resource_list, sizeof(resource_type)*m_count);
      free (m_resource_list);
    }
    m_resource_list = new_resource_list;
    m_max_count = new_max_count;
  }
  m_count = new_count;
  return true;
}


//
// Add element to resource list...
//
int pleo_resource_type::add_new_element(const char *element_name, unsigned char *element_data, int element_datalen, bool assume_ownership)
{
  if (element_data==NULL) return -1;
  if (element_datalen<MIN_ELEMENT_DATALEN) return -1;

  int index = m_count;
  if (!set_element_count(m_count+1)) return -1;
  m_resource_list[index].m_element = NULL;

  return update_element(element_name, index, element_data, element_datalen, assume_ownership);
}


int pleo_resource_type::add_new_element(const char *element_name, resource_type *resource_ptr)
  {return add_new_element (element_name, resource_ptr->m_element, resource_ptr->m_element_size);}



//
// Add filler (for tracking unused element ID's on read archives)...
//
int pleo_resource_type::add_filler()
{
  int index = m_count;
  if (!set_element_count(m_count+1)) return -1;
  resource_type *target = &m_resource_list[index];
  memset (target, 0, sizeof(resource_type));
  return index;
}



//
// Update element with new data...
//
int pleo_resource_type::update_element(const char *element_name, int index, unsigned char *element_data, int element_datalen, bool assume_ownership)
{
  if (index<0) return -1;

  free_element(index);
  resource_type *target = &m_resource_list[index];

  memcpy (target->m_signature, element_data, sizeof(target->m_signature));
  memset (target->m_element_name,0,sizeof(target->m_element_name));
  strncpy (target->m_element_name,element_name,sizeof(target->m_element_name));

  // Copy element data...
  if (assume_ownership)
    target->m_element = element_data;
  else {
    target->m_element = (unsigned char *)malloc(element_datalen);
    if (target->m_element == NULL) return -1;
    memcpy (target->m_element, element_data, element_datalen);
  }

  target->m_element_size = element_datalen;
  return index;
}


int pleo_resource_type::update_element(const char *element_name, unsigned char *element_data, int element_datalen, bool assume_ownership)
  {return update_element(element_name, get_element_index(element_name), element_data, element_datalen, assume_ownership);}



//
// Remove element from resource list...
//
bool pleo_resource_type::remove_element(int index)
{
  if ((index<0) || (index>=m_count)) return false;
  free_element(index);    
  if (index<(m_count-1))
    memcpy (&m_resource_list[index], &m_resource_list[index+1], (m_count-index-1)*sizeof(resource_type));
  m_count--;
  return true;
}


bool pleo_resource_type::remove_element(const char *element_name)
{
  int index = get_element_index(element_name);
  if (index<0) return false;
  return remove_element(index);
}



//
// Find element by index...
//
unsigned char *pleo_resource_type::get_element_dataptr(int index, char *element_name, int *element_datalen)
{
  if ((index<0) || (index>=m_count)) return NULL;
  if (element_name) memcpy (element_name, m_resource_list[index].m_element_name, MAX_RESOURCE_NAMELEN);
  if (element_datalen) *element_datalen = m_resource_list[index].m_element_size;
  return m_resource_list[index].m_element;
}



//
// Find element by name...
//
int pleo_resource_type::get_element_index(const char *element_name)
{
  // Sanity...
  if (element_name==NULL) return -1;
  if (element_name[0]==0) return -1;
  // Linear search (for now)...
  for (int i=0; i<m_count; i++)
    if (m_resource_list[i].m_element_name)
      if (strcmp(m_resource_list[i].m_element_name, element_name)==0)
        return i;
  return -1;
}



//
// Find element by binary compare...
//
int pleo_resource_type::find_element_index(unsigned char *element_data, int element_datalen)
{
  if (element_data==NULL) return -1;
  if (element_datalen<MIN_ELEMENT_DATALEN) return -1;
  // Linear search (for now)...
  for (int i=0; i<m_count; i++)
    if (m_resource_list[i].m_element)
      if (memcmp(m_resource_list[i].m_element, element_data, element_datalen)==0)
        return i;
  return -1;
}

int pleo_resource_type::find_element_index(resource_type *resource_ptr)
  {return find_element_index (resource_ptr->m_element, resource_ptr->m_element_size);}



//
// Write an index file...
//
bool pleo_resource_type::write_index_file(const char *target_file, int resource_base, const char *struct_title, const char *element_prefix)
{
  int i;
  if (m_count<1) return false;

  FILE *fileid = fopen(target_file,"w");
  if (fileid==NULL) return false;

  // Title...
#ifdef TOOL_NAME  
  fprintf (fileid,"/* WARNING:  File auto-generated by %s.  DO NOT EDIT. */\n\n", TOOL_NAME);
#else
  fprintf (fileid,"/* WARNING:  File auto-generated.  DO NOT EDIT. */\n\n");
#endif

  // find longest element name...
  int maxlen=0;
  for (i=0; i<m_count; i++) {
    int namelen = strlen(m_resource_list[i].m_element_name);
    if (namelen>maxlen) maxlen=namelen;
  }

  // Prepare format string...
  char formatstr[1024];
  sprintf (formatstr,"  %s_%%-%ds = %%d,\n",element_prefix,maxlen);

  // Output structure...
  fprintf (fileid,"enum %s {\n",struct_title);
  fprintf (fileid,formatstr,"none",resource_base-1);
  fprintf (fileid,formatstr,"min",resource_base);
  for (i=0; i<m_count; i++) 
    if (m_resource_list[i].m_element_name && (m_resource_list[i].m_element_size>0))
      fprintf (fileid,formatstr,m_resource_list[i].m_element_name,resource_base+i);
  fprintf (fileid,"  %s_max\n",element_prefix);
  fprintf (fileid,"};\n");

  fclose (fileid);
  return true;
}



//
// Write resource element as file...
//
bool pleo_resource_type::write_element_file(int index, char *pathname, char *fileext)
{
  if ((index<0) || (index>=m_count)) return false;
  if (m_resource_list[index].m_element_size<1) return false;

  char target_file[1024];
  if (m_resource_list[index].m_element_name[0]) 
    sprintf (target_file,"%s\\%s.%s",pathname,m_resource_list[index].m_element_name,fileext);
  else sprintf (target_file,"%s\\zzz%d.%s",pathname,index,fileext);

  FILE *fileid = fopen(target_file,"wb");
  if (fileid==NULL) return false;
  int result = fwrite (m_resource_list[index].m_element, 1, m_resource_list[index].m_element_size, fileid);
  fclose (fileid);
  return (result == (int)m_resource_list[index].m_element_size);
}



//
// Write all resource elements to files...
//
bool pleo_resource_type::write_all_element_files(char *pathname, char *fileext)
{
  bool result = true;
  for (int i=0; i<m_count; i++) result &= write_element_file(i, pathname, fileext);
  return result;
}
