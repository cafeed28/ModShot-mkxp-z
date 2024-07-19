#include "binding-util.h"
#include "sharedstate.h"
#include "oneshot/i18n.h"
#include "oneshot/oneshot.h"
#include "oneshot/journal.h"

RB_METHOD(journalActive)
{
	RB_UNUSED_PARAM;

	return shState->oneshot().journal->isActive() ? Qtrue : Qfalse;
}

RB_METHOD(journalSet)
{
	RB_UNUSED_PARAM;

	const char *name;
	rb_get_args(argc, argv, "z", &name RB_ARG_END);

	shState->oneshot().journal->set(name);

	return Qnil;
}

RB_METHOD(journalSetLang)
{
	RB_UNUSED_PARAM;

	const char *lang;
	rb_get_args(argc, argv, "z", &lang RB_ARG_END);

	shState->oneshot().journal->setLang(lang);

	OneshotImpl::i18n::loadLocale(lang);

	return Qnil;
}

void journalBindingInit()
{
	VALUE module = rb_define_module("Journal");

	_rb_define_module_function(module, "active?", journalActive);
	_rb_define_module_function(module, "set", journalSet);
	_rb_define_module_function(module, "setLang", journalSetLang);
}
