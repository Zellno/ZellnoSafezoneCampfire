# Build

## Toolchain

The current pipeline uses:

- `FileBank.exe`
- `DSSignFile.exe`
- `BankRev.exe`
- `CfgConvert.exe`

It does not require AddonBuilder, Workbench, a `P:\` drive, Wine Mono, or Visual Studio.

The build script expects DayZ Tools to be installed through Steam and uses Wine path conversion for the Windows utilities.

## Local path defaults

The current script reflects the tested Zellno Linux development layout:

- DayZ Tools under the default user Steam directory;
- signing keys under `$HOME/dayz/modding/keys/Zellno`;
- optional server installation under `$HOME/dayz/server`.

These are local defaults, not requirements of the PBO itself. Review and adapt
the variables near the beginning of `build.sh` when using another layout.

## Build command

From the repository root:

```bash
chmod +x build.sh
./build.sh
```

The validated result is written to:

```text
build/@ZellnoSafezoneCampfire
```

The default build performs no server installation.

For an explicitly approved local installation:

```bash
./build.sh --install
```

## Signing safety

The signing key path belongs to the local developer environment.

Never commit, publish, copy to a Workshop package, or otherwise distribute a `.biprivatekey` file. Only the public `.bikey` and generated `.bisign` belong in a distributable build.
