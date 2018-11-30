global image_sepia_sse

section .text

%define pixels_in_image r9
align 16
maximum: dd 255, 255, 255, 255

align 16
c1_rgbr: dd 0.393, 0.769, 0.189, 0.393
align 16
c2_rgbr: dd 0.349, 0.686, 0.168, 0.349
align 16
c3_rgbr: dd 0.272, 0.543, 0.131, 0.272

align 16
c1_gbrg: dd 0.769, 0.189, 0.393, 0.769
align 16
c2_gbrg: dd 0.686, 0.168, 0.349, 0.686
align 16
c3_gbrg: dd 0.543, 0.131, 0.272, 0.543

align 16
c1_brgb: dd 0.189, 0.393, 0.769, 0.189
align 16
c2_brgb: dd 0.168, 0.349, 0.686, 0.168
align 16
c3_brgb: dd 0.131, 0.272, 0.543, 0.131

; rdi = pointer to the pixel array
; rsi = number of pixels (must be divisible by 4)

%define xmm_tmp xmm0

; Four channels are processed in parallel,
; first R_{n+0} G_{n+0} B_{n+0} R_{n+1}
; then  G_{n+1} B_{n+1} R_{n+2] G_{n+2}
; then  B_{n+2} R_{n+3} G_{n+3} B_{n+3}

%define xmm_ch1 xmm1
%define xmm_ch2 xmm2
%define xmm_ch3 xmm3

%define xmm_c1_rgbr xmm4
%define xmm_c2_rgbr xmm5
%define xmm_c3_rgbr xmm6

%define xmm_c1_gbrg xmm7
%define xmm_c2_gbrg xmm8
%define xmm_c3_gbrg xmm9

%define xmm_c1_brgb xmm10
%define xmm_c2_brgb xmm11
%define xmm_c3_brgb xmm12

%define xmm_rgbr xmm13
%define xmm_gbrg xmm14
%define xmm_brgb xmm15

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

  mov pixel_ptr, rdi
  lea rsi, [rsi + 2*rsi] ; rsi (number of bytes to process) = number of pixels * 3 (sizeof(pixel) = 3)
  add rsi, rdi           ; rsi = address of the first byte after the last pixel

