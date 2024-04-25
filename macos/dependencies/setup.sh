#!/usr/bin/env ruby

require 'FileUtils'

# Append your Makefile arguments here
$make_args = ""

XCODE_VERSION = `xcodebuild -version`.scan(/Xcode (\d+)\.(\d+)/)[0][0].to_i

def create_fat(a, b, dest)
  # Create FAT (Universal) binaries from two binaries (x86_64 and arm64) with lipo
  a_info = `lipo -info #{a}`[/is architecture: x86_64/]
  b_info = `lipo -info #{b}`[/is architecture: arm64/]

  if a_info && b_info
    dest_fat = File.join(dest, File.basename(a))
    result = system("lipo -create #{a} #{b} -output #{dest_fat}")
    if not result
      printf("\n\e[31m  *  Something went wrong while creating universal binary for #{dest_fat}...\e[0m\n\n")
      return false
    end
    return true
  end
end

def make_x86_64()
  # Build dependencies for Intel (x86_64)
  printf("\n  *  Building dependencies for Intel (x86_64)...\n\n")
  success = system("make -f./x86_64.make #{$make_args}")
  if not success
    printf("\n\e[31m  *  Something went wrong while building for Intel (x86_64)...\e[0m\n\n")
    return false
  end
  return true
end

def make_arm64()
  # Build dependencies for Apple Silicon (arm64)
  printf("\n  *  Building dependencies for Apple Silicon (arm64)...\n\n")
  success = system("make -f./arm64.make #{$make_args}")
  if not success
    printf("\n\e[31m  *  Something went wrong while building for Apple Silicon (arm64)...\e[0m\n\n")
    return false
  end
  return true
end

def make_universal()
  lib_x86_64 = File.join(Dir.pwd, "build-x86_64", "lib")
  lib_arm64 = File.join(Dir.pwd, "build-arm64", "lib")

  return if not Dir.exist?(lib_x86_64) or not Dir.exist?(lib_arm64)

  files_x86_64 = Dir[File.join(lib_x86_64, "*.{a,dylib}")]
  files_arm64 = Dir[File.join(lib_arm64, "*.{a,dylib}")]

  basenames = {
    :x86_64 => files_x86_64.map { |f| File.basename(f) },
    :arm64 => files_arm64.map { |f| File.basename(f) }
  }

  printf("\n  *  Creating Universal dependencies...\n\n")

  dest = File.join(Dir.pwd, "build-universal", "lib")
  FileUtils.mkdir_p(dest) if not Dir.exist?(dest)

  # Create Universal binaries with lipo
  files_arm64.length.times do |i|
    if basenames[:x86_64].include?(basenames[:arm64][i])
      path_x86_64 = files_x86_64[basenames[:x86_64].index(basenames[:arm64][i])]
      path_arm64 = files_arm64[i]

      if not File.exist?(File.join(dest, basenames[:arm64][i]))
        create_fat(path_x86_64, path_arm64, dest)
      end
    end
  end

  # Create symlink of Ruby libraries to Universal
  lib_ruby = File.join(lib_arm64, "ruby")
  FileUtils.ln_s(lib_ruby, dest) if not Dir.exist?(File.join(dest, "ruby"))
end

def run_build()
  printf("\n================================================================================\n")
  printf("  *  Building dependencies. This will take a while...\n")
  printf("================================================================================\n")

  # Build dependencies for Apple Silicon if we have atleast Xcode 12.0
  if XCODE_VERSION >= 12
    result = make_arm64()
    return false if not result
  end

  # Build dependencies for Intel
  result = make_x86_64()
  return false if not result

  return true
end

def run_post()
  printf("\n================================================================================\n")
  printf("  *  Performing Post-Setup...\n")
  printf("================================================================================\n")

  # Try to create universal libraries with both x86_64 and arm64 arch if possible
  make_universal()

  return true
end

# Build dependencies
result = run_build()
exit(1) if not result

# Post build procedures
result = run_post()
exit(1) if not result

# Script exit
printf("\n\e[32m  *  All done!\e[0m\n\n")
exit(0)
