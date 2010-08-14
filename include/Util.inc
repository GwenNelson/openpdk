

#if defined _util_included
#endinput
#endif

#define _util_included

#include <String.inc>
#include <Time.inc>

#if TARGET == TARGET_PLEO
#include "pleo/properties.inc"
#endif


native rand(x);

stock random(x)
{ return rand(x); }

native seed(x);

native abs(i);


native version_get(version_name: type, buffer[], bool: packed = true, size = sizeof buffer);

native version_set(version_name: type, const buffer[]);


native config_get(config_name: config);

native config_set(config_name: config, value);


native monitor_exec(const command[]);

