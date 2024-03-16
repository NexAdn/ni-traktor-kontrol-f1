catch_dep = dependency('catch2-with-main', required: true)

tests = files([
	'io/MidiEvent.cpp',
	'io/MidiStream.cpp',
	'io/Ringbuffer.cpp',
	'io/RingbufferReadIterator.cpp',
])

test_runner = executable(
	'tests',
	tests,
	include_directories: [
		src_include,
	],
	dependencies: [
		catch_dep
	],
	link_with: [
		common_lib,
	]
)

test_tgt = run_target(
	'check',
	command: ['/bin/sh', '-c', '"${MESON_BUILD_ROOT}/test/tests"']
)

test('unit', test_runner)

# vi: noexpandtab
