
        .export _crc16_ccitt
        .importzp tmp1, tmp2, tmp3
        .import popax

.code

CRCLO   := tmp1
CRCHI   := tmp2

;; Ad[ao]pted from http://6502.org/source/integers/crc-more.html (Greg Cook)

;; uint16t __fastcall__ crc16_ccitt(uint16_t crc, uint8_t input)
.proc   _crc16_ccitt
        STA tmp3
        JSR popax
        STA CRCLO
        STX CRCHI
        LDA tmp3

        EOR CRCHI       ; A contained the data
        STA CRCHI       ; XOR it into high byte
        LSR             ; right shift A 4 bits
        LSR             ; to make top of x^12 term
        LSR             ; ($1...)
        LSR
        TAX             ; save it
        ASL             ; then make top of x^5 term
        EOR CRCLO       ; and XOR that with low byte
        STA CRCLO       ; and save
        TXA             ; restore partial term
        EOR CRCHI       ; and update high byte
        STA CRCHI       ; and save
        ASL             ; left shift three
        ASL             ; the rest of the terms
        ASL             ; have feedback from x^12
        TAX             ; save bottom of x^12
        ASL             ; left shift two more
        ASL             ; watch the carry flag
        EOR CRCHI       ; bottom of x^5 ($..2.)
        TAY             ; save high byte
        TXA             ; fetch temp value
        ROL             ; bottom of x^12, middle of x^5!
        EOR CRCLO       ; finally update low byte
        ;STA CRCHI      ; then swap high and low bytes
        ;STY CRCLO

        TAX             ; Store result in registers
        TYA

        RTS
.endproc
