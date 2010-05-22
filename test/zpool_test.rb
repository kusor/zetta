require 'test/unit'
require 'zetta'

# In order to run the tests on this file need to do:
#
# sudo mkdir /export/vdev
# sudo mkfile 128m /export/vdev/d1
# sudo zpool create tpool /export/vdev/d1

# Might have sense to add 'File.exists?('/tpool')' check.

class ZpoolTest < Test::Unit::TestCase
  def setup
    @zlib = LibZfs.new
  end

  def test_initialize
    @zpool = Zpool.new('tpool', @zlib)
    assert_not_nil @zpool
    assert_equal 'tpool', @zpool.name
    assert_kind_of LibZfs, @zpool.libzfs_handle
  end

  def test_initialize_with_symbol
    @zpool = Zpool.new(:tpool, @zlib)
    assert_not_nil @zpool
    assert_equal :tpool.id2name, @zpool.name
    assert_kind_of LibZfs, @zpool.libzfs_handle
  end

  def test_initialize_unexistent
    # It's probably a good idea to completely forget error message check here:
    assert_raise(ZfsError::NoentError, "cannot open 'fakepool': no such pool") {
      @zpool = Zpool.new('fakepool', @zlib)
    }
    assert_not_equal 0, @zlib.errno
    # WARN: This is actually testing strings which might change on C,
    # and can be localized, depending on system locale settings.
    assert_equal "cannot open 'fakepool'", @zlib.error_action
    assert_equal "no such pool", @zlib.error_description
  end

  def initialize_without_handle
    @zpool = Zpool.new('tpool')
    assert_not_nil @zpool
    assert_equal 'tpool', @zpool.name
    assert_kind_of LibZfs, @zpool.libzfs_handle
  end

  def test_initialize_with_wrong_arguments
    # At least one argument is mandatory:
    assert_raise(ArgumentError) { @zpool = Zpool.new }
    # Given the first argument, it must be a string or symbol
    assert_raise(TypeError) { @zpool = Zpool.new(1234) }
    # Given the second argument, it must be an instance of LibZfs
    assert_raise(TypeError) { @zpool = Zpool.new('fakepool', Array.new) }
  end

  def test_zpool_guid
    @zpool = Zpool.new('tpool', @zlib)
    assert_kind_of Integer, @zpool.guid
  end

  def test_zpool_version
    @zpool = Zpool.new('tpool', @zlib)
    assert_kind_of Integer, @zpool.version
  end

  def test_zpool_state
    @zpool = Zpool.new('tpool', @zlib)
    assert_equal ZfsConsts::State::Pool::ACTIVE, @zpool.state
  end

  def test_zpool_health_status
    @zpool = Zpool.new('tpool', @zlib)
    assert_equal ZfsConsts::HealthStatus::OK, @zpool.health_status
  end

  def test_zpool_root
    @zpool = Zpool.new('tpool', @zlib)
    # @zpool#root deprecated, use @zpool.get('altroot') instead.
    # assert_nil @zpool.root
    assert_nil @zpool.get('altroot')
  end

  def test_get_prop
    @zpool = Zpool.new('tpool', @zlib)
    assert_kind_of Integer, @zpool.get('guid')
    assert_kind_of Integer, @zpool.get('version')

    assert ['on','off'].include?(@zpool.get('delegation'))

    # These are "nicestrings" like "123M", "441K"
    assert_kind_of String, @zpool.get('size')
    assert_kind_of String, @zpool.get('used')
    assert_kind_of String, @zpool.get('available')

    # Unexisting properties should return nil:
    assert_nil @zpool.get('fakeprop')
  end

  # Requires root or privileged profile to run:
  def test_set_prop
    @zpool = Zpool.new('tpool', @zlib)
    assert_equal 'off', @zpool.get('listsnaps')
    assert @zpool.set('listsnaps', 'on')
    assert_equal 'on', @zpool.get('listsnaps')
    assert @zpool.set('listsnaps', 'off')
    assert_equal 'off', @zpool.get('listsnaps')
    assert !@zpool.set('guid', 'on')
    assert_equal 2002, @zlib.errno
    # WARN: these might be localized:
    assert_equal "cannot set property for 'tpool'", @zlib.error_action
    assert_equal "'guid' is readonly", @zlib.error_description
  end

  def test_iteration
    Zpool.each(@zlib) do |pool|
      assert_kind_of Zpool, pool
      assert_kind_of String, pool.name
    end
  end

  def test_iteration_without_handle
    Zpool.each do |pool|
      assert_kind_of Zpool, pool
      assert_kind_of String, pool.name
    end
  end

end
