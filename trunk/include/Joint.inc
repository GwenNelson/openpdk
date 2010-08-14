

#if defined _joint_included
#endinput
#endif

#define _joint_included



#if TARGET == TARGET_PLEO
#include "pleo/joints.inc"
#endif

enum angle_type {
angle_degrees,
angle_vr,
angle_mu
};

native joint_move_to(joint_name: joint, angle, speed = 128, angle_type: type = angle_degrees);

native joint_get_position(joint_name: joint, angle_type: type = angle_degrees);


enum joint_attribute {
ja_none,    
ja_status, 
ja_position,  
ja_pwm,     
ja_load,  
ja_speed,    
ja_time,    
ja_setpoint,  
ja_deadband, 
ja_threshold,
};

native joint_get_attribute(joint_name: joint, joint_attribute: type);

native joint_set_attribute(joint_name: joint, joint_attribute: type, value);

stock joint_status: joint_get_status(joint_name: joint)
{ return joint_get_attribute(joint, ja_status); }

native joint_get_min(joint_name: name, angle_type: type);

native joint_set_min(joint_name: joint, angle, angle_type: type);

native joint_get_max(joint_name: joint, angle_type: type);

native joint_set_max(joint_name: joint, angle, angle_type: type);

native joint_get_neutral(joint_name: joint, angle_type: type);

native joint_set_neutral(joint_name: joint, angle, angle_type: type);

native void: joint_set_offset(joint_name: name, angle);

native joint_get_offset(joint_name: name);

native void: joint_control(joint_name: name, who);

native joint_get_name(joint_name: joint, name[], length = sizeof name);


native bool: joint_calibrate(joint_name: joint, bool:reverse_order = false, pwm_min_vr = 0, pwm_max_vr = 0, bool:user_adjust = true);

native joint_calibrate_reset(joint_name: joint);

native bool: joint_set_angle_limits(joint_name: joint, min_angle, neutral, max_angle);


native joint_set_stall_detect_enable(bool: enable);

native bool: joint_get_stall_detect_enable();

native bool: joint_is_moving(joint_name: joint);
