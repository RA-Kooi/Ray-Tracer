# vim: ts=4 sw=4 noexpandtab

def DirectoryOfThisScript():
	import os
	return os.path.dirname(os.path.realpath(__file__)) + os.sep

def FlagsForFile( filename, **kwargs ):
	return { 'flags': [
			'-x', 'c++',
			'-std=c++17',
			'-target', 'x86_64-w64-windows-gnu',
			'-DVIM_WORKAROUND',
			'-DENABLE_GLFW',
			'-DUNICODE',
			'-D_UNICODE',
			'-DGLFW_INCLUDE_NONE',
			'-DGLM_ENABLE_EXPERIMENTAL',
			'-D__MINGW_IMPORT=__declspec(dllimport)',
			'-fno-ms-compatibility',
			'-isystem', 'C:/msys64/mingw64/include/c++/8.2.1',
			'-isystem', 'C:/msys64/mingw64/include/c++/8.2.1/x86_64-w64-mingw32',
			'-isystem', 'C:/msys64/mingw64/include/c++/8.2.1/backward',
			'-isystem', 'C:/msys64/mingw64/lib/clang/7.0.1/include',
			'-isystem', 'C:/msys64/mingw64/x86_64-w64-mingw32/include',
			'-isystem', 'C:/msys64/mingw64/include',
			'-isystem' + DirectoryOfThisScript() + '3rdparty',
			'-isystem' + DirectoryOfThisScript(),
			'-Weverything',
			'-Wno-c++98-compat',
			'-Wno-shadow-field-in-constructor',
			'-Wno-padded',
			'-Wno-gnu-anonymous-struct',
			'-Wno-nested-anon-types',
			'-Wno-shadow-uncaptured-local',
			'-Wno-shadow-field',
			'-Wno-unknown-warning-option',
			'-Wno-used-but-marked-unused',
			'-Wno-reserved-id-macro'
		],
	}
