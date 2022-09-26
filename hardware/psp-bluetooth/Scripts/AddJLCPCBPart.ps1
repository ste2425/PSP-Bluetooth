$partNumbers = Read-Host -Prompt 'Input JLCPCB part numbers'

JLC2KiCadLib $partNumbers -dir ../Library/JLCPCB -schematic_lib JLCPCB_schematic -footprint_lib JLCPCB_footprint

pause