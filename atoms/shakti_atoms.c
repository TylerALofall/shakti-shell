/* shakti_atoms.c — the atoms bundle: every sound, every pin, one file.
 *
 * Format "SAT1" (Shakti AToms v1), append-never, rebuilt whole:
 *   magic(4) "SAT1"
 *   count(u32 LE)
 *   per atom: nameLen(u16 LE) name size(u32 LE) pin(u64 LE)
 *   then raw atom bytes in the same order
 * The header is pinned over names+sizes+pins only (data re-verified
 * per atom on unpack). Pin law: FNV-1a 64.
 *
 * Modes:
 *   pack  <bundle> <wav> [wav...]   build bundle, print header pin
 *   verify <bundle>                 re-pin every atom vs header, report
 *   list  <bundle>                  print the index
 *
 * Pure C99. No heap drift (static buffers), no float, no clock.
 * Gauntlet: -std=c99 -pedantic -Wall -Wextra -Werror, -O0 == -O2.
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define FNV_BASIS 0xCBF29CE484222325ULL
#define FNV_PRIME 0x100000001B3ULL

#define MAX_ATOMS 512
#define MAX_NAME  128
#define MAX_ATOM_BYTES (1u << 22) /* 4 MB per atom ceiling */

static uint64_t fnv(const unsigned char *p, size_t n)
{
    uint64_t h = FNV_BASIS;
    size_t i;
    for (i = 0; i < n; i++) { h ^= p[i]; h *= FNV_PRIME; }
    return h;
}

static void put16(unsigned char *p, uint16_t v)
{ p[0] = (unsigned char)(v & 0xFF); p[1] = (unsigned char)(v >> 8); }
static void put32(unsigned char *p, uint32_t v)
{ put16(p, (uint16_t)(v & 0xFFFF)); put16(p + 2, (uint16_t)(v >> 16)); }
static void put64(unsigned char *p, uint64_t v)
{ put32(p, (uint32_t)(v & 0xFFFFFFFFULL)); put32(p + 4, (uint32_t)(v >> 32)); }

static uint16_t get16(const unsigned char *p)
{ return (uint16_t)(p[0] | ((uint16_t)p[1] << 8)); }
static uint32_t get32(const unsigned char *p)
{ return (uint32_t)get16(p) | ((uint32_t)get16(p + 2) << 16); }
static uint64_t get64(const unsigned char *p)
{ return (uint64_t)get32(p) | ((uint64_t)get32(p + 4) << 32); }

static unsigned char g_data[MAX_ATOM_BYTES];

/* pack: header buffer holds all index entries, pinned whole */
static unsigned char g_hdr[16 + MAX_ATOMS * (2 + MAX_NAME + 4 + 8)];

static int pack(int argc, char **argv)
{
    FILE *out;
    size_t hdr_len = 0, pos;
    int i, n = argc - 3;
    if (n < 1 || n > MAX_ATOMS) { puts("PACK: bad count"); return 2; }
    out = fopen(argv[2], "wb");
    if (!out) { puts("PACK: cannot open output"); return 2; }

    memcpy(g_hdr, "SAT1", 4);
    put32(g_hdr + 4, (uint32_t)n);
    hdr_len = 8;

    /* index entries first; data offsets implicit (same order) */
    pos = hdr_len;
    for (i = 0; i < n; i++) {
        const char *path = argv[3 + i];
        const char *base = strrchr(path, '/');
        size_t nl, sz;
        FILE *f;
        uint64_t pin;
        base = base ? base + 1 : path;
        nl = strlen(base);
        if (nl == 0 || nl >= MAX_NAME) { fclose(out); puts("PACK: bad name"); return 2; }
        f = fopen(path, "rb");
        if (!f) { fclose(out); printf("PACK: cannot open %s\n", path); return 2; }
        sz = fread(g_data, 1, MAX_ATOM_BYTES, f);
        fclose(f);
        if (sz == 0) { fclose(out); printf("PACK: empty %s\n", path); return 2; }
        pin = fnv(g_data, sz);
        put16(g_hdr + pos, (uint16_t)nl); pos += 2;
        memcpy(g_hdr + pos, base, nl); pos += nl;
        put32(g_hdr + pos, (uint32_t)sz); pos += 4;
        put64(g_hdr + pos, pin); pos += 8;
    }
    hdr_len = pos;
    fwrite(g_hdr, 1, hdr_len, out);

    /* pass 2: data in same order, re-read so packing is two clean sweeps */
    for (i = 0; i < n; i++) {
        FILE *f = fopen(argv[3 + i], "rb");
        size_t sz;
        if (!f) { fclose(out); puts("PACK: pass2 open fail"); return 2; }
        sz = fread(g_data, 1, MAX_ATOM_BYTES, f);
        fclose(f);
        fwrite(g_data, 1, sz, out);
    }
    fclose(out);
    printf("packed %d atoms, header pin %016llX\n", n,
           (unsigned long long)fnv(g_hdr, hdr_len));
    return 0;
}

