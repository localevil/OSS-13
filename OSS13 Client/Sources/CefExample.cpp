#include "CefExample.h"

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

//#include "include/cef_render_handler.h"
//#include "include/cef_client.h"
#include "include/cef_app.h"

#include <fstream>
#include <cstdio>

RenderHandler::RenderHandler(GLuint texture) : w(0), h(0), texture(texture)
{
	initialized = false;
	vao = 0; vbo = 0;
	program = 0; positionLoc = 0;
}

void RenderHandler::init(void)
{
	//GLuint vertexShader = compileShader(GL_VERTEX_SHADER, "shaders/gui.vert");
	//GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, "shaders/gui.frag");

	//program = glCreateProgram();

	//glAttachShader(program, vertexShader);
	//glAttachShader(program, fragmentShader);
	//glLinkProgram(program);
	//glDetachShader(program, vertexShader);
	//glDetachShader(program, fragmentShader);

	//positionLoc = glGetAttribLocation(program, "position");

	//float coords[] = { -1.0,-1.0,-1.0,1.0,1.0,-1.0,1.0,-1.0,-1.0,1.0,1.0,1.0 };

	//glGenVertexArrays(1, &vao);
	//glBindVertexArray(vao);
	//glGenBuffers(1, &vbo);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(coords), coords, GL_STATIC_DRAW);
	//glEnableVertexAttribArray(positionLoc);
	//glVertexAttribPointer(positionLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);

	initialized = true;
}

void RenderHandler::draw(void)
{
	//glUseProgram(program);
	//glBindVertexArray(vao);

	//glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//glDrawArrays(GL_TRIANGLES, 0, 6);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glBindVertexArray(0);
	//glUseProgram(0);
}

void RenderHandler::reshape(int w, int h)
{
	this->w = w;
	this->h = h;
}

void RenderHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
	rect = CefRect(0, 0, w, h);
}

void RenderHandler::OnPaint(CefRefPtr<CefBrowser> browser,
	PaintElementType type,
	const RectList& dirtyRects,
	const void* buffer,
	int width,
	int height)
{
	glBindTexture(GL_TEXTURE_2D, texture);
	//glEnable(GL_TEXTURE_2D);
	//sf::Texture::bind(&texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, (unsigned char*)buffer);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glBindTexture(GL_TEXTURE_2D, 0);
}

//GLuint RenderHandler::compileShader(GLenum shaderType, const char* path)
//{
//	GLuint shader = glCreateShader(shaderType);
//
//	std::ifstream ifs(path);
//	std::string shaderStr((std::istreambuf_iterator<char>(ifs)),
//		(std::istreambuf_iterator<char>()));
//
//	const char* shaderData = shaderStr.c_str();
//
//	glShaderSource(shader, 1, &shaderData, NULL);
//	glCompileShader(shader);
//
//	GLint status;
//	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
//	if (status == GL_FALSE)
//		printf("SHADER COMPILE ERROR\n");
//
//	return shader;
//}

BrowserClient::BrowserClient(RenderHandler* renderHandler)
{
	handler = renderHandler;
}

CefRefPtr<CefRenderHandler> BrowserClient::GetRenderHandler() {
	return handler;
};

Cefgui* initCefgui(GLuint texture)
{
	CefMainArgs args;
	CefExecuteProcess(args, nullptr, NULL);

	CefSettings settings;

	//// TODO make cross platform
	//CefString(&settings.locales_dir_path) = "cef/linux/lib/locales";
	CefString(&settings.locales_dir_path) =
		"C:\\Development\\cef_binary_76.1.9 + g2cf916e + chromium - 76.0.3809.87_windows64\\Resources\\locales";

	CefInitialize(args, settings, nullptr, NULL);

	return new Cefgui(texture);
}

Cefgui::Cefgui(GLuint texture) : mouseX(0), mouseY(0)
{
	CefWindowInfo windowInfo;
	CefBrowserSettings settings;

	//windowInfo.SetAsOffScreen(nullptr);
	//windowInfo.SetTransparentPainting(true);
	windowInfo.SetAsWindowless(0);

	renderHandler = new RenderHandler(texture);

	client = new BrowserClient(renderHandler);
	browser = CefBrowserHost::CreateBrowserSync(windowInfo, client.get(), "google.ru", settings, nullptr, nullptr);
}

void Cefgui::load(const char* url)
{
	if (!renderHandler->initialized)
		renderHandler->init();

	browser->GetMainFrame()->LoadURL(url);
}

void Cefgui::draw(void)
{
	CefDoMessageLoopWork();
	renderHandler->draw();
}

void Cefgui::reshape(int w, int h)
{
	renderHandler->reshape(w, h);
	browser->GetHost()->WasResized();
}

void Cefgui::mouseMove(int x, int y)
{
	mouseX = x;
	mouseY = y;

	CefMouseEvent event;
	event.x = x;
	event.y = y;

	browser->GetHost()->SendMouseMoveEvent(event, false);
}

void Cefgui::mouseClick(int btn, int state)
{
	CefMouseEvent event;
	event.x = mouseX;
	event.y = mouseY;

	bool mouseUp = state == 0;
	CefBrowserHost::MouseButtonType btnType = MBT_LEFT;
	browser->GetHost()->SendMouseClickEvent(event, btnType, mouseUp, 1);
}

void Cefgui::keyPress(int key)
{
	CefKeyEvent event;
	event.native_key_code = key;
	event.type = KEYEVENT_KEYDOWN;

	browser->GetHost()->SendKeyEvent(event);
}

//void Cefgui::executeJS(const char* command)
//{
//	CefRefPtr<CefFrame> frame = browser->GetMainFrame();
//	frame->ExecuteJavaScript(command, frame->GetURL(), 0);
//
//	// TODO limit frequency of texture updating
//	CefRect rect;
//	renderHandler->GetViewRect(browser, rect);
//	//browser->GetHost()->Invalidate(rect, PET_VIEW);
//}

void Cefgui::output(const char* command)
{
	CefRefPtr<CefFrame> frame = browser->GetMainFrame();
	frame->LoadURL("data:text/plain;charset=UTF-8;page=21,my%20SpaceStationMessage:1234,5678");

	// TODO limit frequency of texture updating
	CefRect rect;
	renderHandler->GetViewRect(browser, rect);
	//browser->GetHost()->Invalidate(rect, PET_VIEW);
}

Cefgui *CefCheck(GLuint texture)
{
	Cefgui* cefgui;
	cefgui = initCefgui(texture);

	// set window size & url
	cefgui->reshape(500, 500);
	cefgui->load("http://www.google.com");
	//cefgui->output(nullptr);
	return cefgui;
}