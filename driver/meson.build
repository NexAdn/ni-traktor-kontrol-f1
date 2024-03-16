project(
	'tkf1-drv',
	[
		'cpp'
	],
	version: '0.1_alpha1',
	default_options: [
		'warning_level=3',
		'cpp_std=c++20'
	]
)

conf_data = configuration_data({
	'name': meson.project_name(),
	'version': meson.project_version()
})

jack_dep = dependency('jack', required: true)

subdir('src')

if get_option('test')
	subdir('test')
endif

# vi: noexpandtab

