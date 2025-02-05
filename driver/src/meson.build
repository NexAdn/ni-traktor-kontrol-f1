src_include = include_directories('.')

subdir('io')
subdir('tkf1')

common_lib = static_library(
	'tkf1-drv-common',
	[
		common_io_srcs,
		jack_srcs,
		tkf1_srcs,
	],
	dependencies: [
		jack_dep,
	],
	include_directories: [
		src_include,
	]
)

tkf1_lib = library(
	'tkf1',
	[
		tkf1_srcs,
	],
	dependencies: [
	],
	include_directories: [
		src_include,
	],
	version: '1.0.0',
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
