#include <ruby.h>

#ifdef HAVE_LIBZFS_H
  #include <libzfs.h>
#endif

// Define Error Classes:
static VALUE cZfsError = Qnil;
static VALUE cZfsNoMemoryError = Qnil;
static VALUE cZfsInvalidPropertyError = Qnil;
static VALUE cZfsReadOnlyPropertyError = Qnil;
static VALUE cZfsInvalidPropertyTypeError = Qnil;
static VALUE cZfsNonInheritablePropertyError = Qnil;
static VALUE cZfsPropertySpaceError = Qnil;
static VALUE cZfsInvalidDatasetTypeError = Qnil;
static VALUE cZfsDatasetBusyError = Qnil;
static VALUE cZfsDatasetExistsError = Qnil;
static VALUE cZfsNoentError = Qnil;
static VALUE cZfsBadStreamError = Qnil;
static VALUE cZfsDatasetReadOnlyError = Qnil;
static VALUE cZfsVolumeTooBigError = Qnil;
static VALUE cZfsInvalidNameError = Qnil;
static VALUE cZfsBadRestoreError = Qnil;
static VALUE cZfsBadBackupError = Qnil;
static VALUE cZfsBadTargetError = Qnil;
static VALUE cZfsNoDeviceError = Qnil;
static VALUE cZfsBadDeviceError = Qnil;
static VALUE cZfsNoReplicasError = Qnil;
static VALUE cZfsResilveringError = Qnil;
static VALUE cZfsBadVersionError = Qnil;
static VALUE cZfsPoolUnavailableError = Qnil;
static VALUE cZfsDeviceOverflowError = Qnil;
static VALUE cZfsBadPathError = Qnil;
static VALUE cZfsCrossTargetError = Qnil;
static VALUE cZfsZonedError = Qnil;
static VALUE cZfsMountFailedError = Qnil;
static VALUE cZfsUnmountFailedError = Qnil;
static VALUE cZfsUnshareNfsFailedError = Qnil;
static VALUE cZfsShareNfsFailedError = Qnil;
static VALUE cZfsUnshareSmbFailedError = Qnil;
static VALUE cZfsShareSmbFailedError = Qnil;
static VALUE cZfsPermError = Qnil;
static VALUE cZfsNospcError = Qnil;
static VALUE cZfsFaultError = Qnil;
static VALUE cZfsIOError = Qnil;
static VALUE cZfsINTRError = Qnil;
static VALUE cZfsIsSpareError = Qnil;
static VALUE cZfsInvalidConfigError = Qnil;
static VALUE cZfsRecursiveError = Qnil;
static VALUE cZfsNoHistoryError = Qnil;
static VALUE cZfsPoolPropsError = Qnil;
static VALUE cZfsPoolNotSupportedError = Qnil;
static VALUE cZfsPoolInvalidArgError = Qnil;
static VALUE cZfsNameTooLongError = Qnil;
static VALUE cZfsOpenFailedError = Qnil;
static VALUE cZfsNoCapError = Qnil;
static VALUE cZfsLabelFailedError = Qnil;
static VALUE cZfsBadWhoError = Qnil;
static VALUE cZfsBadPermError = Qnil;
static VALUE cZfsBadPermSetError = Qnil;
static VALUE cZfsNoDelegationError = Qnil;
static VALUE cZfsPermReadOnlyError = Qnil;
static VALUE cZfsBadCacheError = Qnil;
static VALUE cZfsIsL2CacheError = Qnil;
static VALUE cZfsVdevNotSupportedError = Qnil;
static VALUE cZfsNotSupportedError = Qnil;
static VALUE cZfsActiveSpareError = Qnil;
static VALUE cZfsUnplayedLogsError = Qnil;
static VALUE cZfsReftagReleError = Qnil;
static VALUE cZfsReftagHoldError = Qnil;
static VALUE cZfsTagTooLongError = Qnil;
static VALUE cZfsPipeFailedError = Qnil;
static VALUE cZfsThreadCreateFailedError = Qnil;
static VALUE cZfsPostSplitOnlineError = Qnil;
// iSCSI:
static VALUE cZfsUnshareISCSIFailedError = Qnil;
static VALUE cZfsShareISCSIFailedError = Qnil;

/*
 * Select the appropriate Ruby Error class based on libzfs_errno
 */
