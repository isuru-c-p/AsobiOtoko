;***************************************************************
;*                                                             *
;*                            Zoomar                           *
;*                                                             *
;*                     Gameboy Demo Program                    *
;*                       by Harry Mulder                       *
;*                     Version 3, 09-20-1997                   *
;*                                                             *
;* This source is placed in public domain. To me, that means   *
;* that you can do anything you want with it, as long as you   *
;* don't sell it or hold me responsible for any consequences.  *
;*                                                             *
;* As I have used source from others, you are allowed to use   *
;* my source.                                                  *
;*                                                             *
;* General info:                                               *
;*                                                             *
;* The purpose of this project was to fool around with the     *
;* gameboy and see how it would survive in a worst-scenario:   *
;* Using calculation-based changing graphics on a slow cpu     *
;* with hardware designed to swift graphics. ie: the gameboy   *
;* was designed for platform-games, not for stuff like this.   *
;*                                                             *
;* Personally, I think the gameboy handles the load quite      *
;* acceptable, although it should not be pushed much further:  *
;* tests with bigger pictures gave unacceptable results.       *
;*                                                             *
;* The pictures are 88x88 big: this gives good speed and       *
;* needs 121 tiles, making it possible to have two pictures    *
;* in mem (although the gameboy has 256 + 256 tiles available, *
;* 128 of these overlap, and are therefore useless). As we     *
;* have two pictures, page-flipping is possible and            *
;* incorporated.                                               *
;*                                                             *
;* The screens themselves are constant; each screen has a set  *
;* pattern of tiles. The only thing changing are the tile-     *
;* tables themselves.                                          *
;*                                                             *
;* Speed has the highest priority; there is serious loop-      *
;* unrolling in the zoom-function and the 4 pictures are       *
;* stored on a 1-byte-per-pixel basis, which is 4 times        *
;* bigger than actually needed. However, this gave the         *
;* possibility of enormous speed-increases, as the zoom-       *
;* function does not have to do a lot of shifting and anding   *
;* just to get the right pixel.                                *
;*                                                             *
;* Although the pictures are stored this way, they are shipped *
;* out in packed form (4 pixels per byte). Before running, the *
;* pictures are unpacked to ram. This accounts for the strange *
;* cartridge-setup: 32K rom + 32K ram. This truly is a weird   *
;* configuration, until you look at this demo as an intro to   *
;* an imaginary game: shipment will have to be compact, but    *
;* the ram should be there for the game itself. As it doesn't  *
;* use it while showing the demo, this arrangement might not   *
;* be so bad afterall.                                         *
;*                                                             *
;* Although the gameboy supports 32K ram, cartridges with 32K  *
;* seem to be rare or totally non-existing. To tackle this     *
;* problem, this source can be build in two ways by defining   *
;* or not defining SMALL:                                      *
;*                                                             *
;* SMALL not defined (default)                                 *
;*   you get an image which requires 32K ram, and whichs shows *
;*   the animated picture.                                     *
;*                                                             *
;* SMALL defined                                               *
;*   you get an image which requires only 8K ram, but has a    *
;*   still picture.                                            *
;*                                                             *
;* If anyone can give extra info on this 32K cartridge         *
;* situation, I would be delighted to hear from you.           *
;*                                                             *
;* The title is simply build up from sprites which remain      *
;* static.                                                     *
;*                                                             *
;* Keep in mind that more info can be found in the source      *
;* itself.                                                     *
;*                                                             *
;* Compile it with TASM, Using table 'tasm69.tab'.             *
;*                                                             *
;* If you want to contact me for some reason, I can be reached *
;* at hpmulder@pi.net                                          *
;*                                                             *
;* Enjoy!                                                      *
;*                                                             *
;*                                                             *
;* History:                                                    *
;*                                                             *
;* Version 1 - 02-09-97 - initial release                      *
;*                                                             *
;* Version 2 - 02-10-97 - Added some tips from Jeff Frohwein   *
;*                        for better support of the real McCoy *
;*                                                             *
;*                        Added SMALL option                   *
;*                                                             *
;* Version 3 - 09-20-97 - Real gameboy fully supported by:     *
;*                        o Cleaning up memory before usage    *
;*                        o Pumping sprites with function in   *
;*                          FF80h-region                       *
;*                        o Reseting palette-values            *
;*                        o Some extra WaitForVRAM's           *
;*                        o Using RGBFIX instead of CRC        *
;*                                                             *
;***************************************************************





;***********************************
;*              CONSTANTS          *
;***********************************


WhiteChar       .EQU 127
PictLoc         .EQU $A000

TitleX          .EQU 56
TitleY          .EQU 30

#DEFINE WaitForVRAM  ldh a,(41h) \ and 2 \ jr nz,$-4



;***********************************
;*               VARS              *
;***********************************


MEMVAR          .EQU $CC00              ; first ram-location for vars

StepSize        .EQU MEMVAR             ;w: fixed-point
ZoomSize        .EQU MEMVAR+2           ;b

YLoc            .EQU MEMVAR+3           ;w
XFixed          .EQU MEMVAR+5           ;w
YFixed          .EQU MEMVAR+7           ;w: fixed point
YHFixed         .EQU MEMVAR+9           ;b
YCnt            .EQU MEMVAR+10          ;b

BlockCnt        .EQU MEMVAR+11          ;b
SpareCnt        .EQU MEMVAR+12          ;b

temp            .EQU MEMVAR+13          ;w 
count           .EQU MEMVAR+15          ;w

Frame           .EQU MEMVAR+17          ;b
FrameDir        .EQU MEMVAR+18          ;b


SpriteScratch   .EQU $CD00




;**********************************
;*              HEADER            *
;**********************************



        .org	0

        .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
        .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
        .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
        .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
        .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
        .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
        .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
        .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
        .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
        .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
        .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
        .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
        .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
        .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
        .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
        .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0


	.byte	00
	.byte	0c3h
	.word	begin		;starting address

;Nintendo Scrolling Title Graphic

	.byte	0ceh,0edh,66h,66h,0cch,13,0,11,3,73h
	.byte	0,83h,0,12,0,13,0,8,17,31,88h,89h,0
	.byte	14,0dch,0cch,6eh,0e6h,0ddh,0ddh,0d9h
	.byte	99h,0bbh,0bbh,67h,63h,6eh,0eh,0ech,0cch
	.byte	0ddh,0dch,99h,09fh,0bbh,0b9h,33h,3eh

;Title of the game

#IFNDEF SMALL
	.byte	"ZOOMAR V3",0,0,0,0,0,0,0
#ELSE
	.byte	"ZOOMAR V3(S)",0,0,0,0
#ENDIF

;Not used

	.byte	0,0,0

;Cartridge type: Rom only
; 0 - Rom only      3 - ROM+MBC1+RAM+Battery
; 1 - ROM+MBC1      5 - ROM+MBC2
; 2 - ROM+MBC1+RAM  6 - ROM+MBC2+Battery

	.byte	3 ; - ROM+MBC1+RAM+Battery      

;Rom Size:
; 0 - 256kBit =  32kB =  2 banks
; 1 - 512kBit =  64kB =  4 banks
; 2 -   1MBit = 128kB =  8 banks
; 3 -   2MBit = 256kB = 16 banks
; 4 -   4MBit = 512kB = 32 banks

	.byte	0 ; 0 - 256kBit =  32kB =  2 banks

;Ram Size:
; 0 - None
; 1 -  16kBit =  2kB = 1 bank
; 2 -  64kBit =  8kB = 1 bank
; 3 - 256kBit = 32kB = 4 banks

#IFNDEF SMALL
  	  .byte   3 ; 3 - 256kBit = 32kB = 4 banks
#ELSE
  	  .byte   2 ; 2 -  64kBit =  8kB = 1 bank
#ENDIF

;Manufacturer code:

	.byte	00,1

;Version Number

	.byte	1

;Complement check
; (Let RGBFIX.EXE fill this out.)

	.byte	0ah

;Checksum
; (Let RGBFIX.EXE fill this out.)

	.word	0



;**********************************
;*              SOURCE            *
;**********************************



begin:


  ;* Init *

  di                    ; deactivate interrupts

  LD HL,$0000
  LD A,$0a
  LD (HL),A             ; Activate ram-bank


  ld sp,$fff4	        ; Put the stack where the GB wants it


  ld a, %10010111
  ldh ($40),a           ; setup display register

  ld a, %11100100
  ldh ($47),a           ; setup BG palette
  ldh ($48),a           ; setup OBJ0 object palette
  ldh ($49),a           ; setup OBJ1 object palette



  ld a,0
  ld (Frame),a          ; Reset Frame
  ld a,1
  ld (FrameDir),a       ; Reset FrameDir




  ;* Fill up ram-banks *


  ld hl, picture1
  ld b,0
  call FillRamBank


#IFNDEF SMALL
 
    ld hl, picture2
    ld b,1
    call FillRamBank

    ld hl, picture3
    ld b,2
    call FillRamBank

    ld hl, picture4
    ld b,3
    call FillRamBank

#ENDIF



  ;* Init screen *



  call ClearVideo

  ld hl,$9800+ (((16*96)/4)+(96/16*2)) + (16*4) + 4
  call BuildGrid

  ld hl,$9C00+ (((16*96)/4)+(96/16*2)) + (16*4) + 4
  call BuildGrid



  ;* Place Title *

  call LoadSpritePatterns
  call BuildTitle


  ;* Main Loop *

beginmain:


  ld c,28

mainl:

  ;* Zooming in.. *

  push bc


#IFNDEF SMALL
    call SetFrame               ; select next frame
#ENDIF

  call DrawZoomedPicture        ; Draw image


  pop bc
  inc c
  inc c
  ld a,c
  cp 90
  jp nz, mainl

  ld c,86



mainr:

  ;* Zooming out.. *

  push bc

