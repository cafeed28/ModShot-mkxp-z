#!/bin/sh

if [[ -z "$MESON_SOURCE_ROOT" || -z "$MESON_BUILD_ROOT" || -z "$MESON_INSTALL_PREFIX" ]]; then
  echo "This script can be only used in Meson build system." >&2
  exit 1
fi

if [[ -z "$MKXPZ_PREFIX" ]] || [[ -z "$MKXPZ_RUBY_PREFIX" ]]; then
  echo "Missing variables from windows/vars.sh script." >&2
  exit 1
fi

# Do exit on command error
set -e

# Get script arguments
GPLV3=$1
STEAM_PATH=$2
STEAM_LIBNAME=$3

# Get source, build and install paths
SOURCE="$MESON_SOURCE_ROOT"
BUILD="$MESON_BUILD_ROOT"
INSTALL="$MESON_INSTALL_PREFIX"

# ------------------------------------------------------------------------------

function make_prefix {
  # Copy Ruby DLL
  echo "Installing $MKXPZ_RUBY_PREFIX-ruby310.dll..."
  cp -pu "$MKXPZ_PREFIX/bin/$MKXPZ_RUBY_PREFIX-ruby310.dll" "$INSTALL/"

  # Copy Steamworks files
  if [[ -n "$STEAM_PATH" ]] && [[ -n "$STEAM_LIBNAME" ]]; then
    # Copy Steamworks SDK DLL
    echo "Installing $STEAM_LIBNAME.dll..."
    cp -pu "$STEAM_PATH/$STEAM_LIBNAME.dll" "$INSTALL/"

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

  # Copy GPL-3.0 license file
  if [[ -n "$GPLV3" ]] && [[ "$GPLV3" == "1" ]]; then
    echo "Installing LICENSE.mkxp-z-with-https.txt..."
    cp -pu "$SOURCE/assets/LICENSE.mkxp-z-with-https.txt" "$INSTALL/"
  fi
}

# ------------------------------------------------------------------------------

mkdir -p "$INSTALL"

make_prefix
