require 'test/unit'
require 'libzfs'

class LibZfsTest < Test::Unit::TestCase
  def setup
    @zlib = LibZfs.new
  end

  def test_initialize
    assert_not_nil @zlib
    assert_equal LibZfs, @zlib.class
  end

  def test_retrieve_error
    assert_equal 0, @zlib.errno
    assert_equal '', @zlib.error_action
    # This is a localizable string, might fail depending on locale settings:
    assert_equal "no error", @zlib.error_description
  end

  def test_toggle_error
    assert_nothing_raised {@zlib.print_on_error(true)}
    assert_nothing_raised {@zlib.print_on_error(false)}
  end

  def test_handle
    assert LibZfs.class_variables.include?("@@handle")
    @hdl1 = LibZfs.handle
    assert_kind_of LibZfs, @hdl1
    @hdl2 = LibZfs.handle
    assert_equal @hdl1, @hdl2
  end

end
