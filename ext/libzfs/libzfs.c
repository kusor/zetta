#include <ruby.h>

#ifdef HAVE_LIBZFS_H
  #include <libzfs.h>
#endif

// Internal method: used to make libzfs_handle argument optional.
static VALUE my_libzfs_get_handle()
{
  ID class_id = rb_intern("LibZfs");
  VALUE class = rb_const_get(rb_cObject, class_id);
  return rb_funcall(class, rb_intern("handle"), 0);
}

// We have to merge alloc and init here because we want to allocate the space
// for the C data structure, but we also need the arguments passed to
// initialize to do so.
//
// FIXME: I'm still not sure this is the right way to do it!
/*
 *
 * Document-class: Zpool
 *
 * <code>Zpool</code> provides access to ZFS storage pools.
 *
 * In adition to the expected zpool initialization, this class provides
 * facilities to iterate over all the storage pools on the system.
 *
 *    @zlib = LibZfs.new
 *    @zpool = Zpool.new('pool_name', @zlib)
 *
 *    Zpool.each do |zpool|
 *      # access to each zpool instance
 *    end
 *
 */

/*
 *
 * Document-method: Zpool#new
 *
 * call-seq:
 *   Zpool.new('pool_name')  => object
 *   Zpool.new(:pool_name)  => object
 *   Zpool.new('pool_name', @zlib)  => object
 *   Zpool.new(:pool_name, @zlib)  => object
 *
 *
 * Zpool initializer.
 * Raise <code>ArgumentError</code> when <code>pool_name</code> is not given.
 * Raise <code>TypeError</code> when <code>pool_name</code> is given and it is
 * not a <code>String</code> or <code>Symbol</code>.
 * Raise <code>TypeError</code> when <code>@zlib</code> handle is given and it
 * is not an instance of <code>LibZfs</code>.
 *
 */
static VALUE my_zpool_new(int argc, VALUE *argv, VALUE klass)
{
  VALUE pool_name, libzfs_handle;
  libzfs_handle_t *libhandle;
  zpool_handle_t  *zpool_handle;

  if(argc < 1) {
    rb_raise(rb_eArgError, "Zpool name is required.");
  }

  pool_name = argv[0];

  if (TYPE(pool_name)!=T_STRING && TYPE(pool_name)!=T_SYMBOL) {
    rb_raise(rb_eTypeError, "Zpool name must be either a string or a symbol.");
  }

  // If name is a symbol, get the C string:
  if(SYMBOL_P(pool_name)) {
    pool_name = rb_funcall(pool_name, rb_intern("to_s"), 0);
  }

  libzfs_handle = (argc == 1) ? my_libzfs_get_handle() : argv[1];

  if(CLASS_OF(libzfs_handle) != rb_const_get(rb_cObject, rb_intern("LibZfs"))) {
    rb_raise(rb_eTypeError, "ZFS Lib handle must be an instance of LibZfs.");
  }

  Data_Get_Struct(libzfs_handle, libzfs_handle_t, libhandle);

  // FIXME: Should be +_canfail+ or not?
  zpool_handle = zpool_open_canfail(libhandle, StringValuePtr(pool_name));

  return Data_Wrap_Struct(klass, 0, zpool_close, zpool_handle);
}

/*
 * call-seq:
 *   @zpool.libzfs_handle  => object, zpool_handle
 *
 * Return <code>zpool_handle</code> associated with a <code>Zpool</code>
 * instance.
 *
 */
static VALUE my_zpool_get_handle(VALUE self)
{
  VALUE klass = rb_const_get(rb_cObject, rb_intern("LibZfs"));
  libzfs_handle_t *handle;
  zpool_handle_t *zpool_handle;

  Data_Get_Struct(self, zpool_handle_t, zpool_handle);

  handle = zpool_get_handle(zpool_handle);

  // Note that we don't need to free the handle here, because it's just a
  // copy of one that's already in the garbage collector.
  return Data_Wrap_Struct(klass, 0, 0, handle);
}

/*
 * call-seq:
 *   @zpool.name  => string, zpool name.
 *
 * Return current <code>Zpool</code> instance name.
 *
 * The <code>name</code> pointer is stored in the <code>handle</code>, hence
 * we don't need to call <code>zpool.get('name')</code>. This method is
 * more effective.
 *
 */
static VALUE my_zpool_get_name(VALUE self)
{
  zpool_handle_t *zpool_handle;
  Data_Get_Struct(self, zpool_handle_t, zpool_handle);

  return rb_str_new2(zpool_get_name(zpool_handle));
}

/*
 * call-seq:
 *   @zpool.get('propname')  => string/integer, zpool property value
 *
 * Given a zpool property name, return its value.
 * Values for properties +version+ and +guid+ will be integers. Any other
 * property value will be a string. For disk space related properties, this
 * string will be nicely formatted as in <code>ZfsLib.nicenum</code>.
 *
 * Raise <code>TypeError</code> when <code>propname</code>
 * is not a <code>String</code>.
 *
 * TODO:
 *
 *  - Ensure that the given property name is a valid zpool property.
 *
 */
static VALUE my_zpool_get_prop(VALUE self, VALUE name)
{
  zpool_handle_t *zpool_handle;

  if( TYPE(name) != T_STRING )
  {
    rb_raise(rb_eTypeError, "Property name must be a string.");
  }

  char *propname = STR2CSTR(name);

  char zpool_prop = zpool_name_to_prop(propname);

  Data_Get_Struct(self, zpool_handle_t, zpool_handle);
  // FIXME: This needs to take into consideration the possibility
  // of unavailable zpools.
  if(zpool_prop == ZPOOL_PROP_GUID || zpool_prop == ZPOOL_PROP_VERSION)
  {
    return ULL2NUM(zpool_get_prop_int(zpool_handle, zpool_prop, NULL));
  } else {
    char propval[ZPOOL_MAXNAMELEN];
    zpool_get_prop(zpool_handle, zpool_prop, propval, sizeof (propval), NULL);
    return ( strcmp( propval, "-" ) == 0 ) ? Qnil: rb_str_new2(propval);
  }
}

/*
 * call-seq:
 *   @zpool.set('propname', "propval")  => string/integer, zpool property value
 *
 * Set the given value for the given zpool property.
 *
 * Raise <code>TypeError</code> when <code>propname</code>
 * is not a <code>String</code>.
 * Raise <code>TypeError</code> when <code>proval</code>
 * is not a <code>String</code>.
 *
 * TODO: Actually, return -1 on failure, 0 on succes, should return false/true
 *
 */
