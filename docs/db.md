# db

Storage process. One logical process owns one store, either a
`key_value_store` or a `key_store`. Both modes register the full class.
The wrong-mode methods throw `db_invalid_argument`.

Implemented by `db-cpp` (native) and `kv-cpp` (wasm memory). `observable-db`
and guests call it. The native host loads it when `NATIVE_HOST_DB` is on.

Header: `include/waos/db.hpp`. Target: `waos_db`.

Scans are not a private message set. `start_scan` and `key_start_scan` are
`streamable` and speak `serialization/stream_protocol.hpp` from `rpc-cpp`:
`socket_opened`, `stream_item`, `stream_key`, `stream_done`, `stream_error`,
`stream_reset`, `stream_opened`. An empty value is a tombstone.

## Methods

| id | method | mode |
| --- | --- | --- |
| 0 | `get(key)` | kv |
| 1 | `set(key, value, timestamp, opts)` | kv |
| 2 | `batch(entries, opts)` | kv |
| 3 | `open_socket(channel)` | both |
| 4 | `close_socket(channel)` | both |
| 5 | `start_scan(stream, query)` | kv |
| 6 | `cancel_scan(stream_id)` | kv |
| 7 | `mark_prefix_min_ts(prefix, min_ts)` | kv |
| 8 | `max_timestamp()` | kv |
| 9 | `key_get(key, min_ts)` | key |
| 10 | `key_set(key, timestamp, opts)` | key |
| 11 | `key_delete(key, timestamp, opts)` | key |
| 12 | `key_batch(entries, opts)` | key |
| 13 | `key_start_scan(stream, query)` | key |
| 14 | `key_cancel_scan(stream_id)` | key |
| 15 | `key_mark_prefix_min_ts(prefix, min_ts)` | key |
| 16 | `key_max_timestamp()` | key |

Ids 0–7 are the original kv surface. 8–16 were appended.

## Types

`db_get_result`: `present=false` means missing. `present=true` and an empty
value is a tombstone.

`key_get_result`: `present=false` covers a missing key and a tombstone.
`min_ts` hides entries older than that timestamp.

`db_write_mode`: `durable` or `instant`.

`db_write_options`: mode plus `notify_visible` and `notify_durable`.

`db_bound_mode`: `none` (bound absent), `exclusive`, `inclusive`.

`db_scan_query`: lower, lower mode, upper, upper mode, limit, reverse,
`keys_only`. A key-store scan ignores `keys_only`. Live keys are
`stream_key`. Tombstones are `stream_item` with an empty value.

`key_write_entry`: `live=false` writes a tombstone.

`db_visible_event` and `db_durable_event` carry the `request_id` of a write
when the matching notify flag is set.

## Exceptions

`db_error`, `db_invalid_argument`, `db_closed`, `db_io_error`.

## Adding a method

Append it on `class db`. Say which mode accepts it. A kv change and a
key-store change share this header, so one tag of this repository covers
both.
