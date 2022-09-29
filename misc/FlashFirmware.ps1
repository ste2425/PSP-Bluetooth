echo "PSP Bluetooth Controller NINA firmware flasher. `nLoading..."

# Check if Python is installed
$python = Invoke-Expression "python -V" 2>&1

if (-not $python -or $python -notlike "python 3*") {
    echo "Python 3 must be installed`n"
    #pause 
    #exit
}

echo "`nreset`n"

pause

# List COMPorts and request user select the one the Arduino is using
$ports = Get-CimInstance -Class Win32_SerialPort | Select-Object Name, Description, DeviceID

echo "`nPlease enter the COMPort which the Arduino is connected to.`nNote: This IS case sensitive.`nIf the Arduino is not connected, connect it and re-run this script.`n"

echo $ports | Format-Table Name, Description, @{L='COMPort';E={$_.DeviceID}}

$port = Read-Host -Prompt "COMPort"

# Check if the provided port is in the list to select from
$isValid = ($ports | Select-Object -ExpandProperty DeviceID).Contains($port)

if (-not $isValid) {
    echo "$port is not a valid COMPort.`nBe sure to use the correct capitalization."
    #exit
}

echo "`n"

$workingDirectory = Split-Path $MyInvocation.MyCommand.Path -Parent

#Flash the passthrough sketch

echo "I will now flash the pass through sketch`n"
# bossac version 1.7.0-arduino3
$bossacCommand = @"
    & "$workingDirectory\bossac.exe" -i -d --port=$port -U true -i -e -w -v "$workingDirectory\SerialNINAPassthrough.ino.bin" -R
"@

Invoke-Expression $bossacCommand

echo "`n"

echo "Flashing firmware `"bluepad32-nina-full-v3.5.0-rc0.bin`"`n"

# Flash the Arduino Nina firmware
$command = @"
    & "$workingDirectory\esptool.py.exe" --port $port --baud 115200 --before no_reset write_flash 0x0000 "$workingDirectory\bluepad32-nina-full-v3.5.0-rc0.bin"
"@

Invoke-Expression $command

echo "`n"
pause
