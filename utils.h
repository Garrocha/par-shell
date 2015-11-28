#ifndef _UTILS_H_
#define _UTILS_H_

void Unlink(const char *path);
void Mkfifo(const char *pathname, mode_t mode);
int Open(const char *pathname, int flags, mode_t mode);
void Close(int fd);
int Dup(int oldfd);
void Kill(pid_t pid, int sig);
void Write(int fdPipe, char *msg);
void Read(int fdPipe);

#endif
