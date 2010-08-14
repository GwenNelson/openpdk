//
// Sound.cpp: Base class for dealing with wave sound files...
//
#include <stdio.h>
#include <stdlib.h>
#include "sound.h"


sound_base_type::sound_base_type()
{
  m_wave_data = NULL;
  init_wave();
}


sound_base_type::sound_base_type(sound_base_type &target, int starttime, int stoptime)
{
  m_wave_data = NULL;
  init_wave();

  // Sanity...
  if (starttime<0) starttime=0;
  if (stoptime<starttime) {
    starttime=0;
    stoptime=target.m_wave_datalen;
  }
  // Init buffer...
  int fill_len = stoptime-starttime;
  set_wave_datalen (fill_len);
  m_wave_format = target.m_wave_format;
  // Copy requested portion of data over...
  unsigned char *fill_ptr = m_wave_data;
  int target_len = target.m_wave_datalen - starttime;
  if (target_len>0) {
    memcpy (fill_ptr, &(target.m_wave_data[starttime]), target_len);
    fill_ptr += target_len;
    fill_len -= target_len;
  }
  // If allocation larger than available data, zero it...
  if (fill_len>0) memset (fill_ptr, 128, fill_len);
}


sound_base_type::~sound_base_type()
{
  init_wave();
}


void sound_base_type::init_wave()
{
  memset (m_original_name,0,sizeof(m_original_name));

  if (m_wave_data != NULL) free(m_wave_data);
  m_wave_data = NULL;
  m_wave_datalen = 0;
  m_wave_maxdatalen = 0;

  m_wave_format.format_tag = 1;
  m_wave_format.channels = 1;
  m_wave_format.samples_per_sec = PLEO_WAVE_SAMPLERATE;
  m_wave_format.average_bytes_per_sec = PLEO_WAVE_SAMPLERATE;
  m_wave_format.block_alignment = 1;
  m_wave_format.bits_per_sample = 8;
}


void sound_base_type::set_wave_datamaxlen(int datalen)
{
  if (datalen>m_wave_maxdatalen) {
    m_wave_maxdatalen = datalen;
    unsigned char *new_wavedata = (unsigned char*)xrealloc(m_wave_data,m_wave_maxdatalen);
    if (new_wavedata != NULL) m_wave_data = new_wavedata;
  }
}


#define WAVE_DATA_MARGIN 32768
void sound_base_type::set_wave_datalen(int datalen)
{
  if (m_wave_data==NULL) m_wave_maxdatalen=0;
  if (datalen > m_wave_maxdatalen)
    set_wave_datamaxlen (datalen+WAVE_DATA_MARGIN);
  m_wave_datalen = datalen;
}



///////////////////////////////////////////////////////////////////////////////////
//
// Read WAVE format file.
// Return positive value on success.  Negative value on error...
//
int sound_base_type::read_wave_file (const char * targetfile, int flags)
{
  init_wave();

  FILE *fileid = fopen(targetfile,"rb");
  if (fileid==NULL) 
    return SOUNDBASE_ERROR_FILENOTFOUND;

  if (fseek(fileid,0,SEEK_END) != 0) {
    fclose (fileid);
    return SOUNDBASE_ERROR_FILENOTFOUND;
  }

  int datalen = ftell(fileid);
  if (datalen<sizeof(int)) { // must be at least a signature...
    fclose (fileid);
    return SOUNDBASE_ERROR_FILENOTFOUND;
  }

  if (fseek(fileid,0,SEEK_SET) != 0) {
    fclose (fileid);
    return SOUNDBASE_ERROR_FILENOTFOUND;
  }

  unsigned char *dataptr = (unsigned char *)xmalloc(datalen);
  if (dataptr==NULL) {
    fclose (fileid);
    return SOUNDBASE_ERROR_FILENOTFOUND;
  }

  int readlen = fread(dataptr, 1, datalen, fileid);
  fclose(fileid);
  if (readlen != datalen) {
    free(dataptr);
    return SOUNDBASE_ERROR_FILENOTFOUND;
  }

  int result = read_wave_image(targetfile, dataptr, datalen, flags);
  free(dataptr);
  return result;
}



