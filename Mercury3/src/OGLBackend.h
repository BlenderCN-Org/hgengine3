#pragma once

#include <RenderBackend.h>

class OGLBackend : public RenderBackend {
public:
	static void Init();

	void Clear();
	void BeginFrame();
	void EndFrame();
};