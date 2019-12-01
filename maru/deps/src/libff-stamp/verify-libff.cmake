set(file "/home/scott/code/wing/kmdx/maru/deps/src/libff-03b719a7.tar.gz")
message(STATUS "verifying file...
     file='${file}'")
set(expect_value "81b476089af43025c8f253cb1a9b5038a1c375baccffea402fa82042e608ab02")
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
    execute_process(COMMAND ${CMAKE_COMMAND} -P "/home/scott/code/wing/kmdx/maru/deps/src/libff-stamp/download-libff.cmake")
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
