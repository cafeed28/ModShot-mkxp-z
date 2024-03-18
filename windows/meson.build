windows = import('windows')

# mkxp-z Windows resources
mkxp_win_resources = windows.compile_resources(
    'resource.rc',
    args: mkxp_windres_args,
    depend_files: files(
        'resource.h',
        'resource.rc',
        'mkxpz.manifest',
        'icon.ico'
    )
)

mkxp_sources += mkxp_win_resources
mkxp_includes += include_directories('.')
