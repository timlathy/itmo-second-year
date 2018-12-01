global image_sepia_sse

section .text

align 16
channel_max_val: dd 255, 255, 255, 255

align 16
c1_rgbr: dd 0.393, 0.349, 0.272, 0.393
align 16
c2_rgbr: dd 0.769, 0.686, 0.543, 0.769
align 16
c3_rgbr: dd 0.189, 0.168, 0.131, 0.189

align 16
c1_gbrg: dd 0.349, 0.272, 0.393, 0.349
align 16
c2_gbrg: dd 0.686, 0.543, 0.769, 0.686
align 16
c3_gbrg: dd 0.168, 0.131, 0.189, 0.168

align 16
c1_brgb: dd 0.272, 0.393, 0.349, 0.272
align 16
c2_brgb: dd 0.543, 0.769, 0.686, 0.543
align 16
c3_brgb: dd 0.131, 0.189, 0.168, 0.131

; rdi = pointer to the pixel array
; rsi = number of pixels (must be divisible by 4)

; Four channels are processed in parallel,
; first R_{n+0} G_{n+0} B_{n+0} R_{n+1}
; then  G_{n+1} B_{n+1} R_{n+2] G_{n+2}
; then  B_{n+2} R_{n+3} G_{n+3} B_{n+3}

%define xmm_ch1 xmm0
%define xmm_ch2 xmm1
%define xmm_ch3 xmm2

%define xmm_c1_rgbr xmm3
%define xmm_c2_rgbr xmm4
%define xmm_c3_rgbr xmm5

%define xmm_c1_gbrg xmm6
%define xmm_c2_gbrg xmm7
%define xmm_c3_gbrg xmm8

%define xmm_c1_brgb xmm9
%define xmm_c2_brgb xmm10
%define xmm_c3_brgb xmm11

%define xmm_rgbr xmm12
%define xmm_gbrg xmm13
%define xmm_brgb xmm14

%define xmm_clamp_to xmm15

%define pixel_ptr r8

image_sepia_sse:
  movaps xmm_c1_rgbr, [c1_rgbr]
  movaps xmm_c2_rgbr, [c2_rgbr]
  movaps xmm_c3_rgbr, [c3_rgbr]
 
  movaps xmm_c1_gbrg, [c1_gbrg]
  movaps xmm_c2_gbrg, [c2_gbrg]
  movaps xmm_c3_gbrg, [c3_gbrg]
 
  movaps xmm_c1_brgb, [c1_brgb]
  movaps xmm_c2_brgb, [c2_brgb]
  movaps xmm_c3_brgb, [c3_brgb]

  movdqa xmm_clamp_to, [channel_max_val]

  mov pixel_ptr, rdi
  lea rsi, [rsi + 2*rsi] ; rsi (number of bytes to process) = number of pixels * 3 (sizeof(pixel) = 3)
  add rsi, rdi           ; rsi = address of the first byte after the last pixel