static VALUE my_zpool_set_prop(VALUE self, VALUE propname, VALUE propval)
{
  zpool_handle_t *zpool_handle;

  if( TYPE(propname) != T_STRING )
  {
    rb_raise(rb_eTypeError, "Property name must be a string.");
  }

  if(TYPE(propval) != T_STRING )  // Maybe should also allow !FIXNUM_P(propval)
  {
    rb_raise(rb_eTypeError, "Property value must be a string or a number.");
  }

  char *name = STR2CSTR(propname);
  // FIXME: Property might require an integer value, so need to check the type.
  char *val = STR2CSTR(propval);

  Data_Get_Struct(self, zpool_handle_t, zpool_handle);

  // TODO: Should check return value here, and do some true/false return or raise error:
  // 0 => success, -1 => Failure. Probably return true/false
  return INT2NUM(zpool_set_prop(zpool_handle, name, val));
}

/*
 * call-seq:
 *   @zpool.guid  => integer, zpool guid.
 *
 * Return current <code>Zpool</code> instance guid property.
 *
 * Actually, this is the equivalent to <code>zpool.get_prop('guid')</code>.
 * This method is here just because it was defined at the old version of this
 * library, and will be removed when "method_missing" is implemented.
 *
 */
static VALUE my_zpool_get_guid(VALUE self)
{
  zpool_handle_t *zpool_handle;
  Data_Get_Struct(self, zpool_handle_t, zpool_handle);

  return ULL2NUM(zpool_get_prop_int(zpool_handle, ZPOOL_PROP_GUID, NULL));
}

static VALUE my_zpool_get_space_used(VALUE self)
{
  zpool_handle_t *zpool_handle;
  Data_Get_Struct(self, zpool_handle_t, zpool_handle);

  return ULL2NUM(zpool_get_space_used(zpool_handle));
}

static VALUE my_zpool_get_space_total(VALUE self)
{
  zpool_handle_t *zpool_handle;
  Data_Get_Struct(self, zpool_handle_t, zpool_handle);

  return ULL2NUM(zpool_get_space_total(zpool_handle));
}

/*
 * call-seq:
 *   @zpool.state  => integer, zpool state.
 *
 * Return current <code>Zpool</code> instance state as an integer.
 *
 * Can be converted into one of the constants defined at module
 * <code>ZfsConsts::State::Pool</code>.
 *
 * <b>TODO</b>:
 *
 * - We don't need constants defined this way, can use strings to
 *   get an human friendly description of zpool state. Also, should
 *   define our own set of constants, and do not depend on the values
 *   defined at the libzfs library.
 *
 */
static VALUE my_zpool_get_state(VALUE self)
{
  zpool_handle_t *zpool_handle;
  Data_Get_Struct(self, zpool_handle_t, zpool_handle);

  return INT2NUM(zpool_get_state(zpool_handle));
}

/*
 * call-seq:
 *   @zpool.version  => integer, zpool version.
 *
 * Return current <code>Zpool</code> instance version property.
 *
 * Actually, this is the equivalent to <code>zpool.get_prop('version')</code>.
 *
 */
static VALUE my_zpool_get_version(VALUE self)
{
  zpool_handle_t *zpool_handle;
  Data_Get_Struct(self, zpool_handle_t, zpool_handle);

  return ULL2NUM(zpool_get_prop_int(zpool_handle, ZPOOL_PROP_VERSION, NULL));
}

static int my_zpool_iter_f(zpool_handle_t *handle, void *klass)
{
  rb_yield(Data_Wrap_Struct((VALUE)klass, 0, zpool_close, handle));
  return 0;
}

/*
 *
 * Document-method: Zpool#each
 *
 * call-seq:
 *   Zpool.each {|zpool| # ... }  => nil. Iterator.
 *   Zpool.each(@zlib) {|zpool| # ... }  => nil. Iterator.
 *
 * Iterates over all the pools defined in the system.
 *
 *    Zpool.each do |zpool|
 *      # access to each zpool instance
 *    end
 *
 * Raise <code>TypeError</code> when <code>@zlib</code> handle is given and it
 * is not an instance of <code>LibZfs</code>.
 *
 */
static VALUE my_zpool_iter(int argc, VALUE *argv, VALUE klass)
{
  VALUE libzfs_handle;

  libzfs_handle_t *libhandle;

  libzfs_handle = (argc == 0) ? my_libzfs_get_handle() : argv[0];

  if(CLASS_OF(libzfs_handle) != rb_const_get(rb_cObject, rb_intern("LibZfs"))) {
    rb_raise(rb_eTypeError, "ZFS Lib handle must be an instance of LibZfs.");
  }

  Data_Get_Struct(libzfs_handle, libzfs_handle_t, libhandle);

  zpool_iter(libhandle, my_zpool_iter_f, (void *)klass);

  return Qnil;
}

// static VALUE my_zpool_create(VALUE libzfs_handle, VALUE name, VALUE vdevs, VALUE altroot)
// {
//   libzfs_handle_t *libhandle;
//   nvlist_t *vdevs = NULL;
//   Data_Get_Struct(libzfs_handle, libzfs_handle_t, libhandle);
//
//   return INT2NUM(zpool_create(libzfs_handle, StringValuePtr(name), vdev_list, StringValuePtr(altroot)));
// }

// FIXME: Doesn't appear to work?  Maybe I actually need to offline it before
// I can destroy?  If so, that's something for the higher level Ruby library.
// static VALUE my_zpool_destroy(VALUE self)
// {
//   zpool_handle_t *zpool_handle;
//   Data_Get_Struct(self, zpool_handle_t, zpool_handle);
//
//   return INT2NUM(zpool_destroy(zpool_handle));
// }

/*
 * ZFS interface
 */

/*
 * call-seq:
 *   @zfs.libzfs_handle  => object, zfs_handle
 *
 * Return <code>zfs_handle</code> associated with a <code>ZFS</code>
 * instance.
 *
 */
static VALUE my_zfs_get_handle(VALUE self)
{
  VALUE klass = rb_const_get(rb_cObject, rb_intern("LibZfs"));
  libzfs_handle_t *handle;
  zfs_handle_t *zfs_handle;

  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  handle = zfs_get_handle(zfs_handle);

  // Note that we don't need to free the handle here, because it's just a
  // copy of one that's already in the garbage collector.
  return Data_Wrap_Struct(klass, 0, 0, handle);
}

/*
 *
 * Document-class: ZFS
 *
 * <code>ZFS</code> provides access to properties and operations over
 * ZFS _filesystems_, _snapshots_ or _volumes_, (_ZFS datasets_).
 *
 * In adition to the expected dataset initialization, this class provides
 * facilities to iterate over _root datasets_, and any of their associated
 * dataset types.
 *
 *    [@zlib = LibZfs.new]
 *    @zfs = ZFS.new('dataset/name', ZfsConsts::Types::FILESYSTEM[, @zlib])
 *
 *    @zfs.each_filesystem do |fs|
 *      # access to all the children filesystems of @zfs dataset.
 *    end
 *
 *    @zfs.each_snapshot do |sp|
 *      # access to all the snapshots of @zfs dataset.
 *    end
 *
 *    @zfs.each_dependent do |ds|
 *      # access to all children filesystem, clones and snapshots of
 *      # @zfs dataset.
 *    end
 *
 */

