// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  uring.cppm
 *! \brief
 *!
 */
module;
#include <liburing.h>

export module fabric.io.rings.requests;

import std;
import reflect;

export import fabric.tasks;

export namespace fabric::io {
  using file_decriptor = int;
}
export namespace fabric::io::request_types {
  using sockaddr  = ::sockaddr;
  using socklen_t = ::socklen_t;

  struct accept {
    file_decriptor sockfd;
    sockaddr*      addr;
    socklen_t*     addrlen;
    int            flags;
  };

  struct bind {
    file_decriptor sockfd;
    sockaddr*      addr;
    socklen_t      addrlen;
  };

  struct cancel {
    void* user_data;
    int   flags;
  };

  struct cancel64 {
    std::uint64_t user_data;
    int           flags;
  };

  struct cancel_fd {
    file_decriptor fd;
    int            flags;
  };

  struct close {
    file_decriptor fd;
  };

  struct cmd_sock {
    int            cmd_op;
    file_decriptor fd;
    int            level;
    int            optname;
    void*          optval;
    int            optlen;
  };

  struct cmd_discard {
    file_decriptor fd;
    std::uint64_t  offset;
    std::uint64_t  nbytes;
  };

  struct connect {
    file_decriptor  sockfd;
    const sockaddr* addr;
    socklen_t       addrlen;
  };

  struct fadvise {
    file_decriptor fd;
    std::uint64_t  offset;
    std::uint32_t  len;
    int            advice;
  };

  struct fadvise64 {
    file_decriptor fd;
    std::uint64_t  offset;
    std::uint64_t  len;
    int            advice;
  };

  struct fallocate {
    file_decriptor fd;
    int            mode;
    std::uint64_t  offset;
    std::uint64_t  len;
  };

  struct fgetxattr {
    file_decriptor fd;
    const char*    name;
    char*          value;
    unsigned int   len;
  };

  struct files_update {
    file_decriptor* fds;
    unsigned int    nr_fds;
    int             offset;
  };

  struct fixed_fd_install {
    file_decriptor fd;
    unsigned int   flags;
  };

  struct fsetxattr {
    file_decriptor fd;
    const char*    name;
    const char*    value;
    int            flags;
    unsigned int   len;
  };

  struct fsync {
    file_decriptor fd;
    unsigned int   flags;
  };

  struct ftruncate {
    file_decriptor fd;
    loff_t         len;
  };

  struct futex_wait {
    std::uint32_t* futex;
    std::uint64_t  val;
    std::uint64_t  mask;
    std::uint32_t  futex_flags;
    unsigned int   flags;
  };

  struct futex_waitv {
    ::futex_waitv* futexv;
    std::uint32_t  nr_futex;
    unsigned int   flags;
  };

  struct futex_wake {
    std::uint32_t* futex;
    std::uint64_t  val;
    std::uint64_t  mask;
    std::uint32_t  futex_flags;
    unsigned int   flags;
  };

  struct getxattr {
    const char*  name;
    char*        value;
    const char*  path;
    unsigned int len;
  };

  struct link {
    const char* oldpath;
    const char* newpath;
    int         flags;
  };

  struct linkat {
    file_decriptor olddirfd;
    const char*    oldpath;
    file_decriptor newdirfd;
    const char*    newpath;
    int            flags;
  };

  struct link_timeout {
    __kernel_timespec* ts;
    unsigned int       flags;
  };

  struct listen {
    file_decriptor sockfd;
    int            backlog;
  };

  struct madvise {
    void*         addr;
    std::uint32_t len;
    int           advice;
  };

  struct madvise64 {
    void*         addr;
    std::uint64_t len;
    int           advice;
  };

  struct mkdir {
    const char* path;
    mode_t      mode;
  };

  struct mkdirat {
    file_decriptor dirfd;
    const char*    path;
    mode_t         mode;
  };

  // struct msg_ring;
  //
  // struct msg_ring_cqe_flags;
  //
  // struct msg_ring_fd;
  //
  // struct msg_ring_fd_alloc;

  // struct multishot_accept {
  //   int        sockfd;
  //   sockaddr*  addr;
  //   socklen_t* addrlen;
  //   int        flags;
  // };

  struct nop {};

  struct open {
    std::filesystem::path path;
    int                   flags;
    mode_t                mode;
  };

  struct openat {
    file_decriptor        dfd;
    std::filesystem::path path;
    int                   flags;
    mode_t                mode;
  };

  struct openat2 {
    file_decriptor        dfd;
    std::filesystem::path path;
    ::open_how*           how;
  };

  struct poll_add {
    file_decriptor fd;
    unsigned int   poll_mask;
  };

  // struct poll_multishot {
  //   int fd;
  //   unsigned int poll_mask;
  // };

  struct poll_remove {
    std::uint64_t user_data;
  };

  struct poll_update {
    std::uint64_t old_user_data;
    std::uint64_t new_user_data;
    unsigned int  poll_mask;
    unsigned int  flags;
  };

