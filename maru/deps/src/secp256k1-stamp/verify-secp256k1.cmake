set(file "/home/scott/code/wing/kmdx/maru/deps/src/secp256k1-ac8ccf29.tar.gz")
message(STATUS "verifying file...
     file='${file}'")
set(expect_value "02f8f05c9e9d2badc91be8e229a07ad5e4984c1e77193d6b00e549df129e7c3a")
set(attempt 0)
set(succeeded 0)
while(${attempt} LESS 3 OR ${attempt} EQUAL 3 AND NOT ${succeeded})
  file(SHA256 "${file}" actual_value)
  if("${actual_value}" STREQUAL "${expect_value}")
    set(succeeded 1)
  elseif(${attempt} LESS 3)
    message(STATUS "SHA256 hash of ${file}
does not match expected value
  expected: ${expect_value}
    actual: ${actual_value}
Retrying download.
")
    file(REMOVE "${file}")
    execute_process(COMMAND ${CMAKE_COMMAND} -P "/home/scott/code/wing/kmdx/maru/deps/src/secp256k1-stamp/download-secp256k1.cmake")
  endif()
  math(EXPR attempt "${attempt} + 1")
endwhile()

if(${succeeded})
  message(STATUS "verifying file... done")
else()
  message(FATAL_ERROR "error: SHA256 hash of
  ${file}
does not match expected value
  expected: ${expect_value}
    actual: ${actual_value}
")
endif()
