# Zellno Modding Guidelines

## Project licensing

Original mods in the Zellno ecosystem use the MIT License for original Zellno source code and these guidelines as the project philosophy.

Zellno projects prioritize:

- independent patches;
- reuse of vanilla mechanics whenever possible;
- respect for original authors' permissions;
- clear attribution;
- documented dependencies;
- compatibility with future upstream updates.

The preferred outcome is a project the community can use, study, modify, and distribute while respecting every author involved.

## Build pipeline

Tools currently used:

- `FileBank.exe`
- `DSSignFile.exe`
- `BankRev.exe`
- `CfgConvert.exe`

Tools and environments not currently required:

- `AddonBuilder.exe`
- Wine Mono
- Workbench
- `P:\` drive
- Visual Studio

```text
Source code -> FileBank.exe -> PBO -> DSSignFile.exe -> Signature -> Local test -> Publication
```

Only projects that genuinely require Workbench assets should use the traditional DayZ Tools pipeline.

## Preferred implementation order

1. Understand the Bohemia Interactive vanilla implementation.
2. Determine whether an independent patch is sufficient.
3. Use the original mod as a dependency whenever possible.
4. Create an original reusable module when necessary.
5. Fork or repack only when technically necessary and properly permitted.

Always prioritize the least invasive and most compatible solution.
