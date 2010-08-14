

#if defined _log_included
#endinput
#endif

#define _log_included

#include "common/message_type.inc"

native getchar();

native void: putchar(c);

native bool: haschar();


native void: print(const string[]);

native void: printf(const string[], ...);


native void: Message(message_type: type, const string[], ...);

native void: Debug(const string[], ...);

native void: Info(const string[], ...);

native void: Warning(const string[], ...);

native void: Error(const string[], ...);

native void: log_enable(message_type: types);

native void: log_disable(message_type: types);

native bool: log_set_file(const file_name[]);

native void: log_flush();
