if(EXISTS "/home/scott/code/wing/kmdx/maru/deps/src/libff-03b719a7.tar.gz")
  file("SHA256" "/home/scott/code/wing/kmdx/maru/deps/src/libff-03b719a7.tar.gz" hash_value)
  if("x${hash_value}" STREQUAL "x81b476089af43025c8f253cb1a9b5038a1c375baccffea402fa82042e608ab02")
    return()
  endif()
endif()
message(STATUS "downloading...
     src='https://github.com/scipr-lab/libff/archive/03b719a7c81757071f99fc60be1f7f7694e51390.tar.gz'
     dst='/home/scott/code/wing/kmdx/maru/deps/src/libff-03b719a7.tar.gz'
     timeout='none'")




file(DOWNLOAD
  "https://github.com/scipr-lab/libff/archive/03b719a7c81757071f99fc60be1f7f7694e51390.tar.gz"
  "/home/scott/code/wing/kmdx/maru/deps/src/libff-03b719a7.tar.gz"
  
  # no TIMEOUT
  STATUS status
  LOG log)

list(GET status 0 status_code)
list(GET status 1 status_string)

if(NOT status_code EQUAL 0)
  message(FATAL_ERROR "error: downloading 'https://github.com/scipr-lab/libff/archive/03b719a7c81757071f99fc60be1f7f7694e51390.tar.gz' failed
  status_code: ${status_code}
  status_string: ${status_string}
  log: ${log}
")
endif()

message(STATUS "downloading... done")