image_sepia_sse_loop_4_pixels:
  ; === xmm_rgbr

  movzx eax, byte [pixel_ptr + 3*0 + 2] ; eax = (uint32_t) pixel[0].r 
  movd xmm_ch1, eax                     ; xmm_ch1[0] = pixel[0].r
  movzx eax, byte [pixel_ptr + 3*1 + 2] ; eax = (uint32_t) pixel[1].r 
  pinsrd xmm_ch1, eax, 3                ; xmm_ch1[3] = pixel[1].r
  ; xmm_ch1[0] = xmm_ch1[0], xmm_ch1[1] = xmm_ch1[0], xmm_ch1[2] = xmm_ch1[0], xmm_ch1[3] = xmm_ch1[3]
  shufps xmm_ch1, xmm_ch1, 0b00000011
  ; xmm_ch1 is now [pixel[0].r, pixel[0].r, pixel[0].r, pixel[1].r]

  movzx eax, byte [pixel_ptr + 3*0 + 1] ; same for the green channel (pixel.g)
  movd xmm_ch2, eax
  movzx eax, byte [pixel_ptr + 3*1 + 1]
  pinsrd xmm_ch2, eax, 3
  shufps xmm_ch2, xmm_ch2, 0b00000011
  ; xmm_ch2 is now [pixel[0].g, pixel[0].g, pixel[0].g, pixel[1].g]

  movzx eax, byte [pixel_ptr + 3*0 + 0] ; same for the blue channel (pixel.b)
  movd xmm_ch3, eax
  movzx eax, byte [pixel_ptr + 3*0 + 3]
  pinsrd xmm_ch3, eax, 3
  shufps xmm_ch3, xmm_ch3, 0b00000011
  ; xmm_ch3 is now [pixel[0].b, pixel[0].b, pixel[0].b, pixel[1].b]
  
  xorps xmm_rgbr, xmm_rgbr
  vfmadd231ps xmm_rgbr, xmm_ch1, xmm_c1_rgbr ; xmm_rgbr[i] += xmm_ch1[i] * xmm_c1_rgbr[i]
  vfmadd231ps xmm_rgbr, xmm_ch2, xmm_c2_rgbr ; xmm_rgbr[i] += xmm_ch1[i] * xmm_c1_rgbr[i]
  vfmadd231ps xmm_rgbr, xmm_ch3, xmm_c3_rgbr ; xmm_rgbr[i] += xmm_ch1[i] * xmm_c1_rgbr[i]

  ; === xmm_gbrg

  movzx eax, byte [pixel_ptr + 3*1 + 2]
  movd xmm_ch1, eax
  movzx eax, byte [pixel_ptr + 3*2 + 2]
  pinsrd xmm_ch1, eax, 2
  ; xmm_ch1[0] = xmm_ch1[0], xmm_ch1[1] = xmm_ch1[0], xmm_ch1[2] = xmm_ch1[2], xmm_ch1[3] = xmm_ch1[2]
  shufps xmm_ch1, xmm_ch1, 0b00001010
  ; xmm_ch1 is now [pixel[1].r, pixel[1].r, pixel[2].r, pixel[2].r]

  movzx eax, byte [pixel_ptr + 3*1 + 1]
  movd xmm_ch2, eax
  movzx eax, byte [pixel_ptr + 3*2 + 1]
  pinsrd xmm_ch2, eax, 2
  shufps xmm_ch2, xmm_ch2, 0b00001010
  ; xmm_ch2 is now [pixel[1].g, pixel[1].g, pixel[2].g, pixel[2].g]

  movzx eax, byte [pixel_ptr + 3*1 + 0]
  movd xmm_ch3, eax
  movzx eax, byte [pixel_ptr + 3*2 + 0]
  pinsrd xmm_ch3, eax, 2
  shufps xmm_ch3, xmm_ch3, 0b00001010
  ; xmm_ch3 is now [pixel[1].b, pixel[1].b, pixel[2].b, pixel[2].b]
  
  xorps xmm_gbrg, xmm_gbrg
  vfmadd231ps xmm_gbrg, xmm_ch1, xmm_c1_gbrg
  vfmadd231ps xmm_gbrg, xmm_ch2, xmm_c2_gbrg
  vfmadd231ps xmm_gbrg, xmm_ch3, xmm_c3_gbrg

  ; === xmm_grgb

  movzx eax, byte [pixel_ptr + 3*2 + 2]
  movd xmm_ch1, eax
  movzx eax, byte [pixel_ptr + 3*3 + 2]
  pinsrd xmm_ch1, eax, 1
  ; xmm_ch1[0] = xmm_ch1[0], xmm_ch1[1] = xmm_ch1[1], xmm_ch1[2] = xmm_ch1[1], xmm_ch1[3] = xmm_ch1[1]
  shufps xmm_ch1, xmm_ch1, 0b00010101
  ; xmm_ch1 is now [pixel[2].r, pixel[3].r, pixel[3].r, pixel[3].r]

  movzx eax, byte [pixel_ptr + 3*2 + 1]
  movd xmm_ch2, eax
  movzx eax, byte [pixel_ptr + 3*3 + 1]
  pinsrd xmm_ch2, eax, 2
  shufps xmm_ch2, xmm_ch2, 0b00001010
  ; xmm_ch2 is now [pixel[2].g, pixel[3].g, pixel[3].g, pixel[3].g]

  movzx eax, byte [pixel_ptr + 3*2 + 0]
  movd xmm_ch3, eax
  movzx eax, byte [pixel_ptr + 3*3 + 0]
  pinsrd xmm_ch3, eax, 2
  shufps xmm_ch3, xmm_ch3, 0b00001010
  ; xmm_ch3 is now [pixel[2].b, pixel[3].b, pixel[3].b, pixel[3].b]
  
  xorps xmm_brgb, xmm_brgb
  vfmadd231ps xmm_brgb, xmm_ch1, xmm_c1_brgb
  vfmadd231ps xmm_brgb, xmm_ch2, xmm_c2_brgb
  vfmadd231ps xmm_brgb, xmm_ch3, xmm_c3_brgb

  ; === export results

  cvtps2dq xmm0, xmm0 ; float -> int
  pminsd xmm0, [maximum] ; cut to 255

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
