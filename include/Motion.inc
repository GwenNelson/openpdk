

#if defined _motion_included
#endinput
#endif

#define _motion_included


enum Motion {
Motion_Any = 0xAA,
Motion_Invalid = 0xFF,
};


native Motion: motion_play(motion_name: name);


native Motion: motion_play_file(const file_name[]);


native bool: motion_pause({Motion,motion_name}: motion, bool: pause );


native bool: motion_stop({Motion,motion_name}: motion);


native bool: motion_is_playing({Motion,motion_name}: motion);


native motion_set_playback_speed(percent_of_normal);


native motion_get_playback_speed();