/*
 * call-seq:
 *   @zfs = ZFS.new('dataset/name', ZfsConsts::Types)  => object
 *   @zfs = ZFS.new('dataset/name', ZfsConsts::Types, @zlib)  => object
 *
 *
 * ZFS initializer. Raises <code>ArgumentError</code> when
 * <code>dataset_name</code> or <code>dataset_type</code> are not given.
 *
 * Raise <code>ArgumentError</code> when <code>dataset_name</code> and
 * <code>dataset_type</code> are not given.
 * Raise <code>TypeError</code> when <code>dataset_name</code> is given and it
 * is not a <code>String</code>.
 * Raise <code>TypeError</code> when <code>dataset_type</code> is given and it
 * is not an <code>Integer</code>.
 * Raise <code>TypeError</code> when <code>@zlib</code> handle is given and it
 * is not an instance of <code>LibZfs</code>.
 *
 * TODO: Check that the given type is a valid type
 *
 */
static VALUE my_zfs_new(int argc, VALUE *argv, VALUE klass)
{
  VALUE fs_name, libzfs_handle, types;
  libzfs_handle_t *libhandle;
  zfs_handle_t  *zfs_handle;

  if(argc < 2) {
    rb_raise(rb_eArgError, "Filesystem name and ZFS Type are required");
  }
  fs_name = argv[0];
  types = argv[1];

  if( TYPE(fs_name) != T_STRING ) {
    rb_raise(rb_eTypeError, "ZFS Dataset name must be a string.");
  }

  if( !FIXNUM_P(types) ) {
    rb_raise(rb_eTypeError, "ZFS Dataset type must be an integer.");
  }

  libzfs_handle = (argc == 2) ? my_libzfs_get_handle() : argv[2];

  if(CLASS_OF(libzfs_handle) != rb_const_get(rb_cObject, rb_intern("LibZfs"))) {
    rb_raise(rb_eTypeError, "ZFS Lib handle must be an instance of LibZfs.");
  }

  Data_Get_Struct(libzfs_handle, libzfs_handle_t, libhandle);
  zfs_handle = zfs_open(libhandle, StringValuePtr(fs_name), NUM2INT(types));
  // Prevent Segementation Faults when the given Dataset does not exist and
  // somebody tries to access to a given property:
  return (zfs_handle == NULL) ? Qnil: Data_Wrap_Struct(klass, 0, zfs_close, zfs_handle);
}

/*
 * call-seq:
 *   @zfs.name  => string, zfs dataset name.
 *
 * Return current <code>ZFS</code> instance name.
 *
 * The <code>name</code> pointer is stored in the <code>handle</code>, hence
 * we don't need to call <code>zfs.get('name')</code>. This method is
 * more effective.
 *
 */
static VALUE my_zfs_get_name(VALUE self)
{
  zfs_handle_t *zfs_handle;
  const char *name = NULL;
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);
  name = zfs_get_name(zfs_handle);
  if(name) {
    return rb_str_new2(name);
  }
  return Qnil;
}

/*
 * call-seq:
 *   @zfs.fs_type  => integer, zfs dataset type constant value
 *
 * Return the ZFS Dataset Type for the current <code>ZFS</code> dataset
 * instance.
 *
 */
static VALUE my_zfs_get_type(VALUE self)
{
  zfs_handle_t *zfs_handle;
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  return INT2NUM(zfs_get_type(zfs_handle));
}

/*
 * call-seq:
 *   @zfs.get('propname')  => string/integer, zfs property value
 *
 * Given a zfs dataset property name, return its value.
 *
 * Any property value will be a string. For disk space related properties,
 * this string will be nicely formatted as in <code>ZfsLib.nicenum</code>.
 *
 * Raise <code>TypeError</code> when <code>propname</code>
 * is not a <code>String</code>.
 *
 * TODO:
 *
 * - Ensure that the given property name is a valid zfs dataset property.
 *
 */
static VALUE my_zfs_get_prop(VALUE self, VALUE name)
{
  zfs_handle_t *zfs_handle;

  if( TYPE(name) != T_STRING )
  {
    rb_raise(rb_eTypeError, "Property name must be a string.");
  }

  char *propname = STR2CSTR(name);
  // Do not use this method to retrieve user properties.
  if ( zfs_prop_user(propname) ) {
    rb_raise(rb_eArgError, "Use 'get_user_prop' in order to access user defined properties");
  }
  char zfs_prop = zfs_name_to_prop(propname);
  char propval[ZFS_MAXNAMELEN];

  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  zfs_prop_get(zfs_handle, zfs_prop, propval, sizeof(propval), NULL, NULL, 0, B_FALSE);
  return rb_str_new2(propval);
}

/*
 * call-seq:
 *   @zfs.set('propname', "propval")  => string/integer, zfs dataset property value
 *
 * Set the given value for the given zfs dataset property.
 *
 *
 * Raise <code>TypeError</code> when <code>propname</code>
 * is not a <code>String</code>.
 * Raise <code>TypeError</code> when <code>proval</code>
 * is not a <code>String</code>.
 *
 * TODO:
 *
 * - Actually, return -1 on failure, 0 on succes, should return false/true.
 *
 */

static VALUE my_zfs_set_prop(VALUE self, VALUE propname, VALUE propval)
{
  zfs_handle_t *zfs_handle;

  if( TYPE(propname) != T_STRING )
  {
    rb_raise(rb_eTypeError, "Property name must be a string.");
  }

  if(TYPE(propval) != T_STRING ) // Maybe should also allow !FIXNUM_P(propval)
  {
    rb_raise(rb_eTypeError, "Property value must be a string.");
  }

  char *name = STR2CSTR(propname);
  // FIXME: Property might receive an integer value, so need to check the type.
  char *val = STR2CSTR(propval);

  Data_Get_Struct(self, zfs_handle_t, zfs_handle);
  // TODO: Should check return value here, and do some true/false return or raise error:
  // 0 => success, -1 => Failure. Probably return true/false
  return INT2NUM(zfs_prop_set(zfs_handle, name, val));
}

/*
 * call-seq:
 *   @zfs.get_user_prop('user:propname')  => string, user defined property value
 *
 * Given a zfs dataset property name set by user, return its value.
 *
 * <b>Warning:</b> This method relies into libzfs internals, and not the public
 * library interface, using direct lookups into ZFS data type user properties;
 * this is something which might perfectly change on future library versions,
 * hence the word of caution.
 *
 * In order to set user defined properties for a ZFS Dataset, use the same
 * method than for system properties, i.e:
 *
 *    @zfs.set('user_prefix:propname', 'propval')
 *
 * Only string user properties are supported for now.
 *
 * Raise <code>TypeError</code> when <code>propname</code>
 * is not a <code>String</code>.
 *
 * TODO:
 *
 * - Support additional types for user properties.
 *
 */
