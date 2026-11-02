# recorder

Records one target process. `recorder-cpp` implements it. A client calls
`start_recording` on socket 0. The recorder watches the target through
kernel-shield and writes the session through debug-store.

Header: `include/waos/recorder.hpp`. Target: `waos_recorder`.

The implementation also includes the kernel and debug-store contracts. This
header does not.

## Methods

| id | method |
| --- | --- |
| 0 | `start_recording(request)` |
| 1 | `stop_recording(recording_id)` |
| 2 | `dump(recording_id)` |
| 3 | `dump_and_clear(recording_id)` |
| 4 | `rotate(recording_id)` |

`start_recording` takes the target process, the debug-store process, a
session name, labels, `record_config`, and the target wasm bytes (`code`,
empty for a local or soft-empty process).

It returns `recording_id`, `monitor_socket`, `log_socket`,
`store_session_id`, and `store_socket`.

`dump` persists the current window and keeps it. `dump_and_clear` persists
it and clears `io_bytes`.

## Types

`record_config.event_buffer_snapshot_mult` is 1 or 2. The threshold is that
multiplier times the anchor memory size. `record_mana` and `dump_on_error`
are the other switches.

## Exceptions

`recorder_error`, `recorder_invalid_argument`, `recorder_not_found`,
`recorder_conflict`, `recorder_closed`.

## Adding a method

Append it on `class recorder` and implement it in `recorder-cpp`. Monitor
events stay on the kernel contract. Session records stay on debug-store.
