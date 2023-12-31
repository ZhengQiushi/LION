//
// Created by Yi Lu on 9/14/18.
//

#pragma once

#include <glog/logging.h>

namespace star {

class HermesRWKey {
public:
  // local index read bit

  void set_local_index_read_bit() {
    clear_local_index_read_bit();
    bitvec |= LOCAL_INDEX_READ_BIT_MASK << LOCAL_INDEX_READ_BIT_OFFSET;
  }

  void clear_local_index_read_bit() {
    bitvec &= ~(LOCAL_INDEX_READ_BIT_MASK << LOCAL_INDEX_READ_BIT_OFFSET);
  }

  uint32_t get_local_index_read_bit() const {
    return (bitvec >> LOCAL_INDEX_READ_BIT_OFFSET) & LOCAL_INDEX_READ_BIT_MASK;
  }

  // read lock bit

  void set_read_lock_bit() {
    clear_read_lock_bit();
    bitvec |= READ_LOCK_BIT_MASK << READ_LOCK_BIT_OFFSET;
  }

  void clear_read_lock_bit() {
    bitvec &= ~(READ_LOCK_BIT_MASK << READ_LOCK_BIT_OFFSET);
  }

  uint32_t get_read_lock_bit() const {
    return (bitvec >> READ_LOCK_BIT_OFFSET) & READ_LOCK_BIT_MASK;
  }

  // write lock bit

  void set_write_lock_bit() {
    clear_write_lock_bit();
    bitvec |= WRITE_LOCK_BIT_MASK << WRITE_LOCK_BIT_OFFSET;
  }

  void clear_write_lock_bit() {
    bitvec &= ~(WRITE_LOCK_BIT_MASK << WRITE_LOCK_BIT_OFFSET);
  }

  uint32_t get_write_lock_bit() const {
    return (bitvec >> WRITE_LOCK_BIT_OFFSET) & WRITE_LOCK_BIT_MASK;
  }

  // prepare processed bit

  void set_prepare_processed_bit() {
    clear_prepare_processed_bit();
    bitvec |= PREPARE_PROCESSED_BIT_MASK << PREPARE_PROCESSED_BIT_OFFSET;
  }

  void clear_prepare_processed_bit() {
    bitvec &= ~(PREPARE_PROCESSED_BIT_MASK << PREPARE_PROCESSED_BIT_OFFSET);
  }

  uint32_t get_prepare_processed_bit() const {
    return (bitvec >> PREPARE_PROCESSED_BIT_OFFSET) &
           PREPARE_PROCESSED_BIT_MASK;
  }

  // execution processed bit

  void set_execution_processed_bit() {
    clear_execution_processed_bit();
    bitvec |= EXECUTION_PROCESSED_BIT_MASK << EXECUTION_PROCESSED_BIT_OFFSET;
  }

  void clear_execution_processed_bit() {
    bitvec &= ~(EXECUTION_PROCESSED_BIT_MASK << EXECUTION_PROCESSED_BIT_OFFSET);
  }

  uint32_t get_execution_processed_bit() const {
    return (bitvec >> EXECUTION_PROCESSED_BIT_OFFSET) &
           EXECUTION_PROCESSED_BIT_MASK;
  }

  // table id

  void set_table_id(uint32_t table_id) {
    DCHECK(table_id < (1 << 5));
    clear_table_id();
    bitvec |= table_id << TABLE_ID_OFFSET;
  }

  void clear_table_id() { bitvec &= ~(TABLE_ID_MASK << TABLE_ID_OFFSET); }

  uint32_t get_table_id() const {
    return (bitvec >> TABLE_ID_OFFSET) & TABLE_ID_MASK;
  }
  // partition id

  void set_partition_id(uint32_t partition_id) {
    DCHECK(partition_id < (1 << 8));
    clear_partition_id();
    bitvec |= partition_id << PARTITION_ID_OFFSET;
  }

  void clear_partition_id() {
    bitvec &= ~(PARTITION_ID_MASK << PARTITION_ID_OFFSET);
  }

