#!/bin/bash

if [[ -z "$MESON_SOURCE_ROOT" || -z "$MESON_BUILD_ROOT" || -z "$MESON_INSTALL_PREFIX" ]]; then
  echo "This script can be only used in Meson build system." >&2
  exit 1
fi

if [[ -z "$MKXPZ_PREFIX" ]]; then
  echo "Missing variables from linux/vars.sh script." >&2
  exit 1
fi

# Do exit on command error
set -e

# Get script arguments
MKXP_NAME=$1
BITS=$2
GPLV3=$3
APPIMAGETOOL=$4
STEAM_PATH=$5

# Get source, build and install paths
SOURCE="$MESON_SOURCE_ROOT"
BUILD="$MESON_BUILD_ROOT"
INSTALL="$MESON_INSTALL_PREFIX"

# Variables for AppImage packaging .AppDir
APPDIR_ROOT="$MESON_BUILD_ROOT/$MKXP_NAME.AppDir"
APPDIR_BIN="$APPDIR_ROOT/usr/bin"
APPDIR_LIB="$APPDIR_ROOT/usr/lib"
APPDIR_SHARE="$APPDIR_ROOT/usr/share"

# ------------------------------------------------------------------------------

function make_prefix {
  # Create directory for shared libraries
  mkdir -p "$INSTALL/lib$BITS"

  # Patch RPATH in executables
  echo "Patching $MKXP_NAME RPATH..."
  patchelf "$INSTALL/$MKXP_NAME" --set-rpath "\$ORIGIN/lib$BITS"

  if [[ -n "$STEAM_PATH" ]]; then
    echo "Patching steamshim RPATH..."
    patchelf "$INSTALL/steamshim" --set-rpath "\$ORIGIN/lib$BITS"
  fi

  # Copy Ruby shared library
  echo "Installing lib$BITS/libruby.so.3.1..."
  cp -pu "$MKXPZ_PREFIX/lib/libruby.so.3.1" "$INSTALL/lib$BITS/"

  # Remove RPATH from shared libraries
  patchelf "$INSTALL/lib$BITS/libruby.so.3.1" --remove-rpath

  # Copy Steamworks files
  if [[ -n "$STEAM_PATH" ]]; then
    # Copy Steamworks SDK shared library
    echo "Installing lib$BITS/libsteam_api.so..."
    cp -pu "$STEAM_PATH/libsteam_api.so" "$INSTALL/lib$BITS/"

    # Copy steam_appid.txt (Steam AppID)
    if [[ ! -f "$INSTALL/steam_appid.txt" ]]; then
      echo "Installing steam_appid.txt..."
      cp -pu "$SOURCE/assets/steam_appid.txt" "$INSTALL/"
    fi
  fi

  # Copy configuration file
  if [[ ! -f "$INSTALL/mkxp.json" ]]; then
    echo "Installing mkxp.json..."
    cp -pu "$SOURCE/mkxp.json" "$INSTALL/"
  fi

  # Copy scripts directory
  echo "Installing scripts..."
  cp -pur "$SOURCE/scripts/." "$INSTALL/scripts/"

  # Copy Ruby library (gems/extensions)
  echo "Installing Ruby library to rubylib/3.1.0..."
  mkdir -p "$INSTALL/rubylib"
  cp -pur "$MKXPZ_PREFIX/lib/ruby/3.1.0/." "$INSTALL/rubylib/3.1.0/"

  # Remove RPATH from native extensions in Ruby library
  patchelf "$INSTALL/rubylib/3.1.0"/*/*.so --remove-rpath

  # Copy GPL-3.0 license file
  if [[ -n "$GPLV3" ]]; then
    echo "Installing LICENSE.mkxp-z-with-https.txt..."
    cp -pu "$SOURCE/assets/LICENSE.mkxp-z-with-https.txt" "$INSTALL/"
  fi
}

function make_appdir {
  # Prepare AppDir structure
  mkdir -p "$APPDIR_ROOT"
  mkdir -p "$APPDIR_BIN" "$APPDIR_LIB" "$APPDIR_SHARE"
  mkdir -p "$APPDIR_SHARE/applications"
  mkdir -p "$APPDIR_SHARE/icons/hicolor/256x256/apps"
  mkdir -p "$APPDIR_SHARE/licenses/$MKXP_NAME"

  # Copy AppRun script
  echo "Copying AppRun to $MKXP_NAME.AppDir..."
  cp -pu "$BUILD/linux/AppRun" "$APPDIR_ROOT/"
  chmod +x "$APPDIR_ROOT/AppRun"

  # Copy .desktop file
  echo "Copying $MKXP_NAME.desktop to $MKXP_NAME.AppDir..."
  cp -pu "$BUILD/linux/$MKXP_NAME.desktop" "$APPDIR_ROOT/"
  chmod +x "$APPDIR_ROOT/$MKXP_NAME.desktop"

  # Create symlink to .desktop file in usr/applications
  if [[ ! -e "$APPDIR_SHARE/applications/$MKXP_NAME.desktop" ]]; then
    ln -srf "$APPDIR_ROOT/$MKXP_NAME.desktop" "$APPDIR_SHARE/applications/$MKXP_NAME.desktop"
  fi

  # Copy application icon file
  echo "Copying $MKXP_NAME.png to $MKXP_NAME.AppDir..."
  cp -pu "$SOURCE/linux/icon.png" "$APPDIR_ROOT/$MKXP_NAME.png"

  # Create symlinks for application icons
  if [[ ! -e "$APPDIR_ROOT/.DirIcon" ]]; then
    ln -srf "$APPDIR_ROOT/$MKXP_NAME.png" "$APPDIR_ROOT/.DirIcon"
  fi
  if [[ ! -e "$APPDIR_SHARE/icons/hicolor/256x256/apps/$MKXP_NAME.png" ]]; then
    ln -srf "$APPDIR_ROOT/$MKXP_NAME.png" "$APPDIR_SHARE/icons/hicolor/256x256/apps/$MKXP_NAME.png"
  fi

  # Copy executables
  echo "Copying $MKXP_NAME to $MKXP_NAME.AppDir/usr/bin..."
  cp -pu "$BUILD/$MKXP_NAME" "$APPDIR_BIN/"

  if [[ -n "$STEAM_PATH" ]]; then
    echo "Copying steamshim to $MKXP_NAME.AppDir/usr/bin..."
    cp -pu "$BUILD/steamshim" "$APPDIR_BIN/"
  fi

  # Patch RPATH in executables
  echo "Patching $MKXP_NAME RPATH..."
  patchelf "$APPDIR_BIN/$MKXP_NAME" --set-rpath "\$ORIGIN/../lib"

  if [[ -n "$STEAM_PATH" ]]; then
    echo "Patching steamshim RPATH..."
    patchelf "$APPDIR_BIN/steamshim" --set-rpath "\$ORIGIN/../lib"
  fi

  # Copy Ruby shared library
  echo "Copying libruby.so.3.1 to $MKXP_NAME.AppDir/usr/lib..."
  cp -pu "$MKXPZ_PREFIX/lib/libruby.so.3.1" "$APPDIR_LIB/"

  # Remove RPATH from shared libraries
  patchelf "$APPDIR_LIB/libruby.so.3.1" --remove-rpath

  # Copy Steamworks SDK shared library
  if [[ -n "$STEAM_PATH" ]]; then
    echo "Copying libsteam_api.so to $MKXP_NAME.AppDir/usr/lib..."
    cp -pu "$STEAM_PATH/libsteam_api.so" "$APPDIR_LIB/"
  fi

  # Copy GPL-3.0 license file
  echo "Copying LICENSE.mkxp-z-with-https.txt to $MKXP_NAME.AppDir/usr/licenses/$MKXP_NAME..."
  cp -pu "$SOURCE/assets/LICENSE.mkxp-z-with-https.txt" "$APPDIR_SHARE/licenses/$MKXP_NAME/"
}

function make_appimage {
  # Run AppImageTool to export .AppDir into AppImage file
  echo "Generating $MKXP_NAME.$BITS.AppImage..."
  "$APPIMAGETOOL" -n "$APPDIR_ROOT" "$BUILD/$MKXP_NAME.$BITS.AppImage"

  # Install generated AppImage file
  echo "Installing $MKXP_NAME.$BITS.AppImage..."
  cp -pu "$BUILD/$MKXP_NAME.$BITS.AppImage" "$INSTALL/"

  # Copy configuration file
  if [[ ! -f "$INSTALL/mkxp.json" ]]; then
    echo "Installing mkxp.json..."
    cp -pu "$SOURCE/mkxp.json" "$INSTALL/"
  fi

  # Copy scripts directory
  echo "Installing scripts..."
  cp -pur "$SOURCE/scripts/." "$INSTALL/scripts/"

  # Copy Ruby library (gems/extensions)
  echo "Installing Ruby library to rubylib/3.1.0..."
  mkdir -p "$INSTALL/rubylib"
  cp -pur "$MKXPZ_PREFIX/lib/ruby/3.1.0/." "$INSTALL/rubylib/3.1.0/"

  # Copy steam_appid.txt (Steam AppID)
  if [[ -n "$STEAM_PATH" ]]; then
    if [[ ! -f "$INSTALL/steam_appid.txt" ]]; then
      echo "Installing steam_appid.txt..."
      cp -pu "$SOURCE/assets/steam_appid.txt" "$INSTALL/"
    fi
  fi

  # Copy GPL-3.0 license file
  if [[ "$GPLV3" ]]; then
    echo "Installing LICENSE.mkxp-z-with-https.txt..."
    cp -pu "$SOURCE/assets/LICENSE.mkxp-z-with-https.txt" "$INSTALL/"
  fi
}

# ------------------------------------------------------------------------------

mkdir -p "$INSTALL"

if [[ "$APPIMAGETOOL" ]]; then
  make_appdir
  make_appimage
else
  make_prefix
fi
