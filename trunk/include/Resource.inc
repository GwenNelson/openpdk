

#if defined _resource_included
#endinput
#endif

#define _resource_included


enum resource_id {
SOUND_BASE    = 0x1000,
MOTION_BASE   = 0x2000,
COMMAND_BASE  = 0x3000,
SCRIPT_BASE   = 0x4000,
PROPERTY_BASE = 0x5000
};

enum resource_type {
res_sound,
res_motion,
res_command,
res_script,
res_property
};


native resource_get_id(const name[]);

native resource_get_name(id, name[], bool: packed = false, length = sizeof name);

native bool: resource_verify();

native resource_file_get_version(const name[], override = 0);


