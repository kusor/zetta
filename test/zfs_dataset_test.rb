require 'test/unit'
require 'libzfs'

# In order to run the tests on this file need to do:
#
# sudo zfs create tpool/home
# sudo zfs create tpool/thome
# sudo zfs snapshot tpool/thome@snap
# sudo zfs clone tpool/thome@snap tpool/thomeclone
# sudo zfs set zfs_rb:sample=test tpool/thome
#
# Might have sense to add 'File.exists?('/tpool')' check.

class ZfsDatasetTest < Test::Unit::TestCase
  def setup
    @zlib = LibZfs.new
  end

  def test_initialize_file_system
    @zfs = ZFS.new('tpool/home', @zlib, ZfsConsts::Types::FILESYSTEM)
    assert_not_nil @zfs
    assert_equal 'tpool/home', @zfs.name
    assert_equal ZfsConsts::Types::FILESYSTEM, @zfs.fs_type
    assert_kind_of LibZfs, @zfs.libzfs_handle
  end

  def test_initialize_unexistent
    @zfs = ZFS.new('tpool/this_will_probably_not_exist', @zlib, ZfsConsts::Types::FILESYSTEM)
    assert_nil @zfs
    assert_equal ZfsConsts::Errors::NOENT, @zlib.errno
    assert_equal "cannot open 'tpool/this_will_probably_not_exist'", @zlib.error_action
    assert_equal "dataset does not exist", @zlib.error_description
  end

  # REVIEW: Shouldn't this method return true/false?
  def test_rename_file_system
    @zfs = ZFS.new('tpool/thome', @zlib, ZfsConsts::Types::FILESYSTEM)
    # rename to itself:
    assert_equal 0, @zfs.rename('tpool/thome', false)
  end

  def test_get_prop
    @zfs = ZFS.new('tpool/thome', @zlib, ZfsConsts::Types::FILESYSTEM)
    require 'parsedate'
    assert_kind_of Array, ParseDate.parsedate(@zfs.get('creation'))
    # Something like <[2010, 4, 20, 10, 55, nil, nil, 2]>
    assert_equal 'filesystem', @zfs.get('type')
    assert ['on','off'].include?(@zfs.get('compression'))
    assert_equal "/tpool/thome", @zfs.get('mountpoint')
    assert @zfs.get('origin').empty?
  end

  def test_set_prop
    @zfs = ZFS.new('tpool/thome', @zlib, ZfsConsts::Types::FILESYSTEM)
    assert_equal 0, @zfs.set("readonly", 'on')
    assert_equal 'on', @zfs.get("readonly")
    assert_equal 0, @zfs.set("readonly", 'off')
    assert_equal 'off', @zfs.get("readonly")
    # We know this is readonly, hence, exception:
    assert_equal -1, @zfs.set('creation', Time.now.to_s)
    assert_not_equal 0, @zlib.errno
    assert_not_equal '', @zlib.error_action
    assert_not_equal "no error", @zlib.error_description
  end

  def test_userdef_properties
    @zfs = ZFS.new('tpool/thome', @zlib, ZfsConsts::Types::FILESYSTEM)
    assert_equal 'test', @zfs.get_user_prop('zfs_rb:sample')
    assert_equal 0, @zfs.set('zfs_rb:sample', 'foo')
    assert_equal 'foo', @zfs.get_user_prop('zfs_rb:sample')
    assert_equal 0, @zfs.set('zfs_rb:sample', 'test')
    assert_raise(ArgumentError) { @zfs.get('zfs_rb:sample') }
  end

end
