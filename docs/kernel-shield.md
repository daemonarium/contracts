# kernel_shield

Guest-facing kernel. Process 1 implements it in `kernel-shield-cpp`.
The header is the class guests reflect. The request context
(`kernel_shield_request_context`) and the `kernel_shield_api` object stay
in the implementation.

Header: `include/waos/kernel_shield.hpp`. Target: `waos_kernel_shield`
(depends on `waos_kernel`).

Shielded calls forward to `kernel` and update the process registry around
them. `request` and `spawn_native` exist only here.

## Methods

| id | method |
| --- | --- |
| 0 | `spawn(code, memory_limit, mana_limit)` |
| 1 | `resurect(code, memory, memory_limit, mana_limit)` |
| 2 | `spawn_isolated(code, memory_limit, mana_limit)` |
| 3 | `resurect_isolated(code, memory, memory_limit, mana_limit)` |
| 4 | `register_process(id, spawned_by, isolated)` |
| 5 | `unregister_process(id)` |
| 6 | `save(id)` |
| 7 | `kill(id)` |
| 8 | `bind(process_from, socket_from, process_to, socket_to, direction)` |
| 9 | `unbind(process_from, socket_from, process_to, socket_to, direction)` |
| 10 | `monitor(target, monitor, monitor_socket, config)` |
| 11 | `demonitor(target, monitor, monitor_socket)` |
| 12 | `measure_mana(process, new_mana)` |
| 13 | `set_mana_limit(process, mana_limit)` |
| 14 | `list_socket_bindings(id)` |
| 15 | `get_rpc_description(id, socket)` |
| 16 | `request(process_id, request)` |
| 17 | `spawn_native(service_type, data_dir, socket_count, mana_limit)` |

Append only. Ids 0–16 were already shipped before `spawn_native`.

## Types

Ids, `direction`, `save_result`, `monitor_config`, and `socket_binding`
come from [kernel](kernel.md). `spawn_native` uses `uint32` for
`socket_count` and `mana_limit`, same as the kernel method.

## Adding a method

Declare it at the end of `class kernel_shield` and define it in
`kernel_shield_api.cpp`. If it forwards to the kernel, the kernel method
has to exist first, also appended.
