# Generate AppRun and .desktop files for AppImage packaging
if appimagetool != ''
    lin_config = configuration_data()
    lin_config.set('NAME', meson.project_name())
    lin_config.set('APPIMAGE_NAME', meson.project_name())
    lin_config.set('VERSION', meson.project_version())
    lin_config.set('DESCRIPTION', 'A very heavily modified fork of mkxp')
    lin_config.set('EXECNAME', mkxp_name)
    lin_config.set('ARCH', host_arch)

    configure_file(
        configuration: lin_config,
        input: 'AppRun.in',
        output: 'AppRun'
    )

    configure_file(
        configuration: lin_config,
        input: 'AppImage.desktop.in',
        output: '@0@.desktop'.format(mkxp_name)
    )
endif

# Define install script parameters
install_arch = ''
install_gplv3 = '0'

if appimagetool != ''
    install_arch = host_arch
else
    if is_64bit
        install_arch = '64'
    else
        install_arch = '32'
    endif
endif

if mkxp_gplv3 == true
    install_gplv3 = '1'
endif

# Linux install script
meson.add_install_script('install.sh', mkxp_name, install_arch, install_gplv3, appimagetool, steamworks_libpath)