static VALUE zetta_lib_select_error(int zetta_zfserrno)
{
  VALUE error = Qnil;

  switch (zetta_zfserrno) {
    case EZFS_NOMEM: error = cZfsNoMemoryError; break;
    case EZFS_BADPROP: error = cZfsInvalidPropertyError; break;
    case EZFS_PROPREADONLY: error = cZfsReadOnlyPropertyError; break;
    case EZFS_PROPTYPE: error = cZfsInvalidPropertyTypeError; break;
    case EZFS_PROPNONINHERIT: error = cZfsNonInheritablePropertyError; break;
    case EZFS_PROPSPACE: error = cZfsPropertySpaceError; break;
    case EZFS_BADTYPE: error = cZfsInvalidDatasetTypeError; break;
    case EZFS_BUSY: error = cZfsDatasetBusyError; break;
    case EZFS_EXISTS: error = cZfsDatasetExistsError; break;
    case EZFS_NOENT: error = cZfsNoentError; break;
    case EZFS_BADSTREAM: error = cZfsBadStreamError; break;
    case EZFS_DSREADONLY: error = cZfsDatasetReadOnlyError; break;
    case EZFS_VOLTOOBIG: error = cZfsVolumeTooBigError; break;
    case EZFS_INVALIDNAME: error = cZfsInvalidNameError; break;
    case EZFS_BADRESTORE: error = cZfsBadRestoreError; break;
    case EZFS_BADBACKUP: error = cZfsBadBackupError; break;
    case EZFS_BADTARGET: error = cZfsBadTargetError; break;
    case EZFS_NODEVICE: error = cZfsNoDeviceError; break;
    case EZFS_BADDEV: error = cZfsBadDeviceError; break;
    case EZFS_NOREPLICAS: error = cZfsNoReplicasError; break;
    case EZFS_RESILVERING: error = cZfsResilveringError; break;
    case EZFS_BADVERSION: error = cZfsBadVersionError; break;
    case EZFS_POOLUNAVAIL: error = cZfsPoolUnavailableError; break;
    case EZFS_DEVOVERFLOW: error = cZfsDeviceOverflowError; break;
    case EZFS_BADPATH: error = cZfsBadPathError; break;
    case EZFS_CROSSTARGET: error = cZfsCrossTargetError; break;
    case EZFS_ZONED: error = cZfsZonedError; break;
    case EZFS_MOUNTFAILED: error = cZfsMountFailedError; break;
    case EZFS_UMOUNTFAILED: error = cZfsUnmountFailedError; break;
    case EZFS_UNSHARENFSFAILED: error = cZfsUnshareNfsFailedError; break;
    case EZFS_SHARENFSFAILED: error = cZfsShareNfsFailedError; break;
    case EZFS_UNSHARESMBFAILED: error = cZfsUnshareSmbFailedError; break;
    case EZFS_SHARESMBFAILED: error = cZfsShareSmbFailedError; break;
    case EZFS_PERM: error = cZfsPermError; break;
    case EZFS_NOSPC: error = cZfsNospcError; break;
    case EZFS_IO: error = cZfsIOError; break;
    case EZFS_INTR: error = cZfsINTRError; break;
    case EZFS_ISSPARE: error = cZfsIsSpareError; break;
    case EZFS_INVALCONFIG: error = cZfsInvalidConfigError; break;
    case EZFS_RECURSIVE: error = cZfsRecursiveError; break;
    case EZFS_NOHISTORY: error = cZfsNoHistoryError; break;
    case EZFS_POOLPROPS: error = cZfsPoolPropsError; break;
    case EZFS_POOL_NOTSUP: error = cZfsPoolNotSupportedError; break;
    case EZFS_POOL_INVALARG: error = cZfsPoolInvalidArgError; break;
    case EZFS_NAMETOOLONG: error = cZfsNameTooLongError; break;
    case EZFS_OPENFAILED: error = cZfsOpenFailedError; break;
    case EZFS_NOCAP: error = cZfsNoCapError; break;
    case EZFS_LABELFAILED: error = cZfsLabelFailedError; break;
    case EZFS_BADWHO: error = cZfsBadWhoError; break;
    case EZFS_BADPERM: error = cZfsBadPermError; break;
    case EZFS_BADPERMSET: error = cZfsBadPermSetError; break;
    case EZFS_NODELEGATION: error = cZfsNoDelegationError; break;
    case EZFS_PERMRDONLY: error = cZfsPermReadOnlyError; break;
    case EZFS_BADCACHE: error = cZfsBadCacheError; break;
    case EZFS_ISL2CACHE: error = cZfsIsL2CacheError; break;
    case EZFS_VDEVNOTSUP: error = cZfsVdevNotSupportedError; break;

// Trying to keep it compatible with snv_89:
#ifdef SPA_VERSION_11
    case EZFS_NOTSUP: error = cZfsNotSupportedError; break;
#endif

#ifdef SPA_VERSION_12
    case EZFS_ACTIVE_SPARE: error = cZfsActiveSpareError; break;
#endif
// Open Solaris 2009.06 - Will set the supported systems here? (snv_111b)
#ifdef SPA_VERSION_16
    case EZFS_UNPLAYED_LOGS: error = cZfsUnplayedLogsError; break;
#endif

#ifdef SPA_VERSION_18
    case EZFS_TAGTOOLONG: error = cZfsTagTooLongError; break;
    case EZFS_REFTAG_RELE: error = cZfsReftagReleError; break;
    case EZFS_REFTAG_HOLD: error = cZfsReftagHoldError; break;
#endif

#ifdef SPA_VERSION_22
    case EZFS_PIPEFAILED: error = cZfsPipeFailedError; break;
    case EZFS_THREADCREATEFAILED: error = cZfsThreadCreateFailedError; break;
#endif

#ifdef SPA_VERSION_23
    case EZFS_POSTSPLIT_ONLINE: error = cZfsPostSplitOnlineError; break;
#endif

#ifndef SPA_VERSION_24
    case EZFS_UNSHAREISCSIFAILED: error = cZfsUnshareISCSIFailedError; break;
    case EZFS_SHAREISCSIFAILED: error = cZfsShareISCSIFailedError; break;
#else
    case EZFS_FAULT: error = cZfsFaultError; break;
#endif

    default: error = cZfsError;
  }
  return error;
}


/*
 * Raise the proper Ruby error bassed into libzfs error.
 * (Extracted from zetta_lib_raise_error so we can raise ruby exceptions
 * directly from C functions).
 */
static VALUE zetta_lib_error_exception(libzfs_handle_t *handle)
{
  rb_raise(
    zetta_lib_select_error(libzfs_errno(handle)),
    "%s: %s", libzfs_error_action(handle), libzfs_error_description(handle));
}

// Internal method: used to make libzfs_handle argument optional.
static VALUE zetta_lib_get_handle()
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
 * Raise <code>ZfsError::NoentError</code> when the ZFS storage pool does not
 * exist.
 *
 */
static VALUE zetta_pool_new(int argc, VALUE *argv, VALUE klass)
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

  libzfs_handle = (argc == 1) ? zetta_lib_get_handle() : argv[1];

  if(CLASS_OF(libzfs_handle) != rb_const_get(rb_cObject, rb_intern("LibZfs"))) {
    rb_raise(rb_eTypeError, "ZFS Lib handle must be an instance of LibZfs.");
  }

  Data_Get_Struct(libzfs_handle, libzfs_handle_t, libhandle);

  zpool_handle = zpool_open_canfail(libhandle, StringValuePtr(pool_name));

  if(zpool_handle != NULL) {
    return Data_Wrap_Struct(klass, 0, zpool_close, zpool_handle);
  }
  // Raise exception when cannot get a proper Zpool handle:
  zetta_lib_error_exception(libhandle);
}

/*
 * call-seq:
 *   @zpool.libzfs_handle  => object, zpool_handle
 *
 * Return <code>zpool_handle</code> associated with a <code>Zpool</code>
 * instance.
 *
 */
static VALUE zetta_pool_get_handle(VALUE self)
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
static VALUE zetta_pool_get_name(VALUE self)
{
  zpool_handle_t *zpool_handle;
  Data_Get_Struct(self, zpool_handle_t, zpool_handle);

  return rb_str_new2(zpool_get_name(zpool_handle));
}

/*
 * call-seq:
 *   @zpool.get('propname')  => string/integer, zpool property value or Nil.
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
static VALUE zetta_pool_get_prop(VALUE self, VALUE name)
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
    if ( zpool_get_prop(zpool_handle, zpool_prop, propval, sizeof (propval), NULL) != 0 ) {
      return Qnil;
    }
    return ( strcmp( propval, "-" ) == 0 ) ? Qnil: rb_str_new2(propval);
  }
}

/*
 * call-seq:
 *   @zpool.set('propname', "propval")  => Boolean
 *
 * Set the given value for the given zpool property.
 *
 * Raise <code>TypeError</code> when <code>propname</code>
 * is not a <code>String</code>.
 * Raise <code>TypeError</code> when <code>proval</code>
 * is not a <code>String</code>.
 *
 */
static VALUE zetta_pool_set_prop(VALUE self, VALUE propname, VALUE propval)
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

  return ( zpool_set_prop(zpool_handle, name, val) == 0 ) ? Qtrue : Qfalse;
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
static VALUE zetta_pool_get_guid(VALUE self)
{
  zpool_handle_t *zpool_handle;
  Data_Get_Struct(self, zpool_handle_t, zpool_handle);

  return ULL2NUM(zpool_get_prop_int(zpool_handle, ZPOOL_PROP_GUID, NULL));
}

static VALUE zetta_pool_get_space_used(VALUE self)
{
  zpool_handle_t *zpool_handle;
  Data_Get_Struct(self, zpool_handle_t, zpool_handle);

  return ULL2NUM(zpool_get_space_used(zpool_handle));
}

