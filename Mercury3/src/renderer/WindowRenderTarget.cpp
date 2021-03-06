#include <WindowRenderTarget.h>
#include <HgUtils.h>
#include <HgCamera.h>
#include <MercuryWindow.h>
#include <RenderBackend.h>

WindowRenderTarget::WindowRenderTarget()
{

}

bool WindowRenderTarget::Init()
{
	ENGINE::StartWindowSystem(1920, 1080);

	auto window = MercuryWindow::GetCurrentWindow();

	int width = window->CurrentWidth();
	int height = window->CurrentHeight();

	float projection[16];

	m_windowViewport.width = width;
	m_windowViewport.height = height;

	double renderWidth = width;
	double renderHeight = height;
	double aspect = renderWidth / renderHeight;
	Perspective2(60, aspect, 0.1f, 100.0f, projection);
	m_projection.load(projection);
	//auto tmp = HgMath::mat4f::perspective(60*DEG_RAD, aspect, 0.1f, 100.0f);
	//tmp.store(projection);

	return true;
}

void WindowRenderTarget::Render(HgCamera* camera, RenderQueue* queue)
{
	Renderer::Render(m_windowViewport, camera->toViewMatrix(), m_projection, queue);
}