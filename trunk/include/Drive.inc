

#if defined _drive_included
#endinput
#endif

#define _drive_included

native drive_add(const drive_name[], tvl, tvh, interval);

native set_trigger_values(const drive_name[], tvl, tvh);

#pragma deprecated set_trigger_values

stock drive_set_trigger_values(const drive_name[], tvl, tvh)
{ set_trigger_values(drive_name[], tvl, tvh); }

native drive_set_value(const drive_name[], value);


native behavior_add(behavior_id, const drive_name[], lead_time, upreg, samereg, downreg, interval);

native behavior_set_value(const drive_name[], behavior_id, value);
