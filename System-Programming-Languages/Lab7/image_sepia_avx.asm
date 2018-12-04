global image_sepia_avx

section .data

; bgr|bgr|bg r|bgr|bgr|b gr|bgr|bgr
; 8

align 32
c1_bgrb_grbg: dd 0.131, 0.168, 0.189, 0.131, 0.168, 0.189, 0.131, 0.168
align 32
c2_bgrb_grbg: dd 0.543, 0.686, 0.769, 0.543, 0.686, 0.769, 0.543, 0.686
align 32
c3_bgrb_grbg: dd 0.272, 0.349, 0.393, 0.272, 0.349, 0.393, 0.272, 0.349

align 32
c1_rbgr_bgrb: dd 0.189, 0.131, 0.168, 0.189, 0.131, 0.168, 0.189, 0.131
align 32
c2_rbgr_bgrb: dd 0.769, 0.543, 0.686, 0.769, 0.543, 0.686, 0.769, 0.543
align 32
c3_rbgr_bgrb: dd 0.393, 0.272, 0.349, 0.393, 0.272, 0.349, 0.393, 0.272

align 32
c1_grbg_rbgr: dd 0.168, 0.189, 0.131, 0.168, 0.189, 0.131, 0.168, 0.189
align 32
c2_grbg_rbgr: dd 0.686, 0.769, 0.543, 0.686, 0.769, 0.543, 0.686, 0.769
align 32
c3_grbg_rbgr: dd 0.349, 0.393, 0.272, 0.349, 0.393, 0.272, 0.349, 0.393

section .text

; rdi = pointer to the pixel array
; rsi = number of pixels (must be divisible by 4)

%define ymm_ch1 ymm0
%define xmm_ch1 xmm0
%define ymm_ch2 ymm1
%define xmm_ch2 xmm1
%define ymm_ch3 ymm2
%define xmm_ch3 xmm2

%define ymm_c1_bgrb_grbg ymm3
%define ymm_c2_bgrb_grbg ymm4
%define ymm_c3_bgrb_grbg ymm5

%define ymm_c1_rbgr_bgrb ymm6
%define ymm_c2_rbgr_bgrb ymm7
%define ymm_c3_rbgr_bgrb ymm8

%define ymm_c1_grbg_rbgr ymm9
%define ymm_c2_grbg_rbgr ymm10
%define ymm_c3_grbg_rbgr ymm11

%define ymm_bgrb_grbg ymm12
%define xmm_bgrb_grbg xmm12
%define ymm_rbgr_bgrb ymm13
%define xmm_rbgr_bgrb xmm13
%define ymm_grbg_rbgr ymm14
%define xmm_grbg_rbgr xmm14

%define pixel_ptr rcx

image_sepia_avx:
  vmovaps ymm_c1_bgrb_grbg, [c1_bgrb_grbg]
  vmovaps ymm_c2_bgrb_grbg, [c2_bgrb_grbg]
  vmovaps ymm_c3_bgrb_grbg, [c3_bgrb_grbg]
 
  vmovaps ymm_c1_rbgr_bgrb, [c1_rbgr_bgrb]
  vmovaps ymm_c2_rbgr_bgrb, [c2_rbgr_bgrb]
  vmovaps ymm_c3_rbgr_bgrb, [c3_rbgr_bgrb]
 
  vmovaps ymm_c1_grbg_rbgr, [c1_grbg_rbgr]
  vmovaps ymm_c2_grbg_rbgr, [c2_grbg_rbgr]
  vmovaps ymm_c3_grbg_rbgr, [c3_grbg_rbgr]

  mov pixel_ptr, rdi
  lea rsi, [rsi + 2*rsi] ; rsi (number of bytes to process) = number of pixels * 3 (sizeof(pixel) = 3)
  add rsi, rdi           ; rsi = address of the first byte after the last pixel

