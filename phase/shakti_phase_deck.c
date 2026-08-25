/* shakti_phase_deck.c — the phase game deck: card data for the native
 * sheet, generated deterministically and pinned.
 *
 * The phase game (engine pin EB32B4A547E3B901): perfect recall vs
 * deterministic RANDOM. The deck is the fixed sequence of phases she
 * must recall — generated here, pinned, bundled with the app.
 *
 * Deck law:
 *   64 phases. Phase k rides wheel prime wheel[k % 5] raised by the
 *   binary lane step (k % 4), scaled by F7 when k is in lane 13's
 *   family shadow (k % 13 == 0). Every card: index, prime, power,
 *   value, lane flag. Value fits uint64 by construction.
 *
 * Output: phase_deck.txt (the bundle) + prints deck pin.
 * Pure C99. Gauntlet: -std=c99 -pedantic -Wall -Wextra -Werror, -O0==-O2.
 */
#include <stdio.h>
#include <stdint.h>

#define FNV_BASIS 0xCBF29CE484222325ULL
#define FNV_PRIME 0x100000001B3ULL
#define DECK_N 64

static uint64_t fnv(const unsigned char *p, size_t n)
{
    uint64_t h = FNV_BASIS;
    size_t i;
    for (i = 0; i < n; i++) { h ^= p[i]; h *= FNV_PRIME; }
    return h;
}

static const uint32_t g_wheel[5] = {2, 3, 5, 7, 19};

static uint64_t ipow(uint64_t b, uint32_t e)
{
    uint64_t r = 1;
    while (e--) r *= b;
    return r;
}

static int lane13(uint64_t n)
{
    uint64_t v = 13;
    if (n < 13) return 0;
    while (v < n) { if (v > UINT64_MAX / 2) return 0; v *= 2; }
    return v == n;
}

int main(void)
{
    static unsigned char buf[DECK_N * 80];
    size_t pos = 0;
    int k, n;
    FILE *f;
    for (k = 0; k < DECK_N; k++) {
        uint32_t prime = g_wheel[k % 5];
        uint32_t power = (uint32_t)(k % 4);
        uint64_t value = ipow(prime, power);
        int shadow = (k % 13 == 0) ? 1 : 0;
        if (shadow) value *= 13; /* F7 shadow card */
        n = snprintf((char *)buf + pos, sizeof buf - pos,
                     "%02d p%u^%u v%llu %s\n",
                     k, (unsigned)prime, (unsigned)power,
                     (unsigned long long)value,
                     shadow ? "F7" : (lane13(value) ? "lane13" : "-"));
        if (n < 0 || (size_t)n >= sizeof buf - pos) return 1;
        pos += (size_t)n;
    }
    f = fopen("phase_deck.txt", "wb");
    if (!f) return 1;
    fwrite(buf, 1, pos, f);
    fclose(f);
    printf("phase deck %d cards, pin %016llX\n", DECK_N,
           (unsigned long long)fnv(buf, pos));
    return 0;
}
