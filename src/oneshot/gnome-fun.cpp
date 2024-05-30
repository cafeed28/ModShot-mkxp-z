#include "gnome-fun.h"
#include "debugwriter.h"

#include <SDL_loadso.h>

#ifdef __LINUX__
#define GTK2_LIBNAME "libgtk-x11-2.0.so"
#define GTK3_LIBNAME "libgtk-3.so.0"
#else
#error "This code is currently only for Linux platforms."
#endif

struct GnomeFunctions dynGnome;

#define G_FUNC(name, type) \
	dynGnome.name = (type)SDL_LoadFunction(so, #name); \
	if (dynGnome.name == NULL) { \
		Debug() << "[gnome-fun] Unable to load function:" << SDL_GetError(); \
		fail = true; \
	}

void initGnomeFunctions()
{
	bool fail = false;

	static void *so;

	// Try to load GTK+ 3
	so = SDL_LoadObject(GTK3_LIBNAME);

	if (so == NULL) {
		Debug() << "[gnome-fun] Couldn't load GTK+ 3 library:" << SDL_GetError();

		// Try to load GTK+ 2
		so = SDL_LoadObject(GTK2_LIBNAME);

		if (so == NULL) {
			Debug() << "[gnome-fun] Couldn't load GTK+ 2 library:" << SDL_GetError();
			fail = true;
		}
	}

	if (!fail) {
		GLIB_FUNCS
		GTK_FUNCS
	}

	if (fail) {
		memset(&dynGnome, 0, sizeof(dynGnome));
		SDL_UnloadObject(so);
		so = nullptr;
	}
}
