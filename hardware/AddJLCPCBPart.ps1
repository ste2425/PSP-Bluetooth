$path = "$PSScriptRoot/lib/JLCPCB"

while ($true)
{
    $partNumbers = Read-Host -Prompt 'Input JLCPCB part numbers'

    JLC2KiCadLib $partNumbers -dir $path -symbol_lib JLCPCB_schematic -footprint_lib JLCPCB_footprint

}