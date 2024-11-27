# Installer

This is the source code the the Angular Web app.

It is built in TypeScript and uses Angular Material for its UI components.

It also uses Web Serial and Web Bluetooth API's for programming the ESP.

Serial programming is provided by the [ESPtool-js](https://github.com/espressif/esptool-js) library provided by ESpressif.

As such it only works on Chromium bases browsers.

# Release binaries for the web app Issue

See the readme [here](../releaseBinaries/Readme.md)

# Installation

Ensure NodeJS is installed and the command `npm` is globally available.

Within the `installer` folder run `npm install`.

# Starting

Within the `installer` folder run `npm start`. The terminal output will report the URL for the web app locally.

# Hosting in GitHub

See the `buildPSPBluetoothWebProgrammer.yml` file in the `.github/workflows` folder in the root.

## Routing problem

The app is hosted within GitHub and deployed using GitHub actions. There was some jiggery-pokery to make the routing work.

GitHub expects a static site where every page is its own `html` file. Angular is an SPA where there is a single root `index.html` file and routing is handled via JS.

These two worlds clash. If you hit the root of the site, navigate to another page say `/serial` then hit F5 you will get a `404` page from GitHub. This is because GitHub is looking for a html file called `serial.html` which obviously does not exist.

### Routing Solution

The solution is to duplicate, via CI, the compiled `index.html` and rename it for every route the application has.

## Route Case Sensitivity Problem

GitHub pages routes are case sensitive. IF the route is `/serial` but you hit `/Serial` it will get a `404`.

What we do is have a `404.html` page in the root which GitHub actions hits. Some JS checks the current URL, lower cases it and compares it to a list of expected routes. If it matches it will redirect the user to that correct casing of that route.