cmake_minimum_required(VERSION 3.17)
project(waos_contracts LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Header-only contracts. They need rpc headers (annotations, promise, stream
# protocol) and must not link libcpp_rpc: wasm modules compile those C sources
# into the module image themselves.
if(NOT TARGET cpp_rpc)
  include(FetchContent)
  FetchContent_Declare(cpp_rpc
    GIT_REPOSITORY https://github.com/daemonarium/rpc-cpp.git
    GIT_TAG v0.3.0
    GIT_SHALLOW TRUE
  )
  FetchContent_MakeAvailable(cpp_rpc)
endif()

function(waos_contract name)
  add_library(${name} INTERFACE)
  target_include_directories(${name} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    $<TARGET_PROPERTY:cpp_rpc,INTERFACE_INCLUDE_DIRECTORIES>
  )
endfunction()

waos_contract(waos_kernel)
waos_contract(waos_db)
waos_contract(waos_fs)
waos_contract(waos_recorder)
waos_contract(waos_debug_store)
target_link_libraries(waos_debug_store INTERFACE waos_kernel)
waos_contract(waos_kernel_shield)
target_link_libraries(waos_kernel_shield INTERFACE waos_kernel)
waos_contract(waos_observable_db)
target_link_libraries(waos_observable_db INTERFACE waos_db)

# Registration modules and the header smoke stay out of a consumer
# add_subdirectory(). A standalone configure is what builds them.
if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  return()
endif()

if(NOT cpp_rpc_SOURCE_DIR)
  message(FATAL_ERROR "cpp_rpc_SOURCE_DIR is not set")
endif()

set(RPC_SRC ${cpp_rpc_SOURCE_DIR}/src)
set(WASI_STUBS ${RPC_SRC}/utils/wasi_stubs.cpp)
set(MODULE_FILES ${RPC_SRC}/utils/module.cpp)
set(SOCKET_HANDLERS ${RPC_SRC}/utils/socket_handlers.cpp)

if(WASM)
  function(add_contract_module name contract)
    add_executable(${name}
      examples/modules/${name}.cpp
      ${WASI_STUBS}
      ${MODULE_FILES}
      ${SOCKET_HANDLERS}
    )
    target_link_libraries(${name} PRIVATE ${contract})
    target_compile_options(${name} PRIVATE -fwasm-exceptions)
    target_link_options(${name} PRIVATE
      -s STANDALONE_WASM=1
      -fwasm-exceptions
      -mbulk-memory
      -Wl,--import-memory
      --no-entry
      -sALLOW_MEMORY_GROWTH=1
      -sINITIAL_MEMORY=640kB
      -sTOTAL_STACK=20000
      -sEXPORTED_FUNCTIONS=_rpc_description,_start,_malloc,_waos_socket_data,_waos_socket_output_drain
      -sERROR_ON_UNDEFINED_SYMBOLS=0
      -Wl,--allow-undefined-file=${RPC_SRC}/examples/modules/allowed-undefined-symbols.syms
    )
    target_compile_definitions(${name} PRIVATE RPC_SERVER RPC_TYPE_DESCRIPTION)
    set_target_properties(${name} PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/modules
      SUFFIX ".wasm"
    )
  endfunction()

  add_contract_module(db waos_db)
  add_contract_module(kernel waos_kernel)
  add_contract_module(fs waos_fs)
  add_contract_module(debug_store waos_debug_store)
  add_contract_module(recorder waos_recorder)
  add_contract_module(db_bench_client waos_db)
  add_contract_module(dbg_forward waos_db)

  target_link_options(db_bench_client PRIVATE
    -sTOTAL_STACK=65536
    -sINITIAL_MEMORY=2MB
    -sEXPORTED_FUNCTIONS=_rpc_description,_start,_malloc,_waos_socket_data,_waos_socket_output_drain,_waos_set_process_id
  )
  target_link_options(dbg_forward PRIVATE
    -sEXPORTED_FUNCTIONS=_rpc_description,_start,_malloc,_waos_socket_data,_waos_socket_output_drain,_waos_set_process_id
  )
else()
  include(FetchContent)
  FetchContent_Declare(ut
    GIT_REPOSITORY https://github.com/boost-ext/ut.git
    GIT_TAG v2.0.0
  )
  FetchContent_MakeAvailable(ut)

  add_executable(contracts_smoke examples/smoke.cpp)
  target_link_libraries(contracts_smoke PRIVATE
    waos_kernel waos_db waos_fs waos_debug_store waos_recorder
    waos_kernel_shield waos_observable_db
  )

  add_library(contracts_socket_test_stub STATIC ${RPC_SRC}/test/socket_flush_stub.c)
  target_include_directories(contracts_socket_test_stub PRIVATE ${RPC_SRC})

  add_executable(dbg_forward_integration_ut test/dbg_forward_integration_ut.cpp)
  target_include_directories(dbg_forward_integration_ut PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/examples
    ${RPC_SRC}
  )
  target_link_libraries(dbg_forward_integration_ut PRIVATE
    Boost::ut waos_db contracts_socket_test_stub
  )
  set_target_properties(contracts_smoke dbg_forward_integration_ut PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
  )

  enable_testing()
  add_test(NAME contracts_smoke COMMAND contracts_smoke WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests)
  add_test(NAME dbg_forward_integration_ut COMMAND dbg_forward_integration_ut WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests)
endif()