static VALUE my_zfs_get_user_prop(VALUE self, VALUE name)
{
  zfs_handle_t *zfs_handle;

  if( TYPE(name) != T_STRING ) {
    rb_raise(rb_eTypeError, "Property name must be a string.");
  }
  char *propname = STR2CSTR(name);

  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  if ( zfs_prop_user(propname) ) {
    nvlist_t *user_props = zfs_get_user_props(zfs_handle);
    nvlist_t *nv;
    char *value;

    if ( nvlist_lookup_nvlist(user_props, propname, &nv) == 0 ) {
      if ( nvlist_lookup_string(nv, ZPROP_VALUE, &value) == 0 ) {
        return rb_str_new2(value);
      }
      // TODO: This conditional must continue, and lookup additional types
      // for user defined properties.
    }
  }

  return Qnil;
}

/*
 * call-seq:
 *   @zfs.rename('dataset/name', [false|true])  => integer, 0|-1
 *
 * Rename ZFS Dataset using the given <code>dataset_name</code>.
 * When <code>recursive</code> is true, also renames all the children
 * datasets for the current one.
 *
 *
 * TODO:
 *
 * - Actually, return -1 on failure, 0 on succes, should return false/true.
 * - Enforce Types, return <code>ArgumentError</code> when appropriated.
 */
static VALUE my_zfs_rename(VALUE self, VALUE target, VALUE recursive)
{
  zfs_handle_t *zfs_handle;
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  return INT2NUM(zfs_rename(zfs_handle, StringValuePtr(target), RTEST(recursive)));
}

/*
 * call-seq:
 *   ZFS#create('dataset/name', ZfsConsts::Types)  => @zfs dataset instance.
 *   ZFS#create('dataset/name', ZfsConsts::Types, @zlib)  => @zfs dataset instance.
 *
 * Given a <code>dataset_name</code>, and a <code>dataset_type</code>,
 * create a zfs dataset.
 *
 * Return a new Zfs instance for the given dataset on success or false on failure.
 *
 * Raise <code>ArgumentError</code> when <code>dataset_name</code> and
 * <code>dataset_type</code> are not given.
 * Raise <code>TypeError</code> when <code>dataset_name</code> is given and it
 * is not a <code>String</code>.
 * Raise <code>TypeError</code> when <code>dataset_type</code> is given and it
 * is not an <code>Integer</code>.
 * Raise <code>TypeError</code> when <code>@zlib</code> handle is given and it
 * is not an instance of <code>LibZfs</code>.
 *
 */
static VALUE my_zfs_create(int argc, VALUE *argv, VALUE klass)
{
  VALUE fs_name, libzfs_handle, types;
  libzfs_handle_t *libhandle;

  if(argc < 2) {
    rb_raise(rb_eArgError, "Filesystem name and ZFS Type are required");
  }
  fs_name = argv[0];
  types = argv[1];

  if( TYPE(fs_name) != T_STRING ) {
    rb_raise(rb_eTypeError, "ZFS Dataset name must be a string.");
  }

  if( !FIXNUM_P(types) ) {
    rb_raise(rb_eTypeError, "ZFS Dataset type must be an integer.");
  }

  libzfs_handle = (argc == 2) ? my_libzfs_get_handle() : argv[2];

  if(CLASS_OF(libzfs_handle) != rb_const_get(rb_cObject, rb_intern("LibZfs"))) {
    rb_raise(rb_eTypeError, "ZFS Lib handle must be an instance of LibZfs.");
  }

  Data_Get_Struct(libzfs_handle, libzfs_handle_t, libhandle);

  if (0 == zfs_create(libhandle, StringValuePtr(fs_name), NUM2INT(types), NULL)){
    zfs_handle_t  *zfs_handle;
    zfs_handle = zfs_open(libhandle, StringValuePtr(fs_name), NUM2INT(types));
    return Data_Wrap_Struct(klass, 0, zfs_close, zfs_handle);
  } else {
    return Qfalse;
  }
}

/*
 * call-seq:
 *   ZFS#exists?('dataset/name', ZfsConsts::Types)  => Boolean
 *   ZFS#exists?('dataset/name', ZfsConsts::Types, @zlib)  => Boolean
 *   ZFS#exist?('dataset/name', ZfsConsts::Types)  => Boolean
 *   ZFS#exist?('dataset/name', ZfsConsts::Types, @zlib)  => Boolean
 *
 * Check if a dataset with the given <code>dataset_name</code>, of the given
 * <code>dataset_type</code> exists.
 *
 * Return true on success or false on failure.
 *
 * Raise <code>ArgumentError</code> when <code>dataset_name</code> and
 * <code>dataset_type</code> are not given.
 * Raise <code>TypeError</code> when <code>dataset_name</code> is given and it
 * is not a <code>String</code>.
 * Raise <code>TypeError</code> when <code>dataset_type</code> is given and it
 * is not an <code>Integer</code>.
 * Raise <code>TypeError</code> when <code>@zlib</code> handle is given and it
 * is not an instance of <code>LibZfs</code>.
 *
 */
static VALUE my_zfs_dataset_exists(int argc, VALUE *argv, VALUE klass)
{
  VALUE fs_name, libzfs_handle, types;
  libzfs_handle_t *libhandle;

  if(argc < 2) {
    rb_raise(rb_eArgError, "Filesystem name and ZFS Type are required");
  }
  fs_name = argv[0];
  types = argv[1];

  if( TYPE(fs_name) != T_STRING ) {
    rb_raise(rb_eTypeError, "ZFS Dataset name must be a string.");
  }

  if( !FIXNUM_P(types) ) {
    rb_raise(rb_eTypeError, "ZFS Dataset type must be an integer.");
  }

  libzfs_handle = (argc == 2) ? my_libzfs_get_handle() : argv[2];

  if(CLASS_OF(libzfs_handle) != rb_const_get(rb_cObject, rb_intern("LibZfs"))) {
    rb_raise(rb_eTypeError, "ZFS Lib handle must be an instance of LibZfs.");
  }

  Data_Get_Struct(libzfs_handle, libzfs_handle_t, libhandle);

  return zfs_dataset_exists(libhandle, StringValuePtr(fs_name), NUM2INT(types)) ? Qtrue : Qfalse;
}

/*
 * call-seq:
 *   ZFS#snapshot('snap/shot@name')  => ZFS instance
 *   ZFS#snapshot('snap/shot@name', @zlib)  => ZFS instance
 *
 * Check if a dataset with the given <code>dataset_name</code>, of the given
 * <code>dataset_type</code> exists.
 *
 * Return a ZFS snapshot instance on success or nil on failure.
 *
 * Raise <code>ArgumentError</code> when <code>snapshot_name</code> is not
 * given.
 * Raise <code>TypeError</code> when <code>snapshot_name</code> is given and it
 * is not a <code>String</code>.
 * Raise <code>TypeError</code> when <code>@zlib</code> handle is given and it
 * is not an instance of <code>LibZfs</code>.
 *
 */