static VALUE zetta_pool_get_space_total(VALUE self)
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
 * The value is one of the constants defined at module
 * <code>ZfsConsts::State::Pool</code>.
 *
 */
static VALUE zetta_pool_get_state(VALUE self)
{
  VALUE state = Qnil;

  VALUE cZfsConsts = rb_const_get(rb_cObject, rb_intern("ZfsConsts"));
  VALUE mState = rb_const_get(cZfsConsts, rb_intern("State"));
  VALUE mPoolState = rb_const_get(mState, rb_intern("Pool"));
  zpool_handle_t *zpool_handle;
  Data_Get_Struct(self, zpool_handle_t, zpool_handle);

  switch (zpool_get_state(zpool_handle)) {
    case POOL_STATE_ACTIVE: state = rb_const_get(mPoolState, rb_intern("ACTIVE")); break;
    case POOL_STATE_EXPORTED: state = rb_const_get(mPoolState, rb_intern("EXPORTED")); break;
    case POOL_STATE_DESTROYED: state = rb_const_get(mPoolState, rb_intern("DESTROYED")); break;
    case POOL_STATE_SPARE: state = rb_const_get(mPoolState, rb_intern("SPARE")); break;
#ifdef SPA_VERSION_10
    case POOL_STATE_L2CACHE: state = rb_const_get(mPoolState, rb_intern("L2CACHE")); break;
#endif
    case POOL_STATE_UNINITIALIZED: state = rb_const_get(mPoolState, rb_intern("UNINITIALIZED")); break;
    case POOL_STATE_UNAVAIL: state = rb_const_get(mPoolState, rb_intern("UNAVAIL")); break;
    case POOL_STATE_POTENTIALLY_ACTIVE: state = rb_const_get(mPoolState, rb_intern("POTENTIALLY_ACTIVE")); break;
    default: state = rb_const_get(mPoolState, rb_intern("UNKNOWN"));
  }

  return state;
}

/*
 * call-seq:
 *   @zpool.health_status  => integer, zpool health status.
 *
 * Return current <code>Zpool</code> instance health status as an integer.
 *
 * The value is one of the constants defined at module
 * <code>ZfsConsts::HealthStatus::Pool</code>.
 *
 */
