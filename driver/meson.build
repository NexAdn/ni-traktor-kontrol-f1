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

llpp_dep = dependency('landlockpp', required: get_option('landlock').enabled())

conf_data = configuration_data({
	'TKF1_NAME': meson.project_name(),
	'TKF1_VERSION': meson.project_version(),
	'TKF1_HAVE_LANDLOCK': get_option('landlock').enabled(),
})

jack_dep = dependency('jack', required: true)

subdir('src')

if get_option('test')
	subdir('test')
endif

# vi: noexpandtab

