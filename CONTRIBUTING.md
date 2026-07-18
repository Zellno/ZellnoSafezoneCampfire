# Contributing

Follow the implementation order documented in `ZELLNO_MODDING_GUIDELINES.md`. Keep changes focused, preserve vanilla behavior where possible, and document every new dependency.

## Enforce Script formatting

The DayZ Enforce Script parser used by the tested Linux dedicated server was sensitive to some multiline expressions and function calls.

For compatibility, keep function calls, `if` conditions, and `foreach` expressions on a single logical line when practical. Formatting changes must be followed by a real dedicated-server compilation test.

## Before submitting a change

- Confirm that no private keys or production coordinates are included.
- Build the PBO successfully.
- Verify the signature and PBO prefix.
- Test both server and client loading.
- Inspect script logs for compilation and runtime errors.
- Update documentation and the changelog when behavior changes.
