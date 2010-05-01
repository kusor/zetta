require 'test/unit'
require 'libzfs'

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
    @zpool = Zpool.new('fakepool', @zlib)
    assert_not_nil @zpool
    assert_kind_of Zpool, @zpool
    assert_equal ZfsConsts::Errors::NOENT, @zlib.errno
    # WARN: This is actually testing strings which might change on C,
    # and can be localized, depending on system locale settings.
    assert_equal "cannot open 'fakepool'", @zlib.error_action
    assert_equal "no such pool", @zlib.error_description
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
  end

  def test_iteration
    Zpool.each(@zlib) do |pool|
      assert_kind_of Zpool, pool
      assert_kind_of String, pool.name
    end
  end
end
