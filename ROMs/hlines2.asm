
        add     a,b             ; 0000 80   .
        ld      b,b             ; 0001 40   @
        jr      nz,l0014        ; 0002 20 10    .
        ld      (l0204),sp      ; 0004 08 04 02   ...
        ld      bc,0c033h       ; 0007 01 33 c0   .3@
        ld      e,(hl)          ; 000a 5e   ^
        ld      e,a             ; 000b 5f   _
        ret                     ; 000c c9   I

l000d:  set     1,b             ; 000d cb c8   KH
        inc     b               ; 000f 04   .
        nop                     ; 0010 00   .
        nop                     ; 0011 00   .
        ld      d,a             ; 0012 57   W
        ld      d,(hl)          ; 0013 56   V
l0014:  adc     a,e             ; 0014 8b   .
        halt                    ; 0015 76   v
        ld      b,8bh           ; 0016 06 8b   ..
        ld      b,(hl)          ; 0018 46   F
        ld      (0d08bh),sp     ; 0019 08 8b d0   ..P
        adc     a,e             ; 001c 8b   .
        cp      8eh             ; 001d fe 8e   ..
        ret     nz              ; 001f c0   @
        cp      c               ; 0020 b9   9
        rst     38h             ; 0021 ff   .
        rst     38h             ; 0022 ff   .
        inc     sp              ; 0023 33   3
        ret     nz              ; 0024 c0   @
        ldh     a,(c)           ; 0025 f2   r
        xor     (hl)            ; 0026 ae   .
        rst     30h             ; 0027 f7   w
        pop     de              ; 0028 d1   Q
        ld      c,c             ; 0029 49   I
        inc     bc              ; 002a 03   .
        adc     a,8bh           ; 002b ce 8b   N.
        jp      nz,0d88eh       ; 002d c2 8e d8   B.X
        adc     a,e             ; 0030 8b   .
        reti                    ; 0031 d9   Y

        adc     a,e             ; 0032 8b   .
        adc     a,8bh           ; 0033 ce 8b   N.
        pop     bc              ; 0035 c1   A
        adc     a,e             ; 0036 8b   .
        .db     0d3h            ; 0037 d3   S
        ld      c,e             ; 0038 4b   K
        dec     sp              ; 0039 3b   ;
        jp      nz,l1d73        ; 003a c2 73 1d   Bs.
        add     a,b             ; 003d 80   .
        ccf                     ; 003e 3f   ?
        ldd     a,(hl)          ; 003f 3a   :
        reti                    ; 0040 d9   Y

        jr      lffc3           ; 0041 18 80   ..

        ccf                     ; 0043 3f   ?
        ld      e,h             ; 0044 5c   \
        ld      (hl),h          ; 0045 74   t
        reti                    ; 0046 d9   Y

        add     a,b             ; 0047 80   .
        reti                    ; 0048 d9   Y

        ccf                     ; 0049 3f   ?
        ld      (hl),h          ; 004a 74   t
        dec     b               ; 004b 05   .
        add     a,b             ; 004c 80   .
        ccf                     ; 004d 3f   ?
        ldi     a,(hl)          ; 004e 2a   *
        ld      (hl),l          ; 004f 75   u
        reti                    ; 0050 d9   Y

        ld      d,1fh           ; 0051 16 1f   ..
        cp      b               ; 0053 b8   8
        ld      bc,l5e00        ; 0054 01 00 5e   ..^
        ld      e,a             ; 0057 5f   _
        reti                    ; 0058 d9   Y

        rl      (hl)            ; 0059 cb 16   K.
        rra                     ; 005b 1f   .
        ld      c,l             ; 005c 4d   M
        nop                     ; 005d 00   .
        nop                     ; 005e 00   .
        ld      h,d             ; 005f 62   b
        reti                    ; 0060 d9   Y

        set     1,b             ; 0061 cb c8   KH
        inc     b               ; 0063 04   .
        nop                     ; 0064 00   .
        nop                     ; 0065 00   .
        ld      d,a             ; 0066 57   W
        ld      d,(hl)          ; 0067 56   V
        adc     a,e             ; 0068 8b   .
        halt                    ; 0069 76   v
        ld      b,8bh           ; 006a 06 8b   ..
        ld      b,(hl)          ; 006c 46   F
        ld      (0d08bh),sp     ; 006d 08 8b d0   ..P
        adc     a,e             ; 0070 8b   .
        cp      8eh             ; 0071 fe 8e   ..
        ret     nz              ; 0073 c0   @
        cp      c               ; 0074 b9   9
        rst     38h             ; 0075 ff   .
        rst     38h             ; 0076 ff   .
        inc     sp              ; 0077 33   3
        ret     nz              ; 0078 c0   @
        ldh     a,(c)           ; 0079 f2   r
        xor     (hl)            ; 007a ae   .
        rst     30h             ; 007b f7   w
        pop     de              ; 007c d1   Q
        ld      c,c             ; 007d 49   I
        inc     bc              ; 007e 03   .
        adc     a,8bh           ; 007f ce 8b   N.
        jp      nz,0d88eh       ; 0081 c2 8e d8   B.X
        adc     a,e             ; 0084 8b   .
        reti                    ; 0085 d9   Y

        adc     a,e             ; 0086 8b   .
        adc     a,8bh           ; 0087 ce 8b   N.
        pop     bc              ; 0089 c1   A
        dec     sp              ; 008a 3b   ;
        jp      l0b73           ; 008b c3 73 0b   Cs.

        ld      c,e             ; 008e 4b   K
        add     a,b             ; 008f 80   .
        ccf                     ; 0090 3f   ?
        ldd     a,(hl)          ; 0091 3a   :
        ld      (hl),h          ; 0092 74   t
        dec     b               ; 0093 05   .
        add     a,b             ; 0094 80   .
        ccf                     ; 0095 3f   ?
        ld      e,h             ; 0096 5c   \
        ld      (hl),l          ; 0097 75   u
        rst     28h             ; 0098 ef   o
        add     a,b             ; 0099 80   .
        ccf                     ; 009a 3f   ?
        ldd     a,(hl)          ; 009b 3a   :
        ld      (hl),l          ; 009c 75   u
        ld      bc,8b43h        ; 009d 01 43 8b   .C.
        ld      b,(hl)          ; 00a0 46   F
        ld      (0da8ch),sp     ; 00a1 08 8c da   ..Z
        dec     sp              ; 00a4 3b   ;
        bit     6,l             ; 00a5 cb 75   Ku
        add     hl,bc           ; 00a7 09   .
        dec     sp              ; 00a8 3b   ;
        jp      nz,l0575        ; 00a9 c2 75 05   Bu.
        add     a,b             ; 00ac 80   .
        ccf                     ; 00ad 3f   ?
        ld      e,h             ; 00ae 5c   \
        ld      (hl),h          ; 00af 74   t
        ld      de,0c18bh       ; 00b0 11 8b c1   ..A
        dec     sp              ; 00b3 3b   ;
        jp      l0c73           ; 00b4 c3 73 0c   Cs.

        add     a,b             ; 00b7 80   .
        ccf                     ; 00b8 3f   ?
        ld      e,h             ; 00b9 5c   \
        ld      (hl),l          ; 00ba 75   u
        rlca                    ; 00bb 07   .
        add     a,b             ; 00bc 80   .
        ld      a,a             ; 00bd 7f   .
        rst     38h             ; 00be ff   .
        ldd     a,(hl)          ; 00bf 3a   :
        ld      (hl),l          ; 00c0 75   u
        ld      bc,0c643h       ; 00c1 01 43 c6   .CF
        rlca                    ; 00c4 07   .
        nop                     ; 00c5 00   .
        ld      d,1fh           ; 00c6 16 1f   ..
        ld      e,(hl)          ; 00c8 5e   ^
        ld      e,a             ; 00c9 5f   _
        ret                     ; 00ca c9   I

        set     1,b             ; 00cb cb c8   KH
        inc     b               ; 00cd 04   .
        nop                     ; 00ce 00   .
        nop                     ; 00cf 00   .
        ld      d,a             ; 00d0 57   W
        ld      d,(hl)          ; 00d1 56   V
        adc     a,e             ; 00d2 8b   .
        halt                    ; 00d3 76   v
        ld      b,8bh           ; 00d4 06 8b   ..
        ld      b,(hl)          ; 00d6 46   F
        ld      (0d08bh),sp     ; 00d7 08 8b d0   ..P
        adc     a,e             ; 00da 8b   .
        cp      8eh             ; 00db fe 8e   ..
        ret     nz              ; 00dd c0   @
        cp      c               ; 00de b9   9
        rst     38h             ; 00df ff   .
        rst     38h             ; 00e0 ff   .
        inc     sp              ; 00e1 33   3
        ret     nz              ; 00e2 c0   @
        ldh     a,(c)           ; 00e3 f2   r
        xor     (hl)            ; 00e4 ae   .
        rst     30h             ; 00e5 f7   w
        pop     de              ; 00e6 d1   Q
        ld      c,c             ; 00e7 49   I
        inc     bc              ; 00e8 03   .
        adc     a,8bh           ; 00e9 ce 8b   N.
        jp      nz,0d88eh       ; 00eb c2 8e d8   B.X
        adc     a,e             ; 00ee 8b   .
        reti                    ; 00ef d9   Y

        adc     a,e             ; 00f0 8b   .
        adc     a,8bh           ; 00f1 ce 8b   N.
        pop     bc              ; 00f3 c1   A
        adc     a,e             ; 00f4 8b   .
        .db     0d3h            ; 00f5 d3   S
        ld      c,e             ; 00f6 4b   K
        dec     sp              ; 00f7 3b   ;
        jp      nz,l0f73        ; 00f8 c2 73 0f   Bs.
        add     a,b             ; 00fb 80   .
        ccf                     ; 00fc 3f   ?
        ldd     a,(hl)          ; 00fd 3a   :
        ld      (hl),h          ; 00fe 74   t
        ld      a,(bc)          ; 00ff 0a   .
        nop                     ; 0100 00   .
        jp      l0150           ; 0101 c3 50 01   CP.

        adc     a,0edh          ; 0104 ce ed   Nm
        ld      h,(hl)          ; 0106 66   f
        ld      h,(hl)          ; 0107 66   f
        call    z,l000d         ; 0108 cc 0d 00   L..
        dec     bc              ; 010b 0b   .
        inc     bc              ; 010c 03   .
        ld      (hl),e          ; 010d 73   s
        nop                     ; 010e 00   .
        add     a,e             ; 010f 83   .
        nop                     ; 0110 00   .
        inc     c               ; 0111 0c   .
        nop                     ; 0112 00   .
        dec     c               ; 0113 0d   .
        nop                     ; 0114 00   .
        ld      (l1f11),sp      ; 0115 08 11 1f   ...
        adc     a,b             ; 0118 88   .
        adc     a,c             ; 0119 89   .
        nop                     ; 011a 00   .
        ld      c,0dch          ; 011b 0e dc   .\
        call    z,0e66eh        ; 011d cc 6e e6   Lnf
        .db     0ddh            ; 0120 dd   ]
        .db     0ddh            ; 0121 dd   ]
        reti                    ; 0122 d9   Y

        sbc     a,c             ; 0123 99   .
        cp      e               ; 0124 bb   ;
        cp      e               ; 0125 bb   ;
        ld      h,a             ; 0126 67   g
        ld      h,e             ; 0127 63   c
        ld      l,(hl)          ; 0128 6e   n
        ld      c,0ech          ; 0129 0e ec   .l
        call    z,0dcddh        ; 012b cc dd dc   L]\
        sbc     a,c             ; 012e 99   .
        sbc     a,a             ; 012f 9f   .
        cp      e               ; 0130 bb   ;
        cp      c               ; 0131 b9   9
        inc     sp              ; 0132 33   3
        ld      a,48h           ; 0133 3e 48   >H
        ld      b,l             ; 0135 45   E
        ld      d,d             ; 0136 52   R
        ld      c,a             ; 0137 4f   O
        ld      e,d             ; 0138 5a   Z
        ld      b,l             ; 0139 45   E
        ld      d,d             ; 013a 52   R
        ld      c,a             ; 013b 4f   O
        jr      nz,l0191        ; 013c 20 53    S
        ld      b,c             ; 013e 41   A
        ld      c,l             ; 013f 4d   M
        ld      d,b             ; 0140 50   P
        ld      c,h             ; 0141 4c   L
        ld      b,l             ; 0142 45   E
        ld      sp,0            ; 0143 31 00 00   1..
        nop                     ; 0146 00   .
        nop                     ; 0147 00   .
        nop                     ; 0148 00   .
        nop                     ; 0149 00   .
        sbc     a,0adh          ; 014a de ad   ^-
        ld      bc,92dah        ; 014c 01 da 92   .Z.
        inc     h               ; 014f 24   $

