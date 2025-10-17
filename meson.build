project('idle-inhibit', 'c',
  version: '0.1.0',
  license: 'GPL-2.0-or-later',
  default_options: [
    'c_std=c11',
    'optimization=2',
    'warning_level=3',
    'werror=true',
  ],
  meson_version: '>=0.56.0',
)

# Dependencies
libsystemd_dep = dependency('libsystemd')

# Source files
sources = files(
  'src/idle-inhibit.c',
)

# Executable
executable('idle-inhibit',
  sources,
  dependencies: [libsystemd_dep],
  install: true,
)
