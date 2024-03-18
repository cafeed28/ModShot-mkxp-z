if appimagetool != ''
    install_data('AppRun', install_mode: 'rwxr-xr-x')
    install_data('mkxp-z.png')
    install_data('mkxp-z.desktop', install_mode: 'rwxr-xr-x')

    meson.add_install_script('make_appimg.sh', mkxp_name, appimagetool, steamworks_libpath)
else
    if is_64bit
        archarg = '64'
    else
        archarg = '32'
    endif

    meson.add_install_script('install.sh', mkxp_name, archarg, steamworks_libpath)
endif
