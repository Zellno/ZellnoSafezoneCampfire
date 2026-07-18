# Architecture

## Design

`Zellno_SafezoneCampfire` derives from the vanilla `Fireplace` class. The project does not replace ordinary fireplaces and does not repack its third-party dependency. This keeps administrative behavior isolated while retaining vanilla cooking mechanics.

## Components

- `zsc_settings.c` defines JSON settings, defaults, and validation.
- `zsc_safezonecampfire.c` defines the managed fireplace behavior.
- `zsc_zenactionpatch.c` extends the Zen action only for the Zellno class.
- `zsc_manager.c` creates, finds, configures, and reconciles campfires.
- `zsc_missionserver.c` starts the manager on the server.

## Configuration safety

Validation rejects incompatible versions, invalid intervals, null entries, missing or duplicate IDs, invalid enabled positions, invalid matching radii, and overlapping management areas.

The last applied configuration is stored separately so removed or disabled entries can be reconciled safely.

## Persistence

The manager searches for a matching persistent object before creating one. Startup reconciliation is idempotent and avoids duplicates.

## Third-party compatibility

Zenarchist's Fire Sticks remains an external dependency. The patch relaxes its terrain restriction only for `Zellno_SafezoneCampfire`; ordinary fireplaces retain the original behavior.
