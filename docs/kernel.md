# kernel

Host kernel process. Guests do not call this header. `kernel-shield` does,
through `kernel_rpc`. The TypeScript implementation is `kernel-ts`
(`kernelApi.ts`).

Header: `include/waos/kernel.hpp`. Target: `waos_kernel`.

## Widths

The class uses `size_t` for process ids, socket ids, and limits. The shipped
wire in `kernel_rpc.hpp` and `kernelApi.ts` uses `uint32` for those fields.
Do not regenerate `kernelApi.ts` from this header until that difference is
closed on purpose. `spawn_native` takes `uint32` counts so it matches the
method already on the wire.

## Methods

| id | method |
| --- | --- |
| 0 | `spawn(code, memory_limit, mana_limit)` |
| 1 | `resurect(code, memory, memory_limit, mana_limit)` |
| 2 | `save(id)` |
| 3 | `kill(id)` |
| 4 | `bind(process_from, socket_from, process_to, socket_to, direction)` |
| 5 | `unbind(process_from, socket_from, process_to, socket_to, direction)` |
| 6 | `monitor(target, monitor, monitor_socket, config)` |
| 7 | `demonitor(target, monitor, monitor_socket)` |
| 8 | `measure_mana(process, new_mana)` |
| 9 | `set_mana_limit(process, mana_limit)` |
| 10 | `list_socket_bindings(id)` |
| 11 | `get_rpc_description(id, socket)` |
| 12 | `spawn_native(service_type, data_dir, socket_count, mana_limit)` |

Append only. The next method is 13.

## Types

`monitor_flag` is a bit set: `KILLED`, `ERROR`, `COPY`, `COPY_PAYLOAD`,
`DATA`, `DRAIN`, `SAVE`, `MEMORY`, `INITIAL_BUFFER_STATE`,
`INITIAL_BUFFER_DATA`, `BUFFER_STATE`, `BUFFER_DATA`, `MANA`, `BIND`.

`direction` is `NONE`, `IN`, `OUT`, `BOTH`.

`socket_binding` is one end of a bind: local socket, peer process, peer
socket, direction.

`save_result` carries `snapshot_uid`, `memory`, `mana_used`, `mana_limit`.

`process_*_event` messages are the monitor stream. They are not methods.
`process_event_pointer` boxes one event for the polymorphic monitor socket.

## Adding a method

Add it at the end of `class kernel`, with a stub that returns a resolved
promise. Document the id here. Update `kernel_rpc` and `kernelApi.ts` in the
same change if the wire should grow.
