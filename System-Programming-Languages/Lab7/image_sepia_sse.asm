global image_sepia_sse

section .rodata

align 16
c1_bgrb: dd 0.131, 0.168, 0.189, 0.131
align 16
c2_bgrb: dd 0.543, 0.686, 0.769, 0.543
align 16
c3_bgrb: dd 0.272, 0.349, 0.393, 0.272

align 16
c1_grbg: dd 0.168, 0.189, 0.131, 0.168
align 16
c2_grbg: dd 0.686, 0.769, 0.543, 0.686
align 16
c3_grbg: dd 0.349, 0.393, 0.272, 0.349

align 16
c1_rbgr: dd 0.189, 0.131, 0.168, 0.189
align 16
c2_rbgr: dd 0.769, 0.543, 0.686, 0.769
align 16
c3_rbgr: dd 0.393, 0.272, 0.349, 0.393

section .text

; rdi = pointer to the pixel array
; rsi = number of pixels (must be divisible by 4)

; Four channels are processed in parallel,
; first R_{n+0} G_{n+0} B_{n+0} R_{n+1}
; then  G_{n+1} B_{n+1} R_{n+2] G_{n+2}
; then  B_{n+2} R_{n+3} G_{n+3} B_{n+3}

%define xmm_ch1 xmm0
%define xmm_ch2 xmm1
%define xmm_ch3 xmm2

%define xmm_c1_bgrb xmm3
%define xmm_c2_bgrb xmm4
%define xmm_c3_bgrb xmm5

%define xmm_c1_grbg xmm6
%define xmm_c2_grbg xmm7
%define xmm_c3_grbg xmm8

%define xmm_c1_rbgr xmm9
%define xmm_c2_rbgr xmm10
%define xmm_c3_rbgr xmm11

%define xmm_bgrb xmm12
%define xmm_grbg xmm13
%define xmm_rbgr xmm14

%define pixel_ptr rcx

image_sepia_sse:
  movaps xmm_c1_bgrb, [c1_bgrb]
  movaps xmm_c2_bgrb, [c2_bgrb]
  movaps xmm_c3_bgrb, [c3_bgrb]
 
  movaps xmm_c1_grbg, [c1_grbg]
  movaps xmm_c2_grbg, [c2_grbg]
  movaps xmm_c3_grbg, [c3_grbg]
 
  movaps xmm_c1_rbgr, [c1_rbgr]
  movaps xmm_c2_rbgr, [c2_rbgr]
  movaps xmm_c3_rbgr, [c3_rbgr]

  mov pixel_ptr, rdi
  lea rsi, [rsi + 2*rsi] ; rsi (number of bytes to process) = number of pixels * 3 (sizeof(pixel) = 3)
  add rsi, rdi           ; rsi = address of the first byte after the last pixel