#IFNDEF SMALL
    call SetFrame
#ENDIF

  call DrawZoomedPicture


  pop bc
  dec c
  dec c
  ld a,c
  cp 28
  jp nz, mainr


  ;* back to zooming in *

  jp beginmain







;**********************************
;*            FUNCTIONS           *
;**********************************






LoadSpritePatterns:
;***************************************************
;* This function loads the tiles needed for        *
;* the title. The title is build out of sprites    *
;* in 8x16 mode, and uses 8 x 2 sprites.           *
;*                                                 *
;* As only the first three y-tiles are used        *
;* (16 * 2 = 32 = 4 y-tile space), the tiles are   *
;* loaded in two loads, were the second load skips *
;* a tile for each loaded tile.                    *
;***************************************************


  ;* load top tiles of title *

  ld hl,$8000 + (128*16)
  ld de,TitleTop
  ld c,00               ;256 bytes 

p1:
    WaitForVRAM
    ld a,(de)
    ld (hli),a
    inc de
    dec c
    jr nz, p1


  ;* load bottom tiles of title *

  ld de,TitleBottom
  ld c,8

p2:
    ld b,8
p3:
      WaitForVRAM
      ld a,(de)
      ld (hli),a
      inc de
      dec b
      jr nz, p3

    push bc
    ld bc,24
    add hl,bc
    pop bc

  dec c
  jr nz, p2

  ret
  






BuildTitle:
;**********************************************
;* This function installs the sprites which   *
;* show the title.                            *
;* This is done in two loads for convienence. *
;**********************************************


  ;* clean up scratch-pad (to suppress ghost-sprites) *

  ld hl,SpriteScratch
  ld c,40 * 4
  ld a,0
t4:
    ld (hli),a
    dec c
  jr nz, t4

  
  ;* setup top sprites *

  ld b,TitleX
  ld d,128
  ld hl, SpriteScratch
  ld c,8


t1:
    ld a,TitleY
    ld (hli),a
    ld a,b
    ld (hli),a
    ld a,d
    ld (hli),a
    ld a,$00
    ld (hli),a

    ld a,b
    add a,8
    ld b,a              ; b = next position on screen

    inc d
    inc d               ; d = next tile

    dec c
  jr nz, t1


  ;* setup bottom sprites *

  ld b,TitleX
  ld c,8

t2:
    ld a,TitleY+16
    ld (hli),a
    ld a,b
    ld (hli),a
    ld a,d
    ld (hli),a
    ld a,$00
    ld (hli),a

    ld a,b
    add a,8
    ld b,a              ; b = next position on screen

    inc d
    inc d               ; d = next tile

    dec c
  jr nz, t2


  ;* Copy OAM-handler to upper RAM *

  ld c,SpriteCopyEnd-SpriteCopy
  ld hl,$FF80
  ld de,SpriteCopy
t3:
  ld a,(de)
  inc de
  ld (hli),a
  dec c
  jr nz, t3

  ;* call it *

  call $FF80

 ret


SpriteCopy:
;*******************************
;* pump sprites to OAM-ram     *
;* copy this function to FF80h *
;* before usage !              *
;*******************************
  WaitForVRAM
  ld a, SpriteScratch >> 8
  ld ($ff46),a   
  ld a,28h       
Wait:                
    dec a          
  jr nz,Wait     
  ret
SpriteCopyEnd:





SetFrame:
;*****************************************
;* Sets up the correct picture to be     *
;* zoomed (ie. selects the correct       *
;* ram-bank).                            *
;*                                       *
;* Selection is done based on Frame      *
;* and FrameDir: Frame is the current    *
;* picture, FrameDir tells if the next   *
;* or previous frame should be selected. *
;*****************************************

  ld a,(FrameDir)
  ld b,a
  ld a,(Frame)
  add a,b

  jr z, f1              ; beginning of Frame reached ?

  cp 3
  jr nz, f2             ; end of Frame reached ?


f1:
  ld l,a
  ld a,b
  xor $FF
  inc a                 ; NEG FrameDir
  ld (FrameDir),a
  ld a,l


f2:
    ld (Frame),a
    ld hl,$4000
    ld (hl),a           ; Select ram-bank

 ret






ClearVideo:
;********************************
;* Fills all video-mem with 0,  *
;* and tile-mem with WhiteChar. *
;********************************

  ld c,31
  ld hl,$8000

w1:
  push bc
  ld c,$ff
w2:
    WaitForVRAM
    ld a,0
    ld (hl),a
    inc hl
    dec c
    jr nz, w2

  pop bc
  dec c
  jr nz, w1


  ld c,8
  ld hl,$9800

w3:
  push bc
  ld c,$ff
w4:
    WaitForVRAM
    ld a,WhiteChar
    ld (hli),a
    dec c
    jr nz, w4

  pop bc
  dec c
  jr nz, w3

 ret







DrawZoomedPicture:
;************************************************
;* Main function of this project; this does     *
;* the zooming. Be prepared for mayor trouble   *
;* when analyzing this beauty:                  *
;*  forget readability, welcome to optimization *
;*                                              *
;* in:                                          *
;*  c = ZoomSize                                *
;************************************************



  ;* reset vars *

  ld a,0

  ld (YCnt),a
  ld (XFixed),a
  ld (XFixed+1),a

  ld (YFixed),a 
  ld (YFixed+1),a 
  ld (YHFixed),a


  ;* calc StepSize *

  ld a,c
  ld (ZoomSize),a

  ld b,0
  ld c,a
  ld de,88<<8
  
  call Div

  ld a,e
  ld (StepSize),a
  ld a,d
  ld (StepSize+1),a



  ;* calc block-count *

  ld a,(ZoomSize)
  and $07               ; first 3 bytes
  ld (SpareCnt),a

  scf
  ccf                   ; clear CF
  ld a,(ZoomSize)
  rr a
  rr a
  rr a
  and $1F
  ld (BlockCnt),a       ; a = 8p tile-block counter



  ;* Reset YLoc-counter *

  ld a, PictLoc & $FF
  ld (YLoc), a
  ld a, PictLoc >> 8
  ld (YLoc+1), a



  ;* determine result-loc *

  ld a,($FF40)
  bit 4,a               ; which tile-table to use
  ld de,$8000
  jr z, l1
  ld de,$9000
l1:



  ;* Y-Lines *

  ld a,(ZoomSize)
  cp 77                 ; lines below 78 are not drawn
                        ; (88x88 pic-size => 88x78 effective)
  jr c, k2
    ld a,77
k2:
  ld c,a


ZoomY:

    push bc
     
    push de


    ;* x-lines *

    ld a,(BlockCnt)
    cp 0
    jp z, NoBlocks

    ld c,a              ; c = 8p tile-block counter
ZoomXb:

      ;* 8p tile block *
      push bc

      push de
      ld de,0000        ; d = p0
                        ; e = p1



      ; these values are stored on the stack
      ; to enhance performance
      ; (check out pop bc/pop hl and push hl/push bc
      ;  combinations)


      ld a,(XFixed)     
      ld l,a
      ld a,(XFixed+1)
      ld h,a

      ld a,(StepSize)
      ld c,a
      ld a,(StepSize+1)
      ld b,a


      ;* Loop-unrolling galore ! *



      ;* pixel 0 *


      add hl,bc         ; h = new x-offset (Fixed)

      push hl           ; for next pixel
      push bc

      ld l,h
      ld h,0            ; hl = new x-offset 

      ld a,(YLoc)
      ld c,a
      ld a,(YLoc+1)
      ld b,a

      add hl,bc         ; hl = new xy-offset

      ld a,(hl)         ; get byte
      rra
      rl e
      rra
      rl d



      ;* pixel 1 *

      pop bc
      pop hl
      add hl,bc         ; h = new x-offset (Fixed)

      push hl           ; for next pixel
      push bc


      ld l,h
      ld h,0            ; hl = new x-offset 

      ld a,(YLoc)
      ld c,a
      ld a,(YLoc+1)
      ld b,a

      add hl,bc         ; hl = new xy-offset

      ld a,(hl)         ; get byte
      rra
      rl e
      rra
      rl d



      ;* pixel 2 *

      pop bc
      pop hl
      add hl,bc         ; h = new x-offset (Fixed)

      push hl           ; for next pixel
      push bc


      ld l,h
      ld h,0            ; hl = new x-offset 

      ld a,(YLoc)
      ld c,a
      ld a,(YLoc+1)
      ld b,a

      add hl,bc         ; hl = new xy-offset

      ld a,(hl)         ; get byte
      rra
      rl e
      rra
      rl d



      ;* pixel 3 *

      pop bc
      pop hl
      add hl,bc         ; h = new x-offset (Fixed)

      push hl           ; for next pixel
      push bc



      ld l,h
      ld h,0            ; hl = new x-offset 

      ld a,(YLoc)
      ld c,a
      ld a,(YLoc+1)
      ld b,a

      add hl,bc         ; hl = new xy-offset

      ld a,(hl)         ; get byte
      rra
      rl e
      rra
      rl d
      and b




      ;* pixel 4 *

      pop bc
      pop hl
      ld a,(StepSize)
      ld c,a
      ld a,(StepSize+1)
      ld b,a

      add hl,bc         ; h = new x-offset (Fixed)

      push hl           ; for next pixel
      push bc


      ld l,h
      ld h,0            ; hl = new x-offset 

      ld a,(YLoc)
      ld c,a
      ld a,(YLoc+1)
      ld b,a

      add hl,bc         ; hl = new xy-offset

      ld a,(hl)         ; get byte
      rra
      rl e
      rra
      rl d
      and b




      ;* pixel 5 *

      pop bc
      pop hl
      add hl,bc         ; h = new x-offset (Fixed)

      push hl           ; for next pixel
      push bc


      ld l,h
      ld h,0            ; hl = new x-offset 

      ld a,(YLoc)
      ld c,a
      ld a,(YLoc+1)
      ld b,a

      add hl,bc         ; hl = new xy-offset

      ld a,(hl)         ; get byte
      rra
      rl e
      rra
      rl d
      and b





      ;* pixel 6 *

      pop bc
      pop hl
      add hl,bc         ; h = new x-offset (Fixed)

      push hl           ; for next pixel
      push bc

      ld l,h
      ld h,0            ; hl = new x-offset 

      ld a,(YLoc)
      ld c,a
      ld a,(YLoc+1)
      ld b,a

      add hl,bc         ; hl = new xy-offset

      ld a,(hl)         ; get byte
      rra
      rl e
      rra
      rl d





      ;* pixel 7 *

      pop bc
      pop hl
      add hl,bc         ; h = new x-offset (Fixed)

      ld a,l
      ld (XFixed),a
      ld a,h
      ld (XFixed+1),a

      ld l,h
      ld h,0            ; hl = new x-offset 

      ld a,(YLoc)
      ld c,a
      ld a,(YLoc+1)
      ld b,a

      add hl,bc         ; hl = new xy-offset

      ld a,(hl)         ; get byte
      rra
      rl e
      rra
      rl d


    ;* store result *

    pop hl              ; hl = de = Tile-loc
    WaitForVRAM
    ld a,e
    ld (hli),a
    ld a,d
    ld (hl),a           ; hl = Tile-loc



    ;* goto next tile *

    ld bc,15
    add hl,bc
    ld d,h
    ld e,l              ; de = new Tile-loc


    pop bc
    dec c
    jp nz, ZoomXb


