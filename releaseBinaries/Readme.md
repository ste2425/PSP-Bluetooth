# Release Binaries Folder

Due to CORS issues the web app can't download artifacts directly from a GitHub release. 

The CI instead commits back into the repository the compiled binaries in this folder. It then creates a new tag with the release number.

The web app then uses GitHub's RAW api and the release tag to find the binaries needed to flash to the ESP32.

**TLDR; ignore this folder and its contents, its for CI.**