static VALUE my_zfs_snapshot(int argc, VALUE *argv, VALUE klass)
{
  VALUE snapshot_name, libzfs_handle;
  libzfs_handle_t *libhandle;

  if(argc < 1) {
    rb_raise(rb_eArgError, "Snapshot name is required required");
  }

  snapshot_name = argv[0];

  if( TYPE(snapshot_name) != T_STRING ) {
    rb_raise(rb_eTypeError, "Snapshot name must be a string.");
  }

  libzfs_handle = (argc == 1) ? my_libzfs_get_handle() : argv[1];

  if(CLASS_OF(libzfs_handle) != rb_const_get(rb_cObject, rb_intern("LibZfs"))) {
    rb_raise(rb_eTypeError, "ZFS Lib handle must be an instance of LibZfs.");
  }

  Data_Get_Struct(libzfs_handle, libzfs_handle_t, libhandle);

  if ( 0 == zfs_snapshot(libhandle, StringValuePtr(snapshot_name), B_FALSE, NULL) ){
    zfs_handle_t  *zfs_handle;
    zfs_handle = zfs_open(libhandle, StringValuePtr(snapshot_name), ZFS_TYPE_SNAPSHOT);
    return Data_Wrap_Struct(klass, 0, zfs_close, zfs_handle);
  } else {
    return Qnil;
  }
}

/*
 * call-seq:
 *   @zfs.clone!('clone_name')  => ZFS instance | nil
 *
 * Create a clone with the given <code>clone_name</code> for the current
 * ZFS Snapshot instance.
 *
 *
 * Raise <code>TypeError</code> when <code>clone_name</code>
 * is not a <code>String</code>.
 * Raise <code>NoMethodError</code> when the current <code>ZfsLib</code>
 * instance is not a Snapshot.
 *
 * TODO:
 *
 * - Actually, return nil on failure, will raise error.
 *
 * NOTE: This method cannot be <i>clone</i> due to obvious Ruby reasons.
 *
 */
static VALUE my_zfs_clone(VALUE self, VALUE clone_name)
{
  zfs_handle_t *zfs_handle;

  if( TYPE(clone_name) != T_STRING ) {
    rb_raise(rb_eTypeError, "Clone name must be a string.");
  }

  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  if(zfs_get_type(zfs_handle) != ZFS_TYPE_SNAPSHOT) {
    rb_raise(rb_eNoMethodError, "Clone operation is only available for Datasets of type snapshot.");
  }

  if (0 == zfs_clone(zfs_handle, StringValuePtr(clone_name), NULL)){
    libzfs_handle_t *libhandle= zfs_get_handle(zfs_handle);
    zfs_handle_t  *zfs_clone_handle;

    zfs_clone_handle = zfs_open(libhandle, StringValuePtr(clone_name), ZFS_TYPE_FILESYSTEM);
    return Data_Wrap_Struct(CLASS_OF(self), 0, zfs_close, zfs_clone_handle);
  } else {
    return Qnil;
  }
}

/*
 * call-seq:
 *   @zfs.promote  => Boolean
 *
 * Promote the current clone to be no longer dependent on its origin.
 *
 */
static VALUE my_zfs_promote(VALUE self)
{
  zfs_handle_t *zfs_handle;

  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  return (zfs_promote(zfs_handle) == 0) ? Qtrue : Qfalse;
}

static VALUE my_zfs_is_shared(VALUE self)
{
  zfs_handle_t *zfs_handle;
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  return zfs_is_shared(zfs_handle) ? Qtrue : Qfalse;
}

static VALUE my_zfs_share(VALUE self)
{
  zfs_handle_t *zfs_handle;
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  return INT2NUM(zfs_share(zfs_handle));
}

static VALUE my_zfs_unshare(VALUE self)
{
  zfs_handle_t *zfs_handle;
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  return INT2NUM(zfs_unshare(zfs_handle));
}

static VALUE my_zfs_nfs_share_name(VALUE self)
{
  zfs_handle_t *zfs_handle;
  char *path;
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  return zfs_is_shared_nfs(zfs_handle, &path) ? rb_str_new2(path) : Qnil;
}

static VALUE my_zfs_share_nfs(VALUE self)
{
  zfs_handle_t *zfs_handle;
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  return INT2NUM(zfs_share_nfs(zfs_handle));
}

static VALUE my_zfs_unshare_nfs(VALUE self)
{
  zfs_handle_t *zfs_handle;
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  return INT2NUM(zfs_unshare_nfs(zfs_handle, NULL));
}

static VALUE my_zfs_is_shared_iscsi(VALUE self)
{
  zfs_handle_t *zfs_handle;
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  return zfs_is_shared_iscsi(zfs_handle) ? Qtrue : Qfalse;
}

static VALUE my_zfs_share_iscsi(VALUE self)
{
  zfs_handle_t *zfs_handle;
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  return INT2NUM(zfs_share_iscsi(zfs_handle));
}

static VALUE my_zfs_unshare_iscsi(VALUE self)
{
  zfs_handle_t *zfs_handle;
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  return INT2NUM(zfs_unshare_iscsi(zfs_handle));
}

/*
 * call-seq:
 *   @zfs.destroy!  => Integer, 0|-1
 *
 * Destroys the current dataset instance.
 *
 * TODO:
 *
 * - Shouldn't this be a class method?. Leaving the destroyed ZFS Dataset
 *   instance around might cause problems when trying to access some methods.
 * - Actually, return -1 on failure, 0 on succes, should return false/true.
 * - Right now, the reason for the failure is on @libzfs.errno, with the
 *   associated messages. Maybe it would have sense to raise a proper ruby
 *   error for failures.
 *
 */
static VALUE my_zfs_destroy(VALUE self)
{
  zfs_handle_t *zfs_handle;
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

// Boolean parameter was added to zfs_destroy:
#ifdef SPA_VERSION_18
  return INT2NUM(zfs_destroy(zfs_handle,B_FALSE));
#else
  return INT2NUM(zfs_destroy(zfs_handle));
#endif
}

static int my_zfs_iter_f(zfs_handle_t *handle, void *klass)
{
  rb_yield(Data_Wrap_Struct((VALUE)klass, 0, zfs_close, handle));
  return 0;
}

/*
 * call-seq:
 *   ZFS.each {|zfs| # ... }  => nil. Iterator.
 *   ZFS.each(@zlib) {|zfs| # ... }  => nil. Iterator.
 *
 * Iterates over all the root datasets defined in the system.
 *
 *    ZFS.each do |zfs|
 *      # access to each zfs instance
 *    end
 *
 * Raise <code>TypeError</code> when <code>@zlib</code> handle is given and it
 * is not an instance of <code>LibZfs</code>.
 *
 */
