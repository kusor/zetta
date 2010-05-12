require 'mkmf'

pkg_name = File.basename(File.dirname(File.expand_path(__FILE__)))

dir_config(pkg_name)

if PLATFORM =~ /freebsd/
  $CFLAGS+= " -I/usr/src/cddl/contrib/opensolaris/lib/libzfs/common"
  $CFLAGS+= " -I/usr/src/cddl/contrib/opensolaris/lib/libzpool/common"
  $CFLAGS+= " -I/usr/src/cddl/compat/opensolaris/include"
  $CFLAGS+= " -I/usr/src/cddl/compat/opensolaris/lib/libumem"
  $CFLAGS+= " -I/usr/src/sys/cddl/compat/opensolaris"
  $CFLAGS+= " -I/usr/src/cddl/contrib/opensolaris/head"
  $CFLAGS+= " -I/usr/src/cddl/contrib/opensolaris/lib/libuutil/common"
  $CFLAGS+= " -I/usr/src/cddl/contrib/opensolaris/lib/libzfs/common"
  $CFLAGS+= " -I/usr/src/cddl/contrib/opensolaris/lib/libumem/common"
  $CFLAGS+= " -I/usr/src/cddl/contrib/opensolaris/lib/libnvpair"
  $CFLAGS+= " -I/usr/src/sys/cddl/contrib/opensolaris/uts/common"
  $CFLAGS+= " -I/usr/src/sys/cddl/contrib/opensolaris/uts/common/fs/zfs"
  $CFLAGS+= " -I/usr/src/sys/cddl/contrib/opensolaris/uts/common/sys"
  $CFLAGS+= " -DNEED_SOLARIS_BOOLEAN"

  $LDFLAGS+= " -lavl -lgeom -lbsdxml -lsbuf -lnvpair -luutil -lutil"
end

have_library("c", "main")

# Check for prerequisite ZFS header and library.
have_library('zfs', 'zpool_create') || failed_prereqs = true
have_header('libzfs.h') || failed_prereqs = true

create_makefile(pkg_name) unless failed_prereqs
