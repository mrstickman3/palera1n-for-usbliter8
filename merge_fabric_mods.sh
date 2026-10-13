#!/usr/bin/env bash
# merge_fabric.sh - bundle multiple Fabric mods into one jar via jar-in-jar
# usage: ./merge_fabric.sh <output.jar> <wrapper_mod_id> <mod1.jar> <mod2.jar> ...
set -euo pipefail

if [ $# -lt 3 ]; then
  echo "usage: $0 <output.jar> <wrapper_mod_id> <mod1.jar> <mod2.jar> ..."
  exit 1
fi

OUT_ARG="$1"; ID="$2"; shift 2

# make output path absolute (we cd later)
case "$OUT_ARG" in
  /*) OUT="$OUT_ARG" ;;
  *)  OUT="$PWD/$OUT_ARG" ;;
esac

# Fabric mod IDs: lowercase, start with a letter, [a-z0-9-_], 2-64 chars
if ! [[ "$ID" =~ ^[a-z][a-z0-9_-]{1,63}$ ]]; then
  echo "error: wrapper_mod_id must match ^[a-z][a-z0-9_-]{1,63}$"
  exit 1
fi

WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
mkdir -p "$WORK/META-INF/jars"

JARS_JSON=""
for jar in "$@"; do
  [ -f "$jar" ] || { echo "error: not found: $jar"; exit 1; }
  name="$(basename "$jar")"
  # sanity check: must contain a fabric.mod.json
  if ! unzip -l "$jar" | grep -q "fabric.mod.json"; then
    echo "warning: $name has no fabric.mod.json (not a Fabric mod?)"
  fi
  cp "$jar" "$WORK/META-INF/jars/$name"
  [ -n "$JARS_JSON" ] && JARS_JSON="$JARS_JSON,"
  JARS_JSON="$JARS_JSON
    { \"file\": \"META-INF/jars/$name\" }"
done

cat > "$WORK/fabric.mod.json" <<EOF
{
  "schemaVersion": 1,
  "id": "$ID",
  "version": "1.0.0",
  "name": "$ID (bundle)",
  "description": "Bundle of nested mods.",
  "environment": "*",
  "jars": [$JARS_JSON
  ]
}
EOF

rm -f "$OUT"
(cd "$WORK" && zip -qr "$OUT" .)

echo "Built $OUT"
unzip -l "$OUT"
