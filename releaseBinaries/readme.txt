Due to CORS issues downloading artifacts from github actions we instead commit the binary into this folder.

Then create the tag to create the release.

That means we should only have one binary in this folder but it is accessible from the raw api for the specific tag needed.

TLDR; ignore this folder, its for CI.