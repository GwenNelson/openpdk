

#if defined _animation_included
#endinput
#endif

#define _animation_included


native Command: command_exec(command_name: command, loops=0);

stock Command: command_play(command_name: command, loops=0)
{ return command_exec(command, loops); }


native bool: command_is_playing({Command,command_name}: command);


native Command: command_add(command_name: command, loops=0);


native bool: command_remove({Command,command_name}: command);
