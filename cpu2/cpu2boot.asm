;//###########################################################################
;//
;// FILE:  F2837xD_CodeStartBranch.asm
;//
;// TITLE: Branch for redirecting code execution after boot.
;//
;// For these examples, code_start is the first code that is executed after
;// exiting the boot ROM code.
;//
;// The codestart section in the linker cmd file is used to physically place
;// this code at the correct memory location.  This section should be placed
;// at the location the BOOT ROM will re-direct the code to.  For example,
;// for boot to FLASH this code will be located at 0x3f7ff6.
;//
;// In addition, the example F2837xD projects are setup such that the codegen
;// entry point is also set to the code_start label.  This is done by linker
;// option -e in the project build options.  When the debugger loads the code,
;// it will automatically set the PC to the "entry point" address indicated by
;// the -e linker option.  In this case the debugger is simply assigning the PC,
;// it is not the same as a full reset of the device.
;//
;// The compiler may warn that the entry point for the project is other then
;//  _c_init00.  _c_init00 is the C environment setup and is run before
;// main() is entered. The code_start code will re-direct the execution
;// to _c_init00 and thus there is no worry and this warning can be ignored.
;//
;//###########################################################################

    .global cpu2boot
    .global _main2

***********************************************************************
* Function: codestart section
* Description: Branch to code starting point
***********************************************************************

    .sect "cpu2bootsect"
    .retain

cpu2boot:

    SETC OBJMODE        ;Set OBJMODE for 28x object code
    EALLOW              ;Enable EALLOW protected register access
    MOVZ DP, #7029h>>6  ;Set data page for WDCR register
    MOV @7029h, #0068h  ;Set WDDIS bit in WDCR to disable WD
    EDIS                ;Disable EALLOW protected register access

****************************************************************************
*  INITIALIZE STACK POINTER.                                               *
****************************************************************************
	MOV  	SP, #0x128  ; set to beginning of stack space

****************************************************************************
* INITIALIZE STATUS BIT FIELDS *NOT* INITIALIZED AT RESET                  *
****************************************************************************
	SPM	0			; set product shift to 0

****************************************************************************
* SET C28x MODES                                                           *
****************************************************************************
        C28OBJ                          ; select C28x object mode
        C28ADDR                         ; clear the addressing mode
        C28MAP                          ; set block M0 and M1 mode

        .if .TMS320C2800_FPU32
        SETFLG            RNDF32=1      ; Enable rounding in FPU32 mode.
        .endif
        .if .TMS320C2800_FPU64
        SETFLG            RNDF64=1      ; Enable rounding in FPU64 mode.
        .endif
****************************************************************************
* SETTING THESE STATUS BITS/REGISTER TO RESET VALUES.  IF YOU RUN          *
* _c_int00 FROM RESET, YOU CAN REMOVE THIS CODE                            *
****************************************************************************
	CLRC    PAGE0			; use stack addressing mode
        MOVW    DP,#0			; initialize DP to point at low 64K
	CLRC    OVM         		; turn off overflow mode

	ASP				; ensure SP is aligned

****************************************************************************
*  CALL USER'S PROGRAM                                                     *
****************************************************************************
	LCR     _main2			; execute main2()

    ESTOP0

cpu2tyjdeadloop:
    LB cpu2tyjdeadloop

	.end
