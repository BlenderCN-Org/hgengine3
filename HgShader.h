#pragma once

typedef struct HgShader_vtable{
	void(*load)(struct HgShader* shader);
	void(*destroy)(struct HgShader* shader);
	void(*enable)(struct HgShader* shader);
} HgShader_vtable;

typedef struct HgShader {
	HgShader_vtable* vptr;
} HgShader;

HgShader* HGShader_acquire(char* vert, char* frag);
void HGShader_release(HgShader* shader);

extern HgShader*(*_create_shader)(const char* vert, const char* frag);