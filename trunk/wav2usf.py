""" Converts a wave file into a USF file
"""

import struct
import audioop
import wave
import sys
import os

def convert_wave_data(f_rate,frame_count,sample_width,channels,data):
    """ Convert wave sample data into pleo format
    """
    if channels==2: data = audioop.tomono(data,sample_width,1,1)
    data = audioop.mul(data,sample_width,0.97999999999999998)
    data = audioop.ratecv(data,sample_width,1,f_rate,11025,None,4,4)[0]
    if sample_width==1:
       data = audioop.bias(data,1,-128)
       data = audioop.lin2lin(data,1,2)
    data = audioop.mul(data,2,(1.0/256))
    data = audioop.lin2adpcm(data,2,None)[0]
    return (11025,frame_count,sample_width,1,data)

def load_wavefile(filename):
    """ Load header and sample data from a .wav file
    """
    wavefile = wave.open(filename,'rb')
    f_rate       = wavefile.getframerate()
    frame_count  = wavefile.getnframes()
    sample_width = wavefile.getsampwidth()
    channels     = wavefile.getnchannels()
    data         = wavefile.readframes(frame_count)
    wavefile.close()
    return (f_rate,frame_count,sample_width,channels,data)

def save_usf(wave_data,filename,sound_name):
    """ Write out ugobe's freaky header with wave data
    """
    f_rate,frame_count,sample_width,channels,data = wave_data
    header = struct.pack('4sb32sbbbHHL','UGSF',1,str(sound_name),17,sample_width*8,1,f_rate,1,4294967295L)
    file = open(filename,'wb')
    file.write(header)
    file.write(data)
    file.close()

def convert_file(filename,sound_name=None):
    """ Top level convert function
    """
    wave_data  = load_wavefile(filename)
    wave_data  = convert_wave_data(*wave_data)
    if sound_name == None:
       sound_name = os.path.basename(filename).split('.')[0]
    save_usf(wave_data, ''.join([sound_name,'.usf']),sound_name)

if __name__ == '__main__':
   print 'Converting %s' % sys.argv[1]
   convert_file(sys.argv[1])
