project(
	'keysmash',
	'c',
	version: '1.0.0',
	default_options: {
		'buildtype': 'debugoptimized',  #
		'warning_level': '3',
	},
)

keyd = get_option('keyd')
raylib = dependency('raylib')

executable(
	'keysmash',
	'src/main.c',
	keyd + '/dbg.c',
	keyd + '/device.c',
	keyd + '/evloop.c',
	keyd + '/keys.c',
	keyd + '/log.c',
	keyd + '/util.c',
	include_directories: [keyd],
	c_args: [
		'-Wno-implicit-function-declaration',
		'-DSOUND_DIR="' + get_option('sound_dir') + '"',
	],
	dependencies: [raylib],
	install: true,
)
