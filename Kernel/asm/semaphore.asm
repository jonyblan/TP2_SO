GLOBAL acquireLock
GLOBAL releaseLock

acquireLock:
  mov al, 1
.retry:
  xchg [rdi], al
  test al, al
  jz .retry
  ret

releaseLock:
  mov byte [rdi], 0
  ret