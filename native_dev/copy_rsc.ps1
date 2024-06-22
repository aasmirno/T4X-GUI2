# PI.ps1
# powershell copy files script

if (Test-Path -Path "build\") {
    # copy textures
    Get-ChildItem -Path "resources\textures" -Recurse |
        Copy-Item -Destination "build\"

    # copy shaders
    Get-Content "required_shaders_list.txt" | ForEach-Object {
        Get-ChildItem -Path "resources\$_" -Recurse |
        Copy-Item -Destination "build\"
    }
}
