	.export	_rom
	.export	_vectors
_rom:	.incbin	"o2rom.bin"
;cart:	.incbin	"computer intro.bin"
cart:	.incbin	"Gunfighter (Europe).bin"
.segment "VECTORS"
_vectors:
	.byte	$03, $00	; NMI
	.byte	$00, $00	; RESET
	.byte	$03, $00	; IRQ
