# Qt6 CMake QML app for Windows (Microsoft Store)

A simple QML project, with CMake code to build, package, bundle and sign the app for Windows, ready for uploading to the Microsoft Store as a Windows 10/11 PC app.

The app has two QML files, `Main.qml` and `Disc.qml`. When running correctly it generates a stream of coloured discs at random positions (or at the mouse position if the mouse is over the application window), which move around and change colour before disappearing. The only QML module used is Quick.

## How to use

This project needs Qt 6.4+.

Install and select the corresponding MSVC 64-bit kit (e.g. "Qt 6.5.2 MSVC2019 64bit")

Build and run the project to test it locally.

## Create a local self-signed certificate for code-signing

To prepare the app for the Microsoft Store, or to install it on your local machine, you'll need a private certificate key to code-sign the app.

For a published app you would need to download this via Visual Studio.

For local installation, we're using a self-signed certificate.

To create this, run the script `MakeSelfSignedCertificate.ps1` from PowerShell.

This will generate a local Certificate-Signing Authority (CA) called "Qt.Experiment.Authority" and a local Certificate, using that authority, called "Qt.Experiment.Certificate" and will save the `.pfx` file in the project source folder.

1. Run PowerShell as Admin.
1. Navigate to your source folder.
1. Enter `.\MakeSelfSignedCertificate.ps1`.
1. You will be asked for a password. Choose your own but make a note of it.
1. Check that the certificate files have been created in your source folder.
1. Check that the certificates exist by running the Windows certificate manager. (Settings > Manage User Certificates; or Win + R, `certmgr.msc`). Look in **Personal > Certificates** for `Qt.Experiment.Certificate` and **Trusted Root Certification > Certificates** for `Qt.Experiment.Authority`. (NB: these don't auto-refresh. Use F5)

## Configure the Release build, then build and install

Configure the Release build as follows:
- set CMake Cache Variables:
    - `APP_PACKAGE_SIGN` (bool): `ON`
    - `APP_BUNDLE` (bool): `ON`
    - `PFX_SIGNATURE_KEY` (string): `LocalQtExperimentKey.pfx` (or your store's private certificate, which must be saved in the project source folder
    - `PFX_PASSWORD` (string): password for your private certificate key
    - `CMAKE_INSTALL_PREFIX` (string): `your-build-directory/install` (within Qt Creator, you can just paste `%{BuildConfig:BuildDirectory:NativeFilePath}/install`)
- After running CMake and then Build, ensure you run the Install step.

## Easy guide for Qt Creator users

If, like me, you're using Qt Creator, you can do the above as follows:
1. Go to the Build Settings (**Project** on the left menu)
1. Edit the **Release** build configuration.
1. Select **Initial Configuration** click **Re-configure with Initial Parameters**. This resets the parameters, runs CMake and sets things up for customising.
1. Select **Current Configuration**, tick the `APP_PACKAGE_SIGN` row, tick the `APP_BUNDLE` row and add/edit the other keys. Then click **Run CMake**
1. Scroll down to **Build Steps** where you'll see the default CMake command. Click **Details**. Tick **install** as well as **all**.
1. Build the project and you should see the additional steps happen in the **Compile Output** panel.

## Check the app package

You can do a simple visual check of the app package. It will be in the build folder, within the `packages` folder. Rename `Qt6-CMake-QML_x64.appx` to `Qt6-CMake-QML_x64.zip` and examine its contents. It should contain:
- AppxManifest.xml
- Assets (containing the icon set and splash screen images)
- plugins (containing various Qt DLLs, e.g. `platform\qwindows.dll`)
- qml (containing a QtQuick folder)
- bin, containing:
    - The .exe
    - qt.conf (which should just reference `..`)
    - Qt DLLs, e.g. QtCore.dll
    - VC runtime DLLs, e.g. `vcruntime140.dll`
    
## Install the app

The app bundle will be in the build directory `Qt6-CMake-QML_x64.appxbundle`
Double-click this to open the installer.
Assuming you've correctly created the private certificate key and the signing step worked this should allow you to install the app.

### Don't need a bundle?

If you only need a single package rather than a bundle, set `APP_BUNDLE`: `OFF`.
This will skip the bundle step and will sign the package instead.
