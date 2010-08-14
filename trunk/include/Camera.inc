

#if defined _camera_included
#endinput
#endif

#define _camera_included

#define FOV_H_DEG 50
#define FOV_V_DEG 40


native void: camera_enable(bool: enable);

native void: camera_track_enable(bool: enable);

native void: camera_set_colormap(entry, rhi, rlo, ghi, glo, bhi, blo);

native void: camera_set_color_preset(color_map_preset: preset);

native void: camera_request_average(x1, y1, x2, y2);

native bool: camera_get_average(&y_g, &u_b, &v_r);


