

set(command "${make};install")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "/home/scott/code/wing/kmdx/maru/deps/src/secp256k1-stamp/secp256k1-install-out.log"
  ERROR_FILE "/home/scott/code/wing/kmdx/maru/deps/src/secp256k1-stamp/secp256k1-install-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  /home/scott/code/wing/kmdx/maru/deps/src/secp256k1-stamp/secp256k1-install-*.log")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "secp256k1 install command succeeded.  See also /home/scott/code/wing/kmdx/maru/deps/src/secp256k1-stamp/secp256k1-install-*.log")
  message(STATUS "${msg}")
endif()
