if(EXISTS "/home/scott/code/wing/kmdx/maru/deps/src/mpir-cmake.tar.gz")
  file("SHA256" "/home/scott/code/wing/kmdx/maru/deps/src/mpir-cmake.tar.gz" hash_value)
  if("x${hash_value}" STREQUAL "xd32ea73cb2d8115a8e59b244f96f29bad7ff03367162b660bae6495826811e06")
    return()
  endif()
endif()
message(STATUS "downloading...
     src='https://github.com/chfast/mpir/archive/cmake.tar.gz'
     dst='/home/scott/code/wing/kmdx/maru/deps/src/mpir-cmake.tar.gz'
     timeout='none'")




file(DOWNLOAD
  "https://github.com/chfast/mpir/archive/cmake.tar.gz"
  "/home/scott/code/wing/kmdx/maru/deps/src/mpir-cmake.tar.gz"
  
  # no TIMEOUT
  STATUS status
  LOG log)

list(GET status 0 status_code)
list(GET status 1 status_string)

if(NOT status_code EQUAL 0)
  message(FATAL_ERROR "error: downloading 'https://github.com/chfast/mpir/archive/cmake.tar.gz' failed
  status_code: ${status_code}
  status_string: ${status_string}
  log: ${log}
")
endif()

message(STATUS "downloading... done")
