; generated by Component: ARM Compiler 5.06 update 6 (build 750) Tool: ArmCC [4d3637]
; commandline ArmCC [--c99 --split_sections --debug -c -S -o.\objects\diskio.s --depend=.\objects\diskio.d --cpu=Cortex-M4.fp --apcs=interwork -O0 --diag_suppress=9931 -I..\..\USER\bsp_nucleof411Re -I..\..\USER\bsp_nucleof411Re\inc -I..\..\Libraries\STM32F4xx_StdPeriph_Driver\inc -I..\..\Libraries\CMSIS\Include -I..\..\Libraries\CMSIS\Device\ST\STM32F4xx\Include -I..\..\USER\IAP -I..\..\USER\FatFS\src -I..\..\Freertos\include -I..\..\Freertos\portable\RVDS\ARM_CM4F -I..\..\USER -I.\RTE\_Target_1 -IC:\Keil_v5\ARM\PACK\Keil\STM32F4xx_DFP\2.13.0\Drivers\CMSIS\Device\ST\STM32F4xx\Include -IC:\Keil_v5\ARM\CMSIS\Include -IC:\Keil_v5\ARM\PACK\Keil\STM32F4xx_DFP\2.13.0 -D__MICROLIB -D__UVISION_VERSION=525 -DSTM32F411xE -DUSE_STDPERIPH_DRIVER -DSTM32F411xE --signed_chars --omf_browse=.\objects\diskio.crf ..\..\USER\fatfs\src\diskio.c]
        THUMB
        REQUIRE8
        PRESERVE8

        AREA ||i.disk_initialize||, CODE, READONLY, ALIGN=1

disk_initialize PROC
        PUSH     {r4-r6,lr}
        MOV      r4,r0
        MOVS     r5,#1
        CMP      r4,#5
        BCS      |L0.42|
        TBB      [pc,r4]
        DCB      0x03,0x0a
        DCB      0x0b,0x0c,0x0d,0x00
        BL       SD_Init
        CBNZ     r0,|L0.30|
        MOVS     r5,#0
        B        |L0.32|
|L0.30|
        MOVS     r5,#2
|L0.32|
        B        |L0.44|
        B        |L0.44|
        B        |L0.44|
        B        |L0.44|
        B        |L0.44|
|L0.42|
        NOP      
|L0.44|
        NOP      
        MOV      r0,r5
        POP      {r4-r6,pc}
        ENDP


        AREA ||i.disk_ioctl||, CODE, READONLY, ALIGN=1

disk_ioctl PROC
        PUSH     {r4,r5,lr}
        MOV      r3,r0
        CMP      r3,#5
        BCS      |L1.92|
        TBB      [pc,r3]
        DCB      0x03,0x06,0x25,0x26
        DCB      0x27,0x00
        MOVS     r4,#0
        MOV      r0,r4
|L1.22|
        POP      {r4,r5,pc}
        MOVS     r0,#0
        MOVS     r0,#1
        CBZ      r1,|L1.44|
        CMP      r1,#1
        BEQ      |L1.48|
        CMP      r1,#2
        BEQ      |L1.58|
        CMP      r1,#3
        BNE      |L1.78|
        B        |L1.68|
|L1.44|
        MOVS     r0,#0
        B        |L1.82|
