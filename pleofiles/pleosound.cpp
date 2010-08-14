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
#include <stdlib.h>
#include <stdio.h>
#include "pleosound.h"


// Intel ADPCM step variation table...
static int g_adpcm_index_table[16] = {
  -1, -1, -1, -1, 2, 4, 6, 8, -1, -1, -1, -1, 2, 4, 6, 8};

#define MAX_STEPSIZE 88
static int g_adpcm_stepsize_table[MAX_STEPSIZE+1] = {
  7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
  19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
  50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
  130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
  337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
  876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
  2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
  5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
  15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
};



//
// Convert PCM input to ADPCM...
//   8 bit input assumed unsigned.
//   16 & 32-bit input assumed signed.
//
int pleo_sound_type::pleo_sound_convert_pcm2adpcm(
  unsigned char *srcdata, int srclen, int src_samplesize,
  unsigned char **destdata, int *destlen,
  int flags)
{
  int state = 0; // None

  unsigned char *src_charptr = (unsigned char *)srcdata;
  short *src_shortptr = (short *)srcdata;
  int *src_intptr = (int *)srcdata;

  int value;
  int last_value = 0;  
  int diff, sign;
  int delta, vpdiff;
  int last_delta=0;

  int stepsize_index = 0;
  int stepsize = g_adpcm_stepsize_table[stepsize_index];

  int newbuf_maxsize = srclen;
  unsigned char *newbuf = (unsigned char *)malloc(newbuf_maxsize);
  if (newbuf==NULL) return -1;
  int newbuf_datalen = 0;
  bool newbuf_increment = false;

  for (int i=0; i<srclen; i += src_samplesize) {
    switch (src_samplesize) {
      case 1 : value = *(src_charptr++) - 128; break;
      case 2 : value = *(src_shortptr++); break;
      case 4 : value = *(src_intptr++) >> 16; break;
      default : return -1; // invalid sample size...
    }

    // Get difference & sign...
    diff = value-last_value;
    if (diff<0) {sign=8; diff=-diff;} else sign=0;

    // Compute: 
    //   delta = diff*4/stepsize;
    //   vpdiff = (delta+0.5)*stepsize/4;
    delta = 0;
    vpdiff = (stepsize >> 3);
    if ( diff >= stepsize ) {
      delta = 4;
      diff -= stepsize;
      vpdiff += stepsize;
    }
    stepsize >>= 1;
    if ( diff >= stepsize  ) {
      delta |= 2;
      diff -= stepsize;
      vpdiff += stepsize;
    }
    stepsize >>= 1;
    if ( diff >= stepsize ) {
      delta |= 1;
      vpdiff += stepsize;
    }

    // Update & clamp previous value...
    if (sign) last_value -= vpdiff; else last_value += vpdiff;
    if (last_value > 32767) last_value = 32767; else if (last_value <-32768) last_value = -32768;

    // Update output...
    delta = (delta|sign)&0xF;
    if (newbuf_increment) {
      if (newbuf_datalen >= newbuf_maxsize) {
        newbuf_maxsize += 2*(srclen-i);
        unsigned char *tempbuf = (unsigned char *)xrealloc(newbuf,newbuf_maxsize);
        if (tempbuf==NULL) {free(newbuf); return -1;}
        newbuf = tempbuf;
      }
      newbuf[newbuf_datalen++] = delta | (last_delta<<4);
    }
    last_delta = delta;
    newbuf_increment = !newbuf_increment;

    // Update step index & value...
    stepsize_index += g_adpcm_index_table[delta];
    if (stepsize_index < 0) stepsize_index = 0;
    if (stepsize_index > MAX_STEPSIZE) stepsize_index = MAX_STEPSIZE;
    stepsize = g_adpcm_stepsize_table[stepsize_index];
  }

  // Return results...
  if (destdata) *destdata = newbuf;
  if (destlen) *destlen = newbuf_datalen;
  return newbuf_datalen;
}



