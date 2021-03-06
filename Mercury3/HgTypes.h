#pragma once

#include <vertex.h>
#include <vertex3d.h>

typedef vertex3f point;
typedef vertex3f vector3;
typedef vertex3f vector3f;

#define L_VERTEX	0
#define L_NORMAL	1
#define L_UV		2
#define L_COLOR		3
#define L_TANGENT	4

//#ifdef __cplusplus
//extern "C" {
//#endif

//	typedef vertex point;
//	typedef vertex vector3;

	typedef struct color8 {
		color8() : 
			r(0),g(0),b(0),a(0)
		{}
		color8(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a) :
			r(_r), g(_g), b(_b), a(_a)
		{}
		uint8_t r, g, b, a;
	} color8;

	typedef struct color16 {
		color16() :
			r(0), g(0), b(0), a(0)
		{}
		color16(uint16_t _r, uint16_t _g, uint16_t _b, uint16_t _a) :
			r(_r), g(_g), b(_b), a(_a)
		{}
		uint16_t r, g, b, a;
	} color16;

	typedef struct uv_coord {
		uint16_t u, v;
	} uv_coord;

	typedef struct normal {
		float x, y, z;
	} normal;

	typedef struct tangent {
		float x, y, z, w;
	} tangent;

	typedef vector3f velocity;

//#ifdef __cplusplus
//}
//#endif