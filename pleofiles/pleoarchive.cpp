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
//
// Code to handle reading/writing Pleo URF resource archives.
//
#include <stdlib.h>
#include <stdio.h>
#include "pleoarchive.h"
#include "time.h"

struct resource_info_type {
  char signature[5]; 
  int index; 
  int resource_base; 
  int archive_alignment;
};

resource_info_type g_resource_info[] = {
  {PLEO_TOC_SOUND_SIGNATURE,    PLEO_TOC_SOUND,    PLEO_SOUND_BASE,    4},
  {PLEO_TOC_MTN_SIGNATURE,      PLEO_TOC_MTN,      PLEO_MOTION_BASE,   4},
  {PLEO_TOC_COMMAND_SIGNATURE,  PLEO_TOC_COMMAND,  PLEO_COMMAND_BASE,  4},
  {PLEO_TOC_SCRIPT_SIGNATURE,   PLEO_TOC_SCRIPT,   PLEO_SCRIPT_BASE,   4},
  {PLEO_TOC_PROPERTY_SIGNATURE, PLEO_TOC_PROPERTY, PLEO_PROPERTY_BASE, 4},
  {"",-1}};


//
// Constructor...
//
pleo_archive_type::pleo_archive_type() 
{
  memset (&m_toc_refinfo, 0, sizeof(m_toc_refinfo));
  m_toc_refinfo[PLEO_TOC_SOUND].resource_list = &m_sounds;
  m_toc_refinfo[PLEO_TOC_MTN].resource_list = &m_motions;
  m_toc_refinfo[PLEO_TOC_COMMAND].resource_list = &m_commands;
  m_toc_refinfo[PLEO_TOC_SCRIPT].resource_list = &m_scripts;
  m_toc_refinfo[PLEO_TOC_PROPERTY].resource_list = &m_properties;
}


int pleo_archive_type::get_resource_type (unsigned char *binfile)
{
  for (int i=0; i<MAX_RESOURCE_TYPES; i++)
    if (strncmp((char*)binfile, g_resource_info[i].signature, 4)==0)
      return i;

  return -1;
}


void pleo_archive_type::init_archive()
{
  for (int i=0; i<MAX_RESOURCE_TYPES; i++)
    if (m_toc_refinfo[i].resource_list)
      m_toc_refinfo[i].resource_list->init_resource();
}



//
// Read Pleo resource archive file...
//
int pleo_archive_type::read_archive_file (const char *targetfile, int flags)
{
  FILE *fileid = fopen(targetfile,"rb");
  if (fileid==NULL) return -1;
  if (fseek(fileid,0,SEEK_END) != 0) {
    fclose (fileid);
    return -1;
  }

  int datalen = ftell(fileid);
  if (datalen<MIN_ELEMENT_DATALEN) { // must be at least a signature...
    fclose (fileid);
    return -1;
  }

  if (fseek(fileid,0,SEEK_SET) != 0) {
    fclose (fileid);
    return -1;
  }

  unsigned char *dataptr = (unsigned char *)malloc(datalen);
  if (dataptr==NULL) {
    fclose (fileid);
    return -1;
  }

  int readlen = fread(dataptr, 1, datalen, fileid);
  fclose(fileid);
  if (readlen != datalen) {
    free(dataptr);
    return -1;
  }

  int result = read_archive_image(targetfile, dataptr, datalen, flags);
  free(dataptr);
  return result;
}