image_sepia_avx_loop_4_pixels:
  ; === ymm_bgrb_grbg

  mov rax, [pixel_ptr]  ; (uint8[8]) b0 g0 r0 b1 g1 r1 b2 g2
  mov rdx, rax
  shr rdx, 24           ; (uint8[8]) b1 g1 r1 b2 g2 00 00 00

  ; b0b0b0b1 b1b1b2b2

  movd xmm_ch1, eax         ; channel 1 (blue)
  pmovzxbd xmm_ch1, xmm_ch1 ; (uint32[4]) b0 g0 r0 b1
  cvtdq2ps xmm_ch1, xmm_ch1
  shufps xmm_ch1, xmm_ch1, 0b11000000 ; b0 b0 b0 b1
  vperm2f128 ymm_ch1, ymm_ch1, ymm_ch1, 0b00000000 ; b0 b0 b0 b1 b0 b0 b0 b1
  movd xmm_ch1, edx
  pmovzxbd xmm_ch1, xmm_ch1 ; (uint32[4]) b1 g1 r1 b2
  cvtdq2ps xmm_ch1, xmm_ch1
  shufps xmm_ch1, xmm_ch1, 0b11110000 ; b1 b1 b2 b2
  ; ymm_ch1 is now b1 b1 b1 b2 b0 b0 b0 b1
  vperm2f128 ymm_ch1, ymm_ch1, ymm_ch1, 0b00000001 ; b0 b0 b0 b1 b1 b1 b1 b2

  shr rax, 8                ; (uint8[8]) g0 r0 b1 g1 r1 b2 g2 00
  movd xmm_ch2, eax         ; channel 2 (green)
  pmovzxbd xmm_ch2, xmm_ch2 ; (uint32[4]) g0 r0 b1 g1
  cvtdq2ps xmm_ch2, xmm_ch2
  shufps xmm_ch2, xmm_ch2, 0b11000000 ; g0 g0 g0 g1
  vperm2f128 ymm_ch2, ymm_ch2, ymm_ch2, 0b00000000
  shr rdx, 8                ; (uint8[8]) g1 r1 b2 g2 00 00 00 00
  movd xmm_ch2, edx
  pmovzxbd xmm_ch2, xmm_ch2 ; (uint32[4]) g1 r1 b2 g2
  cvtdq2ps xmm_ch2, xmm_ch2
  shufps xmm_ch2, xmm_ch2, 0b11110000 ; g1 g1 g2 g2
  vperm2f128 ymm_ch2, ymm_ch2, ymm_ch2, 0b00000001 ; swap lower and higher halfs

  shr rax, 8                ; (uint8[8]) r0 b1 g1 r1 b2 g2 00 00
  movd xmm_ch3, eax         ; channel 3 (red)
  pmovzxbd xmm_ch3, xmm_ch3 ; (uint32[4]) r0 b1 g1 r1
  cvtdq2ps xmm_ch3, xmm_ch3
  shufps xmm_ch3, xmm_ch3, 0b11000000 ; r0 r0 r0 r1
  vperm2f128 ymm_ch3, ymm_ch3, ymm_ch3, 0b00000000
  mov rdx, [pixel_ptr + 4]  ; (uint8[8]) g1 r1 b2 g2 r2 b3 r3 r4
  shr rdx, 8                ; (uint8[8]) r1 b2 g2 r2 b3 r3 00 00
  movd xmm_ch3, edx
  pmovzxbd xmm_ch3, xmm_ch3 ; (uint32[4]) r1 b2 g2 r2
  cvtdq2ps xmm_ch3, xmm_ch3
  shufps xmm_ch3, xmm_ch3, 0b11110000 ; r1 r1 r2 r2
  vperm2f128 ymm_ch3, ymm_ch3, ymm_ch3, 0b00000001 ; swap lower and higher halfs

  vxorps ymm_bgrb_grbg, ymm_bgrb_grbg
  vfmadd231ps ymm_bgrb_grbg, ymm_ch1, ymm_c1_bgrb_grbg
  vfmadd231ps ymm_bgrb_grbg, ymm_ch2, ymm_c2_bgrb_grbg
  vfmadd231ps ymm_bgrb_grbg, ymm_ch3, ymm_c3_bgrb_grbg

  ; bgrb_grbg
  ; b0 b0 b0 b1 b1 b1 b2 b2
  ; rbgr_bgrb
  ; b2 b3 b3 b3 b4 b4 b4 b5
 
  ; b0 g0 r0 b1 g1 r1 b2 g2 r2 b3 g3 r3 b4 g4 r4
  ;  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14
 
  shr rdx, 8                ; (uint8[8]) b2 g2 r2 b3 g3 r3 00 00
  movd xmm_ch1, edx         ; channel 1 (blue)
  pmovzxbd xmm_ch1, xmm_ch1 ; (uint32[4]) b2 g2 r2 b3
  cvtdq2ps xmm_ch1, xmm_ch1
  shufps xmm_ch1, xmm_ch1, 0b11111100 ; b2 b3 b3 b3
  vperm2f128 ymm_ch1, ymm_ch1, ymm_ch1, 0b00000000
  mov rax, [pixel_ptr + 12] ; (uint8[8]) b4 g4 r4 b5 g5 r5 b6 g6
  movd xmm_ch1, eax
  pmovzxbd xmm_ch1, xmm_ch1 ; (uint32[4] b4 g4 r4 b5
  cvtdq2ps xmm_ch1, xmm_ch1
  shufps xmm_ch1, xmm_ch1, 0b11000000 ; b4 b4 b4 b5
  vperm2f128 ymm_ch1, ymm_ch1, ymm_ch1, 0b00000001 ; swap lower and higher halfs
 
  shr rdx, 8                ; (uint8[8]) g2 r2 b3 g3 r3 00 00 00
  movd xmm_ch2, edx         ; channel 2 (green)
  pmovzxbd xmm_ch2, xmm_ch2 ; (uint32[4]) g2 r2 b3 g3
  cvtdq2ps xmm_ch2, xmm_ch2
  shufps xmm_ch2, xmm_ch2, 0b11111100 ; g2 g3 g3 g3
  vperm2f128 ymm_ch2, ymm_ch2, ymm_ch2, 0b00000000
  shr rax, 8                ; (uint8[8]) g4 r4 b5 g5 r5 b6 g6 00
  movd xmm_ch2, eax
  pmovzxbd xmm_ch2, xmm_ch2 ; (uint32[4] g4 r4 b5 g5
  cvtdq2ps xmm_ch2, xmm_ch2
  shufps xmm_ch2, xmm_ch2, 0b11000000 ; g4 g4 g4 g5
  vperm2f128 ymm_ch2, ymm_ch2, ymm_ch2, 0b00000001 ; swap lower and higher halfs
 
  shr rdx, 8                ; (uint8[8]) r2 b3 g3 r3 00 00 00 00
  movd xmm_ch3, edx         ; channel 3 (red)
  pmovzxbd xmm_ch3, xmm_ch3 ; (uint32[4]) r2 b3 g3 r3
  cvtdq2ps xmm_ch3, xmm_ch3
  shufps xmm_ch3, xmm_ch3, 0b11111100 ; r2 r3 r3 r3
  vperm2f128 ymm_ch3, ymm_ch3, ymm_ch3, 0b00000000
  shr rax, 8                ; (uint8[8]) r4 b5 g5 r5 b6 g6 00 00
  movd xmm_ch3, eax
  pmovzxbd xmm_ch3, xmm_ch3 ; (uint32[4] r4 b5 g5 r5
  cvtdq2ps xmm_ch3, xmm_ch3
  shufps xmm_ch3, xmm_ch3, 0b11000000 ; r4 r4 r4 r5
  vperm2f128 ymm_ch3, ymm_ch3, ymm_ch3, 0b00000001 ; swap lower and higher halfs

  vxorps ymm_rbgr_bgrb, ymm_rbgr_bgrb
  vfmadd231ps ymm_rbgr_bgrb, ymm_ch1, ymm_c1_rbgr_bgrb
  vfmadd231ps ymm_rbgr_bgrb, ymm_ch2, ymm_c2_rbgr_bgrb
  vfmadd231ps ymm_rbgr_bgrb, ymm_ch3, ymm_c3_rbgr_bgrb
 
  ; bgrb_grbg
  ; b0 b0 b0 b1 b1 b1 b1 b2
  ; rbgr_bgrb
  ; b2 b3 b3 b3 b4 b4 b4 b5
  ; grbg_rbgr
  ; g5 r5 b6 g6 r6 b7 g7 r7
 
  ; b0 g0 r0 b1 g1 r1 b2 g2 r2 b3 g3 r3 b4 g4 r4 b5 g5 r5 b6 g6 r6 b7 g7 r7
  ;  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23
 
  shr rax, 8                ; (uint8[8]) b5 g5 r5 b6 g6 00 00 00
  movd xmm_ch1, eax         ; channel 1 (blue)
  pmovzxbd xmm_ch1, xmm_ch1 ; (uint32[4]) b5 g5 r5 b6
  cvtdq2ps xmm_ch1, xmm_ch1
  shufps xmm_ch1, xmm_ch1, 0b11110000 ; b5 b5 b6 b6
  vperm2f128 ymm_ch1, ymm_ch1, ymm_ch1, 0b00000000
  mov rax, [pixel_ptr + 16] ; (uint8[8]) g5 r5 b6 g6 r6 b7 g7 r7
  mov rdx, rax              ; store data that we will need later in rdx to avoid extra memory access
  shr rax, 16               ; (uint8[8]) b6 g6 r6 b7 g7 r7 00 00
  movd xmm_ch1, eax
  pmovzxbd xmm_ch1, xmm_ch1 ; (uint32[4]) b6 g6 r6 b7
  cvtdq2ps xmm_ch1, xmm_ch1
  shufps xmm_ch1, xmm_ch1, 0b11111100; b6 b7 b7 b7
  vperm2f128 ymm_ch1, ymm_ch1, ymm_ch1, 0b00000001 ; swap lower and higher halfs
 
  movd xmm_ch2, edx         ; channel 2 (green)
  pmovzxbd xmm_ch2, xmm_ch2 ; (uint32[4]) g5 r5 b6 g6
  cvtdq2ps xmm_ch2, xmm_ch2
  shufps xmm_ch2, xmm_ch2, 0b11110000 ; g5 g5 g6 g6
  vperm2f128 ymm_ch2, ymm_ch2, ymm_ch2, 0b00000000
  shr rax, 8               ; (uint8[8]) g6 r6 b7 g7 r7 00 00 00
  movd xmm_ch2, eax
  pmovzxbd xmm_ch2, xmm_ch2 ; (uint32[4]) g6 r6 b7 g7
  cvtdq2ps xmm_ch2, xmm_ch2
  shufps xmm_ch2, xmm_ch2, 0b11111100; g6 g7 g7 g7
  vperm2f128 ymm_ch2, ymm_ch2, ymm_ch2, 0b00000001 ; swap lower and higher halfs
 
  shr rdx, 8                ; (uint8[8]) r5 b6 g6 r6 b7 g7 r7 00
  movd xmm_ch3, edx         ; channel 3 (red)
  pmovzxbd xmm_ch3, xmm_ch3 ; (uint32[4]) r5 b6 g6 r6
  cvtdq2ps xmm_ch3, xmm_ch3
  shufps xmm_ch3, xmm_ch3, 0b11110000 ; r5 r5 r6 r6
  vperm2f128 ymm_ch3, ymm_ch3, ymm_ch3, 0b00000000
  shr rax, 8               ; (uint8[8]) r6 b7 g7 r7 00 00 00 00
  movd xmm_ch3, eax
  pmovzxbd xmm_ch3, xmm_ch3 ; (uint32[4]) r6 b7 g7 r7
  cvtdq2ps xmm_ch3, xmm_ch3
  shufps xmm_ch3, xmm_ch3, 0b11111100; r6 r7 r7 r7
  vperm2f128 ymm_ch3, ymm_ch3, ymm_ch3, 0b00000001 ; swap lower and higher halfs

  vxorps ymm_grbg_rbgr, ymm_grbg_rbgr
  vfmadd231ps ymm_grbg_rbgr, ymm_ch1, ymm_c1_grbg_rbgr
  vfmadd231ps ymm_grbg_rbgr, ymm_ch2, ymm_c2_grbg_rbgr
  vfmadd231ps ymm_grbg_rbgr, ymm_ch3, ymm_c3_grbg_rbgr
;
  ; === export results

  vcvtps2dq ymm_bgrb_grbg, ymm_bgrb_grbg ; float -> int
  vcvtps2dq ymm_rbgr_bgrb, ymm_rbgr_bgrb
  vcvtps2dq ymm_grbg_rbgr, ymm_grbg_rbgr

  vextracti128 xmm_ch1, ymm_bgrb_grbg, 1
  packssdw xmm_bgrb_grbg, xmm_ch1 ; xmm_bgrb_grbg = (uint16[8]) ymm_bgrb_grbg
  vextracti128 xmm_ch2, ymm_rbgr_bgrb, 1
  packssdw xmm_rbgr_bgrb, xmm_ch2 ; xmm_rbgr_bgrb = (uint16[8]) ymm_rgr_bgrb
  packuswb xmm_bgrb_grbg, xmm_rbgr_bgrb ; xmm_bgrb_grbg = { (uint8[8]) xmm_bgrb_grbg, (uint8[8]) xmm_rbgr_bgrb }, clamped to 0..255 (unsigned saturation)
  vextracti128 xmm_ch3, ymm_grbg_rbgr, 1
  packssdw xmm_grbg_rbgr, xmm_ch3 ; xmm_grbg_rbgr = (uint16[8]) ymm_grbg_rbgr
  packuswb xmm_grbg_rbgr, xmm_grbg_rbgr ; xmm_grbg_rbgr = { (uint8[8]) xmm_grbg_rbgr, ... }

  ; write the processed pixels (24 bytes, dqword + qword) back to pixel_ptr
  movdqu [pixel_ptr], xmm_bgrb_grbg
  movq [pixel_ptr + 16], xmm_grbg_rbgr

  ; === loop

  lea pixel_ptr, [pixel_ptr + 24]
  cmp pixel_ptr, rsi
  jl image_sepia_avx_loop_4_pixels

  ret
