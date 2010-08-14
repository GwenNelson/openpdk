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
#ifndef _SOUND_BASE_H_
#define _SOUND_BASE_H_

#include <malloc.h>
#include <memory.h>
#include <string.h>

#define swapdata(swaptype,a,b) {swaptype temp=a; a=b; b=temp;}

#ifndef M_PI
#define M_PI 3.1415926535897323846
#endif

#define PLEO_WAVE_SAMPLERATE 11025

// Wave file read/write control flags...
#define SOUNDBASE_FLAG_NONE            0x00000000
#define SOUNDBASE_FLAG_NEEDFACTCHUNK   0x00000001

// Sound errors...
#define SOUNDBASE_FILE_CONVERTED 2
#define SOUNDBASE_FILE_OK 1
#define SOUNDBASE_NULL_FILE 0
#define SOUNDBASE_ERROR_FILENOTFOUND -1
#define SOUNDBASE_ERROR_NOIMAGE -2
#define SOUNDBASE_ERROR_BADIMAGE -3
#define SOUNDBASE_ERROR_TOOBIG -4
#define SOUNDBASE_ERROR_INCOMPATIBLE -5
#define SOUNDBASE_ERROR_TOOSMALL -6


#ifndef _MISC_COMMON_H_
#define xmalloc malloc
#define xrealloc realloc
#define xfree free
#endif


// Wave file data structures...
#pragma pack (push,1)
struct riff_header_type {
  char signature[4];
  int chunklen;
  char rifftype[4];
};


struct wave_chunk_type {
  char signature[4];
  int chunklen;
};


struct wave_format_type {
  short          format_tag;
  unsigned short channels;
  unsigned long  samples_per_sec;
  unsigned long  average_bytes_per_sec;
  unsigned short block_alignment;
  unsigned short bits_per_sample;

  bool operator!=(const wave_format_type& target) const 
    {return (memcmp(this, &target, sizeof(wave_format_type))!=0);}
};
#pragma pack (pop)



//
// Wave data class...
//
class sound_base_type  
{
public: 
  // Wave data...
  wave_format_type m_wave_format;
  unsigned char *m_wave_data;
  int m_wave_maxdatalen;
  int m_wave_datalen;

  // Attributes...
  char m_original_name[256];

public:
  sound_base_type();
  sound_base_type(sound_base_type &target, int starttime=-1, int stoptime=-1);
  virtual ~sound_base_type();

  // Assignment operator...
  sound_base_type& operator=(const sound_base_type& target) {
    set_wave_datalen (target.m_wave_datalen);
    m_wave_format = target.m_wave_format;
    memcpy (m_wave_data, target.m_wave_data, m_wave_datalen);
    return *this;
  }

  // Compare operator...
  bool operator==(const sound_base_type& target) const {
    if (m_wave_datalen != target.m_wave_datalen) return false;
    if (m_wave_format != target.m_wave_format) return false;
    return (memcmp(m_wave_data, target.m_wave_data, m_wave_datalen)==0);
  }

  // Swap operation...
  void swap_wave (sound_base_type &target) {
    swapdata (int, m_wave_datalen, target.m_wave_datalen);
    swapdata (int, m_wave_maxdatalen, target.m_wave_maxdatalen);
    swapdata (wave_format_type, m_wave_format, target.m_wave_format);
    swapdata (unsigned char*, m_wave_data, target.m_wave_data);
  }

public:
  // Operations...
  virtual void init_wave();
  virtual void set_wave_datamaxlen(int datalen);
  virtual void set_wave_datalen(int datalen);

  virtual bool convert_sound_data() {return false;}
  virtual int read_wave_file (const char *targetfile, int flags=SOUNDBASE_FLAG_NONE);
  virtual int read_wave_image (const char *msg, unsigned char *imagedata, int imagelen, int flags=SOUNDBASE_FLAG_NONE);
  virtual bool write_wave_file (const char *targetfile, int flags=SOUNDBASE_FLAG_NONE);
  virtual unsigned char *write_wave_image (int *imagelen, int flags=SOUNDBASE_FLAG_NONE, int starttime=-1, int stoptime=-1);
};


#endif // _SOUND_BASE_H_
