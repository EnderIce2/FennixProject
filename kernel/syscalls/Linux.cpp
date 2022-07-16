#include "syscalls.hpp"

#include <internal_task.h>

#include "../../libc/include/syscalls.h"

static uint64_t internal_unimpl(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g)
{
    fixme("Unimplemented Syscall: %d %d %d %d %d %d %d", a, b, c, d, e, f, g);
    return -1;
}

static uint64_t sys_read(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_write(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_open(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_close(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_stat(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_fstat(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_lstat(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_poll(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_lseek(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_mmap(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_mprotect(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_munmap(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_brk(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_rt_sigaction(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_rt_sigprocmask(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_rt_sigreturn(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_ioctl(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_pread64(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_pwrite64(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_readv(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_writev(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_access(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_pipe(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_select(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_sched_yield(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_mremap(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_msync(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_mincore(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_madvise(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_shmget(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_shmat(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_shmctl(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_dup(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_dup2(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_pause(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_nanosleep(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_getitimer(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_alarm(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_setitimer(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_getpid(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_sendfile(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_socket(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_connect(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_accept(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_sendto(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_recvfrom(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_sendmsg(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_recvmsg(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_shutdown(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_bind(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_listen(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_getsockname(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_getpeername(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_socketpair(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_setsockopt(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_getsockopt(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_clone(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_fork(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_vfork(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_execve(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_exit(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_wait4(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_kill(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_uname(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_semget(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_semop(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_semctl(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_shmdt(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_msgget(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_msgsnd(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_msgrcv(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_msgctl(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_fcntl(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_flock(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_fsync(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_fdatasync(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_truncate(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_ftruncate(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_getdents(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_getcwd(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_chdir(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_fchdir(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_rename(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_mkdir(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_rmdir(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_creat(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_link(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_unlink(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_symlink(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_readlink(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_chmod(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_fchmod(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_chown(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_fchown(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_lchown(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_umask(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_gettimeofday(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_getrlimit(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_getrusage(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_sysinfo(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_times(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_ptrace(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_getuid(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_syslog(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_getgid(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_setuid(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_setgid(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_geteuid(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_getegid(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_setpgid(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_getppid(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_getpgrp(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_setsid(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_setreuid(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_setregid(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_getgroups(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_setgroups(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_setresuid(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_getresuid(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_setresgid(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_getresgid(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_getpgid(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_setfsuid(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_setfsgid(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_getsid(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_capget(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_capset(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_rt_sigpending(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_rt_sigtimedwait(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_rt_sigqueueinfo(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_rt_sigsuspend(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_sigaltstack(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_utime(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_mknod(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_uselib(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_personality(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_ustat(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_statfs(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_fstatfs(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_sysfs(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_getpriority(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_setpriority(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_sched_setparam(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_sched_getparam(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_sched_setscheduler(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_sched_getscheduler(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_sched_get_priority_max(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_sched_get_priority_min(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_sched_rr_get_interval(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_mlock(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_munlock(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_mlockall(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_munlockall(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_vhangup(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_modify_ldt(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_pivot_root(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys__sysctl(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_prctl(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_arch_prctl(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_adjtimex(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_setrlimit(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_chroot(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_sync(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_acct(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_settimeofday(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_mount(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_umount2(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_swapon(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_swapoff(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_reboot(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_sethostname(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_setdomainname(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_iopl(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_ioperm(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_create_module(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_init_module(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_delete_module(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_get_kernel_syms(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_query_module(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_quotactl(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_nfsservctl(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_getpmsg(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_putpmsg(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_afs_syscall(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_tuxcall(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_security(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_gettid(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_readahead(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_setxattr(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_lsetxattr(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_fsetxattr(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_getxattr(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_lgetxattr(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_fgetxattr(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_listxattr(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_llistxattr(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_flistxattr(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_removexattr(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_lremovexattr(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_fremovexattr(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_tkill(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_time(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_futex(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_sched_setaffinity(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_sched_getaffinity(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_set_thread_area(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_io_setup(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_io_destroy(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_io_getevents(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_io_submit(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_io_cancel(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_get_thread_area(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_lookup_dcookie(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_epoll_create(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_epoll_ctl_old(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_epoll_wait_old(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_remap_file_pages(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_getdents64(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_set_tid_address(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_restart_syscall(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_semtimedop(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_fadvise64(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_timer_create(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_timer_settime(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_timer_gettime(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_timer_getoverrun(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_timer_delete(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_clock_settime(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_clock_gettime(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_clock_getres(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_clock_nanosleep(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_exit_group(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_epoll_wait(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_epoll_ctl(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_tgkill(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_utimes(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_vserver(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_mbind(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_set_mempolicy(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_get_mempolicy(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_mq_open(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_mq_unlink(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_mq_timedsend(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_mq_timedreceive(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_mq_notify(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_mq_getsetattr(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_kexec_load(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_waitid(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_add_key(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_request_key(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_keyctl(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_ioprio_set(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_ioprio_get(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_inotify_init(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_inotify_add_watch(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_inotify_rm_watch(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_migrate_pages(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_openat(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_mkdirat(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_mknodat(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_fchownat(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_futimesat(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_newfstatat(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_unlinkat(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_renameat(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_linkat(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_symlinkat(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_readlinkat(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_fchmodat(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_faccessat(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_pselect6(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_ppoll(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_unshare(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_set_robust_list(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_get_robust_list(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_splice(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_tee(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_sync_file_range(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_vmsplice(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_move_pages(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_utimensat(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_epoll_pwait(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_signalfd(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_timerfd_create(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_eventfd(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_fallocate(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_timerfd_settime(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_timerfd_gettime(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_accept4(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_signalfd4(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_eventfd2(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_epoll_create1(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_dup3(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_pipe2(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_inotify_init1(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_preadv(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_pwritev(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_rt_tgsigqueueinfo(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_perf_event_open(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_recvmmsg(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_fanotify_init(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_fanotify_mark(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_prlimit64(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_name_to_handle_at(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_open_by_handle_at(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_clock_adjtime(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_syncfs(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_sendmmsg(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_setns(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_getcpu(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_process_vm_readv(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_process_vm_writev(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_kcmp(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_finit_module(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_sched_setattr(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_sched_getattr(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_renameat2(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_seccomp(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_getrandom(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_memfd_create(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_kexec_file_load(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t sys_bpf(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t stub_execveat(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t userfaultfd(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t membarrier(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t mlock2(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t copy_file_range(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t preadv2(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t pwritev2(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t pkey_mprotect(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t pkey_alloc(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t pkey_free(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t statx(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t io_pgetevents(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t rseq(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }
static uint64_t pkey_mprotect_(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f, uint64_t g) { return internal_unimpl(a, b, c, d, e, f, g); }

// Syscalls list: https://filippo.io/linux-syscall-table/ https://blog.rchapman.org/posts/Linux_System_Call_Table_for_x86_64/

static void *LinuxSyscallsTable[] = {
    [0] = (void *)sys_read,
    [1] = (void *)sys_write,
    [2] = (void *)sys_open,
    [3] = (void *)sys_close,
    [4] = (void *)sys_stat,
    [5] = (void *)sys_fstat,
    [6] = (void *)sys_lstat,
    [7] = (void *)sys_poll,
    [8] = (void *)sys_lseek,
    [9] = (void *)sys_mmap,
    [10] = (void *)sys_mprotect,
    [11] = (void *)sys_munmap,
    [12] = (void *)sys_brk,
    [13] = (void *)sys_rt_sigaction,
    [14] = (void *)sys_rt_sigprocmask,
    [15] = (void *)sys_rt_sigreturn,
    [16] = (void *)sys_ioctl,
    [17] = (void *)sys_pread64,
    [18] = (void *)sys_pwrite64,
    [19] = (void *)sys_readv,
    [20] = (void *)sys_writev,
    [21] = (void *)sys_access,
    [22] = (void *)sys_pipe,
    [23] = (void *)sys_select,
    [24] = (void *)sys_sched_yield,
    [25] = (void *)sys_mremap,
    [26] = (void *)sys_msync,
    [27] = (void *)sys_mincore,
    [28] = (void *)sys_madvise,
    [29] = (void *)sys_shmget,
    [30] = (void *)sys_shmat,
    [31] = (void *)sys_shmctl,
    [32] = (void *)sys_dup,
    [33] = (void *)sys_dup2,
    [34] = (void *)sys_pause,
    [35] = (void *)sys_nanosleep,
    [36] = (void *)sys_getitimer,
    [37] = (void *)sys_alarm,
    [38] = (void *)sys_setitimer,
    [39] = (void *)sys_getpid,
    [40] = (void *)sys_sendfile,
    [41] = (void *)sys_socket,
    [42] = (void *)sys_connect,
    [43] = (void *)sys_accept,
    [44] = (void *)sys_sendto,
    [45] = (void *)sys_recvfrom,
    [46] = (void *)sys_sendmsg,
    [47] = (void *)sys_recvmsg,
    [48] = (void *)sys_shutdown,
    [49] = (void *)sys_bind,
    [50] = (void *)sys_listen,
    [51] = (void *)sys_getsockname,
    [52] = (void *)sys_getpeername,
    [53] = (void *)sys_socketpair,
    [54] = (void *)sys_setsockopt,
    [55] = (void *)sys_getsockopt,
    [56] = (void *)sys_clone,
    [57] = (void *)sys_fork,
    [58] = (void *)sys_vfork,
    [59] = (void *)sys_execve,
    [60] = (void *)sys_exit,
    [61] = (void *)sys_wait4,
    [62] = (void *)sys_kill,
    [63] = (void *)sys_uname,
    [64] = (void *)sys_semget,
    [65] = (void *)sys_semop,
    [66] = (void *)sys_semctl,
    [67] = (void *)sys_shmdt,
    [68] = (void *)sys_msgget,
    [69] = (void *)sys_msgsnd,
    [70] = (void *)sys_msgrcv,
    [71] = (void *)sys_msgctl,
    [72] = (void *)sys_fcntl,
    [73] = (void *)sys_flock,
    [74] = (void *)sys_fsync,
    [75] = (void *)sys_fdatasync,
    [76] = (void *)sys_truncate,
    [77] = (void *)sys_ftruncate,
    [78] = (void *)sys_getdents,
    [79] = (void *)sys_getcwd,
    [80] = (void *)sys_chdir,
    [81] = (void *)sys_fchdir,
    [82] = (void *)sys_rename,
    [83] = (void *)sys_mkdir,
    [84] = (void *)sys_rmdir,
    [85] = (void *)sys_creat,
    [86] = (void *)sys_link,
    [87] = (void *)sys_unlink,
    [88] = (void *)sys_symlink,
    [89] = (void *)sys_readlink,
    [90] = (void *)sys_chmod,
    [91] = (void *)sys_fchmod,
    [92] = (void *)sys_chown,
    [93] = (void *)sys_fchown,
    [94] = (void *)sys_lchown,
    [95] = (void *)sys_umask,
    [96] = (void *)sys_gettimeofday,
    [97] = (void *)sys_getrlimit,
    [98] = (void *)sys_getrusage,
    [99] = (void *)sys_sysinfo,
    [100] = (void *)sys_times,
    [101] = (void *)sys_ptrace,
    [102] = (void *)sys_getuid,
    [103] = (void *)sys_syslog,
    [104] = (void *)sys_getgid,
    [105] = (void *)sys_setuid,
    [106] = (void *)sys_setgid,
    [107] = (void *)sys_geteuid,
    [108] = (void *)sys_getegid,
    [109] = (void *)sys_setpgid,
    [110] = (void *)sys_getppid,
    [111] = (void *)sys_getpgrp,
    [112] = (void *)sys_setsid,
    [113] = (void *)sys_setreuid,
    [114] = (void *)sys_setregid,
    [115] = (void *)sys_getgroups,
    [116] = (void *)sys_setgroups,
    [117] = (void *)sys_setresuid,
    [118] = (void *)sys_getresuid,
    [119] = (void *)sys_setresgid,
    [120] = (void *)sys_getresgid,
    [121] = (void *)sys_getpgid,
    [122] = (void *)sys_setfsuid,
    [123] = (void *)sys_setfsgid,
    [124] = (void *)sys_getsid,
    [125] = (void *)sys_capget,
    [126] = (void *)sys_capset,
    [127] = (void *)sys_rt_sigpending,
    [128] = (void *)sys_rt_sigtimedwait,
    [129] = (void *)sys_rt_sigqueueinfo,
    [130] = (void *)sys_rt_sigsuspend,
    [131] = (void *)sys_sigaltstack,
    [132] = (void *)sys_utime,
    [133] = (void *)sys_mknod,
    [134] = (void *)sys_uselib,
    [135] = (void *)sys_personality,
    [136] = (void *)sys_ustat,
    [137] = (void *)sys_statfs,
    [138] = (void *)sys_fstatfs,
    [139] = (void *)sys_sysfs,
    [140] = (void *)sys_getpriority,
    [141] = (void *)sys_setpriority,
    [142] = (void *)sys_sched_setparam,
    [143] = (void *)sys_sched_getparam,
    [144] = (void *)sys_sched_setscheduler,
    [145] = (void *)sys_sched_getscheduler,
    [146] = (void *)sys_sched_get_priority_max,
    [147] = (void *)sys_sched_get_priority_min,
    [148] = (void *)sys_sched_rr_get_interval,
    [149] = (void *)sys_mlock,
    [150] = (void *)sys_munlock,
    [151] = (void *)sys_mlockall,
    [152] = (void *)sys_munlockall,
    [153] = (void *)sys_vhangup,
    [154] = (void *)sys_modify_ldt,
    [155] = (void *)sys_pivot_root,
    [156] = (void *)sys__sysctl,
    [157] = (void *)sys_prctl,
    [158] = (void *)sys_arch_prctl,
    [159] = (void *)sys_adjtimex,
    [160] = (void *)sys_setrlimit,
    [161] = (void *)sys_chroot,
    [162] = (void *)sys_sync,
    [163] = (void *)sys_acct,
    [164] = (void *)sys_settimeofday,
    [165] = (void *)sys_mount,
    [166] = (void *)sys_umount2,
    [167] = (void *)sys_swapon,
    [168] = (void *)sys_swapoff,
    [169] = (void *)sys_reboot,
    [170] = (void *)sys_sethostname,
    [171] = (void *)sys_setdomainname,
    [172] = (void *)sys_iopl,
    [173] = (void *)sys_ioperm,
    [174] = (void *)sys_create_module,
    [175] = (void *)sys_init_module,
    [176] = (void *)sys_delete_module,
    [177] = (void *)sys_get_kernel_syms,
    [178] = (void *)sys_query_module,
    [179] = (void *)sys_quotactl,
    [180] = (void *)sys_nfsservctl,
    [181] = (void *)sys_getpmsg,
    [182] = (void *)sys_putpmsg,
    [183] = (void *)sys_afs_syscall,
    [184] = (void *)sys_tuxcall,
    [185] = (void *)sys_security,
    [186] = (void *)sys_gettid,
    [187] = (void *)sys_readahead,
    [188] = (void *)sys_setxattr,
    [189] = (void *)sys_lsetxattr,
    [190] = (void *)sys_fsetxattr,
    [191] = (void *)sys_getxattr,
    [192] = (void *)sys_lgetxattr,
    [193] = (void *)sys_fgetxattr,
    [194] = (void *)sys_listxattr,
    [195] = (void *)sys_llistxattr,
    [196] = (void *)sys_flistxattr,
    [197] = (void *)sys_removexattr,
    [198] = (void *)sys_lremovexattr,
    [199] = (void *)sys_fremovexattr,
    [200] = (void *)sys_tkill,
    [201] = (void *)sys_time,
    [202] = (void *)sys_futex,
    [203] = (void *)sys_sched_setaffinity,
    [204] = (void *)sys_sched_getaffinity,
    [205] = (void *)sys_set_thread_area,
    [206] = (void *)sys_io_setup,
    [207] = (void *)sys_io_destroy,
    [208] = (void *)sys_io_getevents,
    [209] = (void *)sys_io_submit,
    [210] = (void *)sys_io_cancel,
    [211] = (void *)sys_get_thread_area,
    [212] = (void *)sys_lookup_dcookie,
    [213] = (void *)sys_epoll_create,
    [214] = (void *)sys_epoll_ctl_old,
    [215] = (void *)sys_epoll_wait_old,
    [216] = (void *)sys_remap_file_pages,
    [217] = (void *)sys_getdents64,
    [218] = (void *)sys_set_tid_address,
    [219] = (void *)sys_restart_syscall,
    [220] = (void *)sys_semtimedop,
    [221] = (void *)sys_fadvise64,
    [222] = (void *)sys_timer_create,
    [223] = (void *)sys_timer_settime,
    [224] = (void *)sys_timer_gettime,
    [225] = (void *)sys_timer_getoverrun,
    [226] = (void *)sys_timer_delete,
    [227] = (void *)sys_clock_settime,
    [228] = (void *)sys_clock_gettime,
    [229] = (void *)sys_clock_getres,
    [230] = (void *)sys_clock_nanosleep,
    [231] = (void *)sys_exit_group,
    [232] = (void *)sys_epoll_wait,
    [233] = (void *)sys_epoll_ctl,
    [234] = (void *)sys_tgkill,
    [235] = (void *)sys_utimes,
    [236] = (void *)sys_vserver,
    [237] = (void *)sys_mbind,
    [238] = (void *)sys_set_mempolicy,
    [239] = (void *)sys_get_mempolicy,
    [240] = (void *)sys_mq_open,
    [241] = (void *)sys_mq_unlink,
    [242] = (void *)sys_mq_timedsend,
    [243] = (void *)sys_mq_timedreceive,
    [244] = (void *)sys_mq_notify,
    [245] = (void *)sys_mq_getsetattr,
    [246] = (void *)sys_kexec_load,
    [247] = (void *)sys_waitid,
    [248] = (void *)sys_add_key,
    [249] = (void *)sys_request_key,
    [250] = (void *)sys_keyctl,
    [251] = (void *)sys_ioprio_set,
    [252] = (void *)sys_ioprio_get,
    [253] = (void *)sys_inotify_init,
    [254] = (void *)sys_inotify_add_watch,
    [255] = (void *)sys_inotify_rm_watch,
    [256] = (void *)sys_migrate_pages,
    [257] = (void *)sys_openat,
    [258] = (void *)sys_mkdirat,
    [259] = (void *)sys_mknodat,
    [260] = (void *)sys_fchownat,
    [261] = (void *)sys_futimesat,
    [262] = (void *)sys_newfstatat,
    [263] = (void *)sys_unlinkat,
    [264] = (void *)sys_renameat,
    [265] = (void *)sys_linkat,
    [266] = (void *)sys_symlinkat,
    [267] = (void *)sys_readlinkat,
    [268] = (void *)sys_fchmodat,
    [269] = (void *)sys_faccessat,
    [270] = (void *)sys_pselect6,
    [271] = (void *)sys_ppoll,
    [272] = (void *)sys_unshare,
    [273] = (void *)sys_set_robust_list,
    [274] = (void *)sys_get_robust_list,
    [275] = (void *)sys_splice,
    [276] = (void *)sys_tee,
    [277] = (void *)sys_sync_file_range,
    [278] = (void *)sys_vmsplice,
    [279] = (void *)sys_move_pages,
    [280] = (void *)sys_utimensat,
    [281] = (void *)sys_epoll_pwait,
    [282] = (void *)sys_signalfd,
    [283] = (void *)sys_timerfd_create,
    [284] = (void *)sys_eventfd,
    [285] = (void *)sys_fallocate,
    [286] = (void *)sys_timerfd_settime,
    [287] = (void *)sys_timerfd_gettime,
    [288] = (void *)sys_accept4,
    [289] = (void *)sys_signalfd4,
    [290] = (void *)sys_eventfd2,
    [291] = (void *)sys_epoll_create1,
    [292] = (void *)sys_dup3,
    [293] = (void *)sys_pipe2,
    [294] = (void *)sys_inotify_init1,
    [295] = (void *)sys_preadv,
    [296] = (void *)sys_pwritev,
    [297] = (void *)sys_rt_tgsigqueueinfo,
    [298] = (void *)sys_perf_event_open,
    [299] = (void *)sys_recvmmsg,
    [300] = (void *)sys_fanotify_init,
    [301] = (void *)sys_fanotify_mark,
    [302] = (void *)sys_prlimit64,
    [303] = (void *)sys_name_to_handle_at,
    [304] = (void *)sys_open_by_handle_at,
    [305] = (void *)sys_clock_adjtime,
    [306] = (void *)sys_syncfs,
    [307] = (void *)sys_sendmmsg,
    [308] = (void *)sys_setns,
    [309] = (void *)sys_getcpu,
    [310] = (void *)sys_process_vm_readv,
    [311] = (void *)sys_process_vm_writev,
    [312] = (void *)sys_kcmp,
    [313] = (void *)sys_finit_module,
    [314] = (void *)sys_sched_setattr,
    [315] = (void *)sys_sched_getattr,
    [316] = (void *)sys_renameat2,
    [317] = (void *)sys_seccomp,
    [318] = (void *)sys_getrandom,
    [319] = (void *)sys_memfd_create,
    [320] = (void *)sys_kexec_file_load,
    [321] = (void *)sys_bpf,
    [322] = (void *)stub_execveat,
    [323] = (void *)userfaultfd,
    [324] = (void *)membarrier,
    [325] = (void *)mlock2,
    [326] = (void *)copy_file_range,
    [327] = (void *)preadv2,
    [328] = (void *)pwritev2,
    [329] = (void *)pkey_mprotect,
    [330] = (void *)pkey_alloc,
    [331] = (void *)pkey_free,
    [332] = (void *)statx,
    [333] = (void *)io_pgetevents,
    [334] = (void *)rseq,
    [335] = (void *)pkey_mprotect_,
};

uint64_t HandleLinuxSyscalls(SyscallsRegs *regs)
{
    if (RAX > sizeof(LinuxSyscallsTable))
    {
        return internal_unimpl(regs->rax, regs->rbx, regs->rcx, regs->rdx, regs->rsi, regs->rdi, regs->rbp);
    }

    uint64_t (*call)(uint64_t, ...) = reinterpret_cast<uint64_t (*)(uint64_t, ...)>(LinuxSyscallsTable[RAX]);
    if (!call)
    {
        err("Syscall %#llx failed.", RAX);
        return -1;
    }
    uint64_t ret = call(regs->int_num, 0, 0, 0, 0);
    RAX = ret;
    return ret;
}
