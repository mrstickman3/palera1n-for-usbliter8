#!/bin/bash
set -e

echo "== Step 1: build once so gen/gen-Pongo.bin.c exists =="
sudo make clean >/dev/null 2>&1 || true
sudo make -C src resources/Pongo.bin >/dev/null 2>&1 || true
cd src

GEN_FILE="gen/gen-Pongo.bin.c"
if [ ! -f "$GEN_FILE" ]; then
    echo "ERROR: $GEN_FILE not found. Run 'sudo make' from repo root first, then re-run this script."
    exit 1
fi

echo "== Step 2: detect real symbol names =="
ARR_NAME=$(grep -oE '^(uint8_t|unsigned char) [A-Za-z0-9_]+\[\]' "$GEN_FILE" | head -1 | awk '{print $2}' | tr -d '[]')
LEN_NAME=$(grep -oE '^(uint32_t|unsigned int) [A-Za-z0-9_]+_len' "$GEN_FILE" | head -1 | awk '{print $2}')

if [ -z "$ARR_NAME" ] || [ -z "$LEN_NAME" ]; then
    echo "ERROR: could not auto-detect symbol names. Contents of $GEN_FILE (first 5 lines):"
    head -5 "$GEN_FILE"
    exit 1
fi

echo "Detected array symbol: $ARR_NAME"
echo "Detected length symbol: $LEN_NAME"

echo "== Step 3: write embedded_pongo_helper.c =="
cat > embedded_pongo_helper.c <<EOF
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

extern const uint8_t ${ARR_NAME}[];
extern const unsigned int ${LEN_NAME};

char* get_embedded_pongo_path(void) {
    char *tmp_path = strdup("/tmp/usbliter8-pongo-XXXXXX");
    if (!tmp_path) return NULL;

    int fd = mkstemp(tmp_path);
    if (fd < 0) { perror("mkstemp"); free(tmp_path); return NULL; }

    FILE *f = fdopen(fd, "wb");
    if (!f) { perror("fdopen"); close(fd); unlink(tmp_path); free(tmp_path); return NULL; }

    size_t written = fwrite(${ARR_NAME}, 1, ${LEN_NAME}, f);
    fclose(f);

    if (written != ${LEN_NAME}) {
        fprintf(stderr, "error: short write to %s (%zu / %u bytes)\\n", tmp_path, written, ${LEN_NAME});
        unlink(tmp_path);
        free(tmp_path);
        return NULL;
    }
    return tmp_path;
}
EOF
printf '\n' >> embedded_pongo_helper.c
echo "Wrote src/embedded_pongo_helper.c"

echo "== Step 4: add declaration to palerain.h =="
if ! grep -q "get_embedded_pongo_path" ../include/palerain.h; then
    python3 - <<'PYEOF'
path = "../include/palerain.h"
s = open(path).read()
marker = "int tui(void);"
decl = "char* get_embedded_pongo_path(void);\n"
if marker in s:
    s = s.replace(marker, marker + "\n" + decl, 1)
    open(path, "w").write(s)
    print("Added declaration to palerain.h")
else:
    print("WARNING: marker not found in palerain.h, add manually:")
    print(decl)
PYEOF
fi

echo "== Step 5: patch dfuhelper.c to use fallback path =="
python3 - <<'PYEOF'
path = "dfuhelper.c"
s = open(path).read()
old = "int boot_ret = usbliter8_boot_file(pongo_path);"
new = """char *boot_path = pongo_path;
					char *temp_pongo = NULL;
					if (!boot_path) {
						temp_pongo = get_embedded_pongo_path();
						boot_path = temp_pongo;
					}
					int boot_ret = -1;
					if (!boot_path) {
						LOG(LOG_ERROR, "No PongoOS image available");
					} else {
						boot_ret = usbliter8_boot_file(boot_path);
						if (temp_pongo) { unlink(temp_pongo); free(temp_pongo); }
					}"""
if old in s:
    s = s.replace(old, new, 1)
    open(path, "w").write(s)
    print("Patched dfuhelper.c")
else:
    print("WARNING: exact target line not found in dfuhelper.c -- manual patch needed")
PYEOF

echo "== Step 6: rebuild =="
cd ..
sudo make clean && sudo make && sudo make install

echo "== Done. Test with: =="
echo "sudo palera1n --pwned-dfu -vv"
