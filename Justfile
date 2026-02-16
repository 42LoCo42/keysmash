run: build
	./build/keysmash

build:
	if [ ! -d build ]; then just _wipe; fi
	meson compile -C build

wipe: _wipe
	just run

_wipe:
	meson setup                   \
		--wipe                    \
		-D keyd=${KEYD}           \
		-D sound_dir=${SOUND_DIR} \
		build
