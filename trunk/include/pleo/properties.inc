


enum property_name
{
property_none                =     0, 
property_min                 =     1, 
property_arousal             =     1, 
property_valence             =     2, 
property_stance              =     3, 
property_age                 =     4, 
property_damage              =     5, 
property_energy              =     6, 
property_health              =     7, 
property_mood                =     8, 
property_command             =     9, 
property_command_status      =    10, 
property_layer               =    11, 
property_probability         =    12, 
property_motion              =    13, 
property_command_pending     =    14, 
property_behavior_status     =    15, 
property_script_status       =    15, 
property_fatigue             =    16, 
property_direction           =    17, 
property_pose                =    18, 
property_sequence            =    19, 
property_platform            =    20, 
property_behavior            =    21, 
property_script              =    21, 
property_speed               =    22, 
property_neutral             =    23, 
property_pickedup            =    24, 
property_stand               =    25, 
property_liedown             =    26, 
property_trick_step          =    27, 
property_trick_id            =    28, 
property_sound               =    29, 
property_drive               =    30, 
property_motion_time         =    31, 
property_attn_track_weight   =    32, 
property_attn_hold_flags     =    33, 
property_cam_img_progress    =    34, 
property_attn_track_mindist  =    35, 
property_attn_track_move     =    36, 
property_attn_p2p_timeout    =    37, 
property_max,                         
property_limit               = 65535  
};


enum color_map_preset
{
color_map_none   = 0, 
color_map_red    = 1, 
color_map_green  = 2, 
color_map_blue   = 3, 
color_map_white  = 4, 
color_map_yellow = 5, 
color_map_auto   = 6, 
color_map_custom = 7, 
color_map_max         
};


enum compare_name
{
compare_none,           
compare_equal,          
compare_not_equal,      
compare_less_than,      
compare_greater_than    
};

enum config_name
{
config_access = -1,
config_none,
config_serial_is_usb,   
config_monitor_echo,    
config_log_color,       
config_df_free,         
};

enum version_name
{
version_none,
version_serial,           
version_arm0,             
version_arm1,             
version_motor0,           
version_motor1,           
version_motor2,           
version_motor3,           
version_hardware,         
version_board_id,         
version_app_df,           
version_app,              
version_boot,             
};
