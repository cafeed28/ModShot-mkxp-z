mkxp_assets = files(
    'icon.png',
    'gamecontrollerdb.txt'
)

if get_option('cjk_font') == true
    mkxp_assets += files('wqymicrohei.ttf')
else
    mkxp_assets += files('liberation.ttf')
endif

mkxp_assets_xxd = xxd_gen.process(mkxp_assets)

mkxp_sources += mkxp_assets_xxd
