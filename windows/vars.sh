if [[ "${BASH_SOURCE-}" = "$0" ]]; then
  echo "You must 'source' this script: source $0" >&2
  exit 1
fi

if [[ "$MKXPZ_PREFIX" ]]; then
  echo "Already done" >&2
  return
fi

MKXPZ_ENVDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
MKXPZ_MSYSTEM="$(echo $MSYSTEM | tr '[:upper:]' '[:lower:]')"

# Export environment variables for build stuff
export MKXPZ_PREFIX="${MKXPZ_ENVDIR}/build-${MKXPZ_MSYSTEM}"
export PATH="${MKXPZ_PREFIX}/bin:${PATH}"
export PKG_CONFIG_PATH="${MKXPZ_PREFIX}/lib/pkgconfig:${PKG_CONFIG_PATH}"
export CMAKE_PREFIX_PATH="${MKXPZ_PREFIX}:${CMAKE_PREFIX_PATH}"

# Get Ruby library prefix, depending on MSYSTEM environment
export MKXPZ_RUBY_PREFIX=""
if [[ $MKXPZ_MSYSTEM = "mingw64" ]]; then
    MKXPZ_RUBY_PREFIX="x64-msvcrt"
elif [[ $MKXPZ_MSYSTEM = "mingw32" ]]; then
    MKXPZ_RUBY_PREFIX="msvcrt"
elif [[ $MKXPZ_MSYSTEM = "ucrt64" ]]; then
    MKXPZ_RUBY_PREFIX="x64-ucrt"
elif [[ $MKXPZ_MSYSTEM = "clang64" ]]; then
    MKXPZ_RUBY_PREFIX="x64-ucrt"
elif [[ $MKXPZ_MSYSTEM = "clang32" ]]; then
    MKXPZ_RUBY_PREFIX="ucrt"
elif [[ $MKXPZ_MSYSTEM = "clangarm64" ]]; then
    MKXPZ_RUBY_PREFIX="x64-ucrt"
fi
