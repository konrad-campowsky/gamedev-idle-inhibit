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

systemd_system_unit_dir = get_option('systemd_system_unit_dir') != '' ? \
    get_option('systemd_system_unit_dir') : \
    (dependency(systemd).get_variable(pkgconfig: 'systemdsystemunitdir',
                                      default_value: join_paths(get_option('prefix'), 'lib', 'systemd', 'system')))
install_data('systemd/gamedev-idle-inhibit@.service', install_dir: systemd_system_unit_dir)

udev_base_dir = dependency('udev').get_variable(pkgconfig: 'udev_dir', 
                                                default_value: join_paths(get_option('prefix'), 'lib', 'udev'))
udev_rules_dir = join_paths(udev_base_dir, 'rules.d')
install_data('udev/95-gamedev-idle-inhibit.rules', install_dir: udev_rules_dir)

sources = files(
  'src/idle-inhibit.c',
)

executable('idle-inhibit',
  sources,
  dependencies: [dependency('libsystemd')],
  install: true,
)
