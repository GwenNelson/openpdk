

#if defined _sensor_included
#endinput
#endif

#define _sensor_included


#if TARGET == TARGET_PLEO
#include "pleo/sensors.inc"
#endif


native sensor_read(sensor_name: sensor);

stock sensor_get_value(sensor_name: sensor)
{ return sensor_read(sensor); }


native sensor_read_raw(sensor_name: sensor);


native sensor_write(sensor_name: sensor, value);


native bool: sensor_is_triggered(sensor_name: sensor);


native sensor_reset(sensor_name: sensor);


native sensor_read_data(sensor_name: sensor, data[], length = sizeof data);


native sensor_data_size(sensor_name: sensor);


native sensor_write_data(sensor_name: sensor, const data[], length = sizeof data);


native sensor_get_name(sensor_name: sensor, name[], length = sizeof name);


native sensor_get_config(sensor_name: sensor, sensor_config: type);


native sensor_set_config(sensor_name: sensor, sensor_config: type, value);


stock sensor_enable(sensor_name: sensor)
{ sensor_set_config(sensor, sensor_config_enabled, true); }


stock sensor_disable(sensor_name: sensor)
{ sensor_set_config(sensor, sensor_config_enabled, false); }


stock sensor_set_trigger_level(sensor_name: sensor, value)
{ sensor_set_config(sensor, sensor_config_trigger, value); }

#pragma deprecated sensor_set_level

stock sensor_set_level(sensor_name: sensor, value)
{ sensor_set_config(sensor, sensor_config_trigger, value); }

stock sensor_time_since_last(sensor_name: sensor)
{ return sensor_get_config(sensor, sensor_config_trigger_last); }


stock sensor_get_trigger_time(sensor_name: sensor)
{ return sensor_get_config(sensor, sensor_config_trigger_time); }


stock sensor_get_trigger_count(sensor_name: sensor)
{ return sensor_get_config(sensor, sensor_config_trigger_count); }


native sensor_call_script(sensor_name: sensor, bool: call_script);


forward public on_sensor(time, sensor_name:sensor, value);



stock set_timer_interval(interval)
{ sensor_set_trigger_level(SENSOR_TIMER, interval); }

