if(NOT EXISTS "/home/povelitel/c++/SpecialGame/android-project/app/.cxx/Debug/5c1h461y/x86/install_manifest.txt")
  message(FATAL_ERROR "Cannot find install manifest: /home/povelitel/c++/SpecialGame/android-project/app/.cxx/Debug/5c1h461y/x86/install_manifest.txt")
endif()

file(READ "/home/povelitel/c++/SpecialGame/android-project/app/.cxx/Debug/5c1h461y/x86/install_manifest.txt" files)
string(REGEX REPLACE "\n" ";" files "${files}")
foreach(file ${files})
  message(STATUS "Uninstalling $ENV{DESTDIR}${file}")
  if(IS_SYMLINK "$ENV{DESTDIR}${file}" OR EXISTS "$ENV{DESTDIR}${file}")
    exec_program(
      "/home/povelitel/Android/Sdk/cmake/3.22.1/bin/cmake" ARGS "-E remove \"$ENV{DESTDIR}${file}\""
      OUTPUT_VARIABLE rm_out
      RETURN_VALUE rm_retval
      )
    if(NOT "${rm_retval}" STREQUAL 0)
      message(FATAL_ERROR "Problem when removing $ENV{DESTDIR}${file}")
    endif()
  else(IS_SYMLINK "$ENV{DESTDIR}${file}" OR EXISTS "$ENV{DESTDIR}${file}")
    message(STATUS "File $ENV{DESTDIR}${file} does not exist.")
  endif()
endforeach()
