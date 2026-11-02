# debug_store

One writer for a debug session. `debug-store-cpp` implements it and stores
bytes through the filesystem process. `recorder-cpp` is the caller: `begin`
on socket 0, then a bind to the session socket.

Header: `include/waos/debug_store.hpp`. Target: `waos_debug_store` (depends
on `waos_kernel`, because `bindings_snapshot` stores
`kernel::socket_binding`).

There is no read API. Backpressure is the socket.

## debug_store methods

| id | method |
| --- | --- |
| 0 | `begin(request)` |

`begin` takes `name`, `process_id`, and `labels`. It returns `session_id`,
`store_process`, and `store_socket`.

## debug_session methods

| id | method |
| --- | --- |
| 0 | `set_labels(labels)` |
| 1 | `sync()` |
| 2 | `seal()` |
| 3 | `fail(error)` |

These are a second class, so their ids start at 0 again.

## On-disk records

`session_meta`, `snapshot_meta`, and `bindings_snapshot` are codec records
for the files in the session directory (`meta.bin`, `snap.<uid>.meta.bin`,
`bindings.bin`). They are not RPC methods.

## Events

`debug_snapshot_event`, `debug_io_event`, `debug_logs_event`,
`debug_spans_event`, `debug_metrics_event`, `debug_bindings_event`,
`debug_registry_event`, `debug_code_event`. The recorder sends these on the
session socket. The names are the wire messages, not methods on the class.

## Exceptions

`debug_error`, `debug_invalid_argument`, `debug_conflict`, `debug_closed`,
`debug_io_error`.

## Adding a method

Append it on the class that owns it (`debug_store` or `debug_session`).
A new on-disk record is a struct in this header, not a new method, unless
a caller has to invoke it.