//
// Convert ADPCM to signed PCM...
//
int pleo_sound_type::pleo_sound_convert_adpcm2pcm(
  unsigned char *srcdata, int srclen,
  unsigned char **destdata, int *destlen, int dest_samplesize,
  int flags)
{
  int output_value=0;

  int stepsize_index = 0;
  int stepsize = g_adpcm_stepsize_table[stepsize_index];
  bool srcbuf_increment = true;

  int newbuf_maxsize = (srclen*dest_samplesize*2)+4096;
  unsigned char *newbuf = (unsigned char *)malloc(newbuf_maxsize);
  if (newbuf==NULL) return -1;
  int newbuf_datalen = 0;

  int value,vpdiff,delta,sign;
  for (int i=0; i<srclen*dest_samplesize*2; i += dest_samplesize) {
    if (srcbuf_increment) {
      value = *(srcdata++);
      delta = value>>4;
    }
    else delta = value;
    srcbuf_increment = !srcbuf_increment;

    // Get difference & sign...
    sign = delta & 8;
    delta &= 7;

    // Compute: vpdiff = (delta+0.5)*step/4;
    vpdiff = stepsize >> 3;
    if ( delta & 4 ) vpdiff += stepsize;
    if ( delta & 2 ) vpdiff += stepsize>>1;
    if ( delta & 1 ) vpdiff += stepsize>>2;

    // Update & clamp previous value...
    if (sign) output_value -= vpdiff; else output_value += vpdiff;
    if (output_value > 32767) output_value = 32767; else if (output_value <-32768) output_value = -32768;

    // Update step index & value...
    stepsize_index += g_adpcm_index_table[delta];
    if (stepsize_index < 0) stepsize_index = 0;
    if (stepsize_index > MAX_STEPSIZE) stepsize_index = MAX_STEPSIZE;
    stepsize = g_adpcm_stepsize_table[stepsize_index];

    // Output value...
    if ((newbuf_datalen+dest_samplesize) >= newbuf_maxsize) {
      newbuf_maxsize *= 2;
      unsigned char *tempbuf = (unsigned char *)xrealloc(newbuf,newbuf_maxsize);
      if (tempbuf==NULL) {free(newbuf); return -1;}
      newbuf = tempbuf;
    }

    switch (dest_samplesize) {
      case 1 : {
        int temp = output_value+128;
        if (temp<0) temp=0;
        if (temp>255) temp=255;
        newbuf[newbuf_datalen++] = temp;
        break;
      }

      case 2 :
        *((short*)(&newbuf[newbuf_datalen])) = output_value;
        newbuf_datalen += 2;
        break;

      case 4 :
        *((int*)(&newbuf[newbuf_datalen])) = output_value<<16;
        newbuf_datalen += 4;
        break;
    }
  }

  // Return results...
  if (destdata) *destdata = newbuf;
  if (destlen) *destlen = newbuf_datalen;
  return newbuf_datalen;
}



