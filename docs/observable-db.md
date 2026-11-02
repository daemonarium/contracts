# observable_db

Coordinator in front of a `db` process. `observable-db-cpp` implements it.
Guests talk to tables. The coordinator talks to `key_value_store` and
`key_store` through [db](db.md).

Header: `include/waos/observable_db.hpp`. Target: `waos_observable_db`
(depends on `waos_db`).

`table` and `table_list` are `observable`. A finite call (`flags=none`) ends
with `stream_done`. An observe call stays open after catch-up. The stream
messages themselves are the rpc stream protocol, shared with `db` scans.

## Methods

| id | method |
| --- | --- |
| 0 | `ping(msg)` |
| 1 | `table_get(table_id, object_id)` |
| 2 | `table_put(table_id, object_id, payload, timestamp)` |
| 3 | `table_delete(table_id, object_id, timestamp)` |
| 4 | `table(stream, table_id, lower_id, lower_mode, upper_id, upper_mode, limit, reverse, keys_only)` |
| 5 | `table_set_version(table_id, v, tr_gen, type_id)` |
| 6 | `catalog_boot(schema_pid)` |
| 7 | `catalog_create_table(name, tr_gen, type_id)` |
| 8 | `catalog_rename_table(table_id, new_name)` |
| 9 | `catalog_resolve_table(name)` |
| 10 | `catalog_schema_bump(table_id, new_v, tr_gen, type_id)` |
| 11 | `catalog_delete_table(table_id)` |
| 12 | `table_list(stream)` |

`ping` is the phase-0 probe. It echoes `msg`, forwards a missing get to
native-db, and pings the example schema. The result is
`"<msg>|db_present=<0|1>|schema=<schema-echo>"`.

`catalog_delete_table` tombstones the catalog keys, appends
`{table_id, ts}` to `head.deleted_tables`, errors active table streams,
emits a list tombstone, then `mark_prefix_min_ts` on the table prefix with
`UINT64_MAX`. Table 0 is the catalog and cannot be deleted.

## Types

`table_get_result`: `present`, `value`, `timestamp`, `v`.

`table_list_entry`: `table_id`, `name`, `current_v`, `tr_gen`, `type_id`.

Bounds use `waos::db_bound_mode` and `waos::binary` from the db contract.

## Adding a method

Append it on `class observable_db` and implement it in
`observable-db-cpp`. A change to scan items belongs on the db contract or
the rpc stream protocol, not as a parallel message type here.