  struct provide_buffers {
    void* addr;
    int   len;
    int   nr;
    int   bgid;
    int   bid;
  };

  struct read {
    file_decriptor fd;
    void*          buf;
    unsigned int   nbytes;
    std::uint64_t  offset;
  };

  struct read_fixed {
    file_decriptor fd;
    void*          buf;
    unsigned int   nbytes;
    std::uint64_t  offset;
    int            buf_index;
  };

  // struct read_multishot {
  //   int fd;
  //   void* buf;
  //   unsigned int nbytes;
  //   std::uint64_t offset;
  // };

  struct readv {
    file_decriptor fd;
    const ::iovec* iovecs;
    unsigned int   nr_vecs;
    std::uint64_t  offset;
  };

  struct readv2 {
    file_decriptor fd;
    const ::iovec* iovecs;
    unsigned int   nr_vecs;
    std::uint64_t  offset;
    int            flags;
  };

  struct recv {
    file_decriptor sockfd;
    void*          buf;
    std::size_t    len;
    int            flags;
  };

  // struct recvmsg;

  // struct recvmsg_multishot;

  // struct recv_multishot;

  struct remove_buffers {
    int nr;
    int bgid;
  };

  struct rename {
    std::filesystem::path oldpath;
    std::filesystem::path newpath;
  };

  struct renameat {
    file_decriptor        olddirfd;
    std::filesystem::path oldpath;
    file_decriptor        newdirfd;
    std::filesystem::path newpath;
    unsigned int          flags;
  };

  struct send {
    file_decriptor sockfd;
    const void*    buf;
    std::size_t    len;
    int            flags;
  };

  struct send_bundle {
    file_decriptor sockfd;
    std::size_t    len;
    int            flags;
  };

  // struct sendmsg;

  // struct sendmsg_zc;

  struct send_set_addr {
    const sockaddr* dest_addr;
    std::uint16_t   addr_len;
  };

  struct sendto {
    file_decriptor  sockfd;
    const void*     buf;
    std::size_t     len;
    int             flags;
    const sockaddr* addr;
    socklen_t       addrlen;
  };

  struct send_zc {
    file_decriptor sockfd;
    const void*    buf;
    std::size_t    len;
    int            flags;
    unsigned int   zc_flags;
  };

  // struct send_zc_fixed;

  struct setxattr {
    const char*  name;
    const char*  value;
    const char*  path;
    int          flags;
    unsigned int len;
  };

  struct shutdown {
    file_decriptor sockfd;
    int            how;
  };

  struct socket {
    int          domain;
    int          type;
    int          protocol;
    unsigned int flags;
  };

  struct splice {
    file_decriptor fd_in;
    std::int64_t   off_in;
    file_decriptor fd_out;
    std::int64_t   off_out;
    unsigned int   nbytes;
    unsigned int   splice_flags;
  };

  struct statx {
    file_decriptor  dirfd;
    const char*     path;
    int             flags;
    unsigned int    mask;
    struct ::statx* statxbuf;
  };

  struct symlink {
    const char* target;
    const char* linkpath;
  };

  struct symlinkat {
    const char*    target;
    file_decriptor newdirfd;
    const char*    linkpath;
  };

  struct sync_file_range {
    file_decriptor fd;
    unsigned int   len;
    std::uint64_t  offset;
    int            flags;
  };

  struct tee {
    file_decriptor fd_in;
    file_decriptor fd_out;
    unsigned int   nbytes;
    unsigned int   splice_flags;
  };

  struct timeout {
    __kernel_timespec* ts;
    unsigned int       count;
    unsigned int       flags;
  };

  struct timeout_remove {
    std::uint64_t user_data;
    unsigned int  flags;
  };

  struct timeout_update {
    __kernel_timespec* ts;
    std::uint64_t      user_data;
    unsigned int       flags;
  };

  struct unlink {
    const char* path;
    int         flags;
  };

  struct unlinkat {
    file_decriptor dirfd;
    const char*    path;
    int            flags;
  };

  struct waitid {
    idtype_t     idtype;
    id_t         id;
    siginfo_t*   infop;
    int          options;
    unsigned int flags;
  };

  struct write {
    file_decriptor fd;
    const void*    buf;
    unsigned int   nbytes;
    std::uint64_t  offset;
  };

  struct write_fixed {
    file_decriptor fd;
    const void*    buf;
    unsigned int   nbytes;
    std::uint64_t  offset;
    int            buf_index;
  };

  struct writev {
    file_decriptor fd;
    const iovec*   iovecs;
    unsigned int   nr_vecs;
    std::uint64_t  offset;
  };

  struct writev2 {
    file_decriptor fd;
    const iovec*   iovecs;
    unsigned int   nr_vecs;
    std::uint64_t  offset;
    int            flags;
  };

} // namespace fabric::io::request_types