static VALUE zetta_pool_get_health_status(VALUE self)
{
  VALUE status = Qnil;
  VALUE cZfsConsts = rb_const_get(rb_cObject, rb_intern("ZfsConsts"));
  VALUE mHealthStatus = rb_const_get(cZfsConsts, rb_intern("HealthStatus"));

  char *msgid;
  zpool_handle_t *zpool_handle;
  Data_Get_Struct(self, zpool_handle_t, zpool_handle);

  switch (zpool_get_status(zpool_handle, &msgid)) {
    case ZPOOL_STATUS_CORRUPT_CACHE: status = rb_const_get(mHealthStatus, rb_intern("CORRUPT_CACHE"));
    case ZPOOL_STATUS_MISSING_DEV_R: status = rb_const_get(mHealthStatus, rb_intern("MISSING_DEV_R"));
    case ZPOOL_STATUS_MISSING_DEV_NR: status = rb_const_get(mHealthStatus, rb_intern("MISSING_DEV_NR"));
    case ZPOOL_STATUS_CORRUPT_LABEL_R: status = rb_const_get(mHealthStatus, rb_intern("CORRUPT_LABEL_R"));
    case ZPOOL_STATUS_CORRUPT_LABEL_NR: status = rb_const_get(mHealthStatus, rb_intern("CORRUPT_LABEL_NR"));
    case ZPOOL_STATUS_BAD_GUID_SUM: status = rb_const_get(mHealthStatus, rb_intern("BAD_GUID_SUM"));
    case ZPOOL_STATUS_CORRUPT_POOL: status = rb_const_get(mHealthStatus, rb_intern("CORRUPT_POOL"));
    case ZPOOL_STATUS_CORRUPT_DATA: status = rb_const_get(mHealthStatus, rb_intern("CORRUPT_DATA"));
    case ZPOOL_STATUS_FAILING_DEV: status = rb_const_get(mHealthStatus, rb_intern("FAILING_DEV"));
    case ZPOOL_STATUS_VERSION_NEWER: status = rb_const_get(mHealthStatus, rb_intern("VERSION_NEWER"));
    case ZPOOL_STATUS_HOSTID_MISMATCH: status = rb_const_get(mHealthStatus, rb_intern("HOSTID_MISMATCH"));
#ifdef SPA_VERSION_11
    case ZPOOL_STATUS_IO_FAILURE_WAIT: status = rb_const_get(mHealthStatus, rb_intern("IO_FAILURE_WAIT"));
    case ZPOOL_STATUS_IO_FAILURE_CONTINUE: status = rb_const_get(mHealthStatus, rb_intern("IO_FAILURE_CONTINUE"));
#endif

#ifdef SPA_VERSION_14
    case ZPOOL_STATUS_BAD_LOG: status = rb_const_get(mHealthStatus, rb_intern("BAD_LOG"));
#endif

  // Older than SPA_VERSION, safe:
    case ZPOOL_STATUS_FAULTED_DEV_R: status = rb_const_get(mHealthStatus, rb_intern("FAULTED_DEV_R"));
    case ZPOOL_STATUS_FAULTED_DEV_NR: status = rb_const_get(mHealthStatus, rb_intern("FAULTED_DEV_NR"));
    case ZPOOL_STATUS_VERSION_OLDER: status = rb_const_get(mHealthStatus, rb_intern("VERSION_OLDER"));
    case ZPOOL_STATUS_RESILVERING: status = rb_const_get(mHealthStatus, rb_intern("RESILVERING"));
    case ZPOOL_STATUS_OFFLINE_DEV: status = rb_const_get(mHealthStatus, rb_intern("OFFLINE_DEV"));

#ifdef SPA_VERSION_18
    case ZPOOL_STATUS_REMOVED_DEV: status = rb_const_get(mHealthStatus, rb_intern("REMOVED_DEV"));
#endif

    case ZPOOL_STATUS_OK: status = rb_const_get(mHealthStatus, rb_intern("OK"));
    default: state = rb_const_get(mHealthStatus, rb_intern("UNKNOWN"));
  }
  return status;
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
static VALUE zetta_pool_get_version(VALUE self)
{
  zpool_handle_t *zpool_handle;
  Data_Get_Struct(self, zpool_handle_t, zpool_handle);

  return ULL2NUM(zpool_get_prop_int(zpool_handle, ZPOOL_PROP_VERSION, NULL));
}

static int zetta_pool_iter_f(zpool_handle_t *handle, void *klass)
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
static VALUE zetta_pool_iter(int argc, VALUE *argv, VALUE klass)
{
  VALUE libzfs_handle;

  libzfs_handle_t *libhandle;

  libzfs_handle = (argc == 0) ? zetta_lib_get_handle() : argv[0];

  if(CLASS_OF(libzfs_handle) != rb_const_get(rb_cObject, rb_intern("LibZfs"))) {
    rb_raise(rb_eTypeError, "ZFS Lib handle must be an instance of LibZfs.");
  }

  Data_Get_Struct(libzfs_handle, libzfs_handle_t, libhandle);

  zpool_iter(libhandle, zetta_pool_iter_f, (void *)klass);

  return Qnil;
}

// static VALUE zetta_pool_create(VALUE libzfs_handle, VALUE name, VALUE vdevs, VALUE altroot)
// {
//   libzfs_handle_t *libhandle;
//   nvlist_t *vdevs = NULL;
//   Data_Get_Struct(libzfs_handle, libzfs_handle_t, libhandle);
//
//   return INT2NUM(zpool_create(libzfs_handle, StringValuePtr(name), vdev_list, StringValuePtr(altroot)));
// }

// FIXME: Doesn't appear to work?  Maybe I actually need to offline it before
// I can destroy?  If so, that's something for the higher level Ruby library.
// static VALUE zetta_pool_destroy(VALUE self)
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
static VALUE zetta_fs_get_handle(VALUE self)
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
static VALUE zetta_fs_new(int argc, VALUE *argv, VALUE klass)
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

  libzfs_handle = (argc == 2) ? zetta_lib_get_handle() : argv[2];

  if(CLASS_OF(libzfs_handle) != rb_const_get(rb_cObject, rb_intern("LibZfs"))) {
    rb_raise(rb_eTypeError, "ZFS Lib handle must be an instance of LibZfs.");
  }

  Data_Get_Struct(libzfs_handle, libzfs_handle_t, libhandle);
  zfs_handle = zfs_open(libhandle, StringValuePtr(fs_name), NUM2INT(types));
  // Prevent Segementation Faults when the given Dataset does not exist and
  // somebody tries to access to a given property:
  if(zfs_handle != NULL) {
    return Data_Wrap_Struct(klass, 0, zfs_close, zfs_handle);
  }
  // Raise exception when cannot get a proper ZFS handle:
  zetta_lib_error_exception(libhandle);
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
static VALUE zetta_fs_get_name(VALUE self)
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
static VALUE zetta_fs_get_type(VALUE self)
{
  zfs_handle_t *zfs_handle;
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  return INT2NUM(zfs_get_type(zfs_handle));
}

/*
 * call-seq:
 *   @zfs.get('propname')  => string/integer, zfs property value or Nil
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
static VALUE zetta_fs_get_prop(VALUE self, VALUE name)
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

  if ( zfs_prop_get(zfs_handle, zfs_prop, propval, sizeof(propval), NULL, NULL, 0, B_FALSE) != 0 ) {
    return Qnil;
  }
  return ( strcmp( propval, "-" ) == 0 ) ? Qnil: rb_str_new2(propval);
}

/*
 * call-seq:
 *   @zfs.set('propname', "propval")  => Boolean
 *
 * Set the given value for the given zfs dataset property.
 *
 * Raise <code>TypeError</code> when <code>propname</code>
 * is not a <code>String</code>.
 * Raise <code>TypeError</code> when <code>proval</code>
 * is not a <code>String</code>.
 *
 */

static VALUE zetta_fs_set_prop(VALUE self, VALUE propname, VALUE propval)
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
  return ( zfs_prop_set(zfs_handle, name, val) == 0 ) ? Qtrue : Qfalse;
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
static VALUE zetta_fs_get_user_prop(VALUE self, VALUE name)
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
 *   @zfs.rename('dataset/name', [false|true])  => Boolean
 *
 * Rename ZFS Dataset using the given <code>dataset_name</code>.
 * When <code>recursive</code> is true, also renames all the children
 * datasets for the current one.
 *
 *
 * Raise <code>TypeError</code> when <code>dataset_name</code> is given and it
 * is not a <code>String</code>.
 *
 */
static VALUE zetta_fs_rename(VALUE self, VALUE target, VALUE recursive)
{
  zfs_handle_t *zfs_handle;
  if( TYPE(target) != T_STRING ) {
    rb_raise(rb_eTypeError, "Target dataset name must be a string.");
  }
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  return ( zfs_rename(zfs_handle, StringValuePtr(target), RTEST(recursive)) == 0 ) ? Qtrue : Qfalse;
}

/*
 * call-seq:
 *   ZFS#create('dataset/name', ZfsConsts::Types)  => @zfs dataset instance.
 *   ZFS#create('dataset/name', ZfsConsts::Types, @zlib)  => @zfs dataset instance.
 *
 * Given a <code>dataset_name</code>, and a <code>dataset_type</code>,
 * create a zfs dataset.
 *
 * Return a new Zfs instance for the given dataset on success or raise
 * exception on failure failure.
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
static VALUE zetta_fs_create(int argc, VALUE *argv, VALUE klass)
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

  libzfs_handle = (argc == 2) ? zetta_lib_get_handle() : argv[2];

  if(CLASS_OF(libzfs_handle) != rb_const_get(rb_cObject, rb_intern("LibZfs"))) {
    rb_raise(rb_eTypeError, "ZFS Lib handle must be an instance of LibZfs.");
  }

  Data_Get_Struct(libzfs_handle, libzfs_handle_t, libhandle);

  if (0 == zfs_create(libhandle, StringValuePtr(fs_name), NUM2INT(types), NULL)){
    zfs_handle_t  *zfs_handle;
    zfs_handle = zfs_open(libhandle, StringValuePtr(fs_name), NUM2INT(types));
    return Data_Wrap_Struct(klass, 0, zfs_close, zfs_handle);
  }
  // Raise exception when cannot get a proper ZFS handle:
  zetta_lib_error_exception(libhandle);
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
static VALUE zetta_fs_dataset_exists(int argc, VALUE *argv, VALUE klass)
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

  libzfs_handle = (argc == 2) ? zetta_lib_get_handle() : argv[2];

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
 * Return a ZFS snapshot instance on success or raise error on failure.
 *
 * Raise <code>ArgumentError</code> when <code>snapshot_name</code> is not
 * given.
 * Raise <code>TypeError</code> when <code>snapshot_name</code> is given and it
 * is not a <code>String</code>.
 * Raise <code>TypeError</code> when <code>@zlib</code> handle is given and it
 * is not an instance of <code>LibZfs</code>.
 *
 */
static VALUE zetta_fs_snapshot(int argc, VALUE *argv, VALUE klass)
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

  libzfs_handle = (argc == 1) ? zetta_lib_get_handle() : argv[1];

  if(CLASS_OF(libzfs_handle) != rb_const_get(rb_cObject, rb_intern("LibZfs"))) {
    rb_raise(rb_eTypeError, "ZFS Lib handle must be an instance of LibZfs.");
  }

  Data_Get_Struct(libzfs_handle, libzfs_handle_t, libhandle);

  if ( 0 == zfs_snapshot(libhandle, StringValuePtr(snapshot_name), B_FALSE, NULL) ){
    zfs_handle_t  *zfs_handle;
    zfs_handle = zfs_open(libhandle, StringValuePtr(snapshot_name), ZFS_TYPE_SNAPSHOT);
    return Data_Wrap_Struct(klass, 0, zfs_close, zfs_handle);
  }
  zetta_lib_error_exception(libhandle);
}

