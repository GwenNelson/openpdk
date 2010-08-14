

#if defined _file_included
#endinput
#endif

#define _file_included


enum filemode {
io_read = 0,     
io_write = 1,        
io_append = 3,      
};

enum seek_offset {
seek_set,              
seek_cur,             
seek_end            
};

enum device {
device_sd,     
device_df,       
};

native bool: device_change(device: dst);


native File: file_open(const file_name[], filemode: mode = io_read);

native file_gets(File: file, line[], bool: packed = false, size = sizeof line);

native file_puts(File: file, const line[]);

native file_read(File: file, line[], bool: packed = true, size );

native file_write(File: file, const line[], size );

native file_printf(File: file, const string[], ...);

native file_seek(File: file, offset, seek_offset: origin = seek_set);

native file_close(File: file, bool: flush = true);

native file_flush(File: file);

native file_get_size(File: file);


native Find: find_first(const pattern[], name[], bool: packed = true, len = sizeof name);

native bool: find_next(Find: handle, name[], bool: packed = true, len = sizeof name);


native bool: file_exists(const file_name[]);

native bool: file_copy(const src[], const dst[]);

native bool: file_delete(const file_name[]);
