#!/bin/bash
set -euo pipefail

PROJECT_DIR="$(
    cd "$(dirname "${BASH_SOURCE[0]}")"
    pwd
)"

SOURCE_DIR="$PROJECT_DIR/source/ZellnoSafezoneCampfire"
BUILD_ROOT="$PROJECT_DIR/build"
BUILD_MOD="$BUILD_ROOT/@ZellnoSafezoneCampfire"
BUILD_ADDONS="$BUILD_MOD/addons"
BUILD_KEYS="$BUILD_MOD/keys"

DAYZ_TOOLS="$HOME/.local/share/Steam/steamapps/common/DayZ Tools/Bin"
FILEBANK="$DAYZ_TOOLS/PboUtils/FileBank.exe"
BANKREV="$DAYZ_TOOLS/PboUtils/BankRev.exe"
DSSIGNFILE="$DAYZ_TOOLS/DsUtils/DSSignFile.exe"
DSCHECK="$DAYZ_TOOLS/DsUtils/DSCheckSignatures.exe"

PRIVATE_KEY="$HOME/dayz/modding/keys/Zellno/Zellno.biprivatekey"
PUBLIC_KEY="$HOME/dayz/modding/keys/Zellno/Zellno.bikey"

SERVER_MOD="$HOME/dayz/server/@ZellnoSafezoneCampfire"
SERVER_KEYS="$HOME/dayz/server/keys"
INSTALL_BACKUPS="$PROJECT_DIR/backups/install"

EXPECTED_BUILD_MOD="$PROJECT_DIR/build/@ZellnoSafezoneCampfire"

if [ "$BUILD_MOD" != "$EXPECTED_BUILD_MOD" ]; then
    echo "Destino de build inesperado: $BUILD_MOD" >&2
    exit 1
fi

for required in \
    "$SOURCE_DIR/config.cpp" \
    "$PROJECT_DIR/mod.cpp" \
    "$PROJECT_DIR/meta.cpp" \
    "$FILEBANK" \
    "$BANKREV" \
    "$DSSIGNFILE" \
    "$DSCHECK" \
    "$PRIVATE_KEY" \
    "$PUBLIC_KEY"
do
    if [ ! -f "$required" ]; then
        echo "Arquivo obrigatório não encontrado:" >&2
        echo "$required" >&2
        exit 1
    fi
done

rm -rf -- "$BUILD_MOD"
mkdir -p "$BUILD_ADDONS" "$BUILD_KEYS"

wine "$FILEBANK" \
    -property prefix=ZellnoSafezoneCampfire \
    -dst "$(winepath -w "$BUILD_ADDONS")" \
    "$(winepath -w "$SOURCE_DIR")"

PBO="$BUILD_ADDONS/ZellnoSafezoneCampfire.pbo"

if [ ! -f "$PBO" ]; then
    echo "O FileBank não criou o PBO esperado:" >&2
    echo "$PBO" >&2
    exit 1
fi

wine "$DSSIGNFILE" \
    "$(winepath -w "$PRIVATE_KEY")" \
    "$(winepath -w "$PBO")"

cp "$PUBLIC_KEY" "$BUILD_KEYS/"
cp "$PROJECT_DIR/mod.cpp" "$BUILD_MOD/"
cp "$PROJECT_DIR/meta.cpp" "$BUILD_MOD/"

if [ -f "$PROJECT_DIR/README.md" ]; then
    cp "$PROJECT_DIR/README.md" "$BUILD_MOD/"
fi

VERIFY_OUTPUT="$(
    wine "$DSCHECK" \
        "$(winepath -w "$BUILD_ADDONS")" \
        "$(winepath -w "$(dirname "$PUBLIC_KEY")")" \
        2>&1
)"

echo "$VERIFY_OUTPUT"

if ! grep -q 'is OK' <<< "$VERIFY_OUTPUT"; then
    echo "Falha na validação da assinatura." >&2
    exit 1
fi

echo
echo "Propriedades do PBO:"

wine "$BANKREV" \
    -properties \
    "$(winepath -w "$PBO")"

echo
echo "Build validado:"
echo "$BUILD_MOD"

if [ "${1:-}" != "--install" ]; then
    echo
    echo "Nenhuma instalação foi realizada."
    echo "Use ./build.sh --install somente após aprovação."
    exit 0
fi

if pgrep -x DayZServer >/dev/null; then
    echo "O DayZServer está rodando." >&2
    echo "Encerre-o antes da instalação." >&2
    exit 1
fi

mkdir -p "$INSTALL_BACKUPS" "$SERVER_KEYS"

if [ -e "$SERVER_MOD" ]; then
    TIMESTAMP="$(date '+%Y%m%d-%H%M%S')"
    BACKUP="$INSTALL_BACKUPS/@ZellnoSafezoneCampfire-$TIMESTAMP"

    mv "$SERVER_MOD" "$BACKUP"

    echo "Versão anterior preservada em:"
    echo "$BACKUP"
fi

cp -a "$BUILD_MOD" "$SERVER_MOD"
cp "$PUBLIC_KEY" "$SERVER_KEYS/Zellno.bikey"

echo
echo "Build instalado em:"
echo "$SERVER_MOD"