///////////////////////////////////////////////////////////////////////////////////
//
// Read WAVE format memory image.
// Return positive value on success.  Negative value on error...
//
int sound_base_type::read_wave_image (const char *msg, unsigned char *imagedata, int imagelen, int flags)
{
  // Sanity...
  if ((imagedata==NULL) || (imagelen<(sizeof(riff_header_type)+2*sizeof(wave_chunk_type))))
    return SOUNDBASE_ERROR_NOIMAGE;

  // Read RIFF header...
  int rdofs=0;
  riff_header_type riff_header;
  memcpy (&riff_header, &imagedata[rdofs], sizeof(riff_header));
  rdofs += sizeof(riff_header);

  // RIFF wave file?
  if ((strncmp(riff_header.signature,"RIFF",4)!=0) || (strncmp(riff_header.rifftype,"WAVE",4)!=0))
    return SOUNDBASE_ERROR_BADIMAGE; // image bad

  // Get current file position for checking end of riff chunk...
  int waveblock_filepos = rdofs;

  bool got_format_chunk = false;
  bool got_fact_chunk = false;
  bool got_unknown_chunk = false;

  // Generate "original" name from msg...
  int i=0,j=0,extofs=-1;
  for (i=0; msg[i] && (i<sizeof(m_original_name)); i++) {
    if ((msg[i]=='\\') || (msg[i]==':')) {j=0; extofs=-1;}
    else {
      if (msg[i]=='.') extofs=j;
      if (j<sizeof(m_original_name)) m_original_name[j++] = msg[i];
    }
  }
  if (extofs>=0) m_original_name[extofs]=0;
  else m_original_name[j]=0;

  while (rdofs<imagelen) {
    wave_chunk_type chunk_header;
    memcpy (&chunk_header, &imagedata[rdofs], sizeof(chunk_header));
    rdofs += sizeof(chunk_header);

    // Get current file position...
    int filepos = rdofs;

    // Wave format block?
    if (strncmp(chunk_header.signature,"fmt ",4)==0) {
      if (got_format_chunk) return SOUNDBASE_ERROR_BADIMAGE; // more than one format block is bad...

      memcpy (&m_wave_format, &imagedata[rdofs], sizeof(m_wave_format));
      rdofs += sizeof(m_wave_format);

      if ((m_wave_format.format_tag != 1) || 
          (m_wave_format.channels<1) || (m_wave_format.channels>2) ||
          (m_wave_format.block_alignment<1) || (m_wave_format.block_alignment>4) ||
          ((m_wave_format.bits_per_sample != 8) && (m_wave_format.bits_per_sample != 16)))
      {
        return SOUNDBASE_ERROR_BADIMAGE;
      }

      got_format_chunk = true;
    }

    // Wave data block?
    else if (strncmp(chunk_header.signature,"data",4)==0) {
      if (!got_format_chunk)
        return SOUNDBASE_ERROR_BADIMAGE;

      m_wave_datalen = chunk_header.chunklen;
      m_wave_maxdatalen = m_wave_datalen+32768; // allocate a little extra room for user editing

      m_wave_data = (unsigned char*)xmalloc(m_wave_maxdatalen);
      if (m_wave_data==NULL) return SOUNDBASE_ERROR_TOOBIG;

      // Copy wave data...
      memcpy (m_wave_data, &imagedata[rdofs], m_wave_datalen);
      rdofs += m_wave_datalen;

      // If reading data section doesn't reach end of file, then there is some
      // unknown chunk following we ignor.  Setting the unknown chunk flag will
      // warn the user 
      if ((filepos+chunk_header.chunklen)>(waveblock_filepos+riff_header.chunklen))
        got_unknown_chunk = true;

      bool converted = false;

      // If not single channel 8-bit 11Khz wave data, prompt user & then convert it...
      if ((m_wave_format.channels != 1) ||
          (m_wave_format.samples_per_sec != PLEO_WAVE_SAMPLERATE) ||
          (m_wave_format.block_alignment != 1) ||
          (m_wave_format.bits_per_sample != 8))
      {
        // Convert Wave file...
        if (!convert_sound_data()) {
          init_wave();
      	  return SOUNDBASE_ERROR_INCOMPATIBLE;
        }

        converted = true;
      }

      // If 8-bit 11Khz wave file contains unknown chunks, warn user they are
      // being "converted" (ie: removed)...
      else if (got_unknown_chunk) 
        converted = true;

      return (converted) ? SOUNDBASE_FILE_CONVERTED : SOUNDBASE_FILE_OK;
    }

    // Tolerate "fact" & other blocks.  These arn't needed by Aibo & Skitter filters them.
    else if (strncmp(chunk_header.signature,"fact",4)==0)
      got_fact_chunk = true;
    else got_unknown_chunk = true;

    // If reach end of chunk without finding data block, then error...
    rdofs = filepos+chunk_header.chunklen;
    if (rdofs>=(waveblock_filepos+riff_header.chunklen)) 
      return SOUNDBASE_ERROR_BADIMAGE;
  }

  // If reach end of imagedata without finding wave "data" chunk, then error...
  return SOUNDBASE_ERROR_TOOSMALL;
}



