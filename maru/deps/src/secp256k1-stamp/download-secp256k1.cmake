if(EXISTS "/home/scott/code/wing/kmdx/maru/deps/src/secp256k1-ac8ccf29.tar.gz")
  file("SHA256" "/home/scott/code/wing/kmdx/maru/deps/src/secp256k1-ac8ccf29.tar.gz" hash_value)
  if("x${hash_value}" STREQUAL "x02f8f05c9e9d2badc91be8e229a07ad5e4984c1e77193d6b00e549df129e7c3a")
    return()
  endif()
endif()
message(STATUS "downloading...
     src='https://github.com/chfast/secp256k1/archive/ac8ccf29b8c6b2b793bc734661ce43d1f952977a.tar.gz'
     dst='/home/scott/code/wing/kmdx/maru/deps/src/secp256k1-ac8ccf29.tar.gz'
     timeout='none'")




file(DOWNLOAD
  "https://github.com/chfast/secp256k1/archive/ac8ccf29b8c6b2b793bc734661ce43d1f952977a.tar.gz"
  "/home/scott/code/wing/kmdx/maru/deps/src/secp256k1-ac8ccf29.tar.gz"
  
  # no TIMEOUT
  STATUS status
  LOG log)

list(GET status 0 status_code)
list(GET status 1 status_string)

if(NOT status_code EQUAL 0)
  message(FATAL_ERROR "error: downloading 'https://github.com/chfast/secp256k1/archive/ac8ccf29b8c6b2b793bc734661ce43d1f952977a.tar.gz' failed
  status_code: ${status_code}
  status_string: ${status_string}
  log: ${log}
")
endif()

message(STATUS "downloading... done")
