#!/usr/bin/env python3

from __future__ import annotations

import argparse
import datetime
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys


HOME = Path.home()

PROJECT = HOME / "dayz/modding/ZellnoSafezoneCampfire"

ZEN_SOURCE = (
    HOME
    / ".local/share/Steam/steamapps/workshop"
    / "content/221100/3498006019"
)

ZELLNO_BUILD = (
    PROJECT
    / "build/@ZellnoSafezoneCampfire"
)

SERVER = HOME / "dayz/server"
SERVER_ZEN = SERVER / "@ZensFireSticks"
SERVER_ZELLNO = SERVER / "@ZellnoSafezoneCampfire"
SERVER_KEYS = SERVER / "keys"

DAYZ_CLIENT = (
    HOME
    / ".local/share/Steam/steamapps/common/DayZ"
)

CLIENT_ZEN = DAYZ_CLIENT / "@ZensFireSticks"
CLIENT_ZELLNO = DAYZ_CLIENT / "@ZellnoSafezoneCampfire"

START_SH = SERVER / "start.sh"
CONNECT_SH = HOME / "dayz_connect.sh"

ZEN_KEY_SOURCE = ZEN_SOURCE / "keys/Zenarchist.bikey"
ZEN_KEY_TARGET = SERVER_KEYS / "Zenarchist.bikey"

MODS_TO_APPEND = [
    "@ZensFireSticks",
    "@ZellnoSafezoneCampfire",
]


def server_is_running() -> bool:
    result = subprocess.run(
        ["pgrep", "-x", "DayZServer"],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
        check=False,
    )
    return result.returncode == 0


def require_file(path: Path) -> None:
    if not path.is_file():
        raise RuntimeError(f"Arquivo obrigatório ausente: {path}")


def require_directory(path: Path) -> None:
    if not path.is_dir():
        raise RuntimeError(f"Diretório obrigatório ausente: {path}")


def updated_mod_script(path: Path) -> str:
    text = path.read_text(encoding="utf-8")

    pattern = re.compile(
        r'(?m)^(?P<prefix>-mod=")'
        r'(?P<mods>[^"]*)'
        r'(?P<suffix>".*)$'
    )

    matches = list(pattern.finditer(text))
    if len(matches) != 1:
        raise RuntimeError(
            f"Esperava exatamente uma linha -mod em: {path}"
        )

    match = matches[0]
    mods = [
        item
        for item in match.group("mods").split(";")
        if item
    ]

    for mod in MODS_TO_APPEND:
        if mod not in mods:
            mods.append(mod)

    replacement = (
        match.group("prefix")
        + ";".join(mods)
        + match.group("suffix")
    )

    return (
        text[:match.start()]
        + replacement
        + text[match.end():]
    )


def validate_link(path: Path, expected: Path) -> None:
    if path.is_symlink():
        actual = path.resolve()
        if actual != expected.resolve():
            raise RuntimeError(
                f"Link existente aponta para destino inesperado:\n"
                f"{path}\nAtual: {actual}\nEsperado: {expected}"
            )
    elif path.exists():
        raise RuntimeError(
            f"O caminho existe e não é link simbólico: {path}"
        )


