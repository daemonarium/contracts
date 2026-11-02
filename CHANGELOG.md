# Changelog

## 0.1.0

Contracts moved out of `rpc-cpp` `src/examples/structures`.

- `kernel`, `kernel_shield`, `db`, `fs`, `debug_store`, `recorder`,
  `observable_db`.
- `kernel.spawn_native` is method 12, matching `kernel_rpc` and `kernel-ts`.
- Requires `rpc-cpp` `v0.3.0` for `serialization/stream_protocol.hpp`.
