#include "binding-util.h"
#include "sharedstate.h"
#include "system/system.h"
#include "oneshot/oneshot.h"
#include "oneshot/journal.h"

RB_METHOD(nikoPrepare)
{
	RB_UNUSED_PARAM;

#if MKXPZ_PLATFORM != MKXPZ_PLATFORM_WINDOWS
	// Needed on non-Windows to prepare (start) Journal program
	shState->oneshot().journal->nikoPrepare();
#endif

	return Qnil;
}

RB_METHOD(nikoStart)
{
	RB_UNUSED_PARAM;

	shState->oneshot().journal->nikoStart();

	return Qnil;
}

void nikoBindingInit()
{
	VALUE module = rb_define_module("Niko");

	_rb_define_module_function(module, "get_ready", nikoPrepare);
	_rb_define_module_function(module, "do_your_thing", nikoStart);
}
