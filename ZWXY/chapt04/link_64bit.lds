ENTRY(nomain)

SECTIONS
{
  . = 0x8048000 + SIZEOF_HEADERS;

  tinytext : { *(.text) *(.data) *(.data.rel.local) *(.rodata*) }

  /DISCARD/ : { *(*) }
}