static VALUE my_zfs_iter_root(int argc, VALUE *argv, VALUE klass)
{
  VALUE libzfs_handle;
  libzfs_handle_t *libhandle;

  libzfs_handle = (argc == 0) ? my_libzfs_get_handle() : argv[0];

  if(CLASS_OF(libzfs_handle) != rb_const_get(rb_cObject, rb_intern("LibZfs"))) {
    rb_raise(rb_eTypeError, "ZFS Lib handle must be an instance of LibZfs.");
  }

  Data_Get_Struct(libzfs_handle, libzfs_handle_t, libhandle);

  zfs_iter_root(libhandle, my_zfs_iter_f, (void *)klass);

  return Qnil;
}

/*
 * call-seq:
 *   @zfs.each_filesystem {|zfs| # ... }  => nil. Iterator.
 *
 * Iterates over all the children datasets of type filesystem for
 * the current one.
 *
 *    @zfs.each_filesystem do |zfs|
 *      # access to each zfs instance
 *    end
 *
 */
static VALUE my_zfs_iter_filesystems(VALUE self)
{
  zfs_handle_t *zfs_handle;
  VALUE klass = rb_class_of(self);
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  zfs_iter_filesystems(zfs_handle, my_zfs_iter_f, (void *)klass);

  return Qnil;
}

/*
 * call-seq:
 *   @zfs.each_snapshot {|zfs| # ... }  => nil. Iterator.
 *
 * Iterates over all the children datasets of type snapshot for
 * the current one.
 *
 *    @zfs.each_snapshot do |snap|
 *      # access to each zfs instance
 *    end
 *
 */
static VALUE my_zfs_iter_snapshots(VALUE self)
{
  zfs_handle_t *zfs_handle;
  VALUE klass = rb_class_of(self);
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  zfs_iter_snapshots(zfs_handle, my_zfs_iter_f, (void *)klass);

  return Qnil;
}

/*
 * call-seq:
 *   @zfs.each_dependent {|zfs| # ... }  => nil. Iterator.
 *
 * Iterates over all the datasets depending on the current one.
 * This includes filesystems, snapshots and clones.
 *
 *    @zfs.each_dependent do |dataset|
 *      # access to each zfs instance
 *    end
 *
 */
static VALUE my_zfs_iter_dependents(VALUE self)
{
  zfs_handle_t *zfs_handle;
  VALUE klass = rb_class_of(self);
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);
  // TODO: Allow recursion should be configurable by user?
  zfs_iter_dependents(zfs_handle, B_TRUE, my_zfs_iter_f, (void *)klass);

  return Qnil;
}

/*
 * The low-level libzfs handle widget.
 */


/*
 * Document-class: LibZfs
 *
 * <code>LibZfs</code> provides access to <code>libzfs_handle</code>, required
 * in order to get access to most of the functionalities provided by the ZFS
 * filesystem's library.
 *
 * All the <code>Zpool</code> and <code>ZFS</code> class methods depending on
 * <code>LibZfs</code> offer the possibility to do not pass an instance of this
 * class as their final argument, creating a new instance of the class on
 * those cases.
 *
 *    @zpool = Zpool.new('pool_name')
 *
 * When the <code>libzfs_handle</code> argument is not given to any of the
 * methods using it, the library will use <code>LibZfs.handle</code> class
 * method in order to create and reuse a <code>LibZfs</code> instance into
 * the class variable "@@handle".
 *
 */

/*
 *
 * Document-method: LibZfs#new
 *
 * call-seq:
 *   LibZfs.new  => object, libzfs_handle
 *
 * Return <code>libzfs_handle</code> object, which can be used by some other
 * methods like <code>Zpool</code> and <code>ZFS</code> initializers.
 *
 */
static VALUE my_libzfs_alloc(VALUE klass)
{
  libzfs_handle_t *handle = libzfs_init();
  return Data_Wrap_Struct(klass, 0, libzfs_fini, handle);
}

/*
 * call-seq:
 *   libzfs_handle.errno  => Integer, libzfs defined error number
 *
 * Return an integer with the error number when there has been any error,
 * otherwise, 0.
 *
 */
static VALUE my_libzfs_errno(VALUE self)
{
  libzfs_handle_t *handle;
  Data_Get_Struct(self, libzfs_handle_t, handle);
  return INT2NUM(libzfs_errno(handle));
}

static VALUE my_libzfs_print_on_error(VALUE self, VALUE b)
{
  libzfs_handle_t *handle;
  Data_Get_Struct(self, libzfs_handle_t, handle);

  libzfs_print_on_error(handle, RTEST(b));
  return Qnil;
}

/*
 * call-seq:
 *   libzfs_handle.error_action  => String, libzfs operation name
 *
 * When there has been any error, return the libzfs operation which has
 * triggered the error. Otherwise, return an empty string.
 *
 */
static VALUE my_libzfs_error_action(VALUE self)
{
  libzfs_handle_t *handle;
  Data_Get_Struct(self, libzfs_handle_t, handle);

  return rb_str_new2(libzfs_error_action(handle));
}

/*
 * call-seq:
 *   libzfs_handle.error_description  => String, libzfs error description
 *
 * When there has been any error, return the reason which has caused the
 * action stored at <code>libzfs_handle.error_action</code> to fail. Otherwise
 * return the string +"no error"+.
 *
 */
static VALUE my_libzfs_error_description(VALUE self)
{
  libzfs_handle_t *handle;
  Data_Get_Struct(self, libzfs_handle_t, handle);

  return rb_str_new2(libzfs_error_description(handle));
}

/*
 * call-seq:
 *   LibZfs#handle => object, libzfs_handle, instance of LibZfs
 *
 * Return <code>libzfs_handle</code> object, which can be used by some other
 * methods like <code>Zpool</code> and <code>ZFS</code> initializers.
 *
 * This method will create an instance of <code>LibZfs</code> the fist time
 * it's invoked and will store it into the class variable <code>@@handle</code>,
 * in order to reuse it every time it's required.
 *
 * This is the method used from any other methods when a <code>ZfsLib</code>
 * instance is required and none is supplied on the method call.
 *
 */
static VALUE zfsrb_lib_handle(VALUE klass)
{
  if( rb_cv_get(klass, "@@handle") == Qnil ) {
    VALUE args[] = {};
    rb_cv_set(klass, "@@handle", rb_class_new_instance(0, args, klass));
  }
  return rb_cv_get(klass, "@@handle");
}

