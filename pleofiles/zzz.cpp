//
// Demo application showing how:
//   Read urf
//   Dump wave sound files
//   Reload wave files updating the urf
//   Write urf
//
#include "stdafx.h"
#include "pleoarchive.h"
#include "pleosound.h"
#include <io.h>

typedef char strtype[1024];


// Class to to navigate files that match a spec with wildcards...
class CFileSpec : public _finddata_t {
public:
  long m_hfile;            // handle from findfirstpublic:
  CFileSpec() {m_hfile = -1;}

  ~CFileSpec() {
    if (m_hfile>=0) _findclose(m_hfile);
  }

  bool First(const char* filespec) {
    if (m_hfile>=0) _findclose(m_hfile);
    return (m_hfile=_findfirst((char*)filespec, this)) != -1;      
  }

  bool Next() {
    return m_hfile>=0 && _findnext(m_hfile, this)==0;
  }
};



//
// zzz.cpp : Defines the entry point for the console application.
//
int main(int argc, char* argv[])
{
  strtype target_folder;
  if (argc<3) {
    printf ("zzz [urf-file] [dest_folder]\n");
    return 1;
  }

  /* Read urf resource archive... */
  pleo_archive_type urf;
  if (urf.read_archive_file(argv[1])<0) {
    printf ("%s not found.\n",argv[1]);
    return 2;
  }

  /* Dump urf sound files to a folder... */
  strcpy (target_folder,argv[2]);
  if (target_folder[strlen(target_folder)-1] != '\\') strcat(target_folder,"\\");

  /*
  // If want PLEO USF files, this is all it takes...
  if (!urf.m_sounds.write_all_element_files(target_folder,"usf")) {
    printf ("Error occured writing sound files.\n");
    return 3;
  }
  */
  // Otherwise, to create wave files however, do this...
  for (int i=0; i<urf.m_sounds.m_count; i++) {
    pleo_sound_type snd;
    if (snd.read_pleo_usf_wave_image(
      urf.m_sounds.m_resource_list[i].m_element_name,
      urf.m_sounds.m_resource_list[i].m_element,
      urf.m_sounds.m_resource_list[i].m_element_size)<0) 
    {
      printf ("Error importing sound '%s'.\n", urf.m_sounds.m_resource_list[i].m_element_name);
    }
    else {
      strtype target_file;
      strcpy (target_file, target_folder);
      strcat (target_file, urf.m_sounds.m_resource_list[i].m_element_name);
      strcat (target_file, ".wav");
      if (!snd.write_wave_file(target_file))
        printf ("Error writing wave '%s'.\n", target_file);
      else printf ("Exported wave '%s'\n", target_file);
    }
  }

  /* do your thing here... */
  /* etc... */
  /* etc... */
  /* etc... */

  /* Merge sound files back into urf by name... */
  CFileSpec spec;
  strtype pattern, temp_filename;
  strcpy (pattern,target_folder);
  strcat (pattern,"*.*");

  for (bool more=spec.First(pattern); more; more=spec.Next())
    if (!(spec.attrib&(_A_SUBDIR|_A_HIDDEN|_A_SYSTEM))) {
      strcpy (temp_filename,target_folder);
      strcat (temp_filename,spec.name);

      pleo_sound_type snd;
      if (snd.read_wave_file(temp_filename)<=0) // read wav or usf format
        printf ("Error reading file '%s'\n", spec.name);
      else {        
        int datalen=0;
        unsigned char *dataptr = snd.write_pleo_usf_wave_image(&datalen);
        if (dataptr == NULL)
          printf ("Error generating USF image for file '%s'\n", spec.name);
        else {
          printf ("Updating file '%s' (%s)...\n", spec.name, snd.m_original_name);
          urf.m_sounds.update_element(snd.m_original_name, dataptr, datalen, true);
        }
      }
    }

  /* Write urf resource archive... */
  strtype bakfile;
  strcpy (bakfile,argv[1]);
  strcat (bakfile,".bak");
  MoveFile (argv[1],bakfile);

  if (urf.write_archive_file(argv[1])<=0) {
    printf ("Error occured writing urf...\n");
    return 4;
  }

	return 0;
}

