OUTPUT_FORMAT("elf64-littleaarch64", "elf64-littleaarch64", "elf64-littleaarch64")
OUTPUT_ARCH(aarch64)
ENTRY(_start)
SECTIONS
{
 . = 0x00000000;
 .text : {
  . = ALIGN(8);
  *(.__image_copy_start)
  arch/arm/cpu/armv8/start.o (.text*)
  *(.text*)
 }
 .rodata : {
  . = ALIGN(8);
  *(SORT_BY_ALIGNMENT(SORT_BY_NAME(.rodata*)))
 }
 .data : {
  . = ALIGN(8);
  *(.data*)
 }
 .u_boot_list : {
  . = ALIGN(8);
  KEEP(*(SORT(.u_boot_list*)));
 }
 .image_copy_end : {
  . = ALIGN(8);
  *(.__image_copy_end)
 }
 .end : {
  . = ALIGN(8);
  *(.__end)
 }
 _image_binary_end = .;
 .bss_start (NOLOAD) : {
  . = ALIGN(8);
  KEEP(*(.__bss_start));
 }
 .bss (NOLOAD) : {
  *(.bss*)
   . = ALIGN(8);
 }
 .bss_end (NOLOAD) : {
  KEEP(*(.__bss_end));
 }
 /DISCARD/ : { *(.dynsym) }
 /DISCARD/ : { *(.dynstr*) }
 /DISCARD/ : { *(.dynamic*) }
 /DISCARD/ : { *(.plt*) }
 /DISCARD/ : { *(.interp*) }
 /DISCARD/ : { *(.gnu*) }
}
ASSERT(__image_copy_end - __image_copy_start < (188416), "TPL image too big");
