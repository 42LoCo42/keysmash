#include <raylib.h>
#include <sys/syscall.h>

#include <keyd.h>
#undef err

#define die(...) err(1, __VA_ARGS__)

#ifndef SOUND_DIR
#error "SOUND_DIR must be defined!"
#endif

static pthread_barrier_t barrier = {0};

static int keyd_event_handler(struct event* ev) {
	if(ev->type != EV_DEV_EVENT) return 0;
	if(ev->devev->type != DEV_KEY) return 0;
	if(!ev->devev->pressed) return 0;

	pthread_barrier_wait(&barrier);
	return 0;
}

int main(void) {
	char* sudo_uid  = getenv("SUDO_UID");
	char* sudo_gid  = getenv("SUDO_GID");
	char* sudo_home = getenv("SUDO_HOME");

	if(sudo_uid == NULL) errx(1, "SUDO_UID must be set!");
	if(sudo_gid == NULL) errx(1, "SUDO_GID must be set!");
	if(sudo_home == NULL) errx(1, "SUDO_HOME must be set!");
	if(getuid() != 0) errx(1, "must be run with sudo as root!");

	if(pthread_barrier_init(&barrier, NULL, 2) < 0) die("pthread_barrier_init");

	pthread_t keyd_thread = {0};
	if(pthread_create(&keyd_thread, NULL, evloop, keyd_event_handler) != 0)
		die("pthread_create keyd thread");

	char* endptr = NULL;
	uid_t uid    = strtol(sudo_uid, &endptr, 10);
	if(*endptr != 0) die("SUDO_UID is not a number");

	gid_t gid = strtol(sudo_gid, &endptr, 10);
	if(*endptr != 0) die("SUDO_GID is not a number");

	if(syscall(SYS_setresgid, gid, gid, gid)) die("setresgid %d", gid);
	if(syscall(SYS_setresuid, uid, uid, uid)) die("setresuid %d", uid);

	char* runtime_dir = NULL;
	if(asprintf(&runtime_dir, "/run/user/%d", uid) < 0)
		die("failed to alloc XDG_RUNTIME_DIR path");

	if(setenv("XDG_RUNTIME_DIR", runtime_dir, 1) < 0)
		die("setenv XDG_RUNTIME_DIR");

	if(setenv("HOME", sudo_home, 1) < 0) die("setenv HOME");

	InitAudioDevice();

	FilePathList sound_paths = LoadDirectoryFiles(SOUND_DIR);
	if(sound_paths.count <= 0)
		die("failed to list sound directory %s", SOUND_DIR);

	Sound* sounds = calloc(sound_paths.count, sizeof(Sound));
	for(size_t i = 0; i < sound_paths.count; i++) {
		sounds[i] = LoadSound(sound_paths.paths[i]);
	}

	for(;;) {
		pthread_barrier_wait(&barrier);
		PlaySound(sounds[GetRandomValue(0, sound_paths.count - 1)]);
	}
}