l0150:  di                      ; 0150 f3   s
        ld      sp,0fff4h       ; 0151 31 f4 ff   1t.		; Init stack
        ld      a,0             ; 0154 3e 00   >.
        ldh     (0ffh),a        ; 0156 e0 ff   `.		; Turn off IRQs
        sub     a               ; 0158 97   .
        ldh     (41h),a         ; 0159 e0 41   `A
        ldh     (42h),a         ; 015b e0 42   `B
        ldh     (43h),a         ; 015d e0 43   `C
        call    l0328           ; 015f cd 28 03   M(.
        ld      a,11h           ; 0162 3e 11   >.
        ldh     (40h),a         ; 0164 e0 40   `@
        call    l033c           ; 0166 cd 3c 03   M<.
        call    l0341           ; 0169 cd 41 03   MA.
        call    l0353           ; 016c cd 53 03   MS.
        ld      a,91h           ; 016f 3e 91   >.
        ldh     (40h),a         ; 0171 e0 40   `@
        ld      a,0             ; 0173 3e 00   >.
        ld      (0c001h),a      ; 0175 ea 01 c0   j.@
        ld      a,77h           ; 0178 3e 77   >w
        ld      (0c003h),a      ; 017a ea 03 c0   j.@
l017d:  ld      a,3             ; 017d 3e 03   >.
        ld      (0c006h),a      ; 017f ea 06 c0   j.@
        ld      b,80h           ; 0182 06 80   ..
        ld      c,7fh           ; 0184 0e 7f   ..
l0186:  push    bc              ; 0186 c5   E
        dec     b               ; 0187 05   .
        ld      a,b             ; 0188 78   x
        ld      (0c000h),a      ; 0189 ea 00 c0   j.@
        ld      a,c             ; 018c 79   y
        sub     b               ; 018d 90   .
        ld      (0c002h),a      ; 018e ea 02 c0   j.@
l0191:  call    l01e3           ; 0191 cd e3 01   Mc.
        pop     bc              ; 0194 c1   A
        dec     b               ; 0195 05   .
        jr      nz,l0186        ; 0196 20 ee    n
        ld      a,0             ; 0198 3e 00   >.
        ld      (0c000h),a      ; 019a ea 00 c0   j.@
        ld      a,7fh           ; 019d 3e 7f   >.
        ld      (0c002h),a      ; 019f ea 02 c0   j.@
        ld      a,2             ; 01a2 3e 02   >.
        ld      (0c006h),a      ; 01a4 ea 06 c0   j.@
        ld      b,78h           ; 01a7 06 78   .x
        ld      c,77h           ; 01a9 0e 77   .w
l01ab:  push    bc              ; 01ab c5   E
        dec     b               ; 01ac 05   .
        ld      a,b             ; 01ad 78   x
        ld      (0c003h),a      ; 01ae ea 03 c0   j.@
        ld      a,c             ; 01b1 79   y
        sub     b               ; 01b2 90   .
        ld      (0c001h),a      ; 01b3 ea 01 c0   j.@
        call    l01e3           ; 01b6 cd e3 01   Mc.
        pop     bc              ; 01b9 c1   A
        dec     b               ; 01ba 05   .
        jr      nz,l01ab        ; 01bb 20 ee    n
        ld      a,0             ; 01bd 3e 00   >.
        ld      (0c001h),a      ; 01bf ea 01 c0   j.@
        ld      a,77h           ; 01c2 3e 77   >w
        ld      (0c003h),a      ; 01c4 ea 03 c0   j.@
        ld      a,3             ; 01c7 3e 03   >.
        ld      (0c006h),a      ; 01c9 ea 06 c0   j.@
        ld      b,80h           ; 01cc 06 80   ..
        ld      c,7fh           ; 01ce 0e 7f   ..
l01d0:  push    bc              ; 01d0 c5   E
        dec     b               ; 01d1 05   .
        ld      a,b             ; 01d2 78   x
        ld      (0c000h),a      ; 01d3 ea 00 c0   j.@
        ld      (0c002h),a      ; 01d6 ea 02 c0   j.@
        call    l01e3           ; 01d9 cd e3 01   Mc.
        pop     bc              ; 01dc c1   A
        dec     b               ; 01dd 05   .
        jr      nz,l01d0        ; 01de 20 f0    p
        jp      l017d           ; 01e0 c3 7d 01   C}.

l01e3:  xor     a               ; 01e3 af   /
        ld      (0c00bh),a      ; 01e4 ea 0b c0   j.@
        ld      (0c00ch),a      ; 01e7 ea 0c c0   j.@
        ld      a,(0c002h)      ; 01ea fa 02 c0   z.@
        ld      b,a             ; 01ed 47   G
        ld      a,(0c000h)      ; 01ee fa 00 c0   z.@
        ld      (0c004h),a      ; 01f1 ea 04 c0   j.@
        sub     b               ; 01f4 90   .
        ld      b,0             ; 01f5 06 00   ..
        or      a               ; 01f7 b7   7
        jr      z,l0204         ; 01f8 28 0a   (.
        ld      b,0ffh          ; 01fa 06 ff   ..
        bit     7,a             ; 01fc cb 7f   K.
        jr      z,l0204         ; 01fe 28 04   (.
        cpl                     ; 0200 2f   /
        inc     a               ; 0201 3c   <
        ld      b,1             ; 0202 06 01   ..
l0204:  ld      d,a             ; 0204 57   W
        sla     a               ; 0205 cb 27   K'
        ld      (0c008h),a      ; 0207 ea 08 c0   j.@
        ld      a,b             ; 020a 78   x
        ld      (0c009h),a      ; 020b ea 09 c0   j.@
        ld      a,(0c003h)      ; 020e fa 03 c0   z.@
        ld      b,a             ; 0211 47   G
        ld      a,(0c001h)      ; 0212 fa 01 c0   z.@
        ld      (0c005h),a      ; 0215 ea 05 c0   j.@
        sub     b               ; 0218 90   .
        ld      b,0             ; 0219 06 00   ..
        jr      z,l0227         ; 021b 28 0a   (.
        ld      b,0ffh          ; 021d 06 ff   ..
        bit     7,a             ; 021f cb 7f   K.
        jr      z,l0227         ; 0221 28 04   (.
        cpl                     ; 0223 2f   /
        inc     a               ; 0224 3c   <
        ld      b,1             ; 0225 06 01   ..
l0227:  ld      e,a             ; 0227 5f   _
        sla     a               ; 0228 cb 27   K'
        ld      (0c007h),a      ; 022a ea 07 c0   j.@
        ld      a,b             ; 022d 78   x
        ld      (0c00ah),a      ; 022e ea 0a c0   j.@
        ld      a,d             ; 0231 7a   z
        cp      e               ; 0232 bb   ;
        jr      c,l0293         ; 0233 38 5e   8^
l0235:  ld      a,(0c004h)      ; 0235 fa 04 c0   z.@
        ld      b,a             ; 0238 47   G
        ld      a,(0c005h)      ; 0239 fa 05 c0   z.@
        ld      c,a             ; 023c 4f   O
        call    l02f1           ; 023d cd f1 02   Mq.
        ld      a,(0c004h)      ; 0240 fa 04 c0   z.@
        ld      b,a             ; 0243 47   G
        ld      a,(0c002h)      ; 0244 fa 02 c0   z.@
        cp      b               ; 0247 b8   8
        jr      nz,l0255        ; 0248 20 0b    .
        ld      a,(0c005h)      ; 024a fa 05 c0   z.@
        ld      b,a             ; 024d 47   G
        ld      a,(0c003h)      ; 024e fa 03 c0   z.@
        cp      b               ; 0251 b8   8
        jr      nz,l0255        ; 0252 20 01    .
        ret                     ; 0254 c9   I

l0255:  ld      a,(0c00bh)      ; 0255 fa 0b c0   z.@
        ld      e,a             ; 0258 5f   _
        ld      a,(0c00ch)      ; 0259 fa 0c c0   z.@
        ld      d,a             ; 025c 57   W
        bit     7,d             ; 025d cb 7a   Kz
        jr      nz,l0276        ; 025f 20 15    .
        ld      a,(0c00ah)      ; 0261 fa 0a c0   z.@
        ld      b,a             ; 0264 47   G
        ld      a,(0c005h)      ; 0265 fa 05 c0   z.@
        add     a,b             ; 0268 80   .
        ld      (0c005h),a      ; 0269 ea 05 c0   j.@
        ld      a,(0c008h)      ; 026c fa 08 c0   z.@
        ld      b,a             ; 026f 47   G
        ld      a,e             ; 0270 7b   {
        sub     b               ; 0271 90   .
        ld      e,a             ; 0272 5f   _
        jr      nc,l0276        ; 0273 30 01   0.
        dec     d               ; 0275 15   .
l0276:  ld      a,(0c009h)      ; 0276 fa 09 c0   z.@
        ld      b,a             ; 0279 47   G
        ld      a,(0c004h)      ; 027a fa 04 c0   z.@
        add     a,b             ; 027d 80   .
        ld      (0c004h),a      ; 027e ea 04 c0   j.@
        ld      a,(0c007h)      ; 0281 fa 07 c0   z.@
        ld      b,a             ; 0284 47   G
        ld      a,e             ; 0285 7b   {
        add     a,b             ; 0286 80   .
        ld      (0c00bh),a      ; 0287 ea 0b c0   j.@
        jr      nc,l028d        ; 028a 30 01   0.
        inc     d               ; 028c 14   .
l028d:  ld      a,d             ; 028d 7a   z
        ld      (0c00ch),a      ; 028e ea 0c c0   j.@
        jr      l0235           ; 0291 18 a2   ."

l0293:  ld      a,(0c004h)      ; 0293 fa 04 c0   z.@
        ld      b,a             ; 0296 47   G
        ld      a,(0c005h)      ; 0297 fa 05 c0   z.@
        ld      c,a             ; 029a 4f   O
        call    l02f1           ; 029b cd f1 02   Mq.
        ld      a,(0c004h)      ; 029e fa 04 c0   z.@
        ld      b,a             ; 02a1 47   G
        ld      a,(0c002h)      ; 02a2 fa 02 c0   z.@
        cp      b               ; 02a5 b8   8
        jr      nz,l02b3        ; 02a6 20 0b    .
        ld      a,(0c005h)      ; 02a8 fa 05 c0   z.@
        ld      b,a             ; 02ab 47   G
        ld      a,(0c003h)      ; 02ac fa 03 c0   z.@
        cp      b               ; 02af b8   8
        jr      nz,l02b3        ; 02b0 20 01    .
        ret                     ; 02b2 c9   I

l02b3:  ld      a,(0c00bh)      ; 02b3 fa 0b c0   z.@
        ld      e,a             ; 02b6 5f   _
        ld      a,(0c00ch)      ; 02b7 fa 0c c0   z.@
        ld      d,a             ; 02ba 57   W
        bit     7,d             ; 02bb cb 7a   Kz
        jr      nz,l02d4        ; 02bd 20 15    .
        ld      a,(0c009h)      ; 02bf fa 09 c0   z.@
        ld      b,a             ; 02c2 47   G
        ld      a,(0c004h)      ; 02c3 fa 04 c0   z.@
        add     a,b             ; 02c6 80   .
        ld      (0c004h),a      ; 02c7 ea 04 c0   j.@
        ld      a,(0c007h)      ; 02ca fa 07 c0   z.@
        ld      b,a             ; 02cd 47   G
        ld      a,e             ; 02ce 7b   {
        sub     b               ; 02cf 90   .
        ld      e,a             ; 02d0 5f   _
        jr      nc,l02d4        ; 02d1 30 01   0.
        dec     d               ; 02d3 15   .
l02d4:  ld      a,(0c00ah)      ; 02d4 fa 0a c0   z.@
        ld      b,a             ; 02d7 47   G
        ld      a,(0c005h)      ; 02d8 fa 05 c0   z.@
        add     a,b             ; 02db 80   .
        ld      (0c005h),a      ; 02dc ea 05 c0   j.@
        ld      a,(0c008h)      ; 02df fa 08 c0   z.@
        ld      b,a             ; 02e2 47   G
        ld      a,e             ; 02e3 7b   {
        add     a,b             ; 02e4 80   .
        ld      (0c00bh),a      ; 02e5 ea 0b c0   j.@
        jr      nc,l02eb        ; 02e8 30 01   0.
        inc     d               ; 02ea 14   .
l02eb:  ld      a,d             ; 02eb 7a   z
        ld      (0c00ch),a      ; 02ec ea 0c c0   j.@
        jr      l0293           ; 02ef 18 a2   ."

l02f1:  ld      a,c             ; 02f1 79   y
        rlc     a               ; 02f2 cb 07   K.
        ld      e,a             ; 02f4 5f   _
        ld      a,b             ; 02f5 78   x
        srl     a               ; 02f6 cb 3f   K?
        srl     a               ; 02f8 cb 3f   K?
        srl     a               ; 02fa cb 3f   K?
        add     a,80h           ; 02fc c6 80   F.
        ld      d,a             ; 02fe 57   W
        ld      a,b             ; 02ff 78   x
        and     7               ; 0300 e6 07   f.
        ld      l,a             ; 0302 6f   o
        ld      h,0             ; 0303 26 00   &.
        ld      a,(hl)          ; 0305 7e   .
        ld      b,a             ; 0306 47   G
        ld      hl,0c006h       ; 0307 21 06 c0   !.@
        bit     0,(hl)          ; 030a cb 46   KF
        jr      z,l0319         ; 030c 28 0b   (.
l030e:  ldh     a,(41h)         ; 030e f0 41   pA
        bit     1,a             ; 0310 cb 4f   KO
        jp      nz,l030e        ; 0312 c2 0e 03   B..
        ld      a,(de)          ; 0315 1a   .
        xor     b               ; 0316 a8   (
        ld      (de),a          ; 0317 12   .
        inc     de              ; 0318 13   .
l0319:  bit     1,(hl)          ; 0319 cb 4e   KN
        jr      z,l0327         ; 031b 28 0a   (.
l031d:  ldh     a,(41h)         ; 031d f0 41   pA
        bit     1,a             ; 031f cb 4f   KO
        jp      nz,l031d        ; 0321 c2 1d 03   B..
        ld      a,(de)          ; 0324 1a   .
        xor     b               ; 0325 a8   (
        ld      (de),a          ; 0326 12   .
l0327:  ret                     ; 0327 c9   I

l0328:  ldh     a,(40h)         ; 0328 f0 40   p@
        add     a,a             ; 032a 87   .
        ret     nc              ; 032b d0   P
l032c:  ldh     a,(44h)         ; 032c f0 44   pD
        cp      90h             ; 032e fe 90   ..
        jr      nz,l032c        ; 0330 20 fa    z
        ret                     ; 0332 c9   I

        xor     a               ; 0333 af   /
        ldh     (47h),a         ; 0334 e0 47   `G
        ret                     ; 0336 c9   I

        ld      a,0ffh          ; 0337 3e ff   >.
        ldh     (47h),a         ; 0339 e0 47   `G
        ret                     ; 033b c9   I

l033c:  ld      a,0e4h          ; 033c 3e e4   >d
        ldh     (47h),a         ; 033e e0 47   `G
        ret                     ; 0340 c9   I

l0341:  ld      hl,8000h        ; 0341 21 00 80   !..
        ld      d,0             ; 0344 16 00   ..
        ld      e,8             ; 0346 1e 08   ..
l0348:  xor     a               ; 0348 af   /
        ldi     (hl),a          ; 0349 22   "
        dec     d               ; 034a 15   .
        jp      nz,l0348        ; 034b c2 48 03   BH.
        dec     e               ; 034e 1d   .
        jp      nz,l0348        ; 034f c2 48 03   BH.
        ret                     ; 0352 c9   I

l0353:  ld      hl,9800h        ; 0353 21 00 98   !..
        ld      e,4             ; 0356 1e 04   ..
l0358:  ld      d,0             ; 0358 16 00   ..
l035a:  ld      a,0ffh          ; 035a 3e ff   >.
        ldi     (hl),a          ; 035c 22   "
l035d:  ldh     a,(41h)         ; 035d f0 41   pA
        and     2               ; 035f e6 02   f.
        jp      nz,l035d        ; 0361 c2 5d 03   B].
        dec     d               ; 0364 15   .
        jp      nz,l035a        ; 0365 c2 5a 03   BZ.
        dec     e               ; 0368 1d   .
        jp      nz,l0358        ; 0369 c2 58 03   BX.
        ld      b,0             ; 036c 06 00   ..
        ld      hl,9822h        ; 036e 21 22 98   !".
        ld      e,10h           ; 0371 1e 10   ..
l0373:  ld      d,10h           ; 0373 16 10   ..
l0375:  ld      a,b             ; 0375 78   x
        swap    a               ; 0376 cb 37   K7
        ldi     (hl),a          ; 0378 22   "
l0379:  ldh     a,(41h)         ; 0379 f0 41   pA
        and     2               ; 037b e6 02   f.
        jp      nz,l0379        ; 037d c2 79 03   By.
        inc     b               ; 0380 04   .
        dec     d               ; 0381 15   .
        jp      nz,l0375        ; 0382 c2 75 03   Bu.
        push    de              ; 0385 d5   U
        ld      d,0             ; 0386 16 00   ..
        ld      e,10h           ; 0388 1e 10   ..
        add     hl,de           ; 038a 19   .
        pop     de              ; 038b d1   Q
        dec     e               ; 038c 1d   .
        jp      nz,l0373        ; 038d c2 73 03   Bs.
        ret                     ; 0390 c9   I
 .end
