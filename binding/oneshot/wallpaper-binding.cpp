#include "binding-util.h"
#include "sharedstate.h"
#include "oneshot/oneshot.h"
#include "oneshot/wallpaper.h"

RB_METHOD(wallpaperSet)
{
	RB_UNUSED_PARAM;

	const char *name;
	int color;
	rb_get_args(argc, argv, "zi", &name, &color RB_ARG_END);

	shState->oneshot().wallpaper->set(name, color);

	return Qnil;
}

RB_METHOD(wallpaperReset)
{
	RB_UNUSED_PARAM;

	shState->oneshot().wallpaper->reset();

	return Qnil;
}

void wallpaperBindingInit()
{
	VALUE module = rb_define_module("Wallpaper");

	_rb_define_module_function(module, "set", wallpaperSet);
	_rb_define_module_function(module, "reset", wallpaperReset);
}
