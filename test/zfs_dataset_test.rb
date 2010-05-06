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
    @zfs = ZFS.new('tpool/home', ZfsConsts::Types::FILESYSTEM, @zlib)
    assert_not_nil @zfs
    assert_equal 'tpool/home', @zfs.name
    assert_equal ZfsConsts::Types::FILESYSTEM, @zfs.fs_type
    assert_kind_of LibZfs, @zfs.libzfs_handle
  end

  def test_initialize_unexistent
    @zfs = ZFS.new('tpool/this_will_probably_not_exist', ZfsConsts::Types::FILESYSTEM, @zlib)
    assert_nil @zfs
    assert_equal ZfsConsts::Errors::NOENT, @zlib.errno
    assert_equal "cannot open 'tpool/this_will_probably_not_exist'", @zlib.error_action
    assert_equal "dataset does not exist", @zlib.error_description
  end

  def test_initialize_file_system_without_handle
    @zfs = ZFS.new('tpool/home', ZfsConsts::Types::FILESYSTEM)
    assert_not_nil @zfs
    assert_equal 'tpool/home', @zfs.name
    assert_equal ZfsConsts::Types::FILESYSTEM, @zfs.fs_type
    assert_kind_of LibZfs, @zfs.libzfs_handle
  end

  def test_initialize_with_wrong_arguments
    # At least two arguments are mandatory:
    assert_raise(ArgumentError) { @zfs = ZFS.new }
    assert_raise(ArgumentError) { @zfs = ZFS.new('tpool/home') }
    # Given the first argument, it must be a string
    assert_raise(TypeError) { @zfs = ZFS.new(1234, ZfsConsts::Types::FILESYSTEM) }
    # The second argument must be an Integer
    assert_raise(TypeError) { @zfs = ZFS.new('tpool/home', 'filesystem') }
    # Given the third argument, it must be an instance of LibZfs
    assert_raise(TypeError) { @zfs = ZFS.new('tpool/home', ZfsConsts::Types::FILESYSTEM, Array.new) }
  end

  # REVIEW: Shouldn't this method return true/false?
  def test_rename_file_system
    @zfs = ZFS.new('tpool/thome', ZfsConsts::Types::FILESYSTEM, @zlib)
    # rename to itself:
    assert_equal 0, @zfs.rename('tpool/thome', false)
  end

  def test_get_prop
    @zfs = ZFS.new('tpool/thome', ZfsConsts::Types::FILESYSTEM, @zlib)
    require 'parsedate'
    assert_kind_of Array, ParseDate.parsedate(@zfs.get('creation'))
    # Something like <[2010, 4, 20, 10, 55, nil, nil, 2]>
    assert_equal 'filesystem', @zfs.get('type')
    assert ['on','off'].include?(@zfs.get('compression'))
    assert_equal "/tpool/thome", @zfs.get('mountpoint')
    assert @zfs.get('origin').empty?
  end

  def test_set_prop
    @zfs = ZFS.new('tpool/thome', ZfsConsts::Types::FILESYSTEM, @zlib)
    assert_equal 0, @zfs.set("readonly", 'on')
    assert_equal 'on', @zfs.get("readonly")
    assert_equal 0, @zfs.set("readonly", 'off')
    assert_equal 'off', @zfs.get("readonly")
    # We know this is readonly, hence, exception:
    assert_equal(-1, @zfs.set('creation', Time.now.to_s))
    assert_not_equal 0, @zlib.errno
    assert_not_equal '', @zlib.error_action
    assert_not_equal "no error", @zlib.error_description
  end

  def test_userdef_properties
    @zfs = ZFS.new('tpool/thome', ZfsConsts::Types::FILESYSTEM, @zlib)
    assert_equal 'test', @zfs.get_user_prop('zfs_rb:sample')
    assert_equal 0, @zfs.set('zfs_rb:sample', 'foo')
    assert_equal 'foo', @zfs.get_user_prop('zfs_rb:sample')
    assert_equal 0, @zfs.set('zfs_rb:sample', 'test')
    assert_raise(ArgumentError) { @zfs.get('zfs_rb:sample') }
  end

  # Root Filesystems: rpool, tpool
  def test_root_fs_iteration
    ZFS.each(@zlib) do |zfs|
      assert_equal(ZfsConsts::Types::FILESYSTEM, zfs.fs_type)
    end
  end

  # Root Filesystems: rpool, tpool
  def test_root_fs_iteration_without_handle
    ZFS.each do |zfs|
      assert_equal(ZfsConsts::Types::FILESYSTEM, zfs.fs_type)
    end
  end

  def test_filesystem_iteration
    # tpool/thome has no children of type File System:
    @zfs = ZFS.new('tpool/thome', ZfsConsts::Types::FILESYSTEM, @zlib)
    children_fs = []
    @zfs.each_filesystem do |zfs|
      children_fs << zfs
    end
    assert children_fs.empty?
    # tpool has children of type File System:
    tpool = ZFS.new('tpool', ZfsConsts::Types::FILESYSTEM, @zlib)
    children_fs = []
    tpool.each_filesystem do |zfs|
      assert_equal(ZfsConsts::Types::FILESYSTEM, zfs.fs_type)
      children_fs << zfs
    end
    assert !children_fs.empty?
  end

  # tpool/thome@snap
  def test_snapshots_iteration
    @zfs = ZFS.new('tpool/thome', ZfsConsts::Types::FILESYSTEM, @zlib)
    children_fs = []
    @zfs.each_snapshot do |zfs|
      assert_equal(ZfsConsts::Types::SNAPSHOT, zfs.fs_type)
      children_fs << zfs
    end
    assert !children_fs.empty?
  end

  # tpool/thomeclone
  # tpool/thome@snap
  def test_dependents_iteration
    @zfs = ZFS.new('tpool/thome', ZfsConsts::Types::FILESYSTEM, @zlib)
    children_fs = []
    @zfs.each_dependent do |zfs|
      assert (zfs.fs_type == ZfsConsts::Types::SNAPSHOT || ZfsConsts::Types::FILESYSTEM)
      children_fs << zfs
    end
    assert !children_fs.empty?
  end

  def test_dataset_exists
    assert ZFS.exists?('tpool/thome', ZfsConsts::Types::FILESYSTEM, @zlib)
    assert !ZFS.exists?('tpool/this_will_probably_not_exists', ZfsConsts::Types::FILESYSTEM, @zlib)
    assert ZFS.exist?('tpool/thome', ZfsConsts::Types::FILESYSTEM, @zlib)
    assert !ZFS.exist?('tpool/this_will_probably_not_exists', ZfsConsts::Types::FILESYSTEM, @zlib)
  end

  def test_dataset_exists_without_handle
    assert ZFS.exists?('tpool/thome', ZfsConsts::Types::FILESYSTEM)
    assert ZFS.exist?('tpool/thome', ZfsConsts::Types::FILESYSTEM)
  end

  def test_create_destroy_success
    create_ok_name = "tpool/new_filesystem_name_#{rand(1000)}"
    create_fs = ZFS.create(create_ok_name, ZfsConsts::Types::FILESYSTEM, @zlib)
    # Need to improve the type check here.
    assert create_fs
    assert_respond_to create_fs, :name
    assert_equal create_ok_name, create_fs.get('name')
    assert ZFS.exists?(create_ok_name, ZfsConsts::Types::FILESYSTEM, @zlib)
    assert_equal 0, create_fs.destroy!
    assert !ZFS.exists?(create_ok_name, ZfsConsts::Types::FILESYSTEM, @zlib)
  end

  def test_create_failure
    create_failure_name = "fakepool/new_filesystem_name_#{rand(1000)}"
    create_fs = ZFS.create(create_failure_name, ZfsConsts::Types::FILESYSTEM, @zlib)
    assert !create_fs
    assert_not_equal 0, @zlib.errno
    # cannot create ...
    assert_not_equal '', @zlib.error_action
    # no such pool ...
    assert_not_equal "no error", @zlib.error_description
  end

  def test_create_without_handle
    create_ok_name = "tpool/new_filesystem_name_#{rand(1000)}"
    create_fs = ZFS.create(create_ok_name, ZfsConsts::Types::FILESYSTEM)
    # Need to improve the type check here.
    assert create_fs
    assert_respond_to create_fs, :name
    assert_equal create_ok_name, create_fs.get('name')
    assert ZFS.exists?(create_ok_name, ZfsConsts::Types::FILESYSTEM)
    assert_equal 0, create_fs.destroy!
    assert !ZFS.exists?(create_ok_name, ZfsConsts::Types::FILESYSTEM)
  end

  def test_snapshot_success
    snap_name = "tpool/thome@snap_#{rand(1000)}"
    @snap = ZFS.snapshot(snap_name, @zlib)

    assert_equal 0, @zlib.errno
    assert_equal '', @zlib.error_action
    assert_equal "no error", @zlib.error_description

    assert_kind_of ZFS, @snap
    assert_equal(ZfsConsts::Types::SNAPSHOT, @snap.fs_type)

    assert_equal 0, @snap.destroy!
    assert !ZFS.exists?(snap_name, ZfsConsts::Types::SNAPSHOT, @zlib)
  end

  def test_snapshot_failure
    snap_name = 'tpool/this_will_probably_not_exists@snap'
    @snap = ZFS.snapshot(snap_name, @zlib)
    assert_not_equal 0, @zlib.errno
    # cannot open ...
    assert_not_equal '', @zlib.error_action
    # dataset does not exist
    assert_not_equal "no error", @zlib.error_description
    assert_nil @snap
  end

  def test_can_clone_only_snapshots
    clone_name = "tpool/clone_#{rand(1000)}"
    @zfs = ZFS.new('tpool/thome', ZfsConsts::Types::FILESYSTEM, @zlib)
    assert_raise(NoMethodError) { @zfs.clone!(clone_name) }
  end

  def test_clone_success_and_failure
    clone_name = "tpool/clone_#{rand(1000)}"
    @zfs = ZFS.new('tpool/thome@snap', ZfsConsts::Types::SNAPSHOT, @zlib)
    assert_kind_of ZFS, @zfs
    @clone = @zfs.clone!(clone_name)
    assert_kind_of ZFS, @clone
    assert_equal(ZfsConsts::Types::FILESYSTEM, @clone.fs_type)
    # A second attempt to create the another clone with the same name
    # will result in a failure:
    assert_nil @zfs.clone!(clone_name)
    assert_not_equal 0, @zlib.errno
    # cannot create ...
    assert_not_equal '', @zlib.error_action
    # dataset already exists
    assert_not_equal "no error", @zlib.error_description
    # Cleanup
    assert_equal 0, @clone.destroy!
    assert !ZFS.exists?(clone_name, ZfsConsts::Types::FILESYSTEM, @zlib)
  end

  def test_cannot_promote_snapshots
    @zfs = ZFS.new('tpool/thome@snap', ZfsConsts::Types::SNAPSHOT, @zlib)
    assert !@zfs.promote
    assert_not_equal 0, @zlib.errno
    # cannot promote ...
    assert_not_equal '', @zlib.error_action
    # snapshots can not be promoted
    assert_not_equal "no error", @zlib.error_description
  end

  def test_promote_success
    ds_name = "tpool/dataset_#{rand(1000)}"
    snap_name = "#{ds_name}@snap"
    clone_name = "#{ds_name}_clone"
    @zfs = ZFS.create(ds_name, ZfsConsts::Types::FILESYSTEM, @zlib)
    assert @zfs
    @snap = ZFS.snapshot(snap_name, @zlib)
    assert @snap
    @clone = @snap.clone!(clone_name)
    # Trying to destroy the dataset with clones will fail
    assert_equal(-1, @zfs.destroy!)
    # Promote the clone
    assert @clone.promote
    # We can destroy the original dataset now
    assert_equal 0, @zfs.destroy!
    # Now we cannot destroy the clone:
    assert_equal(-1, @clone.destroy!)
    # The clone has a Snapshot with the same 'suffix' than the original one:
    assert ZFS.exists?("#{clone_name}@snap", ZfsConsts::Types::SNAPSHOT, @zlib)
    # So, the original snapshots does not exist now:
    assert !ZFS.exists?(snap_name, ZfsConsts::Types::SNAPSHOT, @zlib)
    # We have to delete the new snapshot before to try to delete the old one:
    @new_snap = ZFS.new("#{clone_name}@snap", ZfsConsts::Types::SNAPSHOT, @zlib)
    assert_equal 0, @new_snap.destroy!
    assert_equal 0, @clone.destroy!
  end

end