|L1.48|
        MOV      r5,#0x40000
        STR      r5,[r2,#0]
        MOVS     r0,#0
        B        |L1.82|
|L1.58|
        MOV      r5,#0x200
        STRH     r5,[r2,#0]
        MOVS     r0,#0
        B        |L1.82|
|L1.68|
        MOV      r5,#0x200
        STR      r5,[r2,#0]
        MOVS     r0,#0
        B        |L1.82|
|L1.78|
        MOVS     r0,#4
        NOP      
|L1.82|
        NOP      
        B        |L1.22|
        NOP      
        B        |L1.92|
        NOP      
|L1.92|
        NOP      
        MOVS     r0,#4
        B        |L1.22|
        ENDP


        AREA ||i.disk_read||, CODE, READONLY, ALIGN=1

disk_read PROC
        PUSH     {r2-r10,lr}
        MOV      r5,r0
        MOV      r8,r1
        MOV      r6,r2
        MOV      r7,r3
        CMP      r5,#5
        BCS      |L2.124|
        TBB      [pc,r5]
        DCB      0x03,0x30,0x31,0x32
        DCB      0x33,0x00
        MOV      r9,#0
        CMP      r7,#1
        BNE      |L2.54|
        LSLS     r0,r7,#9
        LSLS     r1,r6,#9
        MOV      r2,r1
        MOVS     r3,#0
        STR      r0,[sp,#0]
        MOV      r0,r8
        BL       SD_ReadBlock
        MOV      r9,r0
        B        |L2.76|
|L2.54|
        MOV      r0,#0x200
        LSLS     r1,r6,#9
        MOV      r2,r1
        MOVS     r3,#0
        STRD     r0,r7,[sp,#0]
        MOV      r0,r8
        BL       SD_ReadMultiBlocks
        MOV      r9,r0
|L2.76|
        CMP      r9,#0
        BEQ      |L2.86|
        MOVS     r4,#1
        B        |L2.126|
|L2.86|
        BL       SD_WaitReadOperation
        MOV      r9,r0
        CMP      r9,#0
        BEQ      |L2.102|
        MOVS     r4,#1
        B        |L2.126|
|L2.102|
        NOP      
|L2.104|
        BL       SD_GetStatus
        CMP      r0,#0
        BNE      |L2.104|
        MOVS     r4,#0
        B        |L2.126|
        B        |L2.126|
        B        |L2.126|
        B        |L2.126|
        B        |L2.126|
|L2.124|
        NOP      
|L2.126|
        NOP      
        MOV      r0,r4
        POP      {r2-r10,pc}
        ENDP


        AREA ||i.disk_status||, CODE, READONLY, ALIGN=1

disk_status PROC
        MOV      r1,r0
        MOVS     r0,#1
        CMP      r1,#5
        BCS      |L3.38|
        TBB      [pc,r1]
        DCB      0x03,0x05,0x07,0x09
        DCB      0x0b,0x00
        MOVS     r0,#0
        B        |L3.42|
        MOVS     r0,#0
        B        |L3.42|
        MOVS     r0,#0
        B        |L3.42|
        MOVS     r0,#0
        B        |L3.42|
        MOVS     r0,#0
        B        |L3.42|
|L3.38|
        MOVS     r0,#0
        NOP      
|L3.42|
        NOP      
        BX       lr
        ENDP


        AREA ||i.disk_write||, CODE, READONLY, ALIGN=1

disk_write PROC
        PUSH     {r2-r10,lr}
        MOV      r4,r0
        MOV      r8,r1
        MOV      r5,r2
        MOV      r6,r3
        CMP      r4,#5
        BCS      |L4.130|
        TBB      [pc,r4]
        DCB      0x03,0x33,0x34,0x35
        DCB      0x36,0x00
        MOV      r9,#0
        CMP      r6,#1
        BNE      |L4.54|
        LSLS     r0,r6,#9
        LSLS     r1,r5,#9
        MOV      r2,r1
        MOVS     r3,#0
        STR      r0,[sp,#0]
        MOV      r0,r8
        BL       SD_WriteBlock
        MOV      r9,r0
        B        |L4.76|
|L4.54|
        MOV      r0,#0x200
        LSLS     r1,r5,#9
        MOV      r2,r1
        MOVS     r3,#0
        STRD     r0,r6,[sp,#0]
        MOV      r0,r8
        BL       SD_WriteMultiBlocks
        MOV      r9,r0
|L4.76|
        CMP      r9,#0
        BEQ      |L4.84|
        MOVS     r7,#1
|L4.84|
        BL       SD_WaitReadOperation
        MOV      r9,r0
        CMP      r9,#0
        BEQ      |L4.98|
        MOVS     r7,#1
|L4.98|
        NOP      
|L4.100|
        BL       SD_GetStatus
        CMP      r0,#0
        BNE      |L4.100|
        NOP      
|L4.110|
        BL       SD_GetStatus
        CMP      r0,#0
        BNE      |L4.110|
        MOVS     r7,#0
        B        |L4.134|
        B        |L4.134|
        B        |L4.134|
        B        |L4.134|
        B        |L4.134|
|L4.130|
        MOVS     r7,#4
        NOP      
|L4.134|
        NOP      
        MOV      r0,r7
        POP      {r2-r10,pc}
        ENDP


        AREA ||i.get_fattime||, CODE, READONLY, ALIGN=2

get_fattime PROC
        LDR      r0,|L5.16|
        LDR      r0,[r0,#0]  ; i
        ADDS     r0,r0,#1
        LDR      r1,|L5.16|
        STR      r0,[r1,#0]  ; i
        MOV      r0,r1
        LDR      r0,[r0,#0]  ; i
        BX       lr
        ENDP

|L5.16|
        DCD      i

        AREA ||.arm_vfe_header||, DATA, READONLY, NOALLOC, ALIGN=2

        DCD      0x00000000

        AREA ||.data||, DATA, ALIGN=2

i
        DCD      0x00000000

;*** Start embedded assembler ***

#line 1 "..\\..\\USER\\fatfs\\src\\diskio.c"
	AREA ||.rev16_text||, CODE
	THUMB
	EXPORT |__asm___8_diskio_c_b3252454____REV16|
#line 138 "..\\..\\Libraries\\CMSIS\\Include\\core_cmInstr.h"
|__asm___8_diskio_c_b3252454____REV16| PROC
#line 139

 rev16 r0, r0
 bx lr
	ENDP
	AREA ||.revsh_text||, CODE
	THUMB
	EXPORT |__asm___8_diskio_c_b3252454____REVSH|
#line 153
|__asm___8_diskio_c_b3252454____REVSH| PROC
#line 154

 revsh r0, r0
 bx lr
	ENDP
	AREA ||.rrx_text||, CODE
	THUMB
	EXPORT |__asm___8_diskio_c_b3252454____RRX|
#line 328
|__asm___8_diskio_c_b3252454____RRX| PROC
#line 329

 rrx r0, r0
 bx lr
	ENDP

;*** End   embedded assembler ***

        EXPORT disk_initialize [CODE]
        EXPORT disk_ioctl [CODE]
        EXPORT disk_read [CODE]
        EXPORT disk_status [CODE]
        EXPORT disk_write [CODE]
        EXPORT get_fattime [CODE]
        EXPORT i [DATA,SIZE=4]

        IMPORT ||Lib$$Request$$armlib|| [CODE,WEAK]
        IMPORT SD_Init [CODE]
        IMPORT SD_ReadBlock [CODE]
        IMPORT SD_ReadMultiBlocks [CODE]
        IMPORT SD_WaitReadOperation [CODE]
        IMPORT SD_GetStatus [CODE]
        IMPORT SD_WriteBlock [CODE]
        IMPORT SD_WriteMultiBlocks [CODE]

        ATTR FILESCOPE
        ATTR SETVALUE Tag_ABI_PCS_wchar_t,2
        ATTR SETVALUE Tag_ABI_enum_size,1
        ATTR SETVALUE Tag_ABI_optimization_goals,6
        ATTR SETSTRING Tag_conformance,"2.09"
        ATTR SETVALUE AV,18,1

        ASSERT {ENDIAN} = "little"
        ASSERT {INTER} = {TRUE}
        ASSERT {ROPI} = {FALSE}
        ASSERT {RWPI} = {FALSE}
        ASSERT {IEEE_FULL} = {FALSE}
        ASSERT {IEEE_PART} = {FALSE}
        ASSERT {IEEE_JAVA} = {FALSE}
        END