/*
 * call-seq:
 *   @zfs.rollback(@zfs_snapshot, true|false)  => Boolean
 *
 * Rollback the current <code>ZFS</code> instance to the given
 * ZFS Snapshot instance. Second argument <code>force</code> has the
 * same efect than the <code>-Rf</code> option for <code>zfs rollback</code>.
 *
 *
 * Raise <code>TypeError</code> when <code>snapshot</code>
 * is not an instance of <code>ZFS</code>.
 * Raise <code>TypeError</code> when <code>snapshot</code>
 * instance of <code>ZFS</code> is not of type <i>snapshot</i>.
 * Raise <code>NoMethodError</code> when the current <code>ZfsLib</code>
 * instance is not a Filesystem or Volume.
 *
 */
static VALUE zetta_fs_rollback(VALUE self, VALUE snapshot, VALUE force)
{
  zfs_handle_t *zfs_handle, *snapshot_zfs_handle;

  if(CLASS_OF(snapshot) != rb_const_get(rb_cObject, rb_intern("ZFS"))) {
    rb_raise(rb_eTypeError, "Snapshot must be an instance of ZFS.");
  }
  // Get the snapshot handle:
  Data_Get_Struct(snapshot, zfs_handle_t, snapshot_zfs_handle);
  // Get current volume/filesystem handle:
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);
  // Raise error if the given snapshot instance is not a Snapshot:
  if(zfs_get_type(snapshot_zfs_handle) != ZFS_TYPE_SNAPSHOT) {
    rb_raise(rb_eTypeError, "ZFS snapshot instance must be of type Snapshot.");
  }
  // Raise error when trying to rollback whatever not a volume/filesystem:
  if( zfs_get_type(zfs_handle) != ZFS_TYPE_FILESYSTEM && zfs_get_type(zfs_handle) != ZFS_TYPE_VOLUME ) {
    rb_raise(rb_eNoMethodError, "Rollback operation is only available for Datasets of type filesystem or volume.");
  }

  return ( zfs_rollback(zfs_handle, snapshot_zfs_handle, RTEST(force)) == 0 ) ? Qtrue : Qfalse;
}

/*
 * call-seq:
 *   @zfs.clone!('clone_name')  => ZFS instance
 *
 * Create a clone with the given <code>clone_name</code> for the current
 * ZFS Snapshot instance. Raise <code>ZfsError</code> on failure.
 *
 *
 * Raise <code>TypeError</code> when <code>clone_name</code>
 * is not a <code>String</code>.
 * Raise <code>NoMethodError</code> when the current <code>ZfsLib</code>
 * instance is not a Snapshot.
 *
 *
 * NOTE: This method cannot be <i>clone</i> due to obvious Ruby reasons.
 *
 */
static VALUE zetta_fs_clone(VALUE self, VALUE clone_name)
{
  zfs_handle_t *zfs_handle;

  if( TYPE(clone_name) != T_STRING ) {
    rb_raise(rb_eTypeError, "Clone name must be a string.");
  }

  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  if(zfs_get_type(zfs_handle) != ZFS_TYPE_SNAPSHOT) {
    rb_raise(rb_eNoMethodError, "Clone operation is only available for Datasets of type snapshot.");
  }

  libzfs_handle_t *libhandle = zfs_get_handle(zfs_handle);

  if (0 == zfs_clone(zfs_handle, StringValuePtr(clone_name), NULL)){
    zfs_handle_t  *zfs_clone_handle;

    zfs_clone_handle = zfs_open(libhandle, StringValuePtr(clone_name), ZFS_TYPE_FILESYSTEM);
    return Data_Wrap_Struct(CLASS_OF(self), 0, zfs_close, zfs_clone_handle);
  }
  zetta_lib_error_exception(libhandle);
}

/*
 * call-seq:
 *   @zfs.promote  => Boolean
 *
 * Promote the current clone to be no longer dependent on its origin.
 *
 */
static VALUE zetta_fs_promote(VALUE self)
{
  zfs_handle_t *zfs_handle;

  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  return (zfs_promote(zfs_handle) == 0) ? Qtrue : Qfalse;
}

static VALUE zetta_fs_is_shared(VALUE self)
{
  zfs_handle_t *zfs_handle;
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  return zfs_is_shared(zfs_handle) ? Qtrue : Qfalse;
}

static VALUE zetta_fs_share(VALUE self)
{
  zfs_handle_t *zfs_handle;
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  return INT2NUM(zfs_share(zfs_handle));
}

static VALUE zetta_fs_unshare(VALUE self)
{
  zfs_handle_t *zfs_handle;
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  return INT2NUM(zfs_unshare(zfs_handle));
}

static VALUE zetta_fs_nfs_share_name(VALUE self)
{
  zfs_handle_t *zfs_handle;
  char *path;
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  return zfs_is_shared_nfs(zfs_handle, &path) ? rb_str_new2(path) : Qnil;
}

static VALUE zetta_fs_is_shared_nfs(VALUE self)
{
  zfs_handle_t *zfs_handle;
  char *path;
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);
  return zfs_is_shared_nfs(zfs_handle, &path) ? Qtrue : Qfalse;
}

static VALUE zetta_fs_share_nfs(VALUE self)
{
  zfs_handle_t *zfs_handle;
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  return INT2NUM(zfs_share_nfs(zfs_handle));
}

static VALUE zetta_fs_unshare_nfs(VALUE self)
{
  zfs_handle_t *zfs_handle;
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  return INT2NUM(zfs_unshare_nfs(zfs_handle, NULL));
}

#ifdef SPA_VERSION_9
static VALUE zetta_fs_is_shared_smb(VALUE self)
{
  zfs_handle_t *zfs_handle;
  char *path;
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);
  return zfs_is_shared_smb(zfs_handle, &path) ? Qtrue : Qfalse;
}

static VALUE zetta_fs_smb_share_name(VALUE self)
{
  zfs_handle_t *zfs_handle;
  char *path;
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  return zfs_is_shared_smb(zfs_handle, &path) ? rb_str_new2(path) : Qnil;
}

static VALUE zetta_fs_share_smb(VALUE self)
{
  zfs_handle_t *zfs_handle;
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  return INT2NUM(zfs_share_smb(zfs_handle));
}

static VALUE zetta_fs_unshare_smb(VALUE self)
{
  zfs_handle_t *zfs_handle;
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  return INT2NUM(zfs_unshare_smb(zfs_handle, NULL));
}
#endif

#ifndef SPA_VERSION_24
static VALUE zetta_fs_is_shared_iscsi(VALUE self)
{
  zfs_handle_t *zfs_handle;
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  return zfs_is_shared_iscsi(zfs_handle) ? Qtrue : Qfalse;
}

static VALUE zetta_fs_share_iscsi(VALUE self)
{
  zfs_handle_t *zfs_handle;
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  return INT2NUM(zfs_share_iscsi(zfs_handle));
}

static VALUE zetta_fs_unshare_iscsi(VALUE self)
{
  zfs_handle_t *zfs_handle;
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  return INT2NUM(zfs_unshare_iscsi(zfs_handle));
}
#endif

/*
 * call-seq:
 *   @zfs.is_mounted?  => Boolean
 *
 * Checks if the current dataset instance is or not mounted
 *
 * TODO:
 *
 * - Actually, not taking into consideration anything but default mountpoints
 *
 */
