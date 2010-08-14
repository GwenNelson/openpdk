

#if defined _script_included
#endinput
#endif

#define _script_included

#if TARGET == TARGET_PLEO
#include "pleo/properties.inc"
#endif


enum vm_name {
vm_none,		
vm_sensor,	
vm_main,
vm_aux,
vm_user
};


native vm_exec(const string[], vm_name:vm = vm_aux);

stock exec(const string[], vm_name:vm = vm_aux)
{ return vm_exec(string, vm); }


native vm_exec_id(script_name:script, vm_name:vm = vm_aux);

stock exec_id(script_name:script, vm_name:vm = vm_aux)
{ return vm_exec_id(script, vm); }


stock sensor_exec(const string[])
{ return vm_exec(string, vm_sensor); }


stock sensor_exec_id(script_name:script)
{ return vm_exec_id(script, vm_sensor); }


native Wait: wait_new();

native bool: wait_add_property(Wait:wait, property_name:property, value, compare_name:compare);


native void: vm_wait(Wait:wait, vm_name:vm = vm_aux);


native void: yield();


forward public init();


forward public main();


forward public close();
