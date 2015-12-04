#include "../../../inc/slib/render/opengl.h"

#if defined(SLIB_RENDER_SUPPORT_OPENGL_WGL) && defined(SLIB_PLATFORM_IS_WIN32)

#include "../../../inc/slib/render/engine.h"
#include "../../../inc/slib/core/log.h"
#include "../../../inc/slib/core/thread.h"
#include "../../../inc/slib/core/platform_windows.h"

#pragma comment (lib, "opengl32.lib")

SLIB_RENDER_NAMESPACE_BEGIN
class _WGLRendererImpl : public Renderer
{
public:
	sl_bool m_flagRequestRender;
	Ref<RenderEngine> m_renderEngine;

	HGLRC m_context;

	HWND m_hWindow;
	HDC m_hDC;

	Ref<Thread> m_threadRender;

	_WGLRendererImpl()
	{
		m_context = sl_null;
		m_flagRequestRender = sl_true;
	}

	~_WGLRendererImpl()
	{
		release();
	}

	static Ref<_WGLRendererImpl> create(void* _windowHandle, const RendererParam& _param)
	{
		HWND hWnd = (HWND)_windowHandle;
		if (hWnd == 0) {
			return Ref<_WGLRendererImpl>::null();
		}

		HDC hDC = ::GetDC(hWnd);
		if (hDC) {

			RendererParam param = _param;

			PIXELFORMATDESCRIPTOR pfd;
			Base::zeroMemory(&pfd, sizeof(pfd));
			pfd.nSize = sizeof(pfd);
			pfd.nVersion = 1;
			pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
			pfd.iPixelType = PFD_TYPE_RGBA;
			pfd.cColorBits = param.nRedBits + param.nGreenBits + param.nBlueBits;
			pfd.cAlphaBits = param.nAlphaBits;
			pfd.cAccumBits = param.nAccumBits;
			pfd.cDepthBits = param.nDepthBits;
			pfd.cStencilBits = param.nStencilBits;
			pfd.iLayerType = PFD_MAIN_PLANE;

			int iPixelFormat = ::ChoosePixelFormat(hDC, &pfd);
			if (iPixelFormat) {

				if (::SetPixelFormat(hDC, iPixelFormat, &pfd)) {
					
					HGLRC context = wglCreateContext(hDC);
					if (context) {

						Ref<_WGLRendererImpl> ret = new _WGLRendererImpl();

						if (ret.isNotNull()) {
							ret->m_hWindow = hWnd;
							ret->m_hDC = hDC;
							ret->m_context = context;

							ret->m_threadRender = Thread::start(SLIB_CALLBACK_CLASS(_WGLRendererImpl, run, ret.getObject()));

							return ret;
						}

						wglDeleteContext(context);
					}
				}
			}
			::ReleaseDC(hWnd, hDC);
		}
		return Ref<_WGLRendererImpl>::null();
	}

	void release()
	{
		if (m_threadRender.isNotNull()) {
			m_threadRender->finishAndWait();
		}

		MutexLocker lock(getLocker());

		if (m_renderEngine.isNotNull()) {
			m_renderEngine->release();
			m_renderEngine.setNull();
		}

		if (m_context) {
			wglDeleteContext(m_context);
			::ReleaseDC(m_hWindow, m_hDC);
			m_context = sl_null;
		}
	}

	void run()
	{
		wglMakeCurrent(m_hDC, m_context);

		GL::loadEntries();

		TimeCounter timer;
		while (Thread::isNotStoppingCurrent()) {
			if (m_renderEngine.isNull()) {
				m_renderEngine = GL::createEngine();
				if (m_renderEngine.isNull()) {
					return;
				}
			}
			runStep();
			if (Thread::isNotStoppingCurrent()) {
				sl_uint64 t = timer.getEllapsedMilliseconds();
				if (t < 20) {
					Thread::sleep((sl_uint32)(20 - t));
				}
				timer.reset();
			}
		}
		wglMakeCurrent(NULL, NULL);
	}

	void runStep()
	{
		PtrLocker<IRenderCallback> callback(getCallback());
		if (callback.isNotNull()) {
			if (!(Windows::isWindowVisible(m_hWindow))) {
				return;
			}
			sl_bool flagUpdate = sl_false;
			if (isRenderingContinuously()) {
				flagUpdate = sl_true;
			} else {
				if (m_flagRequestRender) {
					flagUpdate = sl_true;
				}
			}
			m_flagRequestRender = sl_false;
			if (flagUpdate) {
				RECT rect;
				::GetClientRect(m_hWindow, &rect);
				if (rect.right != 0 && rect.bottom != 0) {
					m_renderEngine->setViewport(0, 0, rect.right, rect.bottom);
					callback->onFrame(m_renderEngine.get());
					SwapBuffers(m_hDC);
				}
			}
		}
	}

	void requestRender()
	{
		m_flagRequestRender = sl_true;
	}

};

Ref<Renderer> WGL::createRenderer(void* windowHandle, const RendererParam& param)
{
	Ref<_WGLRendererImpl> ret = _WGLRendererImpl::create(windowHandle, param);
	return Ref<Renderer>::from(ret);
}
SLIB_RENDER_NAMESPACE_END

#else

SLIB_RENDER_NAMESPACE_BEGIN
Ref<Renderer> WGL::createRenderer(void* windowHandle, const RendererParam& param)
{
	return Ref<Renderer>::null();
}
SLIB_RENDER_NAMESPACE_END

#endif