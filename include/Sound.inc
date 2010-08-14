
#if defined _sound_included
#endinput
#endif

#define _sound_included


enum Sound {
Sound_Any = 0,
Sound_Invalid = 0
};

native Sound: sound_play(sound_name: sound, bool: interrupt = false);


native Sound: sound_play_file(const string[]);


native bool: sound_stop({Sound,sound_name}: sound = Sound_Any);


native bool: sound_is_playing({Sound,sound_name}: sound = Sound_Any);


native sound_command(command_name: name, bool: interrupt = false);


native sound_get_volume();


native sound_set_volume(volume);


native sound_get_speed();


native sound_set_speed(speed);
