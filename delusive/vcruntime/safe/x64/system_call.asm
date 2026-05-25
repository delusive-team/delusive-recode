.code

system_call_stub proc
  mov    eax, dword ptr gs:[ 0h ]
  xor    r10, r10
  mov    qword ptr gs:[ 0h ], r10
  mov    r10, rcx

  jmp qword ptr gs:[ 150h ]
system_call_stub endp

end