def main() -> int:
    parser = argparse.ArgumentParser(
        description=(
            "Instala Zen Fire Sticks e "
            "Zellno Safezone Campfire localmente."
        )
    )
    parser.add_argument(
        "--apply",
        action="store_true",
        help="Aplica as mudanças. Sem esta opção, apenas simula.",
    )
    args = parser.parse_args()

    require_directory(ZEN_SOURCE)
    require_file(ZEN_SOURCE / "addons/ZenFireplaceStick.pbo")
    require_file(ZEN_KEY_SOURCE)

    require_directory(ZELLNO_BUILD)
    require_file(
        ZELLNO_BUILD
        / "addons/ZellnoSafezoneCampfire.pbo"
    )
    require_file(
        ZELLNO_BUILD
        / "addons/ZellnoSafezoneCampfire.pbo.Zellno.bisign"
    )

    require_file(START_SH)
    require_file(CONNECT_SH)
    require_directory(DAYZ_CLIENT)

    if server_is_running():
        raise RuntimeError(
            "O DayZServer está rodando. Encerre-o primeiro."
        )

    if SERVER_ZEN.exists() or SERVER_ZEN.is_symlink():
        raise RuntimeError(
            f"Destino do Zen já existe: {SERVER_ZEN}"
        )

    if SERVER_ZELLNO.exists() or SERVER_ZELLNO.is_symlink():
        raise RuntimeError(
            f"Destino Zellno já existe: {SERVER_ZELLNO}"
        )

    validate_link(CLIENT_ZEN, ZEN_SOURCE)
    validate_link(CLIENT_ZELLNO, SERVER_ZELLNO)

    new_start = updated_mod_script(START_SH)
    new_connect = updated_mod_script(CONNECT_SH)

    print("============================================================")
    print(" Zellno Safezone Campfire — instalação local")
    print("============================================================")
    print()
    print(f"Modo: {'APLICAÇÃO' if args.apply else 'SIMULAÇÃO'}")
    print()
    print(f"Copiar Workshop:\n  {ZEN_SOURCE}\n  -> {SERVER_ZEN}")
    print()
    print(f"Copiar build:\n  {ZELLNO_BUILD}\n  -> {SERVER_ZELLNO}")
    print()
    print(f"Copiar chave:\n  {ZEN_KEY_SOURCE}\n  -> {ZEN_KEY_TARGET}")
    print()
    print(f"Criar link:\n  {CLIENT_ZEN}\n  -> {ZEN_SOURCE}")
    print()
    print(
        f"Criar link:\n  {CLIENT_ZELLNO}\n"
        f"  -> {SERVER_ZELLNO}"
    )
    print()
    print("Acrescentar aos dois scripts:")
    print("  @ZensFireSticks")
    print("  @ZellnoSafezoneCampfire")

    if not args.apply:
        print()
        print("SIMULAÇÃO concluída. Nenhum arquivo foi alterado.")
        print("Use --apply somente após conferir este relatório.")
        return 0

    timestamp = datetime.datetime.now().strftime(
        "%Y%m%d-%H%M%S"
    )
    backup = PROJECT / "backups" / f"pre-local-test-{timestamp}"
    backup.mkdir(parents=True, exist_ok=False)

    shutil.copy2(START_SH, backup / "start.sh")
    shutil.copy2(CONNECT_SH, backup / "dayz_connect.sh")

    created_paths: list[Path] = []
    key_created = False

    try:
        shutil.copytree(ZEN_SOURCE, SERVER_ZEN)
        created_paths.append(SERVER_ZEN)

        shutil.copytree(ZELLNO_BUILD, SERVER_ZELLNO)
        created_paths.append(SERVER_ZELLNO)

        SERVER_KEYS.mkdir(parents=True, exist_ok=True)

        if ZEN_KEY_TARGET.exists():
            if (
                ZEN_KEY_TARGET.read_bytes()
                != ZEN_KEY_SOURCE.read_bytes()
            ):
                shutil.copy2(
                    ZEN_KEY_TARGET,
                    backup / "Zenarchist.bikey.previous",
                )
                shutil.copy2(ZEN_KEY_SOURCE, ZEN_KEY_TARGET)
        else:
            shutil.copy2(ZEN_KEY_SOURCE, ZEN_KEY_TARGET)
            key_created = True

        CLIENT_ZEN.symlink_to(
            ZEN_SOURCE,
            target_is_directory=True,
        )
        created_paths.append(CLIENT_ZEN)

        CLIENT_ZELLNO.symlink_to(
            SERVER_ZELLNO,
            target_is_directory=True,
        )
        created_paths.append(CLIENT_ZELLNO)

        START_SH.write_text(new_start, encoding="utf-8")
        CONNECT_SH.write_text(new_connect, encoding="utf-8")

        if not CLIENT_ZEN.is_symlink():
            raise RuntimeError("Link do Zen não foi criado.")

        if not CLIENT_ZELLNO.is_symlink():
            raise RuntimeError("Link Zellno não foi criado.")

        if not SERVER_ZEN.is_dir():
            raise RuntimeError("Cópia do Zen não foi criada.")

        if not SERVER_ZELLNO.is_dir():
            raise RuntimeError("Cópia Zellno não foi criada.")

    except Exception:
        shutil.copy2(backup / "start.sh", START_SH)
        shutil.copy2(
            backup / "dayz_connect.sh",
            CONNECT_SH,
        )

        for created in reversed(created_paths):
            if created.is_symlink():
                created.unlink()
            elif created.is_dir():
                shutil.rmtree(created)

        if key_created and ZEN_KEY_TARGET.exists():
            ZEN_KEY_TARGET.unlink()

        previous_key = backup / "Zenarchist.bikey.previous"
        if previous_key.exists():
            shutil.copy2(previous_key, ZEN_KEY_TARGET)

        raise

    print()
    print("INSTALAÇÃO concluída.")
    print(f"Backup: {backup}")
    print("O servidor não foi iniciado.")

    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except Exception as exc:
        print(f"ERRO: {exc}", file=sys.stderr)
        raise SystemExit(1)