image_sepia_sse_loop_4_pixels:
  ; === xmm_bgrb

  mov rdx, [pixel_ptr]      ; (uint8[8]) b0 g0 r0 b1 g1 r1 b2 g2

  movd xmm_ch1, edx         ; channel 1 (blue)
  pmovzxbd xmm_ch1, xmm_ch1 ; (uint32[4]) b0 g0 r0 b1
  cvtdq2ps xmm_ch1, xmm_ch1
  shufps xmm_ch1, xmm_ch1, 0b11000000

  shr rdx, 8                ; (uint8[8]) g0 r0 b1 g1 r1 b2 g2 00
  movd xmm_ch2, edx         ; channel 2 (green)
  pmovzxbd xmm_ch2, xmm_ch2 ; (uint32[4]) g0 r0 b1 g1
  cvtdq2ps xmm_ch2, xmm_ch2
  shufps xmm_ch2, xmm_ch2, 0b11000000

  shr rdx, 8                ; (uint8[8]) r0 b1 g1 r1 b2 g2 00 00
  movd xmm_ch3, edx         ; channel 3 (red)
  pmovzxbd xmm_ch3, xmm_ch3 ; (uint32[4]) r0 b1 g1 r1
  cvtdq2ps xmm_ch3, xmm_ch3
  shufps xmm_ch3, xmm_ch3, 0b11000000

  xorps xmm_bgrb, xmm_bgrb
  vfmadd231ps xmm_bgrb, xmm_ch1, xmm_c1_bgrb ; xmm_bgrb[i] += xmm_ch1[i] * xmm_c1_bgrb[i]
  vfmadd231ps xmm_bgrb, xmm_ch2, xmm_c2_bgrb ; xmm_bgrb[i] += xmm_ch1[i] * xmm_c1_bgrb[i]
  vfmadd231ps xmm_bgrb, xmm_ch3, xmm_c3_bgrb ; xmm_bgrb[i] += xmm_ch1[i] * xmm_c1_bgrb[i]

  ; === xmm_grbg

  shr rdx, 8                ; (uint8[8]) b1 g1 r1 b2 g2 00 00 00
  movd xmm_ch1, edx         ; channel 1 (blue)
  pmovzxbd xmm_ch1, xmm_ch1 ; (uint32[4]) b1 g1 r1 b2
  cvtdq2ps xmm_ch1, xmm_ch1
  shufps xmm_ch1, xmm_ch1, 0b11110000

  mov rdx, [pixel_ptr + 4]  ; (uint8[8]) g1 r1 b2 g2 r2 b3 g3 r3
  movd xmm_ch2, edx         ; channel 2 (green)
  pmovzxbd xmm_ch2, xmm_ch2 ; (uint32[4]) g1 r1 b2 g2
  cvtdq2ps xmm_ch2, xmm_ch2
  shufps xmm_ch2, xmm_ch2, 0b11110000

  shr rdx, 8                ; (uint8[8]) r1 b2 g2 r2 b3 g3 r3 00
  movd xmm_ch3, edx         ; channel 3 (red)
  pmovzxbd xmm_ch3, xmm_ch3 ; (uint32[4]) r1 b2 g2 r2
  cvtdq2ps xmm_ch3, xmm_ch3
  shufps xmm_ch3, xmm_ch3, 0b11110000
  
  xorps xmm_grbg, xmm_grbg
  vfmadd231ps xmm_grbg, xmm_ch1, xmm_c1_grbg
  vfmadd231ps xmm_grbg, xmm_ch2, xmm_c2_grbg
  vfmadd231ps xmm_grbg, xmm_ch3, xmm_c3_grbg

  ; === xmm_rbgr

  shr rdx, 8                ; (uint8[8]) b2 g2 r2 b3 g3 r3 00 00
  movd xmm_ch1, edx         ; channel 1 (blue)
  pmovzxbd xmm_ch1, xmm_ch1 ; (uint32[4]) b2 g2 r2 b3
  cvtdq2ps xmm_ch1, xmm_ch1
  shufps xmm_ch1, xmm_ch1, 0b11111100
  ; xmm_ch3[0] = xmm_ch3[0], xmm_ch3[1] = xmm_ch3[3], xmm_ch3[2] = xmm_ch3[3], xmm_ch3[3] = xmm_ch3[3]
  ; xmm_ch3 is now [pixel[2].b, pixel[3].b, pixel[3].b, pixel[3].b]

  shr rdx, 8                ; (uint8[8]) g2 r2 b3 g3 r3 00 00 00
  movd xmm_ch2, edx         ; channel 2 (green)
  pmovzxbd xmm_ch2, xmm_ch2 ; (uint32[4]) g2 r2 b3 g3
  cvtdq2ps xmm_ch2, xmm_ch2
  shufps xmm_ch2, xmm_ch2, 0b11111100

  shr rdx, 8                ; (uint8[8]) r2 b3 g3 r3 00 00 00 00
  movd xmm_ch3, edx         ; channel 3 (red)
  pmovzxbd xmm_ch3, xmm_ch3 ; (uint32[4]) r2 b3 g3 r3
  cvtdq2ps xmm_ch3, xmm_ch3
  shufps xmm_ch3, xmm_ch3, 0b11111100

  xorps xmm_rbgr, xmm_rbgr
  vfmadd231ps xmm_rbgr, xmm_ch1, xmm_c1_rbgr
  vfmadd231ps xmm_rbgr, xmm_ch2, xmm_c2_rbgr
  vfmadd231ps xmm_rbgr, xmm_ch3, xmm_c3_rbgr

  ; === export results

  cvtps2dq xmm_bgrb, xmm_bgrb ; float -> int
  cvtps2dq xmm_grbg, xmm_grbg
  cvtps2dq xmm_rbgr, xmm_rbgr

  ; convert xmm_bgrb (int32[4]) and xmm_grbg (int32[4]) to xmm_bgrb (int16[8])
  ; xmm_bgrb now contains r0 g0 b0 r1 g1 b1 r1 g2
  packssdw xmm_bgrb, xmm_grbg
  ; convert xmm_rbgr to int16[8]
  ; xmm_grbg now contains b2 r3 g3 b3 [ b2 r3 g3 b3 ] (we'll ignore the last four values)
  packssdw xmm_rbgr, xmm_rbgr
  ; convert uint16s to uint8 using unsigned saturation (i.e. clamp the values between 0 and 255)
  ; xmm_bgrb now contains r0 g0 b0 r1 g1 b1 r2 g2 b2 r3 g3 b3 [ b2 r3 g3 b3 ]
  packuswb xmm_bgrb, xmm_rbgr

  ; write the processed pixels (12 bytes, qword + dword) back to pixel_ptr
  pextrq [pixel_ptr], xmm_bgrb, 0
  pextrd [pixel_ptr + 8], xmm_bgrb, 2

  ; === loop

  lea pixel_ptr, [pixel_ptr + 12]
  cmp pixel_ptr, rsi
  jl image_sepia_sse_loop_4_pixels

  ret

