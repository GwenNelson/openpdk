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
#ifndef _PLEOSOUND_H_
#define _PLEOSOUND_H_

#include "sound.h"

#define USF_NAME_MAXLEN 32 // name field is 32 bytes (if present)
#define SOUNDBASE_PLEO_USF_FORMAT 0x00000100
#define SOUNDBASE_PLEO_FORCE_PCM  0x00000200


#pragma pack (push,1)
  struct pleo_usf_sound_hdrtype {
    char signature[4];
    unsigned char version;  // 0=name not given.  1=32-byte name field present
  };

  struct pleo_usf_sound_hdr1type {
    char signature[4];
    unsigned char version;  // 0=name not given.  1=32-byte name field present
    char sound_name[32];
  };

  struct pleo_usf_sound_infotype {
    unsigned char adpcm;
    unsigned char bits_per_sample;
    unsigned char num_channels;
    unsigned short int samples_per_sec;
    unsigned short int loop_count;
    unsigned int num_samples; // 0xFFFFFFFF = entire file
  };
#pragma pack (pop)



class pleo_sound_type : public sound_base_type
{
public:
  pleo_sound_type(){};
  pleo_sound_type(sound_base_type &target, int starttime=-1, int stoptime=-1) : sound_base_type(target,starttime,stoptime){};
  virtual ~pleo_sound_type() {init_wave();};

  // Operations...
  int pleo_sound_convert_pcm2adpcm(
    unsigned char *srcdata, int srclen, int src_samplesize,
    unsigned char **destdata, int *destlen,
    int flags=0);

  int pleo_sound_convert_adpcm2pcm(
    unsigned char *srcdata, int srclen,
    unsigned char **destdata, int *destlen, int dest_samplesize,
    int flags=0);

  virtual int read_pleo_usf_wave_image (const char *msg, unsigned char *imagedata, int imagelen, int flags=0);
  virtual unsigned char *write_pleo_usf_wave_image (int *imagelen, int flags=0, int starttime=-1, int stoptime=-1);
  virtual bool write_pleo_usf_wave_file (const char *targetfile, int flags=0);

  // Overrides...
  virtual int read_wave_image (const char *msg, unsigned char *imagedata, int imagelen, int flags=SOUNDBASE_FLAG_NONE);
  virtual bool write_wave_file (const char *targetfile, int flags=SOUNDBASE_FLAG_NONE);
};

#endif // _PLEOSOUND_H_