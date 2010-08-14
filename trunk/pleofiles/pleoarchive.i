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
%module pleoarchive

class pleo_archive_type
{
public:
  pleo_resource_type m_sounds;
  pleo_resource_type m_motions;
  pleo_resource_type m_commands;
  pleo_resource_type m_scripts;
  pleo_resource_type m_properties;

  pleo_archive_type();

  void init_archive();
  int get_resource_type (unsigned char *binfile);
  int read_archive_file (const char *targetfile, int flags=0);
  int read_archive_image (const char *targetfile, unsigned char *binfile, int binfilelen, int flags=0);
  int compute_archive_filesize (int flags=0);
  unsigned char *write_archive_image (const char *targetfile, int *binfilelen, int flags=0);
  int write_archive_file (const char *targetfile, int flags=0);
  unsigned int adler32(unsigned int adler, unsigned char *buf, unsigned int len);
};