//
// Import image of Pleo resource archive...
//
int pleo_archive_type::read_archive_image (const char *targetfile, unsigned char *binfile, int binfilelen, int flags)
{
  int i;
  if (binfile==NULL) return -2;
  if (binfilelen<MIN_ELEMENT_DATALEN) return -2;
  unsigned char *rdptr = binfile;
  unsigned int rdofs = 0;
  unsigned int rdlen = binfilelen;

  // Check header...
  pleo_archive_hdrtype *hdr = (pleo_archive_hdrtype*)binfile;
  if (strncmp(hdr->m_signature,PLEO_ARCHIVE_SIGNATURE,4) != 0) return -2;
  rdofs += sizeof(pleo_archive_hdrtype);

  // Init reference info for processing table-of-content records...
  for (i=0; i<MAX_RESOURCE_TYPES; i++) m_toc_refinfo[i].binofs=-1;

  // Get table-of-content per-resource type offsets...
  bool found_size_chunk = false;  
  while (!found_size_chunk) {
    pleo_archive_toctype *toc = (pleo_archive_toctype*)(&binfile[rdofs]);
    if (strncmp(toc->m_toc_signature, PLEO_ARCHIVE_SIZE_SIGNATURE, 4)==0)
      found_size_chunk = true;
    else {
      if ((rdofs + sizeof(pleo_archive_toctype))>rdlen) return -2;
      int resource_type = get_resource_type(&binfile[rdofs]);
      if (resource_type>=0) 
        if (m_toc_refinfo[resource_type].resource_list != NULL)
          m_toc_refinfo[resource_type].binofs = toc->m_toc_offset;
      rdofs += sizeof(pleo_archive_toctype);
    }
  }

  // Get expected resource size.  Should be size of resource file minus CRC chunk...
  if ((rdofs + sizeof(pleo_archive_sizetype))>rdlen) return -2;
  pleo_archive_sizetype *sizeinfo = (pleo_archive_sizetype*)(&binfile[rdofs]);
  int expected_size = sizeinfo->m_size;
  rdofs += sizeof(pleo_archive_sizetype);

  // Read resource entries...
  for (i=0; i<MAX_RESOURCE_TYPES; i++) {
    int tocofs = m_toc_refinfo[i].binofs;
    if (tocofs>0) {
      if (tocofs > binfilelen) return -2;
      pleo_archive_toctype *toc = (pleo_archive_toctype*)&binfile[tocofs];
      if (get_resource_type((unsigned char*)toc) != i) return -2; // validate signature...

      int entry_count = (toc->m_toc_offset)/sizeof(pleo_archive_toc_entrytype);
      if ((entry_count>0) && (entry_count<MAX_RESOURCE_ENTRIES)) {
        pleo_archive_toc_entrytype *toc_entry = (pleo_archive_toc_entrytype*)&binfile[tocofs+sizeof(pleo_archive_toctype)];

        if (m_toc_refinfo[i].resource_list)
          if (i==PLEO_TOC_PROPERTY) {
            for (int j=0; j<entry_count; j++,toc_entry++)
              if ((toc_entry->entry_ofs>0) && (toc_entry->entry_ofs<rdlen) && (toc_entry->entry_len>0))
                m_toc_refinfo[i].resource_list->add_new_element(toc_entry->entry_name, &binfile[toc_entry->entry_ofs], 4);
              else m_toc_refinfo[i].resource_list->add_filler();
          }
          else {
            for (int j=0; j<entry_count; j++,toc_entry++)
              if ((toc_entry->entry_ofs>0) && (toc_entry->entry_ofs<rdlen) && (toc_entry->entry_len>0))
                m_toc_refinfo[i].resource_list->add_new_element(toc_entry->entry_name, &binfile[toc_entry->entry_ofs], toc_entry->entry_len);
              else m_toc_refinfo[i].resource_list->add_filler();
          }
      }
    }
  }


  return true;
}



//
// Compute amount of memory needed to store archive...
//
int pleo_archive_type::compute_archive_filesize (int flags)
{
  int i,j;
  int count=0;
  pleo_resource_type *rl;

  // Header...
  count += sizeof(pleo_archive_hdrtype);

  // TOC offsets...
  for (i=0; i<MAX_RESOURCE_TYPES; i++) count += sizeof(pleo_archive_toctype);

  // Size record...
  count += sizeof(pleo_archive_sizetype);


  // Resource entries...
  for (i=0; i<MAX_RESOURCE_TYPES; i++)
    if (rl = m_toc_refinfo[i].resource_list) {
      // Align to 512 byte boundary...
      if (count & 0x1FF) count += 0x200-(count & 0x1FF); 
      int align = g_resource_info[i].archive_alignment;
      for (j=0; j<(rl->m_count); j++) {
        if (count & (align-1)) count += align-(count & (align-1)); 
        count += rl->m_resource_list[j].m_element_size;
      }
    }

  // TOC entries...
  if (count & 0x1FF) count += 0x200-(count & 0x1FF); // align to 512 byte boundary
  for (i=0; i<MAX_RESOURCE_TYPES; i++)
    if (rl = m_toc_refinfo[i].resource_list) {
      count += sizeof(pleo_archive_toctype); // toc header
      for (j=0; j<(rl->m_count); j++) count += sizeof(pleo_archive_toc_entrytype); // toc entries
    }

  // Alder32 CRC entry...
  if (count & 7) count += 8-(count & 7); // align to 8 byte boundary
  count += sizeof(pleo_archive_crctype);

  return count;
}