  uint32_t get_partition_id() const {
    return (bitvec >> PARTITION_ID_OFFSET) & PARTITION_ID_MASK;
  }

  // master coordinator id
  void set_master_coordinator_id(uint32_t coordinator_id) {
    DCHECK(coordinator_id < (1 << 5));
    clear_master_coordinator_id();
    bitvec |= coordinator_id << MASTER_COORDINATOR_ID_OFFSET;
  }

  void clear_master_coordinator_id() { bitvec &= ~(MASTER_COORDINATOR_ID_MASK << MASTER_COORDINATOR_ID_OFFSET); }

  uint32_t get_master_coordinator_id() const {
    return (bitvec >> MASTER_COORDINATOR_ID_OFFSET) & MASTER_COORDINATOR_ID_MASK;
  }

  // secondary coordinator id
  void set_secondary_coordinator_id(uint32_t coordinator_id) {
    DCHECK(coordinator_id < (1 << 5)) << coordinator_id;
    clear_secondary_coordinator_id();
    bitvec |= coordinator_id << SECONDARY_COORDINATOR_ID_OFFSET;
  }

  void clear_secondary_coordinator_id() { bitvec &= ~(SECONDARY_COORDINATOR_ID_MASK << SECONDARY_COORDINATOR_ID_OFFSET); }

  uint32_t get_secondary_coordinator_id() const {
    return (bitvec >> SECONDARY_COORDINATOR_ID_OFFSET) & SECONDARY_COORDINATOR_ID_MASK;
  }

  // key
  void set_key(const void *key) { this->key = key; }

  const void *get_key() const { return key; }

  // value
  void set_value(void *value) { this->value = value; }

  void *get_value() const { return value; }

private:
  /*
   * A bitvec is a 32-bit word.
   *
   * [ table id (5) ] | partition id (8) | 
   *   master coordinator id (5) | secondary coordinator id (5) | unused bit (4) |
   * prepare processed bit (1) | execute processed bit(1) |
   * write lock bit(1) | read lock bit (1) | local index read (1)  ]
   *
   * local index read  is set when the read is from a local read only index.
   * write lock bit is set when a write lock is acquired.
   * read lock bit is set when a read lock is acquired.
   * prepare processed bit is set when process_request has processed this key in
   * prepare phase exucution processed bit is set when process_request has
   * processed this key in execution phase
   */

  uint32_t bitvec = 0;
  const void *key = nullptr;
  void *value = nullptr;

public:
  static constexpr uint32_t TABLE_ID_MASK = 0x1f;
  static constexpr uint32_t TABLE_ID_OFFSET = 27;

  static constexpr uint32_t PARTITION_ID_MASK = 0xff;
  static constexpr uint32_t PARTITION_ID_OFFSET = 19;

  static constexpr uint32_t MASTER_COORDINATOR_ID_MASK = 0x1f;
  static constexpr uint32_t MASTER_COORDINATOR_ID_OFFSET = 14;

  static constexpr uint32_t SECONDARY_COORDINATOR_ID_MASK = 0x1f;
  static constexpr uint32_t SECONDARY_COORDINATOR_ID_OFFSET = 9;

  static constexpr uint32_t EXECUTION_PROCESSED_BIT_MASK = 0x1;
  static constexpr uint32_t EXECUTION_PROCESSED_BIT_OFFSET = 4;

  static constexpr uint32_t PREPARE_PROCESSED_BIT_MASK = 0x1;
  static constexpr uint32_t PREPARE_PROCESSED_BIT_OFFSET = 3;

  static constexpr uint32_t WRITE_LOCK_BIT_MASK = 0x1;
  static constexpr uint32_t WRITE_LOCK_BIT_OFFSET = 2;

  static constexpr uint32_t READ_LOCK_BIT_MASK = 0x1;
  static constexpr uint32_t READ_LOCK_BIT_OFFSET = 1;

  static constexpr uint32_t LOCAL_INDEX_READ_BIT_MASK = 0x1;
  static constexpr uint32_t LOCAL_INDEX_READ_BIT_OFFSET = 0;
};
} // namespace star