NoBlocks:

  ld a,(SpareCnt)
  cp 0
  jp z, NoSpare

      push de
      ld de,0000        ; d = p0
                        ; e = p1



      ; these values are stored on the stack
      ; to enhance performance
      ; (check out pop bc/pop hl and push hl/push bc
      ;  combinations)


      ld a,(XFixed)     
      ld l,a
      ld a,(XFixed+1)
      ld h,a

      ld a,(StepSize)
      ld c,a
      ld a,(StepSize+1)
      ld b,a


      ;* Loop-unrolling galore ! *


      ;* pixel 0 *


      add hl,bc         ; h = new x-offset (Fixed)
      push hl           ; for next pixel
      push bc

      ld a,h
      cp 89
      ld a,0
      jr nc, e1

      ld l,h
      ld h,0            ; hl = new x-offset 

      ld a,(YLoc)
      ld c,a
      ld a,(YLoc+1)
      ld b,a

      add hl,bc         ; hl = new xy-offset

      ld a,(hl)         ; get byte
e1:
      rra
      rl e
      rra
      rl d



      ;* pixel 1 *

      pop bc
      pop hl
      add hl,bc         ; h = new x-offset (Fixed)

      push hl           ; for next pixel
      push bc

      ld a,h
      cp 89
      ld a,0
      jr nc, e2

      ld l,h
      ld h,0            ; hl = new x-offset 

      ld a,(YLoc)
      ld c,a
      ld a,(YLoc+1)
      ld b,a

      add hl,bc         ; hl = new xy-offset

      ld a,(hl)         ; get byte
e2:
      rra
      rl e
      rra
      rl d



      ;* pixel 2 *

      pop bc
      pop hl
      add hl,bc         ; h = new x-offset (Fixed)

      push hl           ; for next pixel
      push bc

      ld a,h
      cp 89
      ld a,0
      jr nc, e3

      ld l,h
      ld h,0            ; hl = new x-offset 

      ld a,(YLoc)
      ld c,a
      ld a,(YLoc+1)
      ld b,a

      add hl,bc         ; hl = new xy-offset

      ld a,(hl)         ; get byte
e3:
      rra
      rl e
      rra
      rl d



      ;* pixel 3 *

      pop bc
      pop hl
      add hl,bc         ; h = new x-offset (Fixed)

      push hl           ; for next pixel
      push bc

      ld a,h
      cp 89
      ld a,0
      jr nc, e4


      ld l,h
      ld h,0            ; hl = new x-offset 

      ld a,(YLoc)
      ld c,a
      ld a,(YLoc+1)
      ld b,a

      add hl,bc         ; hl = new xy-offset

      ld a,(hl)         ; get byte
e4:
      rra
      rl e
      rra
      rl d
      and b




      ;* pixel 4 *

      pop bc
      pop hl
      ld a,(StepSize)
      ld c,a
      ld a,(StepSize+1)
      ld b,a

      add hl,bc         ; h = new x-offset (Fixed)

      push hl           ; for next pixel
      push bc

      ld a,h
      cp 89
      ld a,0
      jr nc, e5

      ld l,h
      ld h,0            ; hl = new x-offset 

      ld a,(YLoc)
      ld c,a
      ld a,(YLoc+1)
      ld b,a

      add hl,bc         ; hl = new xy-offset

      ld a,(hl)         ; get byte
e5: 
      rra
      rl e
      rra
      rl d
      and b




      ;* pixel 5 *

      pop bc
      pop hl
      add hl,bc         ; h = new x-offset (Fixed)

      push hl           ; for next pixel
      push bc

      ld a,h
      cp 89
      ld a,0
      jr nc, e6

      ld l,h
      ld h,0            ; hl = new x-offset 

      ld a,(YLoc)
      ld c,a
      ld a,(YLoc+1)
      ld b,a

      add hl,bc         ; hl = new xy-offset

      ld a,(hl)         ; get byte
e6: 
      rra
      rl e
      rra
      rl d
      and b





      ;* pixel 6 *

      pop bc
      pop hl
      add hl,bc         ; h = new x-offset (Fixed)

      push hl           ; for next pixel
      push bc

      ld a,h
      cp 89
      ld a,0
      jr nc, e7

      ld l,h
      ld h,0            ; hl = new x-offset 

      ld a,(YLoc)
      ld c,a
      ld a,(YLoc+1)
      ld b,a

      add hl,bc         ; hl = new xy-offset

      ld a,(hl)         ; get byte
e7:
      rra
      rl e
      rra
      rl d





      ;* pixel 7 *

      pop bc
      pop hl
      add hl,bc         ; h = new x-offset (Fixed)

      ld a,h
      cp 89
      ld a,0
      jr nc, e8

      ld a,l
      ld (XFixed),a
      ld a,h
      ld (XFixed+1),a

      ld l,h
      ld h,0            ; hl = new x-offset 

      ld a,(YLoc)
      ld c,a
      ld a,(YLoc+1)
      ld b,a

      add hl,bc         ; hl = new xy-offset

      ld a,(hl)         ; get byte
e8:
      rra
      rl e
      rra
      rl d


    ;* store result *

    pop hl              ; hl = de = Tile-loc
    WaitForVRAM
    ld a,e
    ld (hli),a
    ld a,d
    ld (hl),a           ; hl = Tile-loc


  ;* goto next tile-line *

ZoomCont:
NoSpare:



  
  ld a,0                ; due to the page-flip, 
  inc hl                ; pixels from the previous
  ld (hli),a            ; drawing were visible.
  ld (hl),a             ; this code kills it.

  ld a,(YCnt)
  inc a
  and $07
  ld (YCnt),a
  ld hl,2               ; current 8p y-line
  jr nz,CurbLine
    ld hl,(11*16)-14    ; next 8p y-line
  
CurbLine:

  pop de
  add hl,de
  ld d,h
  ld e,l                ; de = next tile y-line



  ;* calc next line in picture

  ld a, (StepSize)
  ld c,a
  ld a, (StepSize+1)
  ld b,a

  ld a, (YFixed)
  ld l,a
  ld a, (YFixed+1)
  ld h,a

  ld a,(YHFixed)
  add hl,bc
  adc a,0


  ld (YHFixed),a      ; store result
  ld b,a

  ld a,l
  ld (YFixed),a
  ld a,h
  ld (YFixed+1),a




  ;* multiply loc by 88 *


  push hl

  ld c,h
  ld b,0

  ld h,b
  ld l,c

  add hl,bc
  add hl,bc

  ld b,h
  ld c,l                ; bc = h * 3

  pop hl

  scf
  ccf                   ; CF = 0
  rl c
  rl b
  rl c
  rl b
  rl c
  rl b                  ; (h*3) + (h*8) = h * 24

  ld l,0
  scf
  ccf                   ; CF = 0
  rr h
  rr l
  rr h
  rr l                  ; hl = h*64

  add hl,bc             ; (h*64)+(h*24) = h * 88



  ld bc,88              ; next line
  add hl,bc


  ld bc,PictLoc


  add hl,bc           ; hl = hl + picture offset


  ld a,l
  ld (YLoc),a
  ld a,h
  ld (YLoc+1),a


  ld a,0
  ld (XFixed),a
  ld (XFixed+1),a       ; reset X-offset


  pop bc
  dec c
  jp nz, ZoomY

;end of draw pic


  ;* calc new window-pos *

  ld a,(ZoomSize)
  ld b,a
  ld a,88
  sub b
  rr a
  and $7f               ; (88-zoomsize)/2
  ld b,a

  ld a,88
  sub b
  ld b,a                ; b= new winpos x/y



  ld a,b
  ld ($FF42),a
  ld ($FF43),a

  ld a,($FF40)
  xor $18               ; page-flip
  ld ($FF40),a

 ret






FillRamBank:
;************************************
;* Fills a ram-bank with a picture. *
;* The incoming picture is packed,  *
;* whereas the resulting ram-bank   *
;* picture is unpacked.             *
;*                                  *
;* in:                              *
;* hl = picture                     *
;* b  = bank                        *
;************************************

  push hl               ; save for later

  ld hl,$4000
  ld a,b
  ld (hl),a             ;select rambank

  pop hl                ; hl = picture
  ld de,PictLoc         ; de = ram-bank
  

  ld c,32/4
r1:
  push bc
  ld c,$FF
