# contracts

C++ contracts for Daemonarium system processes. Each header is the source
that reflection turns into a type registry. Method order in the class is the
`function_id`. Append new methods. Do not reorder or insert.

This repository is not the RPC codec. Annotations, promises, and the
observation stream (`serialization/stream_protocol.hpp`) stay in
[rpc-cpp](https://github.com/daemonarium/rpc-cpp) `v0.3.0`.

## Contracts

| Header | CMake target | Role |
| --- | --- | --- |
| [kernel](docs/kernel.md) | `waos_kernel` | Host kernel: spawn, bind, monitor |
| [kernel_shield](docs/kernel-shield.md) | `waos_kernel_shield` | Guest-facing kernel |
| [db](docs/db.md) | `waos_db` | `key_value_store` and `key_store` |
| [fs](docs/fs.md) | `waos_fs` | Filesystem |
| [debug_store](docs/debug-store.md) | `waos_debug_store` | Debug session store |
| [recorder](docs/recorder.md) | `waos_recorder` | Process recorder |
| [observable_db](docs/observable-db.md) | `waos_observable_db` | Table coordinator |

`waos_debug_store` and `waos_kernel_shield` depend on `waos_kernel`.
`waos_observable_db` depends on `waos_db`.

## Use it

```cmake
include(FetchContent)
FetchContent_Declare(waos_contracts
  GIT_REPOSITORY https://github.com/daemonarium/contracts.git
  GIT_TAG v0.1.0
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(waos_contracts)

target_link_libraries(your_target PRIVATE waos_db)
```

```cpp
#include <waos/db.hpp>
```

The interface targets publish include directories for this repo and for
`rpc-cpp`. They do not link `libcpp_rpc`. Modules that already compile
`circular_buffer.c` into the wasm image keep doing that.

In the stack superbuild the directory is added after `rpc-cpp`, so nothing
is downloaded.

## Build the examples

Standalone configure compiles a header smoke test and
`dbg_forward_integration_ut`. A wasm configure (`-DWASM=ON`) builds the
registration modules under `examples/modules/`. A consumer
`add_subdirectory` builds neither.

Stock Clang cannot compile the headers. Use the
[reflection-toolchain](https://github.com/daemonarium/reflection-toolchain).

## License

MIT. See `LICENSE`.