//
// Write Pleo resource archive to memory image...
//
unsigned char *pleo_archive_type::write_archive_image (const char *targetfile, int *binfilelen, int flags)
{
  int i,j;
  pleo_resource_type *rl;
  if (binfilelen==NULL) return NULL;

  // Compute size of archive..
  int imagesize = compute_archive_filesize(flags);

  unsigned char *wrdata = new unsigned char[imagesize+256];
  if (wrdata==NULL) return NULL;

  // Generate header...
  int wrofs=0;
  pleo_archive_hdrtype *hdr = (pleo_archive_hdrtype *)&wrdata[wrofs];  
  wrofs += sizeof(pleo_archive_hdrtype);
  memcpy (hdr->m_signature, PLEO_ARCHIVE_SIGNATURE, sizeof(hdr->m_signature));
  hdr->m_format = 1;
  time (&(hdr->m_buildtime)); // time in seconds
  hdr->m_version = 0;

  // Generate TOC offset record placeholders...
  pleo_archive_toctype *tocofs = (pleo_archive_toctype *)&wrdata[wrofs];
  for (i=0; i<MAX_RESOURCE_TYPES; i++) {
    pleo_archive_toctype *toc = &tocofs[i];
    wrofs += sizeof(pleo_archive_toctype);
    memcpy (toc->m_toc_signature, g_resource_info[i].signature, sizeof(toc->m_toc_signature));
    toc->m_toc_offset=0; // for now
  }

  // Generate size record...
  pleo_archive_sizetype *sizerec = (pleo_archive_sizetype *)&wrdata[wrofs];
  wrofs += sizeof(pleo_archive_sizetype);
  memcpy (sizerec->m_size_signature, PLEO_ARCHIVE_SIZE_SIGNATURE, sizeof(sizerec->m_size_signature));
  sizerec->m_size = imagesize-sizeof(pleo_archive_crctype);

  // Generate resource entries (each padded to 512 byte boundary)...
  for (i=0; i<MAX_RESOURCE_TYPES; i++)
    if (rl = m_toc_refinfo[i].resource_list) {
      // Align to 512 byte boundary between resource types...
      if (wrofs & 0x1FF) { 
        int padding = 0x200-(wrofs & 0x1FF);
        memset (&wrdata[wrofs], '0', padding);
        wrofs += padding;
      }

      int align = g_resource_info[i].archive_alignment;
      for (j=0; j<(rl->m_count); j++) {
        // Align to per-resource boundary between entries...
        if (wrofs & (align-1)) { 
          int padding = align-(wrofs & (align-1));
          memset (&wrdata[wrofs], '0', padding);
          wrofs += padding;
        }
        
        int esize = rl->m_resource_list[j].m_element_size;
        if (esize>0) {
          rl->m_resource_list[j].m_toc_offset = wrofs;
          memcpy (&wrdata[wrofs], rl->m_resource_list[j].m_element, esize);
          wrofs += esize;
        }
        else rl->m_resource_list[j].m_toc_offset = 0xFFFFFFFF;

        // Sanity check...
        if (wrofs>imagesize) 
          return NULL;
      }
    }

  // Align to 512 byte boundary before TOC...
  if (wrofs & 0x1FF) { 
    int padding = 0x200-(wrofs & 0x1FF);
    memset (&wrdata[wrofs], '0', padding);
    wrofs += padding;
  }

  // Generate TOC...
  for (i=0; i<MAX_RESOURCE_TYPES; i++)
    if (rl = m_toc_refinfo[i].resource_list) {
      tocofs[i].m_toc_offset = wrofs;      
      int entry_count = (rl->m_count);

      pleo_archive_toctype *toc = (pleo_archive_toctype *)&wrdata[wrofs];
      wrofs += sizeof(pleo_archive_toctype);
      memcpy (toc->m_toc_signature, g_resource_info[i].signature, sizeof(toc->m_toc_signature));
      toc->m_toc_offset=sizeof(pleo_archive_toc_entrytype)*entry_count;

      for (j=0; j<entry_count; j++) {
        pleo_archive_toc_entrytype *toc_entry = (pleo_archive_toc_entrytype *)&wrdata[wrofs];
        wrofs += sizeof(pleo_archive_toc_entrytype);

        if (rl->m_resource_list[j].m_toc_offset == 0) { // filler
          memset (toc_entry, 0, sizeof(pleo_archive_toc_entrytype));
          toc_entry->entry_ofs = 0xFFFFFFFF;
        }
        else {
          memcpy (toc_entry->entry_name, rl->m_resource_list[j].m_element_name, sizeof(toc_entry->entry_name));
          toc_entry->entry_ofs = rl->m_resource_list[j].m_toc_offset;

          if (i==PLEO_TOC_PROPERTY)
            toc_entry->entry_len = strlen(rl->m_resource_list[j].m_element_name); // namelen versas datalen for other resources
          else toc_entry->entry_len = rl->m_resource_list[j].m_element_size;
        }
      }
    }

  // Align to 8 byte boundary before CRC...
  if (wrofs & 7) { 
    int padding = 8-(wrofs & 7);
    memset (&wrdata[wrofs], '0', padding);
    wrofs += padding;
  }

  // Sanity check...
  if (wrofs != (int)(imagesize-sizeof(pleo_archive_crctype)))
    return NULL;

  // Compute Alder32 CRC...
  unsigned crc_value = adler32(0, wrdata, wrofs);

  pleo_archive_crctype *crc = (pleo_archive_crctype *)&wrdata[wrofs];
  wrofs += sizeof(pleo_archive_crctype);
  memcpy (crc->m_crc_signature, PLEO_ARCHIVE_CRC_SIGNATURE, sizeof(crc->m_crc_signature));
  crc->m_crc = crc_value;

  // Done!
  *binfilelen = wrofs;
  return wrdata;
}


