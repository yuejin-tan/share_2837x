       .def _F28x_usDelay2

       .cdecls LIST ;;Used to populate __TI_COMPILER_VERSION__ macro
       %{
       %}

       .sect "ramfuncs2"         ;;Used with compilers older than v15.9.0

        .global  __F28x_usDelay2
_F28x_usDelay2:
        SUB    ACC,#1
        BF     _F28x_usDelay2,GEQ    ;; Loop if ACC >= 0
        LRETR

;There is a 9/10 cycle overhead and each loop
;takes five cycles. The LoopCount is given by
;the following formula:
;  DELAY_CPU_CYCLES = 9 + 5*LoopCount
; LoopCount = (DELAY_CPU_CYCLES - 9) / 5
; The macro DELAY_US(A) performs this calculation for you
;
;

;//
;// End of file
;//
