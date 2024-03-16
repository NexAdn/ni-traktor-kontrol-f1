src_include = include_directories('.')

srcs = files([
	'io/JackWrapper.cpp',
	'io/HidDevice.cpp',
	'io/MidiEvent.cpp',
	'io/MidiStream.cpp',
	'io/RingbufferIterator.cpp',
	'tkf1/F1Device.cpp',
])

common_lib = static_library(
	'tkf1-drv-common',
	[
		srcs,
	],
	dependencies: [
		jack_dep,
	],
	include_directories: [
		src_include,
	]
)

exe = executable(
	'tkf1-drv',
	[
		'main.cpp',
	],
	dependencies: [
		jack_dep,
	],
	link_with: [
		common_lib,
	],
	include_directories: [
		src_include,
	],
	install: true
)

devtest_exe = executable(
	'tkf1-devtest',
	[
		'devtest.cpp',
	],
	link_with: [
		common_lib,
	],
	include_directories: [
		src_include,
	],
	install: true
)

# vi: noexpandtab
