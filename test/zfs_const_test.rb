require 'test/unit'
require 'zetta'

class ZsfConstTest < Test::Unit::TestCase
  def test_types
    assert_equal 1, ZfsConsts::Types::FILESYSTEM
    assert_equal 2, ZfsConsts::Types::SNAPSHOT
    assert_equal 4, ZfsConsts::Types::VOLUME
    assert_equal 8, ZfsConsts::Types::POOL
    assert_equal ZfsConsts::Types::ANY, ZfsConsts::Types::DATASET
    assert ZfsConsts::Types::ANY == ZfsConsts::Types::FILESYSTEM | ZfsConsts::Types::SNAPSHOT | ZfsConsts::Types::VOLUME
    assert ZfsConsts::Types::DATASET == ZfsConsts::Types::FILESYSTEM | ZfsConsts::Types::SNAPSHOT | ZfsConsts::Types::VOLUME
  end

  def test_pool_health_status
    assert_equal 0, ZfsConsts::HealthStatus::CORRUPT_CACHE
    assert_equal 1, ZfsConsts::HealthStatus::MISSING_DEV_R
    assert_equal 2, ZfsConsts::HealthStatus::MISSING_DEV_NR
    assert_equal 3, ZfsConsts::HealthStatus::CORRUPT_LABEL_R
    assert_equal 4, ZfsConsts::HealthStatus::CORRUPT_LABEL_NR
    assert_equal 5, ZfsConsts::HealthStatus::BAD_GUID_SUM
    assert_equal 6, ZfsConsts::HealthStatus::CORRUPT_POOL
    assert_equal 7, ZfsConsts::HealthStatus::CORRUPT_DATA
    assert_equal 8, ZfsConsts::HealthStatus::FAILING_DEV
    assert_equal 9, ZfsConsts::HealthStatus::VERSION_NEWER
    assert_equal 10, ZfsConsts::HealthStatus::HOSTID_MISMATCH
    assert_equal 11, ZfsConsts::HealthStatus::IO_FAILURE_WAIT
    assert_equal 12, ZfsConsts::HealthStatus::IO_FAILURE_CONTINUE
    assert_equal 13, ZfsConsts::HealthStatus::BAD_LOG
    assert_equal 14, ZfsConsts::HealthStatus::FAULTED_DEV_R
    assert_equal 15, ZfsConsts::HealthStatus::FAULTED_DEV_NR
    assert_equal 16, ZfsConsts::HealthStatus::VERSION_OLDER
    assert_equal 17, ZfsConsts::HealthStatus::RESILVERING
    assert_equal 18, ZfsConsts::HealthStatus::OFFLINE_DEV
    assert_equal 19, ZfsConsts::HealthStatus::REMOVED_DEV
    assert_equal 20, ZfsConsts::HealthStatus::OK
    assert_equal 21, ZfsConsts::HealthStatus::UNKNOWN
  end

  def test_pool_states
    assert_equal 0, ZfsConsts::State::Pool::ACTIVE
    assert_equal 1, ZfsConsts::State::Pool::EXPORTED
    assert_equal 2, ZfsConsts::State::Pool::DESTROYED
    assert_equal 3, ZfsConsts::State::Pool::SPARE
    assert_equal 4, ZfsConsts::State::Pool::UNINITIALIZED
    assert_equal 5, ZfsConsts::State::Pool::UNAVAIL
    assert_equal 6, ZfsConsts::State::Pool::POTENTIALLY_ACTIVE
    assert_equal 7, ZfsConsts::State::Pool::UNKNOWN
    assert_equal 8, ZfsConsts::State::Pool::L2CACHE
  end
end
