#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <exception>
#include "serialization/annotations.hpp"
#include "serialization/promise.hpp"
#include "serialization/stream_protocol.hpp"

namespace waos {

  enum class db_write_mode : std::uint8_t {
    durable = 0,
    instant = 1,
  };

  // present=false ⇒ missing key; present=true + empty value ⇒ tombstone.
  struct db_get_result {
    bool present = false;
    binary value;
    std::uint64_t timestamp = 0;
  };

  struct db_write_options {
    db_write_mode mode = db_write_mode::durable;
    bool notify_visible = false;
    bool notify_durable = false;
  };

  // empty value ⇒ tombstone (same as store set / set_bulk).
  struct db_write_entry {
    binary key;
    binary value;
    std::uint64_t timestamp = 0;
  };

  // none ⇒ bound absent (key ignored); exclusive ⇒ open (gt/lt); inclusive ⇒ closed (gte/lte).
  enum class db_bound_mode : std::uint8_t {
    none = 0,
    exclusive = 1,
    inclusive = 2,
  };

  struct db_scan_query {
    binary lower;
    db_bound_mode lower_mode = db_bound_mode::none;
    binary upper;
    db_bound_mode upper_mode = db_bound_mode::none;
    std::uint64_t limit = 0;
    bool reverse = false;
    bool keys_only = false;
  };

  // key_store point read. present=false means missing or tombstone.
  struct key_get_result {
    bool present = false;
    std::uint64_t timestamp = 0;
  };

  // key_store batch entry. live=false writes a tombstone.
  struct key_write_entry {
    binary key;
    std::uint64_t timestamp = 0;
    bool live = true;
  };

  struct [[=serialization::exception()]] db_error : public std::exception {
    db_error() : message_() {}
    explicit db_error(const std::string& message) : message_(message) {}
    ~db_error() override {}
    const char* what() const noexcept override { return message_.c_str(); }
  private:
    std::string message_;
  };

  struct [[=serialization::exception()]] db_invalid_argument : public db_error {
    db_invalid_argument() = default;
    explicit db_invalid_argument(const std::string& message) : db_error(message) {}
  };

  struct [[=serialization::exception()]] db_closed : public db_error {
    db_closed() = default;
    explicit db_closed(const std::string& message) : db_error(message) {}
  };

  struct [[=serialization::exception()]] db_io_error : public db_error {
    db_io_error() = default;
    explicit db_io_error(const std::string& message) : db_error(message) {}
  };

  struct [[=serialization::message()]] db_visible_event {
    std::uint64_t request_id = 0;
  };

  struct [[=serialization::message()]] db_durable_event {
    std::uint64_t request_id = 0;
  };

  // Full API on control USER socket and on every private channel (open_socket).
  class db {
    public:
      db() = default;

      static std::shared_ptr<serialization::promise<db_get_result>> get(binary key) {
        return serialization::promise<db_get_result>::resolved(db_get_result{});
      }

      // empty value ⇒ tombstone.
      static std::shared_ptr<serialization::promise<void>> set(
          binary key, binary value, std::uint64_t timestamp, db_write_options opts) {
        return serialization::promise<void>::resolved();
      }

      static std::shared_ptr<serialization::promise<void>> batch(
          std::vector<db_write_entry> entries, db_write_options opts) {
        return serialization::promise<void>::resolved();
      }

      static std::shared_ptr<serialization::promise<void>> open_socket(channel_handle channel) {
        return serialization::promise<void>::resolved();
      }

      static std::shared_ptr<serialization::promise<void>> close_socket(channel_handle channel) {
        return serialization::promise<void>::resolved();
      }

      [[=serialization::streamable()]]
      static std::shared_ptr<serialization::promise<void>> start_scan(
          stream_handle stream, db_scan_query query) {
        return serialization::promise<void>::resolved();
      }

      static std::shared_ptr<serialization::promise<void>> cancel_scan(std::uint64_t stream_id) {
        return serialization::promise<void>::resolved();
      }

      // Register a retention rule for a key prefix: compaction drops entries
      // under `prefix` with timestamp < `min_ts`. Idempotent; re-calling with
      // the same prefix replaces the rule. `min_ts = UINT64_MAX` => drop all
      // (table delete). kv-storage-mem sweeps immediately.
      static std::shared_ptr<serialization::promise<void>> mark_prefix_min_ts(
          binary prefix, std::uint64_t min_ts) {
        return serialization::promise<void>::resolved();
      }

      // Highest timestamp that entered this key_value_store. 0 if empty.
      // Appended after the original methods so existing function ids stay put.
      static std::shared_ptr<serialization::promise<std::uint64_t>> max_timestamp() {
        return serialization::promise<std::uint64_t>::resolved(0);
      }

      // key_store process (presence-only). One process owns one key_store.
      // present=false covers a missing key and a tombstone.
      // min_ts: entries with timestamp < min_ts are treated as absent.
      static std::shared_ptr<serialization::promise<key_get_result>> key_get(
          binary key, std::uint64_t min_ts) {
        (void)key;
        (void)min_ts;
        return serialization::promise<key_get_result>::resolved(key_get_result{});
      }

      static std::shared_ptr<serialization::promise<void>> key_set(
          binary key, std::uint64_t timestamp, db_write_options opts) {
        (void)key;
        (void)timestamp;
        (void)opts;
        return serialization::promise<void>::resolved();
      }

      static std::shared_ptr<serialization::promise<void>> key_delete(
          binary key, std::uint64_t timestamp, db_write_options opts) {
        (void)key;
        (void)timestamp;
        (void)opts;
        return serialization::promise<void>::resolved();
      }

      static std::shared_ptr<serialization::promise<void>> key_batch(
          std::vector<key_write_entry> entries, db_write_options opts) {
        (void)entries;
        (void)opts;
        return serialization::promise<void>::resolved();
      }

      // Presence scan. keys_only on the query is ignored.
      // Live keys are stream_key. Tombstones are stream_item with an empty value.
      [[=serialization::streamable()]]
      static std::shared_ptr<serialization::promise<void>> key_start_scan(
          stream_handle stream, db_scan_query query) {
        (void)stream;
        (void)query;
        return serialization::promise<void>::resolved();
      }

      static std::shared_ptr<serialization::promise<void>> key_cancel_scan(
          std::uint64_t stream_id) {
        (void)stream_id;
        return serialization::promise<void>::resolved();
      }

      // Retention for a key_store prefix. Native key_store has no compaction
      // retention yet (stub). kv-key-store-mem sweeps immediately.
      static std::shared_ptr<serialization::promise<void>> key_mark_prefix_min_ts(
          binary prefix, std::uint64_t min_ts) {
        (void)prefix;
        (void)min_ts;
        return serialization::promise<void>::resolved();
      }

      static std::shared_ptr<serialization::promise<std::uint64_t>> key_max_timestamp() {
        return serialization::promise<std::uint64_t>::resolved(0);
      }
  };

}