r2:   

  ld a,(hli)
  ld b,a

  swap a
  rr a
  rr a
  and $03
  ld (de),a             ; pixel #1
  inc de


  ld a,b
  swap a
  and $03
  ld (de),a             ; pixel #2
  inc de

  ld a,b
  rr a
  rr a
  and $03
  ld (de),a             ; pixel #3
  inc de

  ld a,b
  and $03
  ld (de),a             ; pixel #4
  inc de

  dec c
  jr nz,r2


  pop bc
  dec c
  jr nz,r1

  ret






Div:
;***************************************************
;* Divider function: ripped it from jeff frohwein. *
;*                                                 *
;* de/bc = bc                                      *
;***************************************************
  
	ld	hl,temp	        ;load hl with symbolic address

	ld	(hl),c		;save the LS byte of the divisor

	inc	hl

	ld	(hl),b		;save the MS byte of the divisor

	inc	hl

	ld	(hl),17		;save the divisors bit count (decimal 17)

	ld	bc,0		;BC will store the partial dividend

nxtbit:

	ld	hl,count	;Load hl with address of bit count (dec 17)

	ld	a,e		;get the LS byte of the divisor

	rla			;rotate the MSB into carry

	ld	e,a

	ld	a,d		;get the MS byte of the divisor

	rla			;rotate the MSB into carry

        ld	d,a

	dec	(hl)		;decrement the bit count

	ret	z		;return if zero

	ld	a,c		;rotate the MSB of the dividend

	rla			; into the partial dividend

	ld	c,a		;  stored in BC

	ld	a,b

	rla

	ld	b,a

	dec	hl		;point HL to divisor in memory

	dec	hl

	ld	a,c		;get the LS byte of partial dividend

	sub	(hl)		;subtract the LS byte of the divisor

	ld	c,a

	inc	hl

	ld	a,b		;get the MS byte of partial dividend

	sbc	a,(hl)		;subtract with borrow the divisor

	ld	b,a

	jp	nc,noadd	;if carry=0, do not add divisor to

				; the result of previous subtraction

	dec	hl

	ld	a,c		;The divisor is larger than the partial

	add	a,(hl)		; dividend, so the divisor must be

	ld	c,a		;  added to the result of the subtraction

	inc	hl		;   so that the previous value of the

	ld	a,b		;    partial dividend is re-established.

	adc	a,(hl)

	ld	b,a

noadd:

	ccf			;complement the carry

	jp	nxtbit




  
BuildGrid:
;****************************************
;*                                      *
;* Builds grid of tiles in display-mem, *
;* in the following order:              *
;*                                      *
;*    0  1   ..   10                    *
;*   11  12  ..   21                    *
;*   22  23  ..   32                    *
;*   ..  ..  ..   ..                    *
;*                                      *
;****************************************


  ld a,0


    
    ld c,11
lg1:  push bc
      ld c,11
lg2:    

        push af
        WaitForVRAM
        pop af
        ldi (hl),a
        inc a
        dec c
      jr nz, lg2

      push de
      ld de,21
      add hl,de
      pop de
      pop bc
      dec c
    jr nz, lg1
 ret






waitvbl			
;****************************
;* Wait for Vertical blank. *
;* Ripped from Hero Zero.   *
;****************************
 ldh	a,($40)
 add	a,a		; Is the screen turned off??
 ret	nc              ; If yes then exit..
notyet
 ldh	a,($44)		; $ff44=LCDC Y-Pos
 cp	$90		; $90 and bigger = in VBL
 jr	nz,notyet
 ret







;**********************************
;*              DATA              *
;**********************************




TitleTop:
.byte $00,$00,$7f,$7f,$ff,$80,$ff,$80,$ff,$80,$ff,$80,$83,$fc,$7f,$7c
.byte $07,$04,$0f,$08,$1f,$10,$3e,$21,$7d,$43,$ff,$80,$ff,$80,$ff,$80
.byte $00,$00,$f7,$f7,$ef,$18,$ef,$18,$ef,$18,$ef,$18,$ef,$18,$ed,$1b
.byte $de,$32,$BE,$62,$7e,$c2,$be,$c2,$fe,$f2,$ef,$19,$ef,$18,$ef,$18
.byte $00,$00,$e1,$e1,$d3,$32,$ef,$1c,$f7,$0c,$f7,$0c,$fb,$06,$fb,$86
.byte $7b,$46,$7b,$46,$7b,$46,$7b,$46,$7b,$46,$fb,$86,$fb,$06,$f7,$0c
.byte $00,$00,$fb,$fb,$f7,$0c,$fb,$06,$fd,$03,$fd,$03,$fe,$01,$7e,$e1
.byte $9e,$91,$9e,$91,$9e,$91,$9e,$91,$9e,$91,$fe,$61,$fe,$01,$fd,$03
.byte $00,$00,$f7,$f7,$ef,$18,$ff,$00,$ff,$00,$ff,$00,$ff,$80,$ff,$a2
.byte $bb,$e6,$bb,$e6,$bb,$e6,$bb,$e6,$bb,$e6,$bb,$e6,$bb,$e6,$bb,$66
.byte $00,$00,$e3,$e3,$d7,$34,$df,$38,$df,$38,$df,$30,$df,$30,$df,$30
.byte $df,$30,$df,$30,$df,$30,$df,$30,$df,$30,$df,$30,$de,$31,$de,$31
.byte $00,$00,$e7,$e7,$df,$38,$ef,$18,$ef,$18,$f7,$8c,$f7,$8c,$fb,$86
.byte $fb,$86,$fb,$06,$fb,$06,$fb,$06,$fb,$06,$fb,$86,$fb,$86,$fb,$86
.byte $03,$03,$f3,$f3,$ed,$1d,$f2,$0e,$fd,$03,$fd,$43,$fd,$43,$fd,$43
.byte $fd,$43,$fd,$03,$fa,$06,$f4,$0c,$fe,$06,$fd,$43,$7d,$c3,$7d,$c3


TitleBottom:
.byte $ff,$80,$ff,$80,$80,$ff,$7f,$7f
.byte $ef,$18,$ef,$18,$08,$ff,$f7,$f7
.byte $f7,$0c,$ef,$1c,$12,$f3,$e1,$e1
.byte $fd,$03,$fb,$06,$04,$ff,$fb,$ff
.byte $bb,$66,$bb,$66,$22,$ff,$dd,$dd
.byte $de,$31,$de,$31,$10,$ff,$ef,$ef
.byte $fb,$86,$fb,$86,$82,$ff,$fd,$ff
.byte $7d,$c3,$7d,$c3,$61,$ff,$de,$de