// =========================================================================
// adler32.c -- compute the Adler-32 checksum of a data stream
// Copyright (C) 1995-1998 Mark Adler
// For conditions of distribution and use, see copyright notice in zlib.h

// @(#) $Id$

#define AD_DO1(buf,i)  {s1 += buf[i]; s2 += s1;}
#define AD_DO2(buf,i)  AD_DO1(buf,i); AD_DO1(buf,i+1);
#define AD_DO4(buf,i)  AD_DO2(buf,i); AD_DO2(buf,i+2);
#define AD_DO8(buf,i)  AD_DO4(buf,i); AD_DO4(buf,i+4);
#define AD_DO16(buf)   AD_DO8(buf,0); AD_DO8(buf,8);

#define BASE 65521L // largest prime smaller than 65536
#define NMAX 5552
// NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1

unsigned int pleo_archive_type::adler32(unsigned int adler, unsigned char *buf, unsigned int len)
{
    unsigned long s1 = adler & 0xffff;
    unsigned long s2 = (adler >> 16) & 0xffff;
    int k;

    if (buf == NULL) return 1L;

    while (len > 0) {
        k = len < NMAX ? len : NMAX;
        len -= k;
        while (k >= 16) {
            AD_DO16(buf);
	    buf += 16;
            k -= 16;
        }
        if (k != 0) do {
            s1 += *buf++;
	    s2 += s1;
        } while (--k);
        s1 %= BASE;
        s2 %= BASE;
    }
    return (s2 << 16) | s1;
}



//
// Write Pleo resource archive file...
//
int pleo_archive_type::write_archive_file (const char *targetfile, int flags)
{
  // Sanity...
  if (targetfile==NULL) return -1;
  if (targetfile[0]==0) return -1;

  // Generate archive image...
  int imagelen;
  unsigned char *dataptr = write_archive_image (targetfile, &imagelen, flags);
  if (dataptr==NULL) return -1;

  // Create file...
  FILE *fileid = fopen(targetfile,"wb");
  if (fileid==NULL) {
    delete [] dataptr;
    return -1;
  }

  // Save image to file...
  int writelen = fwrite (dataptr, 1, imagelen, fileid);

  // Cleanup...
  fclose (fileid);
  delete [] dataptr;
  return (writelen==imagelen);
}
