# Get Ruby dependency
if get_option('mri_includes') == ''
    mri_version = get_option('mri_version')

    if mri_version.version_compare('<=1.8')
        mkxp_cflags += '-DMKXPZ_LEGACY_RUBY'
    elif mri_version.version_compare('>=3.0') and cxx.get_id() == 'clang'
        mkxp_cflags += '-fdeclspec'
    endif

    mkxp_dependencies += dependency('ruby-@0@'.format(mri_version))
else
    mri_includes = get_option('mri_includes')
    mri_libname = get_option('mri_libname')
    mri_libpath = get_option('mri_libpath')

    mkxp_cflags += '-I@0@'.format(mri_includes)
    mkxp_dependencies += cc.find_library(mri_libname, dirs: [mri_libpath])
endif

mkxp_binding_sources = files(
    'module_rpg.cpp',
    'binding-mri.cpp',
    'binding-util.cpp',
    'etc-binding.cpp',
    'table-binding.cpp',
    'graphics-binding.cpp',
    'input-binding.cpp',
    'audio-binding.cpp',
    'bitmap-binding.cpp',
    'sprite-binding.cpp',
    'plane-binding.cpp',
    'viewport-binding.cpp',
    'font-binding.cpp',
    'window-binding.cpp',
    'windowvx-binding.cpp',
    'tilemap-binding.cpp',
    'tilemapvx-binding.cpp',
    'filesystem-binding.cpp',
    'http-binding.cpp',

    'oneshot/oneshot-binding.cpp'
)

if mkxp_steam
    mkxp_binding_sources += files(
        'cusl-binding.cpp'
    )
endif

if mkxp_miniffi
    mkxp_binding_sources += files(
        'miniffi.cpp',
        'miniffi-binding.cpp'
    )
endif

mkxp_sources += mkxp_binding_sources
mkxp_includes += include_directories('.')
