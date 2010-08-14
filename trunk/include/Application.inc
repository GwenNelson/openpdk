

#if defined _application_included
#endinput
#endif

#define _application_included


native bool: app_load(const path[]);


native bool: app_unload(const path[], bool: power_down = false, power_down_time_ms = 0);


native bool: app_is_loaded();


enum app_config {
app_wait_for_init = 1,
app_auto_load = 2,
app_run_init = 3,
};

native app_get_config(app_config: type);


native app_set_config(app_config: type, value);