picture1:
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,63,-1,-1,-4,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,63,-1,-1,-1,-1,-4,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,-1
.byte -6,-86,-86,-81,-1,-16,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,3,-1,-6,-86,-86,-86,-86,-81,-1,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,15,-2,-86,-86,-86,-86,-86
.byte -86,-65,-16,0,0,0,0,0,0,0,0,0,0,0,0,-1
.byte -22,-86,-86,-86,-86,-86,-86,-85,-1,0,0,0,0,0,0,0
.byte 0,0,0,0,3,-2,-86,-86,-86,-86,-86,-86,-86,-86,-65,-64
.byte 0,0,0,0,0,0,0,0,0,0,63,-22,-86,-86,-86,-86
.byte -86,-86,-86,-86,-85,-4,0,0,0,0,0,0,0,0,0,0
.byte -1,-86,-86,-86,-81,-1,-1,-6,-86,-86,-86,-1,0,0,0,0
.byte 0,0,0,0,0,3,-6,-86,-86,-81,-1,-1,-1,-1,-6,-86
.byte -86,-81,-64,0,0,0,0,0,0,0,0,63,-22,-86,-86,-1
.byte -3,85,85,127,-1,-86,-86,-85,-16,0,0,0,0,0,0,0
.byte 0,-1,-86,-86,-81,-3,85,85,85,85,127,-6,-86,-86,-4,0
.byte 0,0,0,0,0,0,3,-2,-86,-86,-1,-43,85,85,85,85
.byte 87,-1,-86,-86,-65,0,0,0,0,0,0,0,3,-6,-86,-85
.byte -3,85,127,-1,-1,85,85,127,-22,-86,-81,-64,0,0,0,0
.byte 0,0,15,-22,-86,-65,-43,85,-6,-86,-85,-43,85,87,-2,-86
.byte -85,-16,0,0,0,0,0,0,63,-86,-86,-1,85,95,-22,-86
.byte -86,-3,85,85,-1,-86,-86,-4,0,0,0,0,0,0,62,-86
.byte -85,-11,85,-2,-86,-86,-86,-81,-43,85,95,-22,-86,-68,0,0
.byte 0,0,0,0,-2,-86,-81,-43,87,-22,-86,-86,-86,-86,-11,85
.byte 87,-6,-86,-65,0,0,0,0,0,3,-6,-86,-65,85,95,-86
.byte -88,0,0,-86,-67,85,85,-2,-86,-81,-64,0,0,0,0,3
.byte -22,-86,-3,85,94,-86,-96,5,64,-86,-81,85,85,127,-86,-85
.byte -64,0,0,0,0,15,-22,-86,-11,85,126,-86,-96,21,80,-86
.byte -85,-43,85,95,-85,-1,-80,0,0,0,0,63,-86,-85,-11,85
.byte 122,-86,-96,80,20,-86,-86,-75,85,95,-17,0,-8,0,0,0
.byte 0,63,-86,-81,-43,85,122,-86,-88,0,0,-86,-86,-75,85,87
.byte -4,0,56,0,0,0,0,62,-86,-81,85,85,122,-86,-86,-86
.byte -86,-86,-86,-75,85,85,-80,0,56,0,0,0,0,-2,-86,-65
.byte 85,85,126,-81,-1,-1,-1,-1,-22,-11,85,85,-80,0,56,42
.byte -128,0,0,-6,-86,-67,85,85,95,-6,-86,-86,-86,-86,-65,-43
.byte 85,85,-80,0,58,-65,-32,0,0,-6,-86,-3,85,85,95,-81
.byte -1,-86,-85,-1,-21,-43,85,85,-80,0,59,-64,-8,0,3,-6
.byte -86,-11,85,85,127,-1,95,-1,-1,87,-1,-11,85,85,-80,0
.byte -1,0,56,0,3,-22,-85,-11,85,85,122,-43,5,85,85,65
.byte 94,-75,85,85,-80,0,-16,0,56,0,3,-22,-85,-43,85,85
.byte 123,-44,1,85,85,-64,95,-75,85,85,-80,3,-64,0,56,0
.byte 3,-22,-85,-43,85,-87,-5,80,63,84,87,-16,23,-67,-87,90
.byte -4,15,0,0,-8,0,15,-22,-85,-43,90,-2,-21,80,-1,84
.byte 87,-4,23,-82,-2,-81,-61,-4,0,3,-32,0,15,-86,-81,-43
.byte 111,95,-85,83,-13,-48,95,63,23,-85,-41,-32,0,3,-64,15
.byte -128,0,15,-86,-81,85,-67,87,-85,83,-61,-47,95,15,23,-85
.byte 85,-8,0,0,-16,62,0,0,15,-86,-81,86,-11,87,-85,80
.byte -1,-47,95,-4,23,-85,85,126,0,0,63,-8,0,0,15,-86
.byte -81,86,-43,127,-65,80,-1,-15,95,-4,23,-5,-11,94,0,0
.byte 44,62,0,0,15,-86,-81,86,-43,31,-75,85,63,-63,87,-15
.byte 85,123,-47,94,0,0,60,15,-128,0,15,-86,-81,86,-44,127
.byte -43,85,111,5,85,85,85,95,-12,94,0,0,-16,3,-128,0
.byte 15,-86,-81,86,-44,87,87,-43,-80,21,85,85,87,-41,84,94
.byte 0,63,-64,3,-128,0,15,-86,-81,86,-43,21,87,-11,-80,85
.byte 85,85,95,-43,81,94,0,-1,0,15,-128,0,15,-86,-81,86
.byte -11,21,95,-3,-79,85,85,85,127,-11,81,126,3,-53,0,62
.byte 0,0,15,-86,-81,85,-67,85,95,-1,-15,85,85,-33,-1,-11
.byte 85,-8,15,47,-4,56,0,0,15,-86,-81,85,111,85,87,-1
.byte -11,85,85,-1,-1,-43,87,-32,12,-68,15,-32,0,0,15,-86
.byte -81,-43,90,-11,85,-1,-1,85,87,-1,-1,85,126,0,-16,-80
.byte 0,-32,0,0,15,-22,-85,-43,85,-67,85,127,-1,-1,-1,-1
.byte -3,85,-1,-64,48,-68,15,-64,0,0,3,-22,-85,-43,85,109
.byte 85,95,-1,-1,-1,-1,-11,85,-1,-64,15,-17,-1,-64,0,0
.byte 3,-22,-85,-43,85,109,85,87,-1,-1,-1,-1,-43,85,-1,-4
.byte 15,-22,-85,-64,0,0,3,-22,-85,-6,-87,111,85,85,-1,-1
.byte -1,-3,85,87,-1,-1,63,-22,-85,-64,0,0,3,-6,-86,-1
.byte -2,-65,85,85,127,-1,-1,-43,85,87,-1,-1,-1,-86,-81,-64
.byte 0,0,0,-6,-81,85,87,-21,-43,85,94,-65,-21,85,85,95
.byte -22,-86,-66,-86,-81,0,0,0,0,-6,-11,85,85,126,-11,85
.byte 95,-22,-81,85,85,126,-65,-1,-69,-1,-5,0,0,0,0,-5
.byte 85,85,85,87,-65,85,87,-91,-83,85,87,-5,-64,5,-20,0
.byte 95,-128,0,0,0,61,85,85,85,85,-17,-11,-41,-22,-67,93
.byte 127,-20,5,85,113,85,86,-32,0,0,0,-75,85,64,5,85
.byte 123,-1,-43,-1,-11,95,-1,-80,84,85,69,85,85,-72,0,0
.byte 0,-75,84,21,81,85,123,-22,-75,85,85,-6,-81,-79,84,85
.byte 85,85,85,-72,0,0,2,-43,81,85,85,85,94,-22,-67,85
.byte 87,-22,-86,-63,80,85,85,85,85,-82,0,0,2,-43,69,85
.byte 85,121,95,-86,-81,-1,-1,-86,-86,-59,0,1,85,85,85,110
.byte 0,0,11,85,69,95,-43,121,87,-86,-81,-1,-1,-86,-86,-59
.byte 84,21,85,85,85,110,0,0,11,85,21,-75,85,94,87,-86
.byte -81,-1,-1,-81,-2,-59,84,85,85,85,85,110,0,0,11,85
.byte 21,-43,81,94,87,-1,-81,-1,-1,-68,14,-43,84,85,85,85
.byte 85,110,0,0,11,85,21,-43,81,94,87,-125,-17,-86,-81,-80
.byte 2,-43,85,85,85,85,85,-82,0,0,11,85,21,-27,81,94
.byte 87,-128,-17,-86,-81,-80,3,-75,85,85,85,85,85,-72,0,0
.byte 11,85,21,121,69,94,87,-128,-17,-86,-81,-80,3,-75,85,85
.byte 85,85,86,-72,0,0,11,85,69,-75,81,121,87,-4,-17,-86
.byte -81,-68,-1,-19,85,85,85,85,86,-32,0,0,2,-43,69,-128
.byte 5,121,95,-1,-17,-86,-81,-81,-1,-19,85,85,85,85,90,-32
.byte 0,0,2,-43,81,85,85,-27,94,-1,-1,-86,-81,-65,-1,-85
.byte 85,85,85,85,91,-128,0,0,0,-75,85,85,95,-107,122,-81
.byte -1,-1,-1,-1,-6,-86,-43,85,85,85,110,0,0,0,0,-75
.byte 85,127,-6,85,122,-86,-81,-1,-1,-6,-86,-86,-75,85,85,85
.byte -72,0,0,0,0,45,85,90,-91,85,-22,-86,-86,-86,-86,-86
.byte -18,-86,-83,85,85,86,-32,0,0,0,0,11,85,85,85,87
.byte -86,-86,-86,-86,-86,-86,-2,-86,-65,85,85,91,-128,0,0,0
.byte 0,2,-11,85,85,127,-22,-86,-86,-86,-86,-86,-18,-21,-2,-43
.byte 85,110,0,0,0,0,0,0,-81,85,87,-17,-2,-86,-86,-86
.byte -86,-86,-86,-65,-16,-75,86,-72,0,0,0,0,0,0,10,-1
.byte -2,-128,-1,-6,-86,-86,-86,-86,-81,-1,0,47,91,-32,0,0
.byte 0,0,0,0,0,-86,-88,0,15,-1,-6,-86,-86,-81,-1,-16
.byte 0,10,-2,-128,0,0,0,0,0,0,0,0,0,0,0,63
.byte -1,-1,-1,-1,-4,0,0,0,-88,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,63,-1,-1,-4,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0


#IFNDEF SMALL


