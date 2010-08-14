

#if defined _property_included
#endinput
#endif

#define _property_included

#if TARGET == TARGET_PLEO
#include "pleo/properties.inc"
#endif

enum prop_value {
prop_error = 0xFFFFFFFF,
};


native property_get({property_name,user_property_name}: property);

stock get({property_name,user_property_name}: property)
{ return property_get(property); }


native property_set({property_name,user_property_name}: property, value);

stock set({property_name,user_property_name}: property, value)
{ return property_set(property, value); }


native property_set_leak({property_name,user_property_name}: property, delta, interval, max, min);


native property_leak_enable(bool: enable_leaking);


native bool: property_load(const file_name[]);


native bool: property_save(const file_name[]);




native bool: property_add_report({property_name,user_property_name}: property, min_change, trigger);


native property_enable_reporting(bool: enable);


forward public on_property(time, property_name: property, value);
