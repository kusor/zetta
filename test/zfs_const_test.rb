require 'test/unit'
require 'libzfs'

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

  # Will hide all these constants behind proper ruby error classes
  def test_error_types
    assert_equal 2000, ZfsConsts::Errors::NOMEM
    assert_equal 2001, ZfsConsts::Errors::BADPROP
    assert_equal 2002, ZfsConsts::Errors::PROPREADONLY
    assert_equal 2003, ZfsConsts::Errors::PROPTYPE
    assert_equal 2004, ZfsConsts::Errors::PROPNONINHERIT
    assert_equal 2005, ZfsConsts::Errors::PROPSPACE
    assert_equal 2006, ZfsConsts::Errors::BADTYPE
    assert_equal 2007, ZfsConsts::Errors::BUSY
    assert_equal 2008, ZfsConsts::Errors::EXISTS
    assert_equal 2009, ZfsConsts::Errors::NOENT
    assert_equal 2010, ZfsConsts::Errors::BADSTREAM
    assert_equal 2011, ZfsConsts::Errors::DSREADONLY
    assert_equal 2012, ZfsConsts::Errors::VOLTOOBIG
    assert_equal 2013, ZfsConsts::Errors::VOLHASDATA
    assert_equal 2014, ZfsConsts::Errors::INVALIDNAME
    assert_equal 2015, ZfsConsts::Errors::BADRESTORE
    assert_equal 2016, ZfsConsts::Errors::BADBACKUP
    assert_equal 2017, ZfsConsts::Errors::BADTARGET
    assert_equal 2018, ZfsConsts::Errors::NODEVICE
    assert_equal 2019, ZfsConsts::Errors::BADDEV
    assert_equal 2020, ZfsConsts::Errors::NOREPLICAS
    assert_equal 2021, ZfsConsts::Errors::RESILVERING
    assert_equal 2022, ZfsConsts::Errors::BADVERSION
    assert_equal 2023, ZfsConsts::Errors::POOLUNAVAIL
    assert_equal 2024, ZfsConsts::Errors::DEVOVERFLOW
    assert_equal 2025, ZfsConsts::Errors::BADPATH
    assert_equal 2026, ZfsConsts::Errors::CROSSTARGET
    assert_equal 2027, ZfsConsts::Errors::ZONED
    assert_equal 2028, ZfsConsts::Errors::MOUNTFAILED
    assert_equal 2029, ZfsConsts::Errors::UMOUNTFAILED
    assert_equal 2030, ZfsConsts::Errors::UNSHARENFSFAILED
    assert_equal 2031, ZfsConsts::Errors::SHARENFSFAILED
    assert_equal 2032, ZfsConsts::Errors::DEVLINKS
    assert_equal 2033, ZfsConsts::Errors::PERM
    assert_equal 2034, ZfsConsts::Errors::NOSPC
    assert_equal 2035, ZfsConsts::Errors::IO
    assert_equal 2036, ZfsConsts::Errors::INTR
    assert_equal 2037, ZfsConsts::Errors::ISSPARE
    assert_equal 2038, ZfsConsts::Errors::INVALCONFIG
    assert_equal 2039, ZfsConsts::Errors::RECURSIVE
    assert_equal 2040, ZfsConsts::Errors::NOHISTORY
    assert_equal 2041, ZfsConsts::Errors::UNSHAREISCSIFAILED
    assert_equal 2042, ZfsConsts::Errors::SHAREISCSIFAILED
    assert_equal 2043, ZfsConsts::Errors::POOLPROPS
    assert_equal 2044, ZfsConsts::Errors::POOL_NOTSUP
    assert_equal 2045, ZfsConsts::Errors::POOL_INVALARG
    assert_equal 2046, ZfsConsts::Errors::NAMETOOLONG
    # This is a clear indication that defining errors this way is an error.
    # EZFS_UNKNOWN changes when new errors are added. The right approach is
    # to define the proper error classes in ruby, and raise those errors
    # after performing the error type check in C.
    # Known to probably fail test:
    assert_equal 2047, ZfsConsts::Errors::UNKNOWN
  end

  # FIXME: Do the status check using C, and define ruby constants
  # without any dependency to the Integer values assigned to the
  # C constants
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
    # From here, known it might fail:
    assert_equal 11, ZfsConsts::HealthStatus::VERSION_OLDER
    assert_equal 12, ZfsConsts::HealthStatus::RESILVERING
    assert_equal 13, ZfsConsts::HealthStatus::OFFLINE_DEV
    assert_equal 14, ZfsConsts::HealthStatus::OK
  end

  # Is there any need to define Zpool state as a numeric constant instead of
  # just return the proper human readable representation, and do the state
  # check using C?
  def test_pool_states
    assert_equal 0, ZfsConsts::State::Pool::ACTIVE
    assert_equal 1, ZfsConsts::State::Pool::EXPORTED
    assert_equal 2, ZfsConsts::State::Pool::DESTROYED
    assert_equal 3, ZfsConsts::State::Pool::SPARE
    # From here, known it might fail:
    assert_equal 4, ZfsConsts::State::Pool::UNINITIALIZED
    assert_equal 5, ZfsConsts::State::Pool::UNAVAIL
    assert_equal 6, ZfsConsts::State::Pool::POTENTIALLY_ACTIVE
  end
end