picture2:
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,63,-1,-1,-4,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,63,-1,-1,-1,-1,-4,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,-1
.byte -6,-86,-86,-81,-1,-16,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,3,-1,-6,-86,-86,-86,-86,-81,-1,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,15,-2,-86,-86,-86,-86,-86
.byte -86,-65,-16,0,0,0,0,0,0,0,0,0,0,0,0,-1
.byte -22,-86,-86,-86,-86,-86,-86,-85,-1,0,0,0,0,0,0,0
.byte 0,0,0,0,3,-2,-86,-86,-86,-86,-86,-86,-86,-86,-65,-64
.byte 0,0,0,0,0,0,0,0,0,0,63,-22,-86,-86,-86,-86
.byte -86,-86,-86,-86,-85,-4,0,0,0,0,0,0,0,0,0,0
.byte -1,-86,-86,-86,-81,-1,-1,-6,-86,-86,-86,-1,0,0,0,0
.byte 0,0,0,0,0,3,-6,-86,-86,-81,-1,-1,-1,-1,-6,-86
.byte -86,-81,-64,0,0,0,0,0,0,0,0,63,-22,-86,-86,-1
.byte -3,85,85,127,-1,-86,-86,-85,-16,0,0,0,0,0,0,0
.byte 0,-1,-86,-86,-81,-3,85,85,85,85,127,-6,-86,-86,-4,0
.byte 0,0,0,0,0,0,3,-2,-86,-86,-1,-43,85,85,85,85
.byte 87,-1,-86,-86,-65,0,0,0,0,0,0,0,3,-6,-86,-85
.byte -3,85,127,-1,-1,85,85,127,-22,-86,-81,-64,0,0,0,0
.byte 0,0,15,-22,-86,-65,-43,85,-6,-86,-85,-43,85,87,-2,-86
.byte -85,-16,0,0,0,0,0,0,63,-86,-86,-1,85,95,-22,-86
.byte -86,-3,85,85,-1,-86,-86,-4,0,0,0,0,0,0,62,-86
.byte -85,-11,85,-2,-86,-86,-86,-81,-43,85,95,-22,-86,-68,0,0
.byte 0,0,0,0,-2,-86,-81,-43,87,-22,-86,-86,-86,-86,-11,85
.byte 87,-6,-86,-65,0,0,0,0,0,3,-6,-86,-65,85,95,-86
.byte -88,0,0,-86,-67,85,85,-2,-86,-81,-64,0,0,0,0,3
.byte -22,-86,-3,85,94,-86,-96,5,64,-86,-81,85,85,127,-1,-21
.byte -64,0,0,0,0,15,-22,-86,-11,85,126,-86,-96,21,80,-86
.byte -85,-43,85,91,0,-5,-16,0,0,0,0,63,-86,-85,-11,85
.byte 122,-86,-96,80,20,-86,-86,-75,85,108,0,58,-4,0,0,0
.byte 0,63,-86,-81,-43,85,122,-86,-88,0,0,-86,-86,-75,85,108
.byte 0,58,-66,-96,0,0,0,62,-86,-81,85,85,122,-86,-86,-86
.byte -86,-86,-86,-75,85,108,0,14,-65,-8,0,0,0,-2,-86,-65
.byte 85,85,126,-81,-1,-1,-1,-1,-22,-11,85,108,0,14,-16,62
.byte 0,0,0,-6,-86,-67,85,85,95,-6,-86,-86,-86,-86,-65,-43
.byte 85,108,0,15,-64,14,0,0,0,-6,-86,-3,85,85,95,-81
.byte -1,-86,-85,-1,-21,-43,85,108,0,60,0,14,0,0,3,-6
.byte -86,-11,85,85,127,-1,95,-1,-1,87,-1,-11,85,108,0,48
.byte 0,14,0,0,3,-22,-85,-11,85,85,122,-43,5,85,85,65
.byte 94,-75,85,108,0,-64,0,62,0,0,3,-22,-85,-43,85,85
.byte 123,-44,1,85,85,-64,95,-75,86,-65,3,-64,0,-8,0,0
.byte 3,-22,-85,-43,85,-87,-5,80,63,84,87,-16,23,-67,-85,-64
.byte -1,0,3,-16,0,0,15,-22,-85,-43,90,-2,-21,80,-1,84
.byte 87,-4,23,-82,-2,0,3,-16,15,-16,0,0,15,-86,-81,-43
.byte 111,95,-85,83,-13,-48,95,63,23,-85,-41,-32,0,63,-2,-16
.byte 0,0,15,-86,-81,85,-67,87,-85,83,-61,-47,95,15,23,-85
.byte 85,-8,0,44,15,-16,0,0,15,-86,-81,86,-11,87,-85,80
.byte -1,-47,95,-4,23,-85,85,126,0,44,3,-16,0,0,15,-86
.byte -81,86,-43,127,-65,80,-1,-15,95,-4,23,-5,-11,94,0,44
.byte 0,-16,0,0,15,-86,-81,86,-43,31,-75,85,63,-63,87,-15
.byte 85,123,-47,94,0,60,0,-16,0,0,15,-86,-81,86,-44,127
.byte -43,85,111,5,85,85,85,95,-12,94,15,-16,0,-16,0,0
.byte 15,-86,-81,86,-44,87,87,-43,-80,21,85,85,87,-41,84,94
.byte 60,-16,3,-16,0,0,15,-86,-81,86,-43,21,87,-11,-80,85
.byte 85,85,95,-43,81,94,-16,-65,-49,-16,0,0,15,-86,-81,86
.byte -11,21,95,-3,-79,85,85,85,127,-11,81,126,-62,-64,-2,-16
.byte 0,0,15,-86,-81,85,-67,85,95,-1,-15,85,85,-33,-1,-11
.byte 85,-8,11,-64,14,-16,0,0,15,-86,-81,85,111,85,87,-1
.byte -11,85,85,-1,-1,-43,87,-84,11,0,-2,-16,0,0,15,-86
.byte -81,-43,90,-11,85,-1,-1,85,87,-1,-1,85,126,15,11,-1
.byte -22,-16,0,0,15,-22,-85,-43,85,-67,85,127,-1,-1,-1,-1
.byte -3,85,-4,3,-2,-6,-85,-16,0,0,3,-22,-85,-43,85,109
.byte 85,95,-1,-1,-1,-1,-11,85,-1,-64,-21,-22,-85,-64,0,0
.byte 3,-22,-85,-43,85,109,85,87,-1,-1,-1,-1,-43,85,-1,-4
.byte -25,-22,-85,-64,0,0,3,-22,-85,-22,-91,111,85,85,-1,-1
.byte -1,-3,85,87,-1,-1,-17,-22,-85,-64,0,0,3,-6,-86,-1
.byte -6,-65,85,85,127,-1,-1,-43,85,87,-1,-2,-97,-86,-81,-64
.byte 0,0,0,-6,-86,-43,94,-65,-43,85,94,-65,-21,85,85,95
.byte -22,-86,126,-86,-81,0,0,0,0,-6,-81,85,87,-17,-11,85
.byte 95,-22,-81,85,85,126,-65,-1,-69,-1,-5,0,0,0,0,-2
.byte -67,85,85,-5,-1,85,87,-91,-83,85,87,-5,-64,5,-20,0
.byte 95,-128,0,0,0,62,-75,85,85,123,-1,-11,-41,-22,-67,93
.byte 127,-20,5,85,113,85,86,-32,0,0,0,62,-43,80,21,94
.byte -1,-1,-43,-1,-11,95,-1,-80,85,81,69,85,85,-72,0,0
.byte 0,15,-43,5,69,94,-1,-22,-75,85,85,-6,-81,-79,69,69
.byte 85,85,85,-72,0,0,0,15,-44,21,85,94,-1,-22,-67,85
.byte 87,-22,-86,-63,81,21,85,85,85,-82,0,0,0,11,84,85
.byte 87,-105,-65,-86,-81,-1,-1,-86,-86,-59,84,85,85,85,85,110
.byte 0,0,0,11,80,95,-41,-105,-66,-86,-81,-1,-1,-86,-86,-59
.byte 81,21,85,85,85,110,0,0,0,11,81,101,85,-25,-70,-86
.byte -81,-1,-1,-81,-2,-59,69,69,85,85,85,110,0,0,0,11
.byte 81,117,69,-25,-70,-1,-81,-1,-1,-68,14,-43,21,85,85,85
.byte 85,110,0,0,0,11,81,117,69,-25,-21,-61,-17,-86,-81,-80
.byte 2,-43,85,85,85,85,85,-82,0,0,0,11,81,117,69,-25
.byte -5,0,-17,-86,-81,-80,3,-75,85,85,85,85,85,-72,0,0
.byte 0,11,81,117,69,-25,-1,0,-17,-86,-81,-80,3,-11,85,85
.byte 85,85,86,-72,0,0,0,11,81,89,85,-25,-65,-4,-17,-86
.byte -81,-68,-1,-3,85,85,85,85,86,-32,0,0,0,11,80,96
.byte 23,-105,-81,-1,-17,-86,-81,-81,-1,-19,85,85,85,85,90,-32
.byte 0,0,0,2,-44,85,94,94,-86,-1,-1,-86,-81,-65,-1,-85
.byte 85,85,85,85,91,-128,0,0,0,2,-43,21,126,94,-86,-81
.byte -1,-1,-1,-1,-6,-86,-43,85,85,85,110,0,0,0,0,2
.byte -43,95,-23,94,-86,-86,-81,-1,-1,-6,-86,-86,-75,85,85,85
.byte -72,0,0,0,0,0,-75,90,-107,122,-86,-86,-86,-86,-86,-86
.byte -18,-86,-83,85,85,86,-32,0,0,0,0,0,57,85,85,-66
.byte -86,-86,-86,-86,-86,-86,-2,-86,-65,85,85,91,-128,0,0,0
.byte 0,0,47,85,87,-1,-22,-86,-86,-86,-86,-86,-18,-21,-2,-43
.byte 85,110,0,0,0,0,0,0,10,-43,94,-113,-2,-86,-86,-86
.byte -86,-86,-86,-65,-16,-75,86,-72,0,0,0,0,0,0,0,-65
.byte -8,0,-1,-6,-86,-86,-86,-86,-81,-1,0,47,91,-32,0,0
.byte 0,0,0,0,0,42,-96,0,15,-1,-6,-86,-86,-81,-1,-16
.byte 0,10,-2,-128,0,0,0,0,0,0,0,0,0,0,0,63
.byte -1,-1,-1,-1,-4,0,0,0,-88,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,63,-1,-1,-4,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0



