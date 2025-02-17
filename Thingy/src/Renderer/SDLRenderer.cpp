#include "tpch.h"
#include "SDLRenderer.h"
namespace Thingy {
	
	bool IsInHeader(const SDL_Point* pt, int winW, const int edgeSize) {
		ImVec2 headerSize = ImVec2(winW, 50);

		return (pt->x >= edgeSize && pt->x < headerSize.x && pt->y >= edgeSize && pt->y < headerSize.y);
	}
	bool IsInItem(const SDL_Point* pt) {
		ImVec2 mouse = ImGui::GetMousePos();
		return (pt->x >= mouse.x - 1 && pt->x <= mouse.x + 1 &&
			pt->y >= mouse.y - 1 && pt->y <= mouse.y + 1) &&
			ImGui::IsAnyItemHovered();
	}

	SDL_HitTestResult WindowHitTest(SDL_Window* win, const SDL_Point* pos, void*) {
		int w, h;
		SDL_GetWindowSize(win, &w, &h);

		const int EDGE_SIZE = 5;

		// Corners
		if (pos->x < EDGE_SIZE && pos->y < EDGE_SIZE) return SDL_HITTEST_RESIZE_TOPLEFT;
		if (pos->x > w - EDGE_SIZE && pos->y < EDGE_SIZE) return SDL_HITTEST_RESIZE_TOPRIGHT;
		if (pos->x < EDGE_SIZE && pos->y > h - EDGE_SIZE) return SDL_HITTEST_RESIZE_BOTTOMLEFT;
		if (pos->x > w - EDGE_SIZE && pos->y > h - EDGE_SIZE) return SDL_HITTEST_RESIZE_BOTTOMRIGHT;

		// Edges
		if (pos->y < EDGE_SIZE) return SDL_HITTEST_RESIZE_TOP;
		if (pos->y > h - EDGE_SIZE) return SDL_HITTEST_RESIZE_BOTTOM;
		if (pos->x < EDGE_SIZE) return SDL_HITTEST_RESIZE_LEFT;
		if (pos->x > w - EDGE_SIZE) return SDL_HITTEST_RESIZE_RIGHT;

		if (IsInItem(pos)) return SDL_HITTEST_NORMAL;
		if (IsInHeader(pos, w, EDGE_SIZE)) return SDL_HITTEST_DRAGGABLE;

		return SDL_HITTEST_NORMAL;
	}

	SDLRenderer* SDLRenderer::Create(const WindowProps& props) {
		return new SDLRenderer(props);
	}

	void SDLRenderer::ChangeHitTest(bool on) {
		if (!SDL_SetWindowHitTest(window, (on) ? WindowHitTest : NULL, nullptr)) {
			SDL_Log("Failed to set hit test: %s", SDL_GetError());
		}
	}

	SDLRenderer::SDLRenderer(const WindowProps& props) {
		Init(props);
	}

	SDLRenderer::~SDLRenderer() {

	}

	void SDLRenderer::Init(const WindowProps& props) {
		if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
			printf("Error: SDL_Init(): %s\n", SDL_GetError());
		}
		SDL_SetLogPriorities(SDL_LOG_PRIORITY_VERBOSE);
		SetWindowFlags();
		CreateSDLWindow(props);
		CreateRenderer();
		ChangeHitTest(true);
		SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
		SDL_SetWindowMinimumSize(window, 1280, 720);
	}

	void SDLRenderer::CleanUp() {
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}

	void SDLRenderer::SetWindowFlags() {
		windowFlags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
	}

	void SDLRenderer::SetCustomWindowStyle(SDL_Window* window) {
		HWND hwnd = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
		if (hwnd) {
			LONG style = GetWindowLong(hwnd, GWL_STYLE);
			style &= ~WS_OVERLAPPEDWINDOW;
			style |= WS_POPUP | WS_EX_CLIENTEDGE;
			SetWindowLong(hwnd, GWL_STYLE, style);

			SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
		}
	}

	void SDLRenderer::CreateSDLWindow(const WindowProps& props) {
		window = SDL_CreateWindow(props.Title.data(), props.Width, props.Height, windowFlags);
		if (window == nullptr)
		{
			printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
		}
		SetCustomWindowStyle(window);
	}

	void SDLRenderer::CreateRenderer() {
		renderer = SDL_CreateRenderer(window, nullptr);
		if (renderer == nullptr)
		{
			SDL_Log("Error: SDL_CreateRenderer(): %s\n", SDL_GetError());
		}
		SDL_SetRenderVSync(renderer, 1);
		if (renderer == nullptr)
		{
			SDL_Log("Error: SDL_CreateRenderer(): %s\n", SDL_GetError());
		}
	}

	void SDLRenderer::OnRender() {

	}

	

	

}
