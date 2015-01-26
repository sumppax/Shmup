#include "window.h"

Window::Window(int width, int height, std::string title, bool fullscreen):
	window(nullptr),
	surface(nullptr),
	renderer(nullptr),
	texture(nullptr),
	width(width),
	height(height),
	originalWidth(width),
	originalHeight(height)
{
	resize(title, width, height, fullscreen);

	clear();
	refresh();

	// NOTE(jouni): Odotellaan hetki, ettei frameraten laskeminen mene rikki
	SDL_Delay(500);
}

Window::~Window()
{
	destroy();
}

void Window::destroy()
{
	if (this->renderer)
	{
		SDL_DestroyRenderer(this->renderer);
		this->renderer = nullptr;
	}

	if (this->window)
	{
		SDL_DestroyWindow(this->window);
		this->window = nullptr;
	}
}

void Window::resize(std::string title, int width, int height, bool fullscreen)
{
	destroy();

	Uint32 window_flag; 

	if (fullscreen)
	{
		window_flag = SDL_WINDOW_FULLSCREEN;
	} else {
		window_flag = SDL_WINDOW_SHOWN;
	}

	SDL_CreateWindowAndRenderer(width, height, window_flag, &window, &renderer);
	
	// NOTE(jouni): Perus virhetarkistusta; jos window tai renderer ei ssaa arvoa,
	// ei anneta ohjelman vaan raa'asti kaatua vaan ilmotetaan siit� k�ytt�j�lle.
	if (!window || !renderer)
	{
		printf("SDL_Window tai SDL_Renderer ei pelaa!\n");
		return;
	}
	
	// Nearest-neighbour resize
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
	SDL_RenderSetLogicalSize(renderer, width/3, height/3);

	// Asetetaan title
	setTitle(title);

	surface = SDL_GetWindowSurface(window);
	if (!surface)
	{
		printf("SDL_GetWindowSurface failas!\n");
		return;
	}

	this->width = width;
	this->height = height;
}

void Window::clear()
{
	fill(Color("black"));
}

void Window::fill(Color color)
{
	SDL_SetRenderDrawColor(renderer,
						   color.r(),
						   color.g(),
						   color.b(),
						   color.a());

	SDL_RenderClear(renderer);
}

void Window::refresh()
{
	SDL_RenderPresent(renderer);
}

SDL_Texture* Window::loadImage(std::string filename)
{
	SDL_Texture *newTexture = IMG_LoadTexture(renderer, filename.c_str());

	if (!newTexture)
	{
		printf("Tekstuurin lataaminen ei onnistunut\n");
		printf("IMG_LoadTexture: %s\n", IMG_GetError());
	}

	return newTexture;
}

// source = kuvasta leikatun alueen sijainti ja koko
void Window::render(SDL_Texture* texture, int x, int y, SDL_Rect* source)
{
	// NOTE(jouni): Jos source on tyhj� k�ytet��n kuvan alkup. kokoa
	if (!source)
	{
		int width, height;
		SDL_QueryTexture(texture, NULL, NULL, &width, &height);

		SDL_Rect clip = {0, 0, width, height};
		source = &clip;
	}

	SDL_Rect destination = {x, y, source->w, source->h};

	SDL_RenderCopy(renderer, texture, source, &destination);
}

void Window::freeImage(SDL_Texture *image)
{
	if(image)
	{
		SDL_DestroyTexture(image);
	}
}
void Window::freeImage(SDL_Surface *image)
{
	if(image)
	{
		SDL_FreeSurface(image);
	}
}

void Window::drawRect(int X, int Y, int W, int H, Color color)
{
	SDL_Rect fillRect = { X, Y, W, H };
	SDL_SetRenderDrawColor(renderer, color.r(), color.g(), color.b(), color.a());
	SDL_RenderFillRect(renderer, &fillRect );
}

void Window::setTitle(std::string title)
{
	if (window)
	{
		SDL_SetWindowTitle(window, title.c_str());
	}
}

void Window::minimize() 
{
	if (window)
	{
		SDL_MinimizeWindow(window);
	}
}

void Window::maximize()
{
	if (window)
	{
		SDL_MaximizeWindow(window);
	}
}

void Window::restore()
{
	SDL_RestoreWindow(this->window);
}

SDL_Renderer* Window::getRenderer() 
{
	return renderer;
}