picture3:
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,63,-1,-1,-4,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,63,-1,-1,-1,-1,-4,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,-1
.byte -6,-86,-86,-81,-1,-16,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,3,-1,-6,-86,-86,-86,-86,-81,-1,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,15,-2,-86,-86,-86,-86,-86
.byte -86,-65,-16,0,0,0,0,0,0,0,0,0,0,0,0,-1
.byte -22,-86,-86,-86,-86,-86,-86,-85,-1,0,0,0,0,0,0,0
.byte 0,0,0,0,3,-2,-86,-86,-86,-86,-86,-86,-86,-86,-65,-64
.byte 0,0,0,0,0,0,0,0,0,0,63,-22,-86,-86,-86,-86
.byte -86,-86,-86,-86,-85,-4,0,0,0,0,0,0,0,0,0,0
.byte -1,-86,-86,-86,-81,-1,-1,-6,-86,-86,-86,-1,0,0,0,0
.byte 0,0,0,0,0,3,-6,-86,-86,-81,-1,-1,-1,-1,-6,-86
.byte -86,-81,-64,0,0,0,0,0,0,0,0,63,-22,-86,-86,-1
.byte -3,85,85,127,-1,-86,-86,-85,-16,0,0,0,0,0,0,0
.byte 0,-1,-86,-86,-81,-3,85,85,85,85,127,-6,-86,-86,-4,0
.byte 0,0,0,0,0,0,3,-2,-86,-86,-1,-43,85,85,85,85
.byte 87,-1,-86,-86,-65,0,0,0,0,0,0,0,3,-6,-86,-85
.byte -3,85,127,-1,-1,85,85,127,-22,-86,-81,-64,0,0,0,0
.byte 0,0,15,-22,-86,-65,-43,85,-6,-86,-85,-43,85,87,-2,-86
.byte -85,-16,0,0,0,0,0,0,63,-86,-86,-1,85,95,-22,-86
.byte -86,-3,85,85,-1,-86,-86,-4,0,0,0,0,0,0,62,-86
.byte -85,-11,85,-2,-86,-86,-86,-81,-43,85,95,-22,-86,-68,0,0
.byte 0,0,0,0,-2,-86,-81,-43,87,-22,-86,-86,-86,-86,-11,85
.byte 87,-6,-86,-65,0,0,0,0,0,3,-6,-86,-65,85,95,-86
.byte -88,0,0,-86,-67,85,91,-1,-22,-81,-64,0,0,0,0,3
.byte -22,-86,-3,85,94,-86,-96,5,64,-86,-81,85,111,0,-6,-85
.byte -64,0,0,0,0,15,-22,-86,-11,85,126,-86,-96,21,80,-86
.byte -85,-43,108,0,58,-86,-16,0,0,0,0,63,-86,-85,-11,85
.byte 122,-86,-96,80,20,-86,-86,-75,108,0,14,-65,-8,0,0,0
.byte 0,63,-86,-81,-43,85,122,-86,-88,0,0,-86,-86,-75,108,0
.byte 14,-16,62,0,0,0,0,62,-86,-81,85,85,122,-86,-86,-86
.byte -86,-86,-86,-75,108,0,15,-64,14,0,0,0,0,-2,-86,-65
.byte 85,85,126,-81,-1,-1,-1,-1,-22,-11,108,0,63,0,15,0
.byte 0,0,0,-6,-86,-67,85,85,95,-6,-86,-86,-86,-86,-65,-43
.byte 108,0,60,0,15,0,0,0,0,-6,-86,-3,85,85,95,-81
.byte -1,-86,-85,-1,-21,-43,108,0,-16,0,63,0,0,0,3,-6
.byte -86,-11,85,85,127,-1,95,-1,-1,87,-1,-11,91,0,-16,0
.byte -1,-64,0,0,3,-22,-85,-11,85,85,122,-43,5,85,85,65
.byte 94,-75,111,-1,-64,3,-21,-64,0,0,3,-22,-85,-43,85,85
.byte 123,-44,1,85,85,-64,95,-75,-68,0,60,15,-85,-64,0,0
.byte 3,-22,-85,-43,85,-87,-5,80,63,84,87,-16,23,-66,-32,0
.byte 15,63,-85,-64,0,0,15,-22,-85,-43,90,-2,-21,80,-1,84
.byte 87,-4,23,-82,-2,-128,3,-61,-21,-16,0,0,15,-86,-81,-43
.byte 111,95,-85,83,-13,-48,95,63,23,-85,-41,-32,2,-64,-6,-16
.byte 0,0,15,-86,-81,85,-67,87,-85,83,-61,-47,95,15,23,-85
.byte 85,-8,0,-64,58,-16,0,0,15,-86,-81,86,-11,87,-85,80
.byte -1,-47,95,-4,23,-85,85,126,3,-64,58,-16,0,0,15,-86
.byte -81,86,-43,127,-65,80,-1,-15,95,-4,23,-5,-11,94,15,0
.byte 58,-16,0,0,15,-86,-81,86,-43,31,-75,85,63,-63,87,-15
.byte 85,123,-47,94,-4,0,-6,-16,0,0,15,-86,-81,86,-44,127
.byte -43,85,111,5,85,85,85,95,-12,94,44,3,-22,-16,0,0
.byte 15,-86,-81,86,-44,87,87,-43,-80,21,85,85,87,-41,84,94
.byte 63,-1,-86,-16,0,0,15,-86,-81,86,-43,21,87,-11,-80,85
.byte 85,85,95,-43,81,94,60,3,-86,-16,0,0,15,-86,-81,86
.byte -11,21,95,-3,-79,85,85,85,127,-11,81,126,48,15,-86,-16
.byte 0,0,15,-86,-81,85,-67,85,95,-1,-15,85,85,-33,-1,-11
.byte 85,-8,60,62,-86,-16,0,0,15,-86,-81,85,111,85,87,-1
.byte -11,85,85,-1,-1,-43,87,-125,-17,-6,-86,-16,0,0,15,-86
.byte -81,-43,90,-11,85,-1,-1,85,87,-1,-1,85,126,14,-102,-6
.byte -86,-16,0,0,15,-22,-85,-43,85,-67,85,127,-1,-1,-1,-1
.byte -3,85,-4,14,87,-22,-85,-16,0,0,3,-22,-85,-43,85,109
.byte 85,95,-1,-1,-1,-1,-11,85,-1,-50,87,-22,-85,-64,0,0
.byte 3,-22,-85,-43,85,109,85,87,-1,-1,-1,-1,-43,85,-1,-2
.byte 87,-22,-85,-64,0,0,3,-22,-85,-6,-107,111,85,85,-1,-1
.byte -1,-3,85,87,-1,-23,95,-22,-85,-64,0,0,3,-6,-86,-1
.byte -26,-65,85,85,127,-1,-1,-43,85,87,-1,-107,95,-86,-81,-64
.byte 0,0,0,-6,-86,-67,-5,-1,-43,85,94,-65,-21,85,85,95
.byte -22,-86,126,-86,-81,0,0,0,0,-6,-86,-75,123,-1,-11,85
.byte 95,-22,-81,85,85,126,-65,-1,-69,-1,-5,0,0,0,0,-2
.byte -86,-43,94,-1,-1,85,87,-91,-83,85,87,-5,-64,5,-20,0
.byte 95,-128,0,0,0,62,-86,-43,94,-1,-1,-11,-41,-22,-67,93
.byte 127,-20,5,85,113,85,86,-32,0,0,0,63,-85,80,87,-65
.byte -1,-1,-43,-1,-11,95,-1,-80,84,85,69,85,85,-72,0,0
.byte 0,15,-85,69,23,-65,-1,-22,-75,85,85,-6,-81,-79,84,85
.byte 85,85,85,-72,0,0,0,15,-21,69,119,-65,-1,-22,-67,85
.byte 87,-22,-86,-63,80,85,85,85,85,-82,0,0,0,3,-21,69
.byte 119,-65,-1,-86,-81,-1,-1,-86,-86,-59,0,1,85,85,85,110
.byte 0,0,0,3,-3,23,93,-17,-2,-86,-81,-1,-1,-86,-86,-59
.byte 84,21,85,85,85,110,0,0,0,0,-3,29,29,-17,-6,-86
.byte -81,-1,-1,-81,-2,-59,84,85,85,85,85,110,0,0,0,0
.byte 61,29,29,-1,-6,-1,-81,-1,-1,-68,14,-43,84,85,85,85
.byte 85,110,0,0,0,0,45,29,29,-1,-21,-61,-17,-86,-81,-80
.byte 2,-43,85,85,85,85,85,-82,0,0,0,0,45,29,29,-17
.byte -5,0,-17,-86,-81,-80,3,-75,85,85,85,85,85,-72,0,0
.byte 0,0,45,21,93,-21,-1,0,-17,-86,-81,-80,3,-75,85,85
.byte 85,85,86,-72,0,0,0,0,45,29,29,-22,-1,-4,-17,-86
.byte -81,-68,-1,-19,85,85,85,85,86,-32,0,0,0,0,11,20
.byte 119,-86,-81,-1,-17,-86,-81,-81,-1,-19,85,85,85,85,90,-32
.byte 0,0,0,0,11,69,119,-86,-86,-1,-1,-86,-81,-65,-1,-85
.byte 85,85,85,85,91,-128,0,0,0,0,11,85,-41,-86,-86,-81
.byte -1,-1,-1,-1,-6,-86,-43,85,85,85,110,0,0,0,0,0
.byte 11,95,-41,-86,-86,-86,-81,-1,-1,-6,-86,-86,-75,85,85,85
.byte -72,0,0,0,0,0,2,-43,95,-22,-86,-86,-86,-86,-86,-86
.byte -18,-86,-83,85,85,86,-32,0,0,0,0,0,2,-43,95,-2
.byte -86,-86,-86,-86,-86,-86,-2,-86,-65,85,85,91,-128,0,0,0
.byte 0,0,0,-75,120,-1,-22,-86,-86,-86,-86,-86,-18,-21,-2,-43
.byte 85,110,0,0,0,0,0,0,0,-67,-8,15,-2,-86,-86,-86
.byte -86,-86,-86,-65,-16,-75,86,-72,0,0,0,0,0,0,0,47
.byte -32,0,-1,-6,-86,-86,-86,-86,-81,-1,0,47,91,-32,0,0
.byte 0,0,0,0,0,10,-128,0,15,-1,-6,-86,-86,-81,-1,-16
.byte 0,10,-2,-128,0,0,0,0,0,0,0,0,0,0,0,63
.byte -1,-1,-1,-1,-4,0,0,0,-88,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,63,-1,-1,-4,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0