static int walk(const char *path, int list_mode)
{
    static unsigned char buf[16 + MAX_ATOMS * (2 + MAX_NAME + 4 + 8)];
    FILE *f = fopen(path, "rb");
    size_t n, pos, i;
    uint32_t count;
    int bad = 0;
    if (!f) { puts("WALK: cannot open bundle"); return 2; }
    n = fread(buf, 1, sizeof buf, f);
    if (n < 8 || memcmp(buf, "SAT1", 4) != 0) { fclose(f); puts("WALK: bad magic"); return 2; }
    count = get32(buf + 4);
    pos = 8;
    if (list_mode) {
        for (i = 0; i < count; i++) {
            uint16_t nl;
            uint32_t sz;
            uint64_t pin;
            char name[MAX_NAME];
            if (pos + 2 > n) { fclose(f); puts("WALK: truncated index"); return 2; }
            nl = get16(buf + pos); pos += 2;
            if (pos + nl + 12 > n || nl >= MAX_NAME) { fclose(f); puts("WALK: truncated entry"); return 2; }
            memcpy(name, buf + pos, nl); name[nl] = 0; pos += nl;
            sz = get32(buf + pos); pos += 4;
            pin = get64(buf + pos); pos += 8;
            printf("%3u %s %u bytes pin %016llX\n",
                   (unsigned)i, name, (unsigned)sz, (unsigned long long)pin);
        }
        fclose(f);
        return 0;
    }
    fclose(f);

    /* verify pass: re-read index, then stream the data section */
    {
        size_t idx_end;
        f = fopen(path, "rb");
        if (!f) return 2;
        n = fread(buf, 1, sizeof buf, f);
        fclose(f);
        pos = 8;
        for (i = 0; i < count; i++) {
            uint16_t nl = get16(buf + pos); pos += 2 + nl + 12;
        }
        idx_end = pos;
        f = fopen(path, "rb");
        if (!f) return 2;
        if (fseek(f, (long)idx_end, SEEK_SET) != 0) { fclose(f); return 2; }
        pos = 8;
        for (i = 0; i < count; i++) {
            uint16_t nl = get16(buf + pos); pos += 2;
            char name[MAX_NAME];
            uint32_t sz;
            uint64_t pin, got;
            size_t rd;
            memcpy(name, buf + pos, nl); name[nl] = 0; pos += nl;
            sz = get32(buf + pos); pos += 4;
            pin = get64(buf + pos); pos += 8;
            if (sz > MAX_ATOM_BYTES) { fclose(f); printf("VERIFY: %s oversize\n", name); return 2; }
            rd = fread(g_data, 1, sz, f);
            if (rd != sz) { fclose(f); printf("VERIFY: %s short read\n", name); return 2; }
            got = fnv(g_data, sz);
            if (got != pin) {
                printf("VERIFY: %s DRIFT exp %016llX got %016llX\n",
                       name, (unsigned long long)pin, (unsigned long long)got);
                bad++;
            }
        }
        fclose(f);
        if (bad == 0) printf("verified %u atoms, drift 0\n", (unsigned)count);
        else printf("VERIFY FAILS %d\n", bad);
    }
    return bad ? 1 : 0;
}

int main(int argc, char **argv)
{
    if (argc >= 3 && strcmp(argv[1], "pack") == 0) return pack(argc, argv);
    if (argc == 3 && strcmp(argv[1], "verify") == 0) return walk(argv[2], 0);
    if (argc == 3 && strcmp(argv[1], "list") == 0) return walk(argv[2], 1);
    puts("usage: shakti_atoms pack <bundle> <wav>... | verify <bundle> | list <bundle>");
    return 1;
}
