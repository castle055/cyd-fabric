// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  uring.cppm
 *! \brief
 *!
 */
module;
#include <liburing.h>

#define REQ_DETAIL_PARAMS(...)  __VA_OPT__(,) __VA_ARGS__);                                                      \
  }                                                                                                \
  }

#define REQ_DETAIL(NAME)                                                                           \
  template <>                                                                                      \
  struct ioring_req_detail<fabric::io::request_types::NAME> {                                      \
    static auto prepare(struct io_uring_sqe* ring, const fabric::io::request_types::NAME& req) {   \
io_uring_prep_## NAME(ring REQ_DETAIL_PARAMS

export module fabric.io.rings.uring.req_detail;

import std;
import reflect;

export import fabric.io.rings.requests;

export namespace platform {
  template <typename RequestType>
  struct ioring_req_detail;


  REQ_DETAIL(accept)(req.sockfd, req.addr, req.addrlen, req.flags);
  REQ_DETAIL(bind)(req.sockfd, req.addr, req.addrlen);
  REQ_DETAIL(cancel)(req.user_data, req.flags);
  REQ_DETAIL(cancel64)(req.user_data, req.flags);
  REQ_DETAIL(cancel_fd)(req.fd, req.flags);
  REQ_DETAIL(close)(req.fd);
  REQ_DETAIL(cmd_sock)(req.cmd_op, req.fd, req.level, req.optname, req.optval, req.optlen);
  REQ_DETAIL(cmd_discard)(req.fd, req.offset, req.nbytes);
  REQ_DETAIL(connect)(req.sockfd, req.addr, req.addrlen);
  REQ_DETAIL(fadvise)(req.fd, req.offset, req.len, req.advice);
  REQ_DETAIL(fadvise64)(req.fd, req.offset, req.len, req.advice);
  REQ_DETAIL(fallocate)(req.fd, req.mode, req.offset, req.len);
  REQ_DETAIL(fgetxattr)(req.fd, req.name, req.value, req.len);
  REQ_DETAIL(files_update)(req.fds, req.nr_fds, req.offset);
  REQ_DETAIL(fixed_fd_install)(req.fd, req.flags);
  REQ_DETAIL(fsetxattr)(req.fd, req.name, req.value, req.flags, req.len);
  REQ_DETAIL(fsync)(req.fd, req.flags);
  REQ_DETAIL(ftruncate)(req.fd, req.len);
  REQ_DETAIL(futex_wait)(req.futex, req.val, req.mask, req.futex_flags, req.flags);
  REQ_DETAIL(futex_waitv)(req.futexv, req.nr_futex, req.flags);
  REQ_DETAIL(futex_wake)(req.futex, req.val, req.mask, req.futex_flags, req.flags);
  REQ_DETAIL(getxattr)(req.name, req.value, req.path, req.len);
  REQ_DETAIL(link)(req.oldpath, req.newpath, req.flags);
  REQ_DETAIL(linkat)(req.olddirfd, req.oldpath, req.newdirfd, req.newpath, req.flags);
  REQ_DETAIL(link_timeout)(req.ts, req.flags);
  REQ_DETAIL(listen)(req.sockfd, req.backlog);
  REQ_DETAIL(madvise)(req.addr, req.len, req.advice);
  REQ_DETAIL(madvise64)(req.addr, req.len, req.advice);
  REQ_DETAIL(mkdir)(req.path, req.mode);
  REQ_DETAIL(mkdirat)(req.dirfd, req.path, req.mode);
  // REQ_DETAIL(multishot_accept)(req.sockfd, req.addr, req.addrlen, req.flags);
  REQ_DETAIL(nop)();
  REQ_DETAIL(open)(req.path.c_str(), req.flags, req.mode);
  REQ_DETAIL(openat)(req.dfd, req.path.c_str(), req.flags, req.mode);
  REQ_DETAIL(openat2)(req.dfd, req.path.c_str(), req.how);
  REQ_DETAIL(poll_add)(req.fd, req.poll_mask);
  // REQ_DETAIL(poll_multishot)(req.fd, req.poll_mask);
  REQ_DETAIL(poll_remove)(req.user_data);
  REQ_DETAIL(poll_update)(req.old_user_data, req.new_user_data, req.poll_mask, req.flags);
  REQ_DETAIL(provide_buffers)(req.addr, req.len, req.nr, req.bgid, req.bid);
  REQ_DETAIL(read)(req.fd, req.buf, req.nbytes, req.offset);
  REQ_DETAIL(read_fixed)(req.fd, req.buf, req.nbytes, req.offset, req.buf_index);
  REQ_DETAIL(readv)(req.fd, req.iovecs, req.nr_vecs, req.offset);
  REQ_DETAIL(readv2)(req.fd, req.iovecs, req.nr_vecs, req.offset, req.flags);
  REQ_DETAIL(recv)(req.sockfd, req.buf, req.len, req.flags);
  REQ_DETAIL(remove_buffers)(req.nr, req.bgid);
  REQ_DETAIL(rename)(req.oldpath.c_str(), req.newpath.c_str());
  REQ_DETAIL(renameat)(req.olddirfd, req.oldpath.c_str(), req.newdirfd, req.newpath.c_str(), req.flags);
  REQ_DETAIL(send)(req.sockfd, req.buf, req.len, req.flags);
  REQ_DETAIL(send_bundle)(req.sockfd, req.len, req.flags);
  REQ_DETAIL(send_set_addr)(req.dest_addr, req.addr_len);
  REQ_DETAIL(sendto)(req.sockfd, req.buf, req.len, req.flags, req.addr, req.addrlen);
  REQ_DETAIL(send_zc)(req.sockfd, req.buf, req.len, req.flags, req.zc_flags);
  REQ_DETAIL(setxattr)(req.name, req.value, req.path, req.flags, req.len);
  REQ_DETAIL(shutdown)(req.sockfd, req.how);
  REQ_DETAIL(socket)(req.domain, req.type, req.protocol, req.flags);
  REQ_DETAIL(splice)(req.fd_in, req.off_in, req.fd_out, req.off_out, req.nbytes, req.splice_flags);
  REQ_DETAIL(statx)(req.dirfd, req.path, req.flags, req.mask, req.statxbuf);
  REQ_DETAIL(symlink)(req.target, req.linkpath);
  REQ_DETAIL(symlinkat)(req.target, req.newdirfd, req.linkpath);
  REQ_DETAIL(sync_file_range)(req.fd, req.len, req.offset, req.flags);
  REQ_DETAIL(tee)(req.fd_in, req.fd_out, req.nbytes, req.splice_flags);
  REQ_DETAIL(timeout)(req.ts, req.count, req.flags);
  REQ_DETAIL(timeout_remove)(req.user_data, req.flags);
  REQ_DETAIL(timeout_update)(req.ts, req.user_data, req.flags);
  REQ_DETAIL(unlink)(req.path, req.flags);
  REQ_DETAIL(unlinkat)(req.dirfd, req.path, req.flags);
  REQ_DETAIL(waitid)(req.idtype, req.id, req.infop, req.options, req.flags);
  REQ_DETAIL(write)(req.fd, req.buf, req.nbytes, req.offset);
  REQ_DETAIL(write_fixed)(req.fd, req.buf, req.nbytes, req.offset, req.buf_index);
  REQ_DETAIL(writev)(req.fd, req.iovecs, req.nr_vecs, req.offset);
  REQ_DETAIL(writev2)(req.fd, req.iovecs, req.nr_vecs, req.offset, req.flags);

} // namespace platform
