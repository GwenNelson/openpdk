


enum sensor_name
{
SENSOR_NONE          =    -1, 
SENSOR_MIN           =     0, 
SENSOR_BATTERY       =     2, 
SENSOR_IR            =     3, 
SENSOR_IR_ACTIVITY   =     4, 
SENSOR_TOUCH_FIRST   =     6, 
SENSOR_HEAD          =     6, 
SENSOR_CHIN          =     7, 
SENSOR_BACK          =     8, 
SENSOR_LEFT_LEG      =     9, 
SENSOR_RIGHT_LEG     =    10, 
SENSOR_LEFT_ARM      =    11, 
SENSOR_RIGHT_ARM     =    12, 
SENSOR_ARSE          =    13, 
SENSOR_TAIL          =    13, 
SENSOR_TOUCH_LAST    =    13, 

SENSOR_FOOT_FIRST    =    14, 
SENSOR_FRONT_LEFT    =    14, 
SENSOR_FRONT_RIGHT   =    15, 
SENSOR_BACK_LEFT     =    16, 
SENSOR_BACK_RIGHT    =    17, 
SENSOR_FOOT_LAST     =    17, 

SENSOR_CARD_DETECT   =    18, 
SENSOR_WRITE_PROTECT =    19, 
SENSOR_LEFT_LOUD     =    20, 
SENSOR_LIGHT         =    21, 
SENSOR_RIGHT_LOUD    =    22, 
SENSOR_OBJECT        =    23, 
SENSOR_MOUTH         =    24, 
SENSOR_SOUND_DIR     =    26, 
SENSOR_LIGHT_CHANGE  =    27, 
SENSOR_SOUND_LOUD    =    28, 
SENSOR_TILT          =    29, 
SENSOR_TERMINAL      =    30, 
SENSOR_POWER_DETECT  =    31, 
SENSOR_USB_DETECT    =    32, 
SENSOR_WAKEUP        =    33, 
SENSOR_BATTERY_TEMP  =    34, 
SENSOR_CHARGER_STATE =    35, 
SENSOR_SHAKE         =    36, 
SENSOR_SOUND_LOUD_CHANGE =37, 
SENSOR_BEACON        =    38, 
SENSOR_BATTERY_CURRENT =  39, 
SENSOR_PACKET        =    40, 
SENSOR_MAX,

SENSOR_DERIVED_MIN         = 100, 
SENSOR_EDGE_IN_FRONT       = 101, 
SENSOR_EDGE_ON_LEFT        = 102, 
SENSOR_EDGE_ON_RIGHT       = 103, 
SENSOR_OBJECT_IN_FRONT     = 104, 
SENSOR_OBJECT_ON_LEFT      = 105, 
SENSOR_OBJECT_ON_RIGHT     = 106, 
SENSOR_TOUCH_TAP           = 107, 
SENSOR_TOUCH_HOLD          = 108, 
SENSOR_TOUCH_RELEASE       = 109, 
SENSOR_TOUCH_PETTED        = 110, 
SENSOR_ABUSE               = 111, 
SENSOR_PICKED_UP           = 112, 
SENSOR_TRACKABLE_OBJECT    = 113, 
SENSOR_FACE_DETECTION      = 114, 
SENSOR_ROCKING             = 115, 
SENSOR_LIGHT_BRIGHT_CHANGE = 117, 
SENSOR_ANTICIPATED_CUE     = 118, 
SENSOR_EDGE                = 119, 
SENSOR_TOUCH_TAP_HOLD      = 120, 
SENSOR_SEQUENCE            = 121, 
SENSOR_JOINT_STUCK         = 122, 
SENSOR_JOINT_UNSTUCK       = 123, 
SENSOR_TIMER               = 124, 
SENSOR_MSG_RECEIVED        = 125, 
SENSOR_MSG_GONE            = 126, 
SENSOR_MILESTONE           = 127, 
SENSOR_PLOG                = 128, 

SENSOR_DERIVED_MAX                
};

enum derived_sensor_priority
{
D_SENSOR_PRIORITY_NONE    =  0,
D_SENSOR_PRIORITY_MIN     =  1,
D_SENSOR_PRIORITY_DEFAULT =  5,
D_SENSOR_PRIORITY_MAX     = 10
};

enum tilt_name
{
TILT_NONE       =     0,
TILT_MIN        =     1,
TILT_ON_FEET    =     1,
TILT_LEFT_SIDE  =     2,
TILT_RIGHT_SIDE =     3,
TILT_ON_NOSE    =     4,
TILT_ON_TAIL    =     5,
TILT_ON_BACK    =     6,
TILT_MAX
};

enum petted_name
{
PETTED_NONE            =  0,
PETTED_MIN             =  1,
PETTED_STOPPED         =  1,
PETTED_BACKARSE        =  2,
PETTED_ARSEBACK        =  3,
PETTED_HEADBACK        =  4,
PETTED_BACKHEAD        =  5,
PETTED_HEADBACKARSE    =  6,
PETTED_ARSEBACKHEAD    =  7,
PETTED_MAX
};

enum packet_type
{
PACKET_NONE    = 0,
PACKET_AUDIO   = 1 <<  0, 
PACKET_CAMERA  = 1 <<  2, 
PACKET_ECHO    = 1 <<  4, 
PACKET_IR      = 1 <<  8, 
PACKET_LOG     = 1 << 11, 
PACKET_MOUTH   = 1 << 12, 
PACKET_STAT    = 1 << 15, 
PACKET_SRAM    = 1 << 18, 
PACKET_TOUCH   = 1 << 19, 
PACKET_VERSION = 1 << 21, 
PACKET_ERROR   = 1 << 23, 
PACKET_EXT     = 1 << 25, 
PACKET_ALL     = 0x7FFFFFFF
} ;

enum sensor_config
{
sensor_config_none,

sensor_config_value,            
sensor_config_raw_value,        
sensor_config_enabled,          
sensor_config_trigger,          
sensor_config_aux_trigger,      
sensor_config_trigger_last,     
sensor_config_trigger_time,     
sensor_config_trigger_count,    

sensor_config_beacon_id,        

sensor_config_timer_interval,   

sensor_config_packet_filter,
};
