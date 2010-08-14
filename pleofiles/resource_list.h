/*
 * Copyright (c) 2010 Dogsbody and Ratchet Software
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
#ifndef _PLEO_RESOURCE_LIST_H_
#define _PLEO_RESOURCE_LIST_H_

#include <stdlib.h>
#include <string.h>

#define MIN_ELEMENT_DATALEN 4
#define MAX_RESOURCE_NAMELEN 32


//
// Structure for tracking binary elements...
//
struct resource_type {
  char m_signature[4];
  char m_element_name[MAX_RESOURCE_NAMELEN+4];
  int m_toc_offset; // offset within toc archive (when saved)...
  unsigned int m_element_size; // size of element data
  unsigned char *m_element;
};



// Resource tracking structure...
class pleo_resource_type {
public:
  pleo_resource_type();
  virtual ~pleo_resource_type();

  resource_type *m_resource_list; // array of "resource_type"
  int m_max_count;
  int m_count;

  // Operations...
  void init_resource();
  bool set_element_count(int new_count);
  void free_element(int index);
  int add_new_element(const char *element_name, unsigned char *m_element_data, int m_element_datalen, bool assume_ownership=false);
  int add_new_element(const char *element_name, resource_type *resource_ptr);
  int update_element(const char *element_name, int index, unsigned char *m_element_data, int m_element_datalen, bool assume_ownership=false);
  int update_element(const char *element_name, unsigned char *m_element_data, int m_element_datalen, bool assume_ownership=false);
  int add_filler();
  bool remove_element(int index);
  bool remove_element(const char *element_name);

  int get_element_status(int index) {
    if ((index<0) || (index>=m_count)) return 0; // invalid
    if ((m_resource_list[index].m_element == NULL) && (m_resource_list[index].m_element_size==0)) return -1; // filler
    return 2; // valid
  }

  unsigned char *get_element_dataptr(int index, char *element_name=NULL, int *element_datalen=NULL);
  int get_element_index(const char *element_name);
  int find_element_index(unsigned char *m_element_data, int m_element_datalen);
  int find_element_index(resource_type *resource_ptr);
  bool write_index_file(const char *target_file, int resource_base, const char *struct_title, const char *element_prefix);
  bool write_element_file(int index, char *pathname, char *fileext);
  bool write_all_element_files(char *pathname, char *fileext);
};



#endif // _PLEO_RESOURCE_LIST_H_
