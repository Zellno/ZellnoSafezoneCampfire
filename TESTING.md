# Testing

## Verified behavior

The following behavior was verified on a DayZ 1.29 Linux dedicated server:

- Game, World, and Mission script modules compile.
- Multiple configured campfires run simultaneously.
- Existing persistent campfires are reused after restart.
- No duplicate campfire is created after restart.
- Administrative fuel remains installed and cannot be removed.
- Permanent burning survives rain.
- Normal fireplace area damage is disabled.
- Extinguishing with a canteen or cooking pot is unavailable.
- The campfire cannot be moved into hands or cargo.
- Cargo items cannot be inserted into the campfire.
- Stones cannot be attached.
- Stone circles and ovens cannot be built.
- Vanilla tripods, cooking pots, and cooking behavior work.
- Three Zen Fire Sticks can cook on unsupported terrain.
- Zen Fire Sticks drop onto the ground after restart as designed upstream.

Private production coordinates are deliberately excluded from this repository.

## Recommended release checks

1. Build and validate the PBO.
2. Verify its prefix with BankRev.
3. Verify the signature.
4. Compare the build and installed PBO hashes.
5. Start the dedicated server and inspect the latest script log.
6. Connect with a client using the same dependency order.
7. Test persistence through one complete server restart.