static VALUE zetta_fs_is_mounted(VALUE self)
{
  zfs_handle_t *zfs_handle;
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  return zfs_is_mounted(zfs_handle, NULL) ? Qtrue : Qfalse;
}

/*
 * call-seq:
 *   @zfs.mount  => Boolean
 *
 * Mount the current dataset instance.
 *
 */
static VALUE zetta_fs_mount(VALUE self)
{
  zfs_handle_t *zfs_handle;
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  return (zfs_mount(zfs_handle, NULL, 0) == 0) ? Qtrue : Qfalse;
}

/*
 * call-seq:
 *   @zfs.unmount  => Boolean
 *
 * Unmount the current dataset instance.
 *
 */
static VALUE zetta_fs_unmount(VALUE self)
{
  zfs_handle_t *zfs_handle;
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  return (zfs_unmount(zfs_handle, NULL, 0) == 0) ? Qtrue : Qfalse;
}

/*
 * call-seq:
 *   @zfs.destroy!  => Boolean
 *
 * Destroy the current dataset instance.
 *
 */
static VALUE zetta_fs_destroy(VALUE self)
{
  zfs_handle_t *zfs_handle;
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

// Boolean parameter was added to zfs_destroy:
#ifdef SPA_VERSION_18
  return (zfs_destroy(zfs_handle,B_FALSE) == 0) ? Qtrue : Qfalse;
#else
  return (zfs_destroy(zfs_handle) == 0) ? Qtrue : Qfalse;
#endif
}

