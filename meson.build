project('elicit', 'c', 
        version : '0.2.0')

efl_version = '>= 1.27.0'

cc = meson.get_compiler('c')

deps = [ dependency('x11'),
         dependency('ecore', version : efl_version),
         dependency('eina', version : efl_version),
         dependency('elementary', version : efl_version),
         dependency('eet', version : efl_version),
         cc.find_library('m', required : true), ]

#### dir locations  
dir_prefix = get_option('prefix')
dir_bin    = join_paths(dir_prefix, get_option('bindir'))
dir_data   = join_paths(dir_prefix, get_option('datadir'))

add_project_arguments(
  '-DELICIT_DATADIR="' + join_paths(dir_data, 'elicit') + '"',
  language : 'c'
)

edje_cc = find_program('edje_cc', required : true)

subdir('src')
subdir('data')