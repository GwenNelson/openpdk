#ifndef _PLEOARCHIVE_H_
#define _PLEOARCHIVE_H_

#include <string.h>
#include <memory.h>
#include <time.h>
#include "resource_list.h"

#define PLEO_ARCHIVE_SIGNATURE      "UGRF"
#define PLEO_ARCHIVE_SIZE_SIGNATURE "SIZE"
#define PLEO_ARCHIVE_CRC_SIGNATURE  "ADLR"

#define PLEO_TOC_SOUND_SIGNATURE    "UGSF"
#define PLEO_TOC_MTN_SIGNATURE      "UGMF"
#define PLEO_TOC_COMMAND_SIGNATURE  "UGCF"
#define PLEO_TOC_SCRIPT_SIGNATURE   " AMX"
#define PLEO_TOC_PROPERTY_SIGNATURE "PROP"

#define PLEO_TOC_SOUND 0
#define PLEO_TOC_MTN 1
#define PLEO_TOC_COMMAND 2
#define PLEO_TOC_SCRIPT 3
#define PLEO_TOC_PROPERTY 4
#define MAX_RESOURCE_TYPES 5

#define PLEO_SOUND_BASE 0x1000
#define PLEO_MOTION_BASE 0x2000
#define PLEO_COMMAND_BASE 0x3000
#define PLEO_SCRIPT_BASE 0x4000
#define PLEO_PROPERTY_BASE 0x5000

#define MAX_RESOURCE_ENTRIES 0xFFF // maximum number of entries per resource type


//
// Common Pleo Archive file (*.urf) header...
//
#pragma pack (push,1)
struct pleo_archive_hdrtype {
  char m_signature[4];           // "UGRF"
  unsigned int m_format;         // PleoBuild always sets format 1.
  time_t m_buildtime;            // time
  unsigned int m_version;        // from UPF.  PleoBuild always sets 0.
};
#pragma pack (pop)


#pragma pack (push,1)
struct pleo_archive_toctype {
  char m_toc_signature[4];       // "UGSF", "UGMF", "UGCF", " AMX", or "PROP"
  unsigned int m_toc_offset;
};
#pragma pack (pop)

#pragma pack (push,1)
struct pleo_archive_toc_entrytype {
  unsigned int entry_ofs;
  unsigned int entry_len;
  char entry_name[32];
};
#pragma pack (pop)


#pragma pack (push,1)
struct pleo_archive_sizetype {
  char m_size_signature[4];      // "SIZE"
  unsigned int m_size;
};
#pragma pack (pop)


#pragma pack (push,1)
struct pleo_archive_crctype {
  char m_crc_signature[4];      // "ADLR"
  unsigned int m_crc;
};
#pragma pack (pop)



struct pleo_command_header_type {
 char m_signature[4];
 unsigned short m_version;
 char m_name[32];
 unsigned short m_fields_per_record;
 unsigned short m_num_records;
};


//
// Simple command builder type...
//
struct pleo_command_type : public pleo_command_header_type {
  unsigned short m_fields[3];
  pleo_command_type(const char *name, int resource_id) {
    memcpy (m_signature, PLEO_TOC_COMMAND_SIGNATURE, sizeof(m_signature));
    m_version = 4;
    memset (m_name,0,sizeof(m_name));
    strncpy (m_name,name,sizeof(m_name));
    m_fields_per_record = 1;
    m_num_records = 1;
    m_fields[0] = 0xD;
    m_fields[1] = 1;
    m_fields[2] = resource_id;    
  }
};



//
// Archive storage class...
//
class pleo_archive_type
{
public:
  pleo_resource_type m_sounds;
  pleo_resource_type m_motions;
  pleo_resource_type m_commands;
  pleo_resource_type m_scripts;
  pleo_resource_type m_properties;

  // Init reference info for processing table-of-content records...
  struct {
    pleo_resource_type *resource_list; 
    int binofs;
  } m_toc_refinfo[MAX_RESOURCE_TYPES];

  // Constructor...
  pleo_archive_type();

  // Operations...
  void init_archive();
  int get_resource_type (unsigned char *binfile);
  int read_archive_file (const char *targetfile, int flags=0);
  int read_archive_image (const char *targetfile, unsigned char *binfile, int binfilelen, int flags=0);
  int compute_archive_filesize (int flags=0);
  unsigned char *write_archive_image (const char *targetfile, int *binfilelen, int flags=0);
  int write_archive_file (const char *targetfile, int flags=0);
  unsigned int adler32(unsigned int adler, unsigned char *buf, unsigned int len);
};



#endif // _PLEOARCHIVE_H_
