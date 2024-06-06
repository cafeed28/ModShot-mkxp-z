#include "journal.h"
#include "system/system.h"
#include "debugwriter.h"

#if MKXPZ_PLATFORM == MKXPZ_PLATFORM_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pwd.h>
#endif

#include <SDL.h>

#define JOURNAL_BUFFER_SIZE 256

struct JournalPrivate
{
	volatile bool active;
	volatile int messageLen;
	volatile char messageBuf[JOURNAL_BUFFER_SIZE];
	volatile char langBuf[JOURNAL_BUFFER_SIZE];

	SDL_Thread *thread;
	SDL_mutex *mutex;

#if MKXPZ_PLATFORM != MKXPZ_PLATFORM_WINDOWS
	std::string pipePath;
	int pipeFD;
#endif

	JournalPrivate()
		: active(false),
		  messageLen(0),
		  thread(nullptr)
	{
		mutex = SDL_CreateMutex();

		memset((char *)messageBuf, 0, JOURNAL_BUFFER_SIZE);
		langBuf[0] = '_';

#if MKXPZ_PLATFORM != MKXPZ_PLATFORM_WINDOWS
		struct passwd *pw = getpwuid(getuid());

		if (pw && (pw->pw_name && pw->pw_dir[0] != '\0'))
			pipePath = pw->pw_dir;

		if (pipePath.empty()) {
			const char *home = SDL_getenv("HOME");
			if (home && home[0] != '\0')
				pipePath = home;
			else
				pipePath = "/tmp";
		}

		pipePath += "/.oneshot-pipe";

		pipeFD = -1;
#endif
	}

	~JournalPrivate()
	{
		SDL_DestroyMutex(mutex);
	}
};

int journal_server(void *data)
{
	JournalPrivate *p = static_cast<JournalPrivate *>(data);

#if MKXPZ_PLATFORM == MKXPZ_PLATFORM_WINDOWS
	HANDLE pipe = CreateNamedPipeW(
		L"\\\\.\\pipe\\oneshot-journal-to-game",
		PIPE_ACCESS_OUTBOUND,
		PIPE_TYPE_BYTE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		JOURNAL_BUFFER_SIZE, JOURNAL_BUFFER_SIZE,
		0,
		NULL
	);

	for (;;) {
		ConnectNamedPipe(pipe, NULL);

		SDL_LockMutex(p->mutex);

		p->active = true;

		DWORD written;
		if (!WriteFile(pipe, (const void *)p->messageBuf, JOURNAL_BUFFER_SIZE, &written, NULL))
			Debug() << "Failure writing to Journal's pipe!";

		SDL_UnlockMutex(p->mutex);

		FlushFileBuffers(pipe);
		DisconnectNamedPipe(pipe);
	}

	CloseHandle(pipe);
#else
	if (access(p->pipePath.c_str(), F_OK) != -1) {
		p->pipeFD = open(
			p->pipePath.c_str(),
			O_WRONLY | O_CREAT | O_TRUNC,
			S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH
		);

		if (p->pipeFD == -1) {
			Debug() << "Failure to access Journal's pipe!";
		} else {
			SDL_LockMutex(p->mutex);

			p->active = true;

			if (p->messageLen > 0) {
				if (write(p->pipeFD, (char *)p->messageBuf, p->messageLen) == -1)
					Debug() << "Failure writing to Journal's pipe!";
			}

			SDL_UnlockMutex(p->mutex);
		}
	}
#endif

	return 0;
}

Journal::Journal()
{
	p = new JournalPrivate();

#if MKXPZ_PLATFORM != MKXPZ_PLATFORM_WINDOWS
	mkfifo(p->pipePath.c_str(), 0666);
#endif
}

Journal::~Journal()
{
#if MKXPZ_PLATFORM != MKXPZ_PLATFORM_WINDOWS
	unlink(p->pipePath.c_str());
	remove(p->pipePath.c_str());
#endif

	delete p;
}

bool Journal::isActive() const
{
	return p->active;
}

void Journal::set(const char *name)
{
	SDL_LockMutex(p->mutex);

	p->messageLen = strlen(name);
	strcpy((char *)p->messageBuf, name);

	// In the case where Journal is being sent empty string, do not append
	// the language suffix, because empty string is the signifier
	// to terminate the Journal program.
	if (p->messageLen > 0) {
		strcpy((char *)p->messageBuf + p->messageLen, (char *)p->langBuf);
		p->messageLen += strlen((char *)p->langBuf);
	}

	SDL_UnlockMutex(p->mutex);

	// Write message to the Journal pipe.
#if MKXPZ_PLATFORM == MKXPZ_PLATFORM_WINDOWS
	HANDLE pipe = CreateFileW(
		L"\\\\.\\pipe\\oneshot-game-to-journal",
		GENERIC_WRITE, 0,
		NULL,
		OPEN_EXISTING, 0,
		NULL
	);

	if (pipe != INVALID_HANDLE_VALUE) {
		p->active = true;

		DWORD written;
		if (!WriteFile(pipe, (const void *)p->messageBuf, JOURNAL_BUFFER_SIZE, &written, NULL))
			Debug() << "Failure writing to Journal's pipe!";

		FlushFileBuffers(pipe);
		CloseHandle(pipe);
	}

	// Create thread for new Journal connections
	if (!p->thread)
		p->thread = SDL_CreateThread(journal_server, "journal", p);
#else
	// Clean up connection thread
	if (p->thread && p->pipeFD != -1) {
		SDL_WaitThread(p->thread, NULL);
		p->thread = nullptr;
	}

	if (p->pipeFD != -1) {
		// We have a connection, so write it
		if (write(p->pipeFD, (char *)p->messageBuf, p->messageLen) <= 0) {
			// In the case of an error, close
			close(p->pipeFD);
			p->pipeFD = -1;
		}
	} else {
		// We don't have a pipe open, so create connection thread
		p->thread = SDL_CreateThread(journal_server, "journal", p);
	}
#endif
}

void Journal::setLang(const char *lang)
{
	strcpy((char *)p->langBuf + 1, lang);
}
