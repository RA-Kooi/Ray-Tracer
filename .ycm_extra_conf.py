# vim: ts=4 sw=4 noexpandtab

def DirectoryOfThisScript():
	import os
	return os.path.dirname(os.path.realpath(__file__)) + os.sep

def FlagsForFile( filename, **kwargs ):
	return { 'flags': [
			'-x', 'c++',
			'-std=c++17',
			'-D_CRT_SECURE_NO_WARNINGS',
			'-DUNICODE',
			'-DVIM_WORKAROUND',
			'-DGLM_ENABLE_EXPERIMENTAL',
			'-DNOMINMAX',
			'-DWIN32_LEAN_AND_MEAN',
			'-DVC_EXTRALEAN',
			'-isystemD:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\Community\\VC\\Auxiliary\\VS\\UnitTest\\include',
			'-isystemC:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\Community\\VC\\Auxiliary\\VS\\UnitTest\\include',
			'-isystem' + DirectoryOfThisScript() + '3rdparty',
			'-isystem' + DirectoryOfThisScript() + '3rdparty/glfw-master/include',
			'-isystem' + DirectoryOfThisScript() + 'packages/glm.0.9.9.300/build/native/include',
			'-isystem' + DirectoryOfThisScript(),
			'-Weverything',
			'-Wno-c++98-compat',
			'-Wno-c++98-compat-pedantic',
			'-Wno-reserved-id-macro',
			'-Wno-documentation',
			'-Wno-shadow-field-in-constructor',
			'-Wno-global-constructors',
			'-Wno-exit-time-destructors',
			'-Wno-switch-enum',
			'-Wno-gnu-anonymous-struct',
			'-Wno-nested-anon-types',
			'-Wno-shadow-uncaptured-local'
		],
	}
