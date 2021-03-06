#version 420
#extension GL_ARB_explicit_uniform_location : enable

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec4 normal;
layout(location = 3) in vec4 v_color;

//uniforms have their own locations separate from attributes
layout(location = 1) uniform vec4 rotation;
layout(location = 3) uniform vec4 translation;
layout(location = 5) uniform mat4 projection;

layout(location = 6) uniform vec4 cam_rot;
layout(location = 7) uniform vec3 cam_position;

out vec4 color;
out vec3 frag_normal;
//out vec3 eye_vector;
out vec3 position;
//out vec3 eye_pos;

vec3 point_quat_rotate(vec3 v, vec4 quaternion)
{ 
  return v + 2.0 * cross(quaternion.xyz, cross(quaternion.xyz, v) + quaternion.w * v);
}

void main() {
	//roatate around vector first if rotation needs to be around an arbitrary vector
	vec3 trans = translation.xyz;
	float scale = translation.w;
	vec3 v1 = point_quat_rotate(vertex*scale,rotation) + trans;
	vec4 v2 = vec4(point_quat_rotate(v1 - cam_position,cam_rot), 1.0);

	vec3 global_normal = point_quat_rotate(normal.xyz,rotation);
	frag_normal = normalize(global_normal);
//	eye_vector = (cam_position - v1.xyz);

	position = v1;
	gl_Position = projection * v2;
	color = vec4((normal.xyz+1)/2,1.0);
//	eye_pos = cam_position;
}


