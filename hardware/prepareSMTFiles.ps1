echo "Generate SMT files for project"

$files = Get-ChildItem -Path $PSScriptRoot -Directory
$fileChoices = @()

for ($i=0; $i -lt $files.Count; $i++) {
  $fileChoices += [System.Management.Automation.Host.ChoiceDescription]("$($files[$i].Name) &$($i+1)")
}

$userChoice = $host.UI.PromptForChoice('Select Project', '', $fileChoices, 0)

$project = $files[$userChoice].FullName

$output = $project + "/SMT Assembly Files"


echo "Generating for project $project..."
echo "Outputting to $output"

jlc-kicad-tools $project -o $output

pause