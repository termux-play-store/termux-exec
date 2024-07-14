/*
 * Copyright (C) 2018 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include <errno.h>
#include <spawn.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>

#ifndef TERMUX_BASE_DIR
#define TERMUX_BASE_DIR "/data/data/com.termux/files"
#endif

// From libc/malloc_debug/malloc_debug.cpp:
//   Use this because the sigprocmask* functions filter out the reserved bionic
//   signals including the signal this code blocks.
// From libc/private/ScopedSignalBlocker.h:
//   This code needs to really block all the signals, not just the user-visible
//   ones. We call __rt_sigprocmask(2) directly so we don't mask out our own
//   signals (https://issuetracker.google.com/153624226 was a pthread_exit(3)
//   crash because a request to dump the thread's stack came in as it was exiting).
static inline int __rt_sigprocmask(int how, const sigset64_t *new_set, sigset64_t *old_set, size_t sigset_size) {
  return syscall(__NR_rt_sigprocmask, how, new_set, old_set, sigset_size);
}

#define _SYSTEM_CLEANUP                                                                                                \
  __rt_sigprocmask(SIG_SETMASK, &sigchld_blocker_old_set, NULL, sizeof(sigset64_t));                                   \
  sigaction64(SIGINT, &sigint_ignorer_old_action, NULL);                                                               \
  sigaction64(SIGQUIT, &sigquit_ignorer_old_action, NULL)

__attribute__((visibility("default"))) int system(const char *command) {
  // "The system() function shall always return non-zero when command is NULL."
  // http://pubs.opengroup.org/onlinepubs/9699919799/functions/system.html
  if (command == NULL)
    return 1;

  sigset64_t sigchld_blocker_set = {};
  sigset64_t sigchld_blocker_old_set = {};
  sigaddset64(&sigchld_blocker_set, SIGCHLD);
  __rt_sigprocmask(SIG_BLOCK, &sigchld_blocker_set, &sigchld_blocker_old_set, sizeof(sigset64_t));

  struct sigaction64 sigint_ignorer_action = {.sa_flags = 0, .sa_handler = SIG_IGN};
  struct sigaction64 sigint_ignorer_old_action = {};
  sigaction64(SIGINT, &sigint_ignorer_action, &sigint_ignorer_old_action);

  struct sigaction64 sigquit_ignorer_action = {.sa_flags = 0, .sa_handler = SIG_IGN};
  struct sigaction64 sigquit_ignorer_old_action = {};
  sigaction64(SIGQUIT, &sigquit_ignorer_action, &sigquit_ignorer_old_action);

  sigset64_t default_mask = {};
  if (sigint_ignorer_old_action.sa_handler != SIG_IGN)
    sigaddset64(&default_mask, SIGINT);
  if (sigquit_ignorer_old_action.sa_handler != SIG_IGN)
    sigaddset64(&default_mask, SIGQUIT);
  const int flags = POSIX_SPAWN_SETSIGDEF | POSIX_SPAWN_SETSIGMASK;

  posix_spawnattr_t attributes;
  if ((errno = posix_spawnattr_init(&attributes))) {
    _SYSTEM_CLEANUP;
    return -1;
  }
  if ((errno = posix_spawnattr_setsigdefault64(&attributes, &default_mask))) {
    _SYSTEM_CLEANUP;
    return -1;
  }
  if ((errno = posix_spawnattr_setsigmask64(&attributes, &sigchld_blocker_old_set))) {
    _SYSTEM_CLEANUP;
    return -1;
  }
  if ((errno = posix_spawnattr_setflags(&attributes, flags))) {
    _SYSTEM_CLEANUP;
    return -1;
  }

  const char *argv[] = {"sh", "-c", "--", command, NULL};
  pid_t child;
  if ((errno = posix_spawn(&child, TERMUX_BASE_DIR "/usr/bin/sh", NULL, &attributes, (char **)(argv), environ)) != 0) {
    _SYSTEM_CLEANUP;
    return -1;
  }
  posix_spawnattr_destroy(&attributes);

  int status;
  pid_t pid = TEMP_FAILURE_RETRY(waitpid(child, &status, 0));
  _SYSTEM_CLEANUP;
  return (pid == -1 ? -1 : status);
}
