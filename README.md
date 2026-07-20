# Zellno Safezone Campfire

A configurable DayZ server mod that creates permanent administrative campfires for safezones while preserving vanilla cooking mechanics.

## Steam Workshop

- [Zellno Safezone Campfire](https://steamcommunity.com/sharedfiles/filedetails/?id=3767320261)
- Workshop ID: `3767320261`
- Required dependency: [Zenarchist's Fire Sticks](https://steamcommunity.com/sharedfiles/filedetails/?id=3498006019)

## Features

- Multiple campfires configured through JSON.
- Persistence-aware startup without duplicate creation.
- Permanent burning and automatic ignition.
- Optional immunity to rain and weather.
- Disabled fireplace area damage.
- Administrative fuel that players cannot remove.
- Protection against extinguishing, movement, cargo use, and stone upgrades.
- Vanilla tripods, cooking pots, and cooking mechanics.
- Zenarchist's Fire Sticks support on normally unsupported terrain.

## Requirements

- DayZ
- [Zenarchist's Fire Sticks](https://steamcommunity.com/sharedfiles/filedetails/?id=3498006019)

Zenarchist's Fire Sticks must be loaded before this mod. Both mods are required on the client and server.

```text
-mod="@ZensFireSticks;@ZellnoSafezoneCampfire"
```

## Installation

1. Install Zenarchist's Fire Sticks on the client and server.
2. Install `@ZellnoSafezoneCampfire` on the client and server.
3. Copy the Zellno public key to the server `keys` directory.
4. Add both mods to the launch parameters in dependency order.
5. Start the server once to generate the default configuration.
6. Stop the server and define the campfire positions.
7. Start the server again.

The configuration is generated at:

```text
$profile:ZellnoSafezoneCampfire/Config.json
```

The default entry is intentionally invalid until a real position is defined. This prevents a campfire from being created at the world origin.

## Configuration

A public example is available at `Extras/ExampleConfig/Config.json`.

| Field | Purpose |
|---|---|
| `ConfigVersion` | Configuration schema version. |
| `DebugEnabled` | Enables additional diagnostic logging. |
| `CheckIntervalSeconds` | Reconciliation interval. |
| `Id` | Unique stable identifier. |
| `Name` | Human-readable administrative name. |
| `Enabled` | Enables or disables the entry. |
| `Position` | World position in X, Y, Z order. |
| `Orientation` | Object orientation. |
| `MatchRadius` | Radius used to find the managed object. |
| `KeepBurning` | Prevents normal fuel consumption. |
| `IgnoreWeather` | Prevents rain from interfering with the fire. |
| `DisableFireDamage` | Disables the fireplace damage area. |
| `AllowExtinguish` | Controls the normal extinguish condition. |
| `AllowZenFireSticksOnConcrete` | Enables Zen sticks on unsupported terrain. |

Enabled campfires must not use `0 0 0`, duplicate IDs, or overlapping management areas.

## Persistence

Managed campfires use normal DayZ persistence. On startup, the manager reuses an existing matching campfire instead of creating a duplicate.

Zen Fire Sticks intentionally drop onto the ground after a restart. This is behavior implemented by the original dependency and is not overridden here.

## Repository utility scripts

`build.sh` implements the build and optional local installation workflow used
by the Zellno development environment.

`install_local_test.py` is an environment-specific maintenance tool, not a
general-purpose installer. It assumes the Zellno directory layout under
`$HOME/dayz`, a Linux Steam installation, and existing server launch scripts.

The installer runs in simulation mode by default. It changes the local
environment only when explicitly invoked with `--apply`.

Users with a different directory layout should review and adapt the constants
before using either script.

## Documentation

- [Architecture](ARCHITECTURE.md)
- [Build](BUILD.md)
- [Testing](TESTING.md)
- [Third-party software](THIRD_PARTY.md)
- [Contributing](CONTRIBUTING.md)
- [Zellno Modding Guidelines](ZELLNO_MODDING_GUIDELINES.md)
- [Changelog](CHANGELOG.md)

## License

Original Zellno source code in this repository is released under the [MIT License](LICENSE).

Zenarchist's Fire Sticks remains an external dependency and is not redistributed by this repository.

## Monetization Permission

Zellno permits the use of Zellno Safezone Campfire on monetized DayZ servers, provided that the server operator is registered, approved, and listed under Bohemia Interactive's DayZ Server Monetization program and complies with all applicable rules.

This permission applies only to Zellno Safezone Campfire. It does not grant permission to monetize Zenarchist's Fire Sticks or any other third-party dependency. Server operators are responsible for obtaining any additional permissions required by those authors.

- [Official monetization rules](https://www.bohemia.net/monetization)
- [Approved DayZ servers](https://www.bohemia.net/monetization/approved/dayz)
