

#if defined _attention_included
#endinput
#endif

#define _attention_included


@param  object_nv_lo_limit:  Minimum Neck Vertical angle for Object detection to occur, default = 0
@param  edge_nv_hi_limit:    Maximum Neck Vertical angle for Edge detection to occur, default = -40 degrees
@param  object_ir_hi_limit:  high limit for object ir reflection. if it goes over  this, object goes on, default 40
@param  object_ir_lo_limit:  low  limit for object ir reflection. if it goes under this, object goes off, default 20
@param  edge_ir_hi_limit:    high limit for edge ir reflection.   if it goes over  this, edge   goes off, default 40
@param  edge_ir_lo_limit:    low  limit for edge ir reflection.   if it goes under this, edge   goes on, default 20
@param  object_linger:       minimum time(ms) neck v needs to be above object_nv_low_limit for object to trigger default 500ms
@param  edge_linger:         minimum time(ms) neck v needs to be below edge_nv_hi_limit for object to trigger default 500ms
native void: set_object_edge_parms(
object_nv_lo_limit,
edge_nv_hi_limit,
object_ir_lo_limit,
object_ir_hi_limit,
edge_ir_lo_limit,
edge_ir_hi_limit,
object_linger,
edge_linger);


native bool: p2p_reset(from);

native bool: p2p_stop_msg();

native bool: p2p_set_msg(recipient, data1, data2, broadcast);

native bool: p2p_get_msg(from, &data1, &data2, &broadcast, &repeat_count, &total_count);

native p2p_get_id();
