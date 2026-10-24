#include "defuser.h"

#include <inttypes.h>
#include <stdio.h>

#define MAX_FIELDS 8

/*
 * 把 values 中的 count 个整数写成一行“空格分隔的无符号十进制整数”，存入 out。
 * 成功返回 0；count 为 0（还没有实现）、snprintf 出错或结果被截断时返回非零。
 */
static int write_answer(char *out, size_t cap, const uint32_t *values, size_t count) {
    size_t used = 0;
    for (size_t i = 0; i < count; ++i) {
        int size = snprintf(out + used, cap - used, "%s%" PRIu32, i ? " " : "", values[i]);
        if (size < 0 || (size_t)size >= cap - used) return 1;
        used += (size_t)size;
    }
    return count == 0;
}

int solve_phase_1(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    uint32_t low = (uint32_t)challenge;
    uint32_t high = (uint32_t)(challenge >> 32);
    uint32_t mid = (uint32_t)(challenge >> 16);
    uint32_t step1 = 3U * (high - mid) + 0x9864U;
    uint32_t step0 = (low + 2U * high) ^ 0x209cU;
    values[0] = (3U * step0 + step1) ^ 0xb95bU;
    return write_answer(out, cap, values, 1);
}

int solve_phase_2(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    values[0] = (((uint32_t)challenge * 2U) & 0x7eU) + 9U;
    for (uint32_t i = 1; i < 6; ++i) {
        values[i] = (((uint32_t)(challenge >> (i + 2U)) & 0x1fU) ^ (values[i - 1] << 2)) + 10U * i;
    }
    return write_answer(out, cap, values, 6);
}

int solve_phase_3(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    uint32_t arg = (uint32_t)(challenge >> 3) ^ (uint32_t)(challenge >> 32);
    values[0] = (uint32_t)challenge & 7U;
    switch (values[0]) {
        case 0: values[1] = 5U * arg + 0x345c1U; break;
        case 1: values[1] = (arg ^ 0x130fU) + (arg >> 2); break;
        case 2: values[1] = 3U * arg + 0x8a95U; break;
        case 3: values[1] = 5U * arg + 0xd9eeU; break;
        case 4: values[1] = (arg ^ 0x9387U) + (arg >> 2); break;
        case 5: values[1] = (4U * arg) ^ 0xde42U; break;
        case 6: values[1] = (2U * arg) ^ 0xf75eU; break;
        case 7: values[1] = 5U * arg + 0x12caU; break;
    }
    return write_answer(out, cap, values, 2);
}

int solve_phase_4(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    uint32_t target = (uint32_t)challenge & 0x7fU;
    uint32_t key = (uint32_t)(challenge >> 7) ^ (uint32_t)(challenge >> 32);
    uint32_t lo = 0;
    uint32_t hi = 0x7fU;
    uint32_t masks[8];
    uint32_t depth = 0;
    uint32_t result;
    for (;;) {
        uint32_t mid = lo + ((hi - lo) >> 1);
        if (mid == target) {
            result = (mid + 0x9bU) ^ key;
            break;
        }
        if (target < mid) {
            masks[depth++] = (mid + key) ^ 0x19U;
            hi = mid - 1U;
        } else {
            masks[depth++] = (mid + key) ^ 0x14U;
            lo = mid + 1U;
        }
    }
    while (depth > 0) {
        result = (3U * result) ^ masks[--depth];
    }
    values[0] = result;
    return write_answer(out, cap, values, 1);
}

int solve_phase_5(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    static const uint32_t next[12] = {8U, 3U, 5U, 6U, 0U, 7U, 11U, 1U, 10U, 4U, 9U, 0U};
    static const uint32_t field8[12] = {0x420U, 0xb5bU, 0x72aU, 0x225U, 0x920U, 0x658U, 0x52dU, 0x878U, 0x017U, 0xa3aU, 0x345U, 0x161U};
    static const uint32_t field12[12] = {0x6132U, 0xc06cU, 0x5e05U, 0xe3faU, 0x18d0U, 0x9e62U, 0xef15U, 0x24a4U, 0xfb54U, 0xcb20U, 0xcce6U, 0xbfcbU};
    static const uint32_t start[8] = {2U, 5U, 7U, 1U, 3U, 6U, 11U, 0U};
    uint32_t idx = start[(uint32_t)challenge & 7U];
    uint32_t acc = (uint32_t)(challenge >> 6) ^ (uint32_t)(challenge >> 32);
    uint32_t steps = ((uint32_t)(challenge >> 3)) & 3U;
    acc ^= field12[idx];
    acc ^= 0x6eceU;
    acc += field8[idx];
    values[1] = field8[idx];
    for (uint32_t i = 1; i <= steps; ++i) {
        idx = next[idx];
        acc ^= field12[idx];
        acc += field8[idx];
        values[1] = field8[idx];
    }
    values[0] = acc;
    return write_answer(out, cap, values, 2);
}

int solve_phase_6(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    uint32_t x = (uint32_t)(challenge >> 6) ^ (uint32_t)(challenge >> 32);
    uint32_t first;
    uint32_t second;
    switch ((uint32_t)challenge & 7U) {
        case 0:
        case 1: first = (x ^ 0xff6bU) + 8U * x; break;
        case 2: first = ((x >> 2) ^ x) + 0x740fU; break;
        case 3:
        case 5: first = ((x << 5) | (x >> 27)) ^ 0x8db3U; break;
        case 4: first = (x >> 3) | (x << 29); break;
        case 6: first = (x + 0xc395U) ^ (x >> 7); break;
        case 7: first = (8U * x + 0xf50fU) ^ (3U * x); break;
    }
    first ^= 0xe282U;
    switch (((uint32_t)(challenge >> 3)) & 7U) {
        case 0:
        case 1: second = (first ^ 0xff6bU) + 8U * first; break;
        case 2: second = ((first >> 2) ^ first) + 0x740fU; break;
        case 3:
        case 5: second = ((first << 5) | (first >> 27)) ^ 0x8db3U; break;
        case 4: second = (first >> 3) | (first << 29); break;
        case 6: second = (first + 0xc395U) ^ (first >> 7); break;
        case 7: second = (8U * first + 0xf50fU) ^ (3U * first); break;
    }
    values[0] = second + 0x1e3eU;
    return write_answer(out, cap, values, 1);
}

int solve_secret(uint64_t challenge, uint64_t context, char *out, size_t cap) {
    uint32_t c = (uint32_t)context & 3U;
    uint32_t chmix = (uint32_t)(challenge >> 32) ^ (uint32_t)challenge;
    uint32_t ctxmix = (uint32_t)(context >> 32) ^ (uint32_t)context;
    uint32_t mixarg = chmix ^ ctxmix;
    uint32_t mixed = ((mixarg >> 1) ^ 0xd907U) + (mixarg ^ 0x30cdU);
    uint32_t second = mixed ^ (c * 0x30cdU ^ (uint32_t)(context >> 16));
    uint32_t third = second ^ (ctxmix + c + 0xd907U + (chmix >> 1));
    int n = snprintf(out, cap, "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAOPEN%" PRIu32 " %" PRIu32 " %" PRIu32, c, second, third);
    return n < 0 || (size_t)n >= cap;
}
