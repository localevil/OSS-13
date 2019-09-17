#pragma once

#include <SFML/OpenGL.hpp>

#include "include/cef_render_handler.h"
#include "include/cef_client.h"
//#include "include/cef_base.h"

class RenderHandler : public CefRenderHandler {
public:
	RenderHandler(GLuint texture);

	bool initialized;

	void init(void);
	void draw(void);
	void reshape(int, int);

	void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect);

	void OnPaint(CefRefPtr<CefBrowser> browser,
		PaintElementType type,
		const RectList& dirtyRects,
		const void* buffer,
		int width,
		int height);

private:
	int w, h;
	GLuint vao, vbo, positionLoc, program, texture;
	//sf::RenderTexture *buffer;
	//sf::Texture texture;
	//sf::Shader shader;

	//GLuint compileShader(GLenum, const char*);

	IMPLEMENT_REFCOUNTING(RenderHandler);

};

class BrowserClient : public CefClient {
public:
	BrowserClient(RenderHandler*);

	virtual CefRefPtr<CefRenderHandler> GetRenderHandler();

private:
	CefRefPtr<CefRenderHandler> handler;

	IMPLEMENT_REFCOUNTING(BrowserClient);

};

class Cefgui;

Cefgui* initCefgui(GLuint texture);

class Cefgui {
public:
	Cefgui(GLuint texture);

	void load(const char*);
	void draw(void);
	void reshape(int, int);

	void mouseMove(int, int);
	void mouseClick(int, int);
	void keyPress(int);

	//void executeJS(const char*);
	void output(const char*);

private:
	int mouseX, mouseY;

	CefRefPtr<CefBrowser> browser;
	CefRefPtr<BrowserClient> client;

	RenderHandler* renderHandler;

};

Cefgui *CefCheck(GLuint texture);