picture4:
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,63,-1,-1,-4,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,63,-1,-1,-1,-1,-4,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,-1
.byte -6,-86,-86,-81,-1,-16,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,3,-1,-6,-86,-86,-86,-86,-81,-1,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,15,-2,-86,-86,-86,-86,-86
.byte -86,-65,-16,0,0,0,0,0,0,0,0,0,0,0,0,-1
.byte -22,-86,-86,-86,-86,-86,-86,-85,-1,0,0,0,0,0,0,0
.byte 0,0,0,0,3,-2,-86,-86,-86,-86,-86,-86,-86,-86,-65,-64
.byte 0,0,0,0,0,0,0,0,0,0,63,-22,-86,-86,-86,-86
.byte -86,-86,-86,-86,-85,-4,0,0,0,0,0,0,0,0,0,0
.byte -1,-86,-86,-86,-81,-1,-1,-6,-86,-86,-86,-1,0,0,0,0
.byte 0,0,0,0,0,3,-6,-86,-86,-81,-1,-1,-1,-1,-6,-86
.byte -86,-81,-64,0,0,0,0,0,0,0,0,63,-22,-86,-86,-1
.byte -3,85,85,127,-1,-86,-86,-85,-16,0,0,0,0,0,0,0
.byte 0,-1,-86,-86,-81,-3,85,85,85,85,127,-6,-86,-86,-4,0
.byte 0,0,0,0,0,0,3,-2,-86,-86,-1,-43,85,85,85,85
.byte 87,-1,-86,-86,-65,0,0,0,0,0,0,0,3,-6,-86,-85
.byte -3,85,127,-1,-1,85,85,127,-22,-86,-81,-64,0,0,0,0
.byte 0,0,15,-22,-86,-65,-43,85,-6,-86,-85,-43,85,87,-2,-86
.byte -85,-16,0,0,0,0,0,0,63,-86,-86,-1,85,95,-22,-86
.byte -86,-3,85,85,-1,-86,-86,-4,0,0,0,0,0,0,62,-86
.byte -85,-11,85,-2,-86,-86,-86,-81,-43,85,-81,-22,-86,-68,0,0
.byte 0,0,0,0,-2,-86,-81,-43,87,-22,-86,-86,-86,-86,-11,90
.byte -1,-6,-86,-65,0,0,0,0,0,3,-6,-86,-65,85,95,-86
.byte -88,0,0,-86,-67,111,0,-2,-86,-81,-64,0,0,0,0,3
.byte -22,-86,-3,85,94,-86,-96,5,64,-86,-81,108,0,63,-81,-5
.byte -64,0,0,0,0,15,-22,-86,-11,85,126,-86,-96,21,80,-86
.byte -85,-20,0,63,-68,62,-16,0,0,0,0,63,-86,-85,-11,85
.byte 122,-86,-96,80,20,-86,-86,-68,0,15,-16,15,-68,0,0,0
.byte 0,63,-86,-81,-43,85,122,-86,-88,0,0,-86,-86,-68,0,15
.byte -64,3,-68,0,0,0,0,62,-86,-81,85,85,122,-86,-86,-86
.byte -86,-86,-86,-68,0,15,0,3,-68,0,0,0,0,-2,-86,-65
.byte 85,85,126,-81,-1,-1,-1,-1,-22,-4,0,12,0,15,-65,0
.byte 0,0,0,-6,-86,-67,85,85,95,-6,-86,-86,-86,-86,-65,-37
.byte 0,60,0,62,-81,0,0,0,0,-6,-86,-3,85,85,95,-81
.byte -1,-86,-85,-1,-21,-37,0,-16,0,-6,-81,0,0,0,3,-6
.byte -86,-11,85,85,127,-1,95,-1,-1,87,-1,-5,0,-64,3,-22
.byte -81,-64,0,0,3,-22,-85,-11,85,85,122,-43,5,85,85,65
.byte 94,-69,-1,-1,-53,-22,-85,-64,0,0,3,-22,-85,-43,85,85
.byte 123,-44,1,85,85,-64,95,-69,0,3,-4,-6,-85,-64,0,0
.byte 3,-22,-85,-43,85,-87,-5,80,63,84,87,-16,23,-68,0,0
.byte 44,62,-85,-64,0,0,15,-22,-85,-43,90,-2,-21,80,-1,84
.byte 87,-4,23,-82,-2,-128,60,14,-85,-16,0,0,15,-86,-81,-43
.byte 111,95,-85,83,-13,-48,95,63,23,-85,-41,-32,-16,14,-86,-16
.byte 0,0,15,-86,-81,85,-67,87,-85,83,-61,-47,95,15,23,-85
.byte 85,-8,-16,14,-86,-16,0,0,15,-86,-81,86,-11,87,-85,80
.byte -1,-47,95,-4,23,-85,85,126,-64,14,-86,-16,0,0,15,-86
.byte -81,86,-43,127,-65,80,-1,-15,95,-4,23,-5,-11,94,-64,62
.byte -86,-16,0,0,15,-86,-81,86,-43,31,-75,85,63,-63,87,-15
.byte 85,123,-47,94,-1,-6,-86,-16,0,0,15,-86,-81,86,-44,127
.byte -43,85,111,5,85,85,85,95,-12,94,-64,58,-86,-16,0,0
.byte 15,-86,-81,86,-44,87,87,-43,-80,21,85,85,87,-41,84,94
.byte -64,58,-86,-16,0,0,15,-86,-81,86,-43,21,87,-11,-80,85
.byte 85,85,95,-43,81,94,0,-6,-86,-16,0,0,15,-86,-81,86
.byte -11,21,95,-3,-79,85,85,85,127,-11,81,126,-1,-6,-86,-16
.byte 0,0,15,-86,-81,85,-67,85,95,-1,-15,85,85,-33,-1,-11
.byte 85,-13,-86,-6,-86,-16,0,0,15,-86,-81,85,111,85,87,-1
.byte -11,85,85,-1,-1,-43,87,3,-107,-6,-86,-16,0,0,15,-86
.byte -81,-43,90,-11,85,-1,-1,85,87,-1,-1,85,126,14,87,-6
.byte -86,-16,0,0,15,-22,-85,-43,85,-67,85,127,-1,-1,-1,-1
.byte -3,85,-1,-2,87,-22,-85,-16,0,0,3,-22,-85,-43,85,109
.byte 85,95,-1,-1,-1,-1,-11,85,-1,-7,87,-22,-85,-64,0,0
.byte 3,-22,-85,-43,85,109,85,87,-1,-1,-1,-1,-43,85,-1,-7
.byte 87,-22,-85,-64,0,0,3,-22,-85,-10,85,111,85,85,-1,-1
.byte -1,-3,85,87,-1,-27,95,-22,-85,-64,0,0,3,-6,-86,-5
.byte -106,-65,85,85,127,-1,-1,-43,85,87,-1,-27,95,-86,-81,-64
.byte 0,0,0,-6,-86,-5,-101,-1,-43,85,94,-65,-21,85,85,95
.byte -22,-86,126,-86,-81,0,0,0,0,-6,-86,-69,-81,-1,-11,85
.byte 95,-22,-81,85,85,126,-65,-1,-69,-1,-5,0,0,0,0,-2
.byte -86,-69,-65,-1,-1,85,87,-91,-83,85,87,-5,-64,5,-20,0
.byte 95,-128,0,0,0,62,-86,-85,-65,-1,-1,-11,-41,-22,-67,93
.byte 127,-20,5,85,113,85,86,-32,0,0,0,63,-86,-85,-65,-1
.byte -1,-1,-43,-1,-11,95,-1,-80,85,81,69,85,85,-72,0,0
.byte 0,15,-86,-85,-65,-1,-1,-22,-75,85,85,-6,-81,-79,69,69
.byte 85,85,85,-72,0,0,0,15,-22,-85,-65,-1,-1,-22,-67,85
.byte 87,-22,-86,-63,81,21,85,85,85,-82,0,0,0,3,-22,-85
.byte -65,-1,-1,-86,-81,-1,-1,-86,-86,-59,84,85,85,85,85,110
.byte 0,0,0,3,-6,-85,-65,-1,-2,-86,-81,-1,-1,-86,-86,-59
.byte 81,21,85,85,85,110,0,0,0,0,-2,-85,-81,-1,-6,-86
.byte -81,-1,-1,-81,-2,-59,69,69,85,85,85,110,0,0,0,0
.byte 62,-85,-85,-1,-6,-1,-81,-1,-1,-68,14,-43,21,85,85,85
.byte 85,110,0,0,0,0,63,-85,-86,-1,-21,-61,-17,-86,-81,-80
.byte 2,-43,85,85,85,85,85,-82,0,0,0,0,15,-21,-86,-65
.byte -5,0,-17,-86,-81,-80,3,-75,85,85,85,85,85,-72,0,0
.byte 0,0,3,-5,-86,-85,-1,0,-17,-86,-81,-80,3,-75,85,85
.byte 85,85,86,-72,0,0,0,0,0,-5,-86,-86,-1,-4,-17,-86
.byte -81,-68,-1,-19,85,85,85,85,86,-32,0,0,0,0,0,59
.byte -86,-86,-81,-1,-17,-86,-81,-81,-1,-19,85,85,85,85,90,-32
.byte 0,0,0,0,0,11,-86,-86,-86,-1,-1,-86,-81,-65,-1,-85
.byte 85,85,85,85,91,-128,0,0,0,0,0,11,-70,-86,-86,-81
.byte -1,-1,-1,-1,-6,-86,-43,85,85,85,110,0,0,0,0,0
.byte 0,11,-65,-86,-86,-86,-81,-1,-1,-6,-86,-86,-75,85,85,85
.byte -72,0,0,0,0,0,0,11,-65,-22,-86,-86,-86,-86,-86,-86
.byte -18,-86,-83,85,85,86,-32,0,0,0,0,0,0,11,-125,-2
.byte -86,-86,-86,-86,-86,-86,-2,-86,-65,85,85,91,-128,0,0,0
.byte 0,0,0,11,-128,-1,-22,-86,-86,-86,-86,-86,-18,-21,-2,-43
.byte 85,110,0,0,0,0,0,0,0,11,-128,15,-2,-86,-86,-86
.byte -86,-86,-86,-65,-16,-75,86,-72,0,0,0,0,0,0,0,11
.byte -128,0,-1,-6,-86,-86,-86,-86,-81,-1,0,47,91,-32,0,0
.byte 0,0,0,0,0,2,0,0,15,-1,-6,-86,-86,-81,-1,-16
.byte 0,10,-2,-128,0,0,0,0,0,0,0,0,0,0,0,63
.byte -1,-1,-1,-1,-4,0,0,0,-88,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,63,-1,-1,-4,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0


#ENDIF



.end
