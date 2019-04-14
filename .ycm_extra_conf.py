# vim: ts=4 sw=4 noexpandtab

def DirectoryOfThisScript():
	import os
	return os.path.dirname(os.path.realpath(__file__)) + os.sep

def FlagsForFile( filename, **kwargs ):
	return { 'flags': [
			'-x', 'c++',
			'-std=c++17',
			'-DVIM_WORKAROUND',
			'-DENABLE_GLFW',
			'-isystem/usr/include/eigen3',
			'-isystem' + DirectoryOfThisScript() + '3rdparty',
			'-isystem' + DirectoryOfThisScript(),
			'-Weverything',
			'-Wno-c++98-compat',
			'-Wno-shadow-field-in-constructor',
			'-Wno-padded',
			'-Wno-gnu-anonymous-struct',
			'-Wno-nested-anon-types',
			'-Wno-shadow-uncaptured-local',
			'-Wno-shadow-field'
		],
	}