static void Init_libzfs_consts()
{
  VALUE cZfsConsts = rb_define_module("ZfsConsts");
  VALUE mErrors = rb_define_module_under(cZfsConsts, "Errors");
  VALUE mTypes = rb_define_module_under(cZfsConsts, "Types");
  VALUE mHealthStatus = rb_define_module_under(cZfsConsts, "HealthStatus");
  VALUE mState = rb_define_module_under(cZfsConsts, "State");
  VALUE mPoolState = rb_define_module_under(mState, "Pool");

// Current version
// ZFS_VERSION has been replaced with SPA_VERSION:
#ifdef SPA_VERSION
  rb_define_const(cZfsConsts, "VERSION", INT2NUM(SPA_VERSION));
#else
  rb_define_const(cZfsConsts, "VERSION", INT2NUM(ZFS_VERSION));
#endif

  // Filesystem types
  rb_define_const(mTypes, "FILESYSTEM", INT2NUM(ZFS_TYPE_FILESYSTEM));
  rb_define_const(mTypes, "SNAPSHOT", INT2NUM(ZFS_TYPE_SNAPSHOT));
  rb_define_const(mTypes, "VOLUME", INT2NUM(ZFS_TYPE_VOLUME));
  rb_define_const(mTypes, "POOL", INT2NUM(ZFS_TYPE_POOL));
// ZFS_TYPE_ANY has been replaced with ZFS_TYPE_DATASET:
#ifdef ZFS_TYPE_DATASET
  rb_define_const(mTypes, "DATASET", INT2NUM(ZFS_TYPE_DATASET));
  rb_define_const(mTypes, "ANY", INT2NUM(ZFS_TYPE_DATASET));
#else
  rb_define_const(mTypes, "DATASET", INT2NUM(ZFS_TYPE_ANY));
  rb_define_const(mTypes, "ANY", INT2NUM(ZFS_TYPE_ANY));
#endif

  // Error codes
  rb_define_const(mErrors, "NOMEM", INT2NUM(EZFS_NOMEM));
  rb_define_const(mErrors, "BADPROP", INT2NUM(EZFS_BADPROP));
  rb_define_const(mErrors, "PROPREADONLY", INT2NUM(EZFS_PROPREADONLY));
  rb_define_const(mErrors, "PROPTYPE", INT2NUM(EZFS_PROPTYPE));
  rb_define_const(mErrors, "PROPNONINHERIT", INT2NUM(EZFS_PROPNONINHERIT));
  rb_define_const(mErrors, "PROPSPACE", INT2NUM(EZFS_PROPSPACE));
  rb_define_const(mErrors, "BADTYPE", INT2NUM(EZFS_BADTYPE));
  rb_define_const(mErrors, "BUSY", INT2NUM(EZFS_BUSY));
  rb_define_const(mErrors, "EXISTS", INT2NUM(EZFS_EXISTS));
  rb_define_const(mErrors, "NOENT", INT2NUM(EZFS_NOENT));
  rb_define_const(mErrors, "BADSTREAM", INT2NUM(EZFS_BADSTREAM));
  rb_define_const(mErrors, "DSREADONLY", INT2NUM(EZFS_DSREADONLY));
  rb_define_const(mErrors, "VOLTOOBIG", INT2NUM(EZFS_VOLTOOBIG));
  rb_define_const(mErrors, "VOLHASDATA", INT2NUM(EZFS_VOLHASDATA));
  rb_define_const(mErrors, "INVALIDNAME", INT2NUM(EZFS_INVALIDNAME));
  rb_define_const(mErrors, "BADRESTORE", INT2NUM(EZFS_BADRESTORE));
  rb_define_const(mErrors, "BADBACKUP", INT2NUM(EZFS_BADBACKUP));
  rb_define_const(mErrors, "BADTARGET", INT2NUM(EZFS_BADTARGET));
  rb_define_const(mErrors, "NODEVICE", INT2NUM(EZFS_NODEVICE));
  rb_define_const(mErrors, "BADDEV", INT2NUM(EZFS_BADDEV));
  rb_define_const(mErrors, "NOREPLICAS", INT2NUM(EZFS_NOREPLICAS));
  rb_define_const(mErrors, "RESILVERING", INT2NUM(EZFS_RESILVERING));
  rb_define_const(mErrors, "BADVERSION", INT2NUM(EZFS_BADVERSION));
  rb_define_const(mErrors, "POOLUNAVAIL", INT2NUM(EZFS_POOLUNAVAIL));
  rb_define_const(mErrors, "DEVOVERFLOW", INT2NUM(EZFS_DEVOVERFLOW));
  rb_define_const(mErrors, "BADPATH", INT2NUM(EZFS_BADPATH));
  rb_define_const(mErrors, "CROSSTARGET", INT2NUM(EZFS_CROSSTARGET));
  rb_define_const(mErrors, "ZONED", INT2NUM(EZFS_ZONED));
  rb_define_const(mErrors, "MOUNTFAILED", INT2NUM(EZFS_MOUNTFAILED));
  rb_define_const(mErrors, "UMOUNTFAILED", INT2NUM(EZFS_UMOUNTFAILED));
  rb_define_const(mErrors, "UNSHARENFSFAILED", INT2NUM(EZFS_UNSHARENFSFAILED));
  rb_define_const(mErrors, "SHARENFSFAILED", INT2NUM(EZFS_SHARENFSFAILED));
  rb_define_const(mErrors, "DEVLINKS", INT2NUM(EZFS_DEVLINKS));
  rb_define_const(mErrors, "PERM", INT2NUM(EZFS_PERM));
  rb_define_const(mErrors, "NOSPC", INT2NUM(EZFS_NOSPC));
  rb_define_const(mErrors, "IO", INT2NUM(EZFS_IO));
  rb_define_const(mErrors, "INTR", INT2NUM(EZFS_INTR));
  rb_define_const(mErrors, "ISSPARE", INT2NUM(EZFS_ISSPARE));
  rb_define_const(mErrors, "INVALCONFIG", INT2NUM(EZFS_INVALCONFIG));
  rb_define_const(mErrors, "RECURSIVE", INT2NUM(EZFS_RECURSIVE));
  rb_define_const(mErrors, "NOHISTORY", INT2NUM(EZFS_NOHISTORY));
  rb_define_const(mErrors, "UNSHAREISCSIFAILED", INT2NUM(EZFS_UNSHAREISCSIFAILED));
  rb_define_const(mErrors, "SHAREISCSIFAILED", INT2NUM(EZFS_SHAREISCSIFAILED));
  rb_define_const(mErrors, "POOLPROPS", INT2NUM(EZFS_POOLPROPS));
  rb_define_const(mErrors, "POOL_NOTSUP", INT2NUM(EZFS_POOL_NOTSUP));
  rb_define_const(mErrors, "POOL_INVALARG", INT2NUM(EZFS_POOL_INVALARG));
  rb_define_const(mErrors, "NAMETOOLONG", INT2NUM(EZFS_NAMETOOLONG));
  rb_define_const(mErrors, "UNKNOWN", INT2NUM(EZFS_UNKNOWN));

  /* Pool health status codes. */
  rb_define_const(mHealthStatus, "CORRUPT_CACHE", INT2NUM(ZPOOL_STATUS_CORRUPT_CACHE));
  rb_define_const(mHealthStatus, "MISSING_DEV_R", INT2NUM(ZPOOL_STATUS_MISSING_DEV_R));
  rb_define_const(mHealthStatus, "MISSING_DEV_NR", INT2NUM(ZPOOL_STATUS_MISSING_DEV_NR));
  rb_define_const(mHealthStatus, "CORRUPT_LABEL_R", INT2NUM(ZPOOL_STATUS_CORRUPT_LABEL_R));
  rb_define_const(mHealthStatus, "CORRUPT_LABEL_NR", INT2NUM(ZPOOL_STATUS_CORRUPT_LABEL_NR));
  rb_define_const(mHealthStatus, "BAD_GUID_SUM", INT2NUM(ZPOOL_STATUS_BAD_GUID_SUM));
  rb_define_const(mHealthStatus, "CORRUPT_POOL", INT2NUM(ZPOOL_STATUS_CORRUPT_POOL));
  rb_define_const(mHealthStatus, "CORRUPT_DATA", INT2NUM(ZPOOL_STATUS_CORRUPT_DATA));
  rb_define_const(mHealthStatus, "FAILING_DEV", INT2NUM(ZPOOL_STATUS_FAILING_DEV));
  rb_define_const(mHealthStatus, "VERSION_NEWER", INT2NUM(ZPOOL_STATUS_VERSION_NEWER));
  rb_define_const(mHealthStatus, "HOSTID_MISMATCH", INT2NUM(ZPOOL_STATUS_HOSTID_MISMATCH));
  rb_define_const(mHealthStatus, "VERSION_OLDER", INT2NUM(ZPOOL_STATUS_VERSION_OLDER));
  rb_define_const(mHealthStatus, "RESILVERING", INT2NUM(ZPOOL_STATUS_RESILVERING));
  rb_define_const(mHealthStatus, "OFFLINE_DEV", INT2NUM(ZPOOL_STATUS_OFFLINE_DEV));
  rb_define_const(mHealthStatus, "OK", INT2NUM(ZPOOL_STATUS_OK));

  /* Pool state codes */
  rb_define_const(mPoolState, "ACTIVE", INT2NUM(POOL_STATE_ACTIVE));
  rb_define_const(mPoolState, "EXPORTED", INT2NUM(POOL_STATE_EXPORTED));
  rb_define_const(mPoolState, "DESTROYED", INT2NUM(POOL_STATE_DESTROYED));
  rb_define_const(mPoolState, "SPARE", INT2NUM(POOL_STATE_SPARE));
  rb_define_const(mPoolState, "UNINITIALIZED", INT2NUM(POOL_STATE_UNINITIALIZED));
  rb_define_const(mPoolState, "UNAVAIL", INT2NUM(POOL_STATE_UNAVAIL));
  rb_define_const(mPoolState, "POTENTIALLY_ACTIVE", INT2NUM(POOL_STATE_POTENTIALLY_ACTIVE));
}