///////////////////////////////////////////////////////////////////////////////////
//
// Write WAVE format file.  Return false if error occurs...
//
bool sound_base_type::write_wave_file (const char * targetfile, int flags)
{
  int imagelen;
  unsigned char *imagedata = write_wave_image(&imagelen,flags);
  if (imagedata==NULL) return false;

  FILE *fileid = fopen (targetfile,"wb");
  if (fileid != NULL) {
    int writelen = fwrite (imagedata,1,imagelen,fileid);
    free(imagedata);
    fclose (fileid);
    return ((ferror(fileid)==0) && (writelen==imagelen));
  }
  return (false);
}



///////////////////////////////////////////////////////////////////////////////////
//
// Create WAVE format file in memory...
//
// Note: negative start/stop times are supported -- this compels inserting silence
// for however much negative starttime was requested.
//
unsigned char *sound_base_type::write_wave_image (int *imagelen, int flags, int starttime, int stoptime)
{
  set_wave_datamaxlen(1);
  if (m_wave_datalen==0) {
    m_wave_data[0] = 128;
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
    sizeof(riff_header_type)+                           // RIFF header
    sizeof(wave_chunk_type)+sizeof(wave_format_type)+   // Format chunk
    sizeof(wave_chunk_type)+datalen;                    // Data chunk

  if ((flags & SOUNDBASE_FLAG_NEEDFACTCHUNK)!=0) 
    templen += sizeof(wave_chunk_type)+sizeof(datalen); // Fact chunk

  unsigned char *imagedata = (unsigned char *)xmalloc(templen);
  memset (imagedata, 128, templen);
  int wrofs=0;

  // Create RIFF header...
  riff_header_type riff_header;
  strncpy (riff_header.signature,"RIFF",4);
  strncpy (riff_header.rifftype,"WAVE",4);
  riff_header.chunklen = templen-sizeof(wave_chunk_type);
  memcpy (imagedata, &riff_header, sizeof(riff_header));
  wrofs += sizeof(riff_header);

  // Create format chunk header...
  wave_chunk_type fmt_chunk_header;
  strncpy (fmt_chunk_header.signature,"fmt ",4);
  fmt_chunk_header.chunklen = sizeof(wave_format_type);
  memcpy (&imagedata[wrofs], &fmt_chunk_header, sizeof(fmt_chunk_header));
  wrofs += sizeof(fmt_chunk_header);

  // Copy format chunk...
  memcpy (&imagedata[wrofs], &m_wave_format, sizeof(wave_format_type));
  wrofs += sizeof(wave_format_type);

  // Create fact chunk header..
  if ((flags & SOUNDBASE_FLAG_NEEDFACTCHUNK)!=0) {
    wave_chunk_type fact_chunk_header;
    strncpy (fact_chunk_header.signature,"fact",4);
    fact_chunk_header.chunklen = sizeof(datalen);
    memcpy (&imagedata[wrofs], &fact_chunk_header, sizeof(fact_chunk_header));
    wrofs += sizeof(fact_chunk_header);
    memcpy (&imagedata[wrofs], &datalen, sizeof(datalen));
    wrofs += sizeof(datalen);
  }

  // Create data chunk header...
  wave_chunk_type data_chunk_header;
  strncpy (data_chunk_header.signature,"data",4);
  data_chunk_header.chunklen = datalen;
  memcpy (&imagedata[wrofs], &data_chunk_header, sizeof(data_chunk_header));
  wrofs += sizeof(data_chunk_header);

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

  if (imagelen != NULL) *imagelen = wrofs;
  return (imagedata); 
}
