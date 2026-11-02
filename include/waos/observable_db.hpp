#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include <waos/db.hpp>
#include <serialization/promise.hpp>

struct table_get_result {
  bool present = false;
  waos::binary value;
  std::uint64_t timestamp = 0;
  std::uint8_t v = 0;
};

struct table_list_entry {
  std::uint32_t table_id = 0;
  std::string name;
  std::uint8_t current_v = 0;
  std::uint64_t tr_gen = 0;
  std::uint64_t type_id = 0;
};

class observable_db {
  public:
    // Phase 0: echo `msg`, forward get(missing) to native-db and ping to
    // example-schema. Result encodes all three hops:
    //   "<msg>|db_present=<0|1>|schema=<schema-echo>"
    static std::shared_ptr<serialization::promise<std::string>> ping(std::string msg);

    static std::shared_ptr<serialization::promise<table_get_result>> table_get(
        std::uint32_t table_id, waos::binary object_id);

    static std::shared_ptr<serialization::promise<void>> table_put(
        std::uint32_t table_id, waos::binary object_id, waos::binary payload,
        std::uint64_t timestamp);

    static std::shared_ptr<serialization::promise<void>> table_delete(
        std::uint32_t table_id, waos::binary object_id, std::uint64_t timestamp);

    // flags=none: finite scan (stream_done closes). flags=observe: live watch.
    [[=serialization::observable()]]
    static std::shared_ptr<serialization::promise<void>> table(
        waos::stream_handle stream,
        std::uint32_t table_id,
        waos::binary lower_id,
        waos::db_bound_mode lower_mode,
        waos::binary upper_id,
        waos::db_bound_mode upper_mode,
        std::uint64_t limit,
        bool reverse,
        bool keys_only);

    static std::shared_ptr<serialization::promise<void>> table_set_version(
        std::uint32_t table_id, std::uint8_t v, std::uint64_t tr_gen, std::uint64_t type_id);

    static std::shared_ptr<serialization::promise<void>> catalog_boot(
        std::uint32_t schema_pid);

    static std::shared_ptr<serialization::promise<std::uint32_t>> catalog_create_table(
        std::string name, std::uint64_t tr_gen, std::uint64_t type_id);

    static std::shared_ptr<serialization::promise<void>> catalog_rename_table(
        std::uint32_t table_id, std::string new_name);

    static std::shared_ptr<serialization::promise<std::uint32_t>> catalog_resolve_table(
        std::string name);

    static std::shared_ptr<serialization::promise<void>> catalog_schema_bump(
        std::uint32_t table_id, std::uint8_t new_v, std::uint64_t tr_gen,
        std::uint64_t type_id);

    // Drop a user table: tombstone catalog keys, append `{table_id, ts}` to
    // head.deleted_tables, error active table streams, emit a list tombstone
    // (empty value), then mark_prefix_min_ts(table prefix, UINT64_MAX).
    // table_id 0 (catalog) and unknown ids throw.
    static std::shared_ptr<serialization::promise<void>> catalog_delete_table(
        std::uint32_t table_id);

    // flags=none: catalog snapshot then stream_done. flags=observe: live list.
    [[=serialization::observable()]]
    static std::shared_ptr<serialization::promise<void>> table_list(
        waos::stream_handle stream);
};