///////////////////////////////////////////////////////////////////////////////////
//
// Read PLEO USF format WAVE memory image.  If result negative, error occured.
//
int pleo_sound_type::read_pleo_usf_wave_image (const char *msg, unsigned char *imagedata, int imagelen, int flags)
{
  int rdofs=0;
  pleo_usf_sound_hdrtype *usfhdr = (pleo_usf_sound_hdrtype *)imagedata;
  rdofs += sizeof(pleo_usf_sound_hdrtype);

  if (imagelen < sizeof(pleo_usf_sound_hdrtype)) 
    return SOUNDBASE_ERROR_BADIMAGE;

  if (strncmp(usfhdr->signature,"UGSF",4)!=0)
    return SOUNDBASE_ERROR_BADIMAGE;

  pleo_usf_sound_infotype *usfinfo;
  switch (usfhdr->version) {
    case 1 : 
      memcpy (m_original_name,&imagedata[rdofs],USF_NAME_MAXLEN);
      rdofs += USF_NAME_MAXLEN;
      // fall-through...

    case 0 : 
      usfinfo = (pleo_usf_sound_infotype*)(&imagedata[rdofs]); 
      rdofs += sizeof(pleo_usf_sound_infotype);
      break;

    default :
      return SOUNDBASE_ERROR_BADIMAGE;
  }

  bool pcm_wavedata_dynamic = false;
  unsigned char *pcm_wavedata = &imagedata[rdofs];
  int pcm_wavedata_len = imagelen-rdofs;
  if (pcm_wavedata_len<0)
    return SOUNDBASE_ERROR_BADIMAGE;

  // Convert ADPCM to standard PCM...
  if (usfinfo->adpcm) {
    unsigned char *newbuf=NULL;
    int newbuf_len=0;
    if (pleo_sound_convert_adpcm2pcm (pcm_wavedata, pcm_wavedata_len, &newbuf, &newbuf_len, 1)<0)
      return SOUNDBASE_ERROR_INCOMPATIBLE;
    pcm_wavedata = newbuf;
    pcm_wavedata_len = newbuf_len;
    pcm_wavedata_dynamic = true;
  }

  // Init wave format info...
  m_wave_format.format_tag = 1;
  m_wave_format.channels = usfinfo->num_channels;
  m_wave_format.samples_per_sec = usfinfo->samples_per_sec;
  m_wave_format.average_bytes_per_sec = usfinfo->samples_per_sec;
  m_wave_format.block_alignment = 1;
  m_wave_format.bits_per_sample = usfinfo->bits_per_sample;

  // Allocate memory...
  if (usfinfo->num_samples == 0xFFFFFFFF)
    m_wave_datalen = pcm_wavedata_len;
  else m_wave_datalen = ((usfinfo->num_samples)*(usfinfo->bits_per_sample))/8;
  m_wave_maxdatalen = m_wave_datalen+32768; // allocate a little extra room for user editing

  m_wave_data = (unsigned char*)malloc(m_wave_maxdatalen);
  if (m_wave_data==NULL) {
    if (pcm_wavedata_dynamic) free(pcm_wavedata);
    return SOUNDBASE_ERROR_TOOBIG;
  }

  // Copy wave data...
  memcpy (m_wave_data, pcm_wavedata, pcm_wavedata_len);

  // If not single channel 8-bit 11Khz wave data, prompt user & then convert it...
  bool converted = false;
  if ((usfinfo->num_channels != 1) ||
      (usfinfo->samples_per_sec != PLEO_WAVE_SAMPLERATE) ||
      (usfinfo->bits_per_sample != 8))
  {
    // Convert Wave file...
    if (!convert_sound_data()) {
      init_wave();
      if (pcm_wavedata_dynamic) free(pcm_wavedata);
      return SOUNDBASE_ERROR_INCOMPATIBLE;
    }

    converted = true;
  }

  if (pcm_wavedata_dynamic) free(pcm_wavedata);
  return (converted) ? SOUNDBASE_FILE_CONVERTED : SOUNDBASE_FILE_OK;
}



///////////////////////////////////////////////////////////////////////////////////
//
// Write PLEO USF format WAVE memory image.  Return NULL if error occurs...
//
unsigned char *pleo_sound_type::write_pleo_usf_wave_image (int *imagelen, int flags, int starttime, int stoptime)
{
  set_wave_datamaxlen(1);
  if (m_wave_datalen==0) {
    m_wave_data[0] = 0;
    m_wave_datalen=1;
  }
  
  // Sanity...
  if ((m_wave_data==NULL) || (starttime>=m_wave_datalen)) 
    return NULL;

  // If starttime/stoptime equal, copy entire wave...
  if (stoptime<=starttime) {
    if (starttime<0) starttime=0;
    stoptime = m_wave_datalen;
  }

  int datalen = stoptime-starttime;
  if (datalen<1) return NULL;
  if (datalen%1) datalen++; // make sure not odd # of bytes (per RIFF Wave spec)

  // Compute overall length of wave image...
  int templen = 
    sizeof(pleo_usf_sound_hdrtype)+    // USF Header
    USF_NAME_MAXLEN+                   // Name...
    sizeof(pleo_usf_sound_infotype)+   // Info block...
    datalen;                           // Data chunk

  unsigned char *imagedata = (unsigned char *)malloc(templen);
  memset (imagedata, 128, templen);
  int wrofs=0;

  pleo_usf_sound_hdrtype *hdr = (pleo_usf_sound_hdrtype*)imagedata;
  strncpy (hdr->signature,"UGSF",4);
  hdr->version = 1; // 32 byte name field immediately after header
  wrofs += sizeof(pleo_usf_sound_hdrtype);

  // USF name field...  
  if (hdr->version == 1) {
    char temp[2*USF_NAME_MAXLEN];
    memset (temp,0,sizeof(temp));
    strncpy (temp, m_original_name, USF_NAME_MAXLEN);
    memcpy (&imagedata[wrofs], temp, USF_NAME_MAXLEN);
    wrofs += USF_NAME_MAXLEN;
  }

  // USF info...  
  pleo_usf_sound_infotype *info = (pleo_usf_sound_infotype*)(&imagedata[wrofs]);
  info->adpcm = 0;
  info->bits_per_sample = (unsigned char)m_wave_format.bits_per_sample;
  info->loop_count = 1;
  info->num_channels = (unsigned char)m_wave_format.channels;
  info->num_samples = 0xFFFFFFFF;
  info->samples_per_sec = (unsigned short)m_wave_format.samples_per_sec;
  wrofs += sizeof(pleo_usf_sound_infotype);
  int data_wrofs = wrofs;

  // Insert silence beforehand?
  int remaining_time = datalen;
  if (starttime<0) {
    int fill_time = -starttime;
    if (fill_time>remaining_time) fill_time = remaining_time;
    if (fill_time>0) {
      memset (&imagedata[wrofs], 128, fill_time);
      wrofs += fill_time; 
      remaining_time -= fill_time;
    }
    starttime=0;
  }

  // Copy data chunk...
  if (remaining_time>0) {
    int fill_time = m_wave_datalen-starttime;
    if (fill_time>remaining_time) fill_time = remaining_time;
    if (fill_time>0) {
      memcpy (&imagedata[wrofs], &m_wave_data[starttime], fill_time);
      wrofs += fill_time; 
      remaining_time -= fill_time;
    }
  }

  // Pad with silence afterwards?
  if (remaining_time>0) {
    memset (&imagedata[wrofs], 128, remaining_time);
    wrofs += remaining_time; 
    remaining_time -= remaining_time;
  }

  // Convert to ADPCM...
  if ((flags & SOUNDBASE_PLEO_FORCE_PCM)==0) {
    unsigned char *adpcm_imagedata=NULL;
    int adpcm_imagelen = 0;
    if (pleo_sound_convert_pcm2adpcm(&imagedata[data_wrofs], wrofs-data_wrofs, m_wave_format.bits_per_sample/8, &adpcm_imagedata, &adpcm_imagelen)>0) {
      if (adpcm_imagelen<(wrofs-data_wrofs)) {
        memcpy (&imagedata[data_wrofs], adpcm_imagedata, adpcm_imagelen);
        wrofs = data_wrofs + adpcm_imagelen;
        info->adpcm = 1;
      }
      free (adpcm_imagedata);
    }
  }

  if (imagelen != NULL) *imagelen = wrofs;
  return (imagedata); 
}