void Init_libzfs()
{
  VALUE cLibZfs = rb_define_class("LibZfs", rb_cObject);
  VALUE cZpool = rb_define_class("Zpool", rb_cObject);
  VALUE cZFS = rb_define_class("ZFS", rb_cObject);

  Init_libzfs_consts();

  rb_define_alloc_func(cLibZfs, my_libzfs_alloc);
  rb_define_class_variable(cLibZfs, "@@handle", Qnil);
  rb_define_singleton_method(cLibZfs, "handle", zfsrb_lib_handle, 0);
  rb_define_method(cLibZfs, "errno", my_libzfs_errno, 0);
  rb_define_method(cLibZfs, "print_on_error", my_libzfs_print_on_error, 1);
  rb_define_method(cLibZfs, "error_action", my_libzfs_error_action, 0);
  rb_define_method(cLibZfs, "error_description", my_libzfs_error_description, 0);

  rb_define_singleton_method(cZpool, "new", my_zpool_new, -1);
  rb_define_method(cZpool, "name", my_zpool_get_name, 0);
  rb_define_method(cZpool, "get", my_zpool_get_prop, 1);
  rb_define_method(cZpool, "set", my_zpool_set_prop, 2);
  rb_define_method(cZpool, "guid", my_zpool_get_guid, 0);
  rb_define_method(cZpool, "space_used", my_zpool_get_space_used, 0);
  rb_define_method(cZpool, "space_total", my_zpool_get_space_total, 0);
  rb_define_method(cZpool, "state", my_zpool_get_state, 0);
  rb_define_method(cZpool, "version", my_zpool_get_version, 0);
  rb_define_method(cZpool, "libzfs_handle", my_zpool_get_handle, 0);
  // rb_define_method(cZpool, "destroy!", my_zpool_destroy, 0);

  rb_define_singleton_method(cZpool, "each", my_zpool_iter, -1);

  rb_define_singleton_method(cZFS, "new", my_zfs_new, -1);
  rb_define_method(cZFS, "libzfs_handle", my_zfs_get_handle, 0);
  rb_define_method(cZFS, "name", my_zfs_get_name, 0);
  rb_define_method(cZFS, "fs_type", my_zfs_get_type, 0);
  rb_define_method(cZFS, "rename", my_zfs_rename, 2);
  // Sharing
  rb_define_method(cZFS, "is_shared?", my_zfs_is_shared, 0);
  rb_define_method(cZFS, "share!", my_zfs_share, 0);
  rb_define_method(cZFS, "unshare!", my_zfs_unshare, 0);
  rb_define_method(cZFS, "nfs_share_name", my_zfs_nfs_share_name, 0);
  rb_define_method(cZFS, "share_nfs!", my_zfs_share_nfs, 0);
  rb_define_method(cZFS, "unshare_nfs!", my_zfs_unshare_nfs, 0);
  rb_define_method(cZFS, "is_shared_iscsi?", my_zfs_is_shared_iscsi, 0);
  rb_define_method(cZFS, "share_iscsi!", my_zfs_share_iscsi, 0);
  rb_define_method(cZFS, "unshare_iscsi!", my_zfs_unshare_iscsi, 0);
  // Exist, Create, Destroy:
  rb_define_singleton_method(cZFS, "create", my_zfs_create, -1);
  // These exist? and exists? are alias.
  rb_define_singleton_method(cZFS, "exists?", my_zfs_dataset_exists, -1);
  rb_define_singleton_method(cZFS, "exist?", my_zfs_dataset_exists, -1);
  rb_define_method(cZFS, "destroy!", my_zfs_destroy, 0);
  // Properties:
  rb_define_method(cZFS, "get", my_zfs_get_prop, 1);
  rb_define_method(cZFS, "get_user_prop", my_zfs_get_user_prop, 1);
  rb_define_method(cZFS, "set", my_zfs_set_prop, 2);
  // ZFS Iteration:
  rb_define_singleton_method(cZFS, "each", my_zfs_iter_root, -1);
  rb_define_method(cZFS, "each_filesystem", my_zfs_iter_filesystems, 0);
  rb_define_method(cZFS, "each_snapshot", my_zfs_iter_snapshots, 0);
  rb_define_method(cZFS, "each_dependent", my_zfs_iter_dependents, 0);
  // Snapshots:
  rb_define_singleton_method(cZFS, "snapshot", my_zfs_snapshot, -1);
  // Clones:
  rb_define_method(cZFS, "clone!", my_zfs_clone, 1);
  rb_define_method(cZFS, "promote", my_zfs_promote, 0);
}
