executable('elicit',
  sources      : files(
    'main.c',
    'callbacks.c',
    'capture.c',
    'config.c',
  ),
  dependencies : deps,
  c_args       : ['-DPROCVUE_DATADIR="' + (get_option('prefix') / get_option('datadir') / 'procvue') + '"'],
  install      : true,
  install_dir  : get_option('bindir'),
)