///////////////////////////////////////////////////////////////////////////////////
//
// Write PLEO USF format sound file.  Return NULL if error occurs...
//
bool pleo_sound_type::write_pleo_usf_wave_file (const char *targetfile, int flags)
{
  int imagelen;
  unsigned char *imagedata = write_pleo_usf_wave_image(&imagelen,flags);
  if (imagedata==NULL) return false;

  if (m_original_name[0]==0) {
    memset (m_original_name,0,sizeof(m_original_name));
    strncpy (m_original_name, targetfile, USF_NAME_MAXLEN);
  }

  FILE *fileid = fopen (targetfile,"wb");
  if (fileid != NULL) {
    int writelen = fwrite (imagedata,1,imagelen,fileid);
    free(imagedata);
    fclose (fileid);
    return ((ferror(fileid)==0) && (writelen==imagelen));
  }
  return false;
}



///////////////////////////////////////////////////////////////////////////////////
//
// Read WAVE format memory image.  Autodetect PLEO format file.   
// Return positive value on success.  Negative value on error...
//
int pleo_sound_type::read_wave_image (const char *msg, unsigned char *imagedata, int imagelen, int flags)
{
  // Sanity...
  if ((imagedata==NULL) || (imagelen<(sizeof(riff_header_type)+2*sizeof(wave_chunk_type))))
    return SOUNDBASE_NULL_FILE;

  // Pleo USF sound file?
  if (memcmp((char*)imagedata,"UGSF",4)==0) 
    return read_pleo_usf_wave_image (msg,imagedata,imagelen,flags);
  else return sound_base_type::read_wave_image(msg,imagedata,imagelen,flags);
}



///////////////////////////////////////////////////////////////////////////////////
//
// Write WAVE format file, or PLEO compatible USF file (controlled by flags).
// Returns pointer to data & sets integer at imagelen with length.
// Returns NULL on error.
//
bool pleo_sound_type::write_wave_file(const char *targetfile, int flags)
{
  if (flags & SOUNDBASE_PLEO_USF_FORMAT)
    return write_pleo_usf_wave_file(targetfile,flags);
  else return sound_base_type::write_wave_file(targetfile,flags);
}

