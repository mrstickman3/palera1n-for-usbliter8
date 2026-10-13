#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

extern const uint8_t Pongo_bin[];
extern const uint32_t Pongo_bin_len;

char* get_embedded_pongo_path(void) {
    const char *tmpdir = getenv("TMPDIR");
    if (!tmpdir) tmpdir = "/tmp";

    size_t path_len = strlen(tmpdir) + strlen("/usbliter8-pongo-XXXXXX") + 1;
    char *tmp_path = malloc(path_len);
    if (!tmp_path) return NULL;

    snprintf(tmp_path, path_len, "%s/usbliter8-pongo-XXXXXX", tmpdir);

    int fd = mkstemp(tmp_path);
    if (fd < 0) { perror("mkstemp"); free(tmp_path); return NULL; }

    FILE *f = fdopen(fd, "wb");
    if (!f) { perror("fdopen"); close(fd); unlink(tmp_path); free(tmp_path); return NULL; }

    size_t written = fwrite(Pongo_bin, 1, Pongo_bin_len, f);
    fclose(f);

    if (written != Pongo_bin_len) {
        fprintf(stderr, "error: short write to %s (%zu / %u bytes)\n", tmp_path, written, Pongo_bin_len);
        unlink(tmp_path);
        free(tmp_path);
        return NULL;
    }
    return tmp_path;
}