image_sepia_sse_loop_4_pixels:
  ; === xmm_rgbr

  mov rdx, [pixel_ptr]  ; (uint8[8]) b0 g0 r0 b1 g1 r1 b2 g2

  movd xmm_ch3, edx         ; channel 3 (blue)
  pmovzxbd xmm_ch3, xmm_ch3 ; (uint32[4]) b0 g0 r0 b1
  shufps xmm_ch3, xmm_ch3, 0b11000000
  ; xmm_ch3[0] = xmm_ch3[0], xmm_ch3[1] = xmm_ch3[0], xmm_ch3[2] = xmm_ch3[0], xmm_ch3[3] = xmm_ch3[3]
  ; xmm_ch3 is now [pixel[0].b, pixel[0].b, pixel[0].b, pixel[1].b]
  cvtdq2ps xmm_ch3, xmm_ch3

  shr rdx, 8                ; (uint8[8]) g0 r0 b1 g1 r1 b2 g2 00
  movd xmm_ch2, edx         ; channel 2 (green)
  pmovzxbd xmm_ch2, xmm_ch2 ; (uint32[4]) g0 r0 b1 g1
  shufps xmm_ch2, xmm_ch2, 0b11000000
  cvtdq2ps xmm_ch2, xmm_ch2

  shr rdx, 8                ; (uint8[8]) r0 b1 g1 r1 b2 g2 00 00
  movd xmm_ch1, edx         ; channel 1 (red)
  pmovzxbd xmm_ch1, xmm_ch1 ; (uint32[4]) r0 b1 g1 r1
  shufps xmm_ch1, xmm_ch1, 0b11000000
  cvtdq2ps xmm_ch1, xmm_ch1

  xorps xmm_rgbr, xmm_rgbr
  vfmadd231ps xmm_rgbr, xmm_ch1, xmm_c1_rgbr ; xmm_rgbr[i] += xmm_ch1[i] * xmm_c1_rgbr[i]
  vfmadd231ps xmm_rgbr, xmm_ch2, xmm_c2_rgbr ; xmm_rgbr[i] += xmm_ch1[i] * xmm_c1_rgbr[i]
  vfmadd231ps xmm_rgbr, xmm_ch3, xmm_c3_rgbr ; xmm_rgbr[i] += xmm_ch1[i] * xmm_c1_rgbr[i]

  ; === xmm_gbrg

  shr rdx, 8                ; (uint8[8]) b1 g1 r1 b2 g2 00 00 00
  movd xmm_ch3, edx         ; channel 3 (blue)
  pmovzxbd xmm_ch3, xmm_ch3 ; (uint32[4]) b1 g1 r1 b2
  shufps xmm_ch3, xmm_ch3, 0b11110000
  ; xmm_ch3[0] = xmm_ch3[0], xmm_ch3[1] = xmm_ch3[0], xmm_ch3[2] = xmm_ch3[2], xmm_ch3[3] = xmm_ch3[2]
  ; xmm_ch3 is now [pixel[1].b, pixel[1].b, pixel[2].b, pixel[2].b]
  cvtdq2ps xmm_ch3, xmm_ch3

  mov rdx, [pixel_ptr + 4]  ; (uint8[8]) g1 r1 b2 g2 r2 b3 g3 r3
  movd xmm_ch2, edx         ; channel 2 (green)
  pmovzxbd xmm_ch2, xmm_ch2 ; (uint32[4]) g1 r1 b2 g2
  shufps xmm_ch2, xmm_ch2, 0b11110000
  cvtdq2ps xmm_ch2, xmm_ch2

  shr rdx, 8                ; (uint8[8]) r1 b2 g2 r2 b3 g3 r3 00
  movd xmm_ch1, edx         ; channel 1 (red)
  pmovzxbd xmm_ch1, xmm_ch1 ; (uint32[4]) r1 b2 g2 r2
  shufps xmm_ch1, xmm_ch1, 0b11110000
  cvtdq2ps xmm_ch1, xmm_ch1
  
  xorps xmm_gbrg, xmm_gbrg
  vfmadd231ps xmm_gbrg, xmm_ch1, xmm_c1_gbrg
  vfmadd231ps xmm_gbrg, xmm_ch2, xmm_c2_gbrg
  vfmadd231ps xmm_gbrg, xmm_ch3, xmm_c3_gbrg

  ; === xmm_brgb

  shr rdx, 8                ; (uint8[8]) b2 g2 r2 b3 g3 r3 00 00
  movd xmm_ch3, edx         ; channel 3 (blue)
  pmovzxbd xmm_ch3, xmm_ch3 ; (uint32[4]) b2 g2 r2 b3
  shufps xmm_ch3, xmm_ch3, 0b01010100
  ; xmm_ch3[0] = xmm_ch3[0], xmm_ch3[1] = xmm_ch3[1], xmm_ch3[2] = xmm_ch3[1], xmm_ch3[3] = xmm_ch3[1]
  ; xmm_ch3 is now [pixel[2].b, pixel[3].b, pixel[3].b, pixel[3].b]
  cvtdq2ps xmm_ch3, xmm_ch3

  shr rdx, 8                ; (uint8[8]) g2 r2 b3 g3 r3 00 00 00
  movd xmm_ch2, edx         ; channel 2 (green)
  pmovzxbd xmm_ch2, xmm_ch2 ; (uint32[4]) g2 r2 b3 g3
  shufps xmm_ch2, xmm_ch2, 0b01010100
  cvtdq2ps xmm_ch2, xmm_ch2

  shr rdx, 8                ; (uint8[8]) r2 b3 g3 r3 00 00 00 00
  movd xmm_ch1, edx         ; channel 1 (red)
  pmovzxbd xmm_ch1, xmm_ch1 ; (uint32[4]) r2 b3 g3 r3
  shufps xmm_ch1, xmm_ch1, 0b01010100
  cvtdq2ps xmm_ch1, xmm_ch1

  xorps xmm_brgb, xmm_brgb
  vfmadd231ps xmm_brgb, xmm_ch1, xmm_c1_brgb
  vfmadd231ps xmm_brgb, xmm_ch2, xmm_c2_brgb
  vfmadd231ps xmm_brgb, xmm_ch3, xmm_c3_brgb

  ; === export results

  cvtps2dq xmm_rgbr, xmm_rgbr   ; float -> int
  pminsd xmm_rgbr, xmm_clamp_to ; min(xmm_rgbr[i], 255) (saturate)
  cvtps2dq xmm_gbrg, xmm_gbrg
  pminsd xmm_gbrg, xmm_clamp_to
  cvtps2dq xmm_brgb, xmm_brgb
  pminsd xmm_brgb, xmm_clamp_to

  pextrb [pixel_ptr + 3*0 + 2], xmm_rgbr, 0
  pextrb [pixel_ptr + 3*0 + 1], xmm_rgbr, 4
  pextrb [pixel_ptr + 3*0 + 0], xmm_rgbr, 8
  pextrb [pixel_ptr + 3*1 + 2], xmm_rgbr, 12
 
  pextrb [pixel_ptr + 3*1 + 1], xmm_gbrg, 0
  pextrb [pixel_ptr + 3*1 + 0], xmm_gbrg, 4
  pextrb [pixel_ptr + 3*2 + 2], xmm_gbrg, 8
  pextrb [pixel_ptr + 3*2 + 1], xmm_gbrg, 12
 
  pextrb [pixel_ptr + 3*2 + 0], xmm_brgb, 0
  pextrb [pixel_ptr + 3*3 + 2], xmm_brgb, 4
  pextrb [pixel_ptr + 3*3 + 1], xmm_brgb, 8
  pextrb [pixel_ptr + 3*3 + 0], xmm_brgb, 12

  lea pixel_ptr, [pixel_ptr + 12]
  cmp pixel_ptr, rsi
  jl image_sepia_sse_loop_4_pixels

  ret