static int zetta_fs_iter_f(zfs_handle_t *handle, void *klass)
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
static VALUE zetta_fs_iter_root(int argc, VALUE *argv, VALUE klass)
{
  VALUE libzfs_handle;
  libzfs_handle_t *libhandle;

  libzfs_handle = (argc == 0) ? zetta_lib_get_handle() : argv[0];

  if(CLASS_OF(libzfs_handle) != rb_const_get(rb_cObject, rb_intern("LibZfs"))) {
    rb_raise(rb_eTypeError, "ZFS Lib handle must be an instance of LibZfs.");
  }

  Data_Get_Struct(libzfs_handle, libzfs_handle_t, libhandle);

  zfs_iter_root(libhandle, zetta_fs_iter_f, (void *)klass);

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
static VALUE zetta_fs_iter_filesystems(VALUE self)
{
  zfs_handle_t *zfs_handle;
  VALUE klass = rb_class_of(self);
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  zfs_iter_filesystems(zfs_handle, zetta_fs_iter_f, (void *)klass);

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
static VALUE zetta_fs_iter_snapshots(VALUE self)
{
  zfs_handle_t *zfs_handle;
  VALUE klass = rb_class_of(self);
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);

  zfs_iter_snapshots(zfs_handle, zetta_fs_iter_f, (void *)klass);

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
static VALUE zetta_fs_iter_dependents(VALUE self)
{
  zfs_handle_t *zfs_handle;
  VALUE klass = rb_class_of(self);
  Data_Get_Struct(self, zfs_handle_t, zfs_handle);
  // TODO: Allow recursion should be configurable by user?
  zfs_iter_dependents(zfs_handle, B_TRUE, zetta_fs_iter_f, (void *)klass);

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
static VALUE zetta_lib_alloc(VALUE klass)
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
static VALUE zetta_lib_errno(VALUE self)
{
  libzfs_handle_t *handle;
  Data_Get_Struct(self, libzfs_handle_t, handle);
  return INT2NUM(libzfs_errno(handle));
}

/*
 * call-seq:
 *   libzfs_handle.print_on_error(true|false)  => Nil
 *
 * When set to true, libzfs will print zfs errors to $stderr.
 *
 */
static VALUE zetta_lib_print_on_error(VALUE self, VALUE b)
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
static VALUE zetta_lib_error_action(VALUE self)
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
static VALUE zetta_lib_error_description(VALUE self)
{
  libzfs_handle_t *handle;
  Data_Get_Struct(self, libzfs_handle_t, handle);

  return rb_str_new2(libzfs_error_description(handle));
}

/*
 * call-seq:
 *   libzfs_handle.raise_error  => Exception, ZfsError::Error instance
 *
 * Raises the libzfs associated error as a proper ruby Error instance.
 * See <code>ZfsError::Error</code> and subclasses for the defined error
 *  classes.
 *
 */
static VALUE zetta_lib_raise_error(VALUE self)
{
  VALUE error = Qnil;

  libzfs_handle_t *handle;
  Data_Get_Struct(self, libzfs_handle_t, handle);

  return zetta_lib_error_exception(handle);
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
static VALUE zetta_lib_handle(VALUE klass)
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

  /* Pool health status codes. */
  rb_define_const(mHealthStatus, "CORRUPT_CACHE", INT2NUM(0));
  rb_define_const(mHealthStatus, "MISSING_DEV_R", INT2NUM(1));
  rb_define_const(mHealthStatus, "MISSING_DEV_NR", INT2NUM(2));
  rb_define_const(mHealthStatus, "CORRUPT_LABEL_R", INT2NUM(3));
  rb_define_const(mHealthStatus, "CORRUPT_LABEL_NR", INT2NUM(4));
  rb_define_const(mHealthStatus, "BAD_GUID_SUM", INT2NUM(5));
  rb_define_const(mHealthStatus, "CORRUPT_POOL", INT2NUM(6));
  rb_define_const(mHealthStatus, "CORRUPT_DATA", INT2NUM(7));
  rb_define_const(mHealthStatus, "FAILING_DEV", INT2NUM(8));
  rb_define_const(mHealthStatus, "VERSION_NEWER", INT2NUM(9));
  rb_define_const(mHealthStatus, "HOSTID_MISMATCH", INT2NUM(10));
  rb_define_const(mHealthStatus, "IO_FAILURE_WAIT", INT2NUM(11));
  rb_define_const(mHealthStatus, "IO_FAILURE_CONTINUE", INT2NUM(12));
  rb_define_const(mHealthStatus, "BAD_LOG", INT2NUM(13));
  rb_define_const(mHealthStatus, "FAULTED_DEV_R", INT2NUM(14));
  rb_define_const(mHealthStatus, "FAULTED_DEV_NR", INT2NUM(15));
  rb_define_const(mHealthStatus, "VERSION_OLDER", INT2NUM(16));
  rb_define_const(mHealthStatus, "RESILVERING", INT2NUM(17));
  rb_define_const(mHealthStatus, "OFFLINE_DEV", INT2NUM(18));
  rb_define_const(mHealthStatus, "REMOVED_DEV", INT2NUM(19));
  rb_define_const(mHealthStatus, "OK", INT2NUM(20));
  rb_define_const(mHealthStatus, "UNKNOWN", INT2NUM(21));

  /* Pool state codes */
  rb_define_const(mPoolState, "ACTIVE", INT2NUM(0));
  rb_define_const(mPoolState, "EXPORTED", INT2NUM(1));
  rb_define_const(mPoolState, "DESTROYED", INT2NUM(2));
  rb_define_const(mPoolState, "SPARE", INT2NUM(3));
  rb_define_const(mPoolState, "UNINITIALIZED", INT2NUM(4));
  rb_define_const(mPoolState, "UNAVAIL", INT2NUM(5));
  rb_define_const(mPoolState, "POTENTIALLY_ACTIVE", INT2NUM(6));
  rb_define_const(mPoolState, "UNKNOWN", INT2NUM(7));
  rb_define_const(mPoolState, "L2CACHE", INT2NUM(8));
}

static void Init_libzfs_errors()
{
  VALUE mZfsError = rb_define_module("ZfsError");
  cZfsError = rb_define_class_under(mZfsError, "Error", rb_eStandardError);

  cZfsNoMemoryError = rb_define_class_under(mZfsError, "NoMemoryError", cZfsError);
  cZfsInvalidPropertyError = rb_define_class_under(mZfsError, "InvalidPropertyError", cZfsError);
  cZfsReadOnlyPropertyError = rb_define_class_under(mZfsError, "ReadOnlyPropertyError", cZfsError);
  cZfsInvalidPropertyTypeError = rb_define_class_under(mZfsError, "InvalidPropertyTypeError", cZfsError);
  cZfsNonInheritablePropertyError = rb_define_class_under(mZfsError, "NonInheritablePropertyError", cZfsError);
  cZfsPropertySpaceError = rb_define_class_under(mZfsError, "PropertySpaceError", cZfsError);
  cZfsInvalidDatasetTypeError = rb_define_class_under(mZfsError, "InvalidDatasetTypeError", cZfsError);
  cZfsDatasetBusyError = rb_define_class_under(mZfsError, "DatasetBusyError", cZfsError);
  cZfsDatasetExistsError = rb_define_class_under(mZfsError, "DatasetExistsError", cZfsError);
  cZfsNoentError = rb_define_class_under(mZfsError, "NoentError", cZfsError);
  cZfsBadStreamError = rb_define_class_under(mZfsError, "BadStreamError", cZfsError);
  cZfsDatasetReadOnlyError = rb_define_class_under(mZfsError, "DatasetReadOnlyError", cZfsError);
  cZfsVolumeTooBigError = rb_define_class_under(mZfsError, "VolumeTooBigError", cZfsError);
  cZfsInvalidNameError = rb_define_class_under(mZfsError, "InvalidNameError", cZfsError);
  cZfsBadRestoreError = rb_define_class_under(mZfsError, "BadRestoreError", cZfsError);
  cZfsBadBackupError = rb_define_class_under(mZfsError, "BadBackupError", cZfsError);
  cZfsBadTargetError = rb_define_class_under(mZfsError, "BadTargetError", cZfsError);
  cZfsNoDeviceError = rb_define_class_under(mZfsError, "NoDeviceError", cZfsError);
  cZfsBadDeviceError = rb_define_class_under(mZfsError, "BadDeviceError", cZfsError);
  cZfsNoReplicasError = rb_define_class_under(mZfsError, "NoReplicasError", cZfsError);
  cZfsResilveringError = rb_define_class_under(mZfsError, "ResilveringError", cZfsError);
  cZfsBadVersionError = rb_define_class_under(mZfsError, "BadVersionError", cZfsError);
  cZfsPoolUnavailableError = rb_define_class_under(mZfsError, "PoolUnavailableError", cZfsError);
  cZfsDeviceOverflowError = rb_define_class_under(mZfsError, "DeviceOverflowError", cZfsError);
  cZfsBadPathError = rb_define_class_under(mZfsError, "BadPathError", cZfsError);
  cZfsCrossTargetError = rb_define_class_under(mZfsError, "CrossTargetError", cZfsError);
  cZfsZonedError = rb_define_class_under(mZfsError, "ZonedError", cZfsError);
  cZfsMountFailedError = rb_define_class_under(mZfsError, "MountFailedError", cZfsError);
  cZfsUnmountFailedError = rb_define_class_under(mZfsError, "UnmountFailedError", cZfsError);
  cZfsUnshareNfsFailedError = rb_define_class_under(mZfsError, "UnshareNfsFailedError", cZfsError);
  cZfsShareNfsFailedError = rb_define_class_under(mZfsError, "ShareNfsFailedError", cZfsError);
  cZfsUnshareSmbFailedError = rb_define_class_under(mZfsError, "UnshareSmbFailedError", cZfsError);
  cZfsShareSmbFailedError = rb_define_class_under(mZfsError, "ShareSmbFailedError", cZfsError);
  cZfsPermError = rb_define_class_under(mZfsError, "PermError", cZfsError);
  cZfsNospcError = rb_define_class_under(mZfsError, "NospcError", cZfsError);
  cZfsFaultError = rb_define_class_under(mZfsError, "FaultError", cZfsError);
  cZfsIOError = rb_define_class_under(mZfsError, "IOError", cZfsError);
  cZfsINTRError = rb_define_class_under(mZfsError, "INTRError", cZfsError);
  cZfsIsSpareError = rb_define_class_under(mZfsError, "IsSpareError", cZfsError);
  cZfsInvalidConfigError = rb_define_class_under(mZfsError, "InvalidConfigError", cZfsError);
  cZfsRecursiveError = rb_define_class_under(mZfsError, "RecursiveError", cZfsError);
  cZfsNoHistoryError = rb_define_class_under(mZfsError, "NoHistoryError", cZfsError);
  cZfsPoolPropsError = rb_define_class_under(mZfsError, "PoolPropsError", cZfsError);
  cZfsPoolNotSupportedError = rb_define_class_under(mZfsError, "PoolNotSupportedError", cZfsError);
  cZfsPoolInvalidArgError = rb_define_class_under(mZfsError, "PoolInvalidArgError", cZfsError);
  cZfsNameTooLongError = rb_define_class_under(mZfsError, "NameTooLongError", cZfsError);
  cZfsOpenFailedError = rb_define_class_under(mZfsError, "OpenFailedError", cZfsError);
  cZfsNoCapError = rb_define_class_under(mZfsError, "NoCapError", cZfsError);
  cZfsLabelFailedError = rb_define_class_under(mZfsError, "LabelFailedError", cZfsError);
  cZfsBadWhoError = rb_define_class_under(mZfsError, "BadWhoError", cZfsError);
  cZfsBadPermError = rb_define_class_under(mZfsError, "BadPermError", cZfsError);
  cZfsBadPermSetError = rb_define_class_under(mZfsError, "BadPermSetError", cZfsError);
  cZfsNoDelegationError = rb_define_class_under(mZfsError, "NoDelegationError", cZfsError);
  cZfsPermReadOnlyError = rb_define_class_under(mZfsError, "PermReadOnlyError", cZfsError);
  cZfsBadCacheError = rb_define_class_under(mZfsError, "BadCacheError", cZfsError);
  cZfsIsL2CacheError = rb_define_class_under(mZfsError, "IsL2CacheError", cZfsError);
  cZfsVdevNotSupportedError = rb_define_class_under(mZfsError, "VdevNotSupportedError", cZfsError);
  cZfsNotSupportedError = rb_define_class_under(mZfsError, "NotSupportedError", cZfsError);
  cZfsActiveSpareError = rb_define_class_under(mZfsError, "ActiveSpareError", cZfsError);
  cZfsUnplayedLogsError = rb_define_class_under(mZfsError, "UnplayedLogsError", cZfsError);
  cZfsReftagReleError = rb_define_class_under(mZfsError, "ReftagReleError", cZfsError);
  cZfsReftagHoldError = rb_define_class_under(mZfsError, "ReftagHoldError", cZfsError);
  cZfsTagTooLongError = rb_define_class_under(mZfsError, "TagTooLongError", cZfsError);
  cZfsPipeFailedError = rb_define_class_under(mZfsError, "PipeFailedError", cZfsError);
  cZfsThreadCreateFailedError = rb_define_class_under(mZfsError, "ThreadCreateFailedError", cZfsError);
  cZfsPostSplitOnlineError = rb_define_class_under(mZfsError, "PostSplitOnlineError", cZfsError);
  // iSCSI:
  cZfsUnshareISCSIFailedError = rb_define_class_under(mZfsError, "UnshareISCSIFailedError", cZfsError);
  cZfsShareISCSIFailedError = rb_define_class_under(mZfsError, "ShareISCSIFailedError", cZfsError);

}

void Init_zetta()
{
  VALUE cLibZfs = rb_define_class("LibZfs", rb_cObject);
  VALUE cZpool = rb_define_class("Zpool", rb_cObject);
  VALUE cZFS = rb_define_class("ZFS", rb_cObject);

  Init_libzfs_consts();
  Init_libzfs_errors();

  rb_define_alloc_func(cLibZfs, zetta_lib_alloc);
  rb_define_class_variable(cLibZfs, "@@handle", Qnil);
  rb_define_singleton_method(cLibZfs, "handle", zetta_lib_handle, 0);
  rb_define_method(cLibZfs, "errno", zetta_lib_errno, 0);
  rb_define_method(cLibZfs, "print_on_error", zetta_lib_print_on_error, 1);
  rb_define_method(cLibZfs, "error_action", zetta_lib_error_action, 0);
  rb_define_method(cLibZfs, "error_description", zetta_lib_error_description, 0);
  rb_define_method(cLibZfs, "raise_error", zetta_lib_raise_error, 0);

  rb_define_singleton_method(cZpool, "new", zetta_pool_new, -1);
  rb_define_method(cZpool, "name", zetta_pool_get_name, 0);
  rb_define_method(cZpool, "get", zetta_pool_get_prop, 1);
  rb_define_method(cZpool, "set", zetta_pool_set_prop, 2);
  rb_define_method(cZpool, "guid", zetta_pool_get_guid, 0);
  rb_define_method(cZpool, "space_used", zetta_pool_get_space_used, 0);
  rb_define_method(cZpool, "space_total", zetta_pool_get_space_total, 0);
  rb_define_method(cZpool, "state", zetta_pool_get_state, 0);
  rb_define_method(cZpool, "health_status", zetta_pool_get_health_status, 0);
  rb_define_method(cZpool, "version", zetta_pool_get_version, 0);
  rb_define_method(cZpool, "libzfs_handle", zetta_pool_get_handle, 0);
  // rb_define_method(cZpool, "destroy!", zetta_pool_destroy, 0);

  rb_define_singleton_method(cZpool, "each", zetta_pool_iter, -1);

  rb_define_singleton_method(cZFS, "new", zetta_fs_new, -1);
  rb_define_method(cZFS, "libzfs_handle", zetta_fs_get_handle, 0);
  rb_define_method(cZFS, "name", zetta_fs_get_name, 0);
  rb_define_method(cZFS, "fs_type", zetta_fs_get_type, 0);
  rb_define_method(cZFS, "rename", zetta_fs_rename, 2);
  // Sharing:
  rb_define_method(cZFS, "is_shared?", zetta_fs_is_shared, 0);
  rb_define_method(cZFS, "share!", zetta_fs_share, 0);
  rb_define_method(cZFS, "unshare!", zetta_fs_unshare, 0);
  // NFS:
  rb_define_method(cZFS, "nfs_share_name", zetta_fs_nfs_share_name, 0);
  rb_define_method(cZFS, "is_shared_nfs?", zetta_fs_is_shared_nfs, 0);
  rb_define_method(cZFS, "share_nfs!", zetta_fs_share_nfs, 0);
  rb_define_method(cZFS, "unshare_nfs!", zetta_fs_unshare_nfs, 0);


  // SMB:
#ifdef SPA_VERSION_9
  rb_define_method(cZFS, "smb_share_name", zetta_fs_smb_share_name, 0);
  rb_define_method(cZFS, "is_shared_smb?", zetta_fs_is_shared_smb, 0);
  rb_define_method(cZFS, "share_smb!", zetta_fs_share_smb, 0);
  rb_define_method(cZFS, "unshare_smb!", zetta_fs_unshare_smb, 0);
#endif

  // iSCSI:
#ifndef SPA_VERSION_24
  rb_define_method(cZFS, "is_shared_iscsi?", zetta_fs_is_shared_iscsi, 0);
  rb_define_method(cZFS, "share_iscsi!", zetta_fs_share_iscsi, 0);
  rb_define_method(cZFS, "unshare_iscsi!", zetta_fs_unshare_iscsi, 0);
#endif

  // Exist, Create, Destroy:
  rb_define_singleton_method(cZFS, "create", zetta_fs_create, -1);
  // These exist? and exists? are alias.
  rb_define_singleton_method(cZFS, "exists?", zetta_fs_dataset_exists, -1);
  rb_define_singleton_method(cZFS, "exist?", zetta_fs_dataset_exists, -1);
  rb_define_method(cZFS, "destroy!", zetta_fs_destroy, 0);
  // Properties:
  rb_define_method(cZFS, "get", zetta_fs_get_prop, 1);
  rb_define_method(cZFS, "get_user_prop", zetta_fs_get_user_prop, 1);
  rb_define_method(cZFS, "set", zetta_fs_set_prop, 2);
  // ZFS Iteration:
  rb_define_singleton_method(cZFS, "each", zetta_fs_iter_root, -1);
  rb_define_method(cZFS, "each_filesystem", zetta_fs_iter_filesystems, 0);
  rb_define_method(cZFS, "each_snapshot", zetta_fs_iter_snapshots, 0);
  rb_define_method(cZFS, "each_dependent", zetta_fs_iter_dependents, 0);
  // Snapshots:
  rb_define_singleton_method(cZFS, "snapshot", zetta_fs_snapshot, -1);
  rb_define_method(cZFS, "rollback", zetta_fs_rollback, 2);
  // Clones:
  rb_define_method(cZFS, "clone!", zetta_fs_clone, 1);
  rb_define_method(cZFS, "promote", zetta_fs_promote, 0);
  // Mount/Unmount:
  rb_define_method(cZFS, "is_mounted?", zetta_fs_is_mounted, 0);
  rb_define_method(cZFS, "mount", zetta_fs_mount, 0);
  rb_define_method(cZFS, "unmount", zetta_fs_unmount, 0);
}
