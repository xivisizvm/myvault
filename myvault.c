/*
 * MyVault — a simple password vault in C for beginners.
 *
 * What it does:
 *   - you pick a master password (it asks every time you open the vault)
 *   - add entries:  site + username + password
 *   - view, list, and delete entries
 *   - everything is saved encrypted in vault.dat (XOR cipher using your
 *     master password as the key)
 *
 * SECURITY NOTE (be honest about it!): XOR "encryption" is very weak —
 * this is a LEARNING project to show file I/O + basic crypto ideas,
 * not a real safe for your bank password.
 *
 * Files: vault.dat  (the encrypted vault)
 *
 * Build:  gcc -std=c99 -Wall -o myvault myvault.c
 * Run:    ./myvault
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VAULT_FILE "vault.dat"
#define MAX_ENTRY  128
#define MAX_ITEMS  100

/* one vault entry: three parallel strings */
struct entry {
    char site[MAX_ENTRY];
    char user[MAX_ENTRY];
    char pass[MAX_ENTRY];
};

struct entry items[MAX_ITEMS];
int item_count = 0;
char master[128];

/* ---------- XOR "encryption" ----------
   XOR each byte with a key byte, cycling through the key.
   Applying it twice with the same key gives back the original
   (because x ^ k ^ k == x) — so the same function encrypts
   and decrypts. That's the fun trick to understand here. */
void xor_buf(char *buf, int len, const char *key) {
    int klen = (int)strlen(key);
    if (klen == 0) klen = 1;                 /* avoid dividing by zero */
    for (int i = 0; i < len; i++)
        buf[i] = (char)(buf[i] ^ key[i % klen]);
}

/* ---------- save & load ---------- */

void save_vault(void) {
    FILE *f = fopen(VAULT_FILE, "wb");
    if (!f) { printf("Error: cannot write %s\n", VAULT_FILE); return; }
    fwrite(&item_count, sizeof item_count, 1, f);
    for (int i = 0; i < item_count; i++) {
        xor_buf(items[i].site, MAX_ENTRY, master);
        xor_buf(items[i].user, MAX_ENTRY, master);
        xor_buf(items[i].pass, MAX_ENTRY, master);
        fwrite(&items[i], sizeof items[i], 1, f);
        /* XOR back so our in-memory copy stays readable */
        xor_buf(items[i].site, MAX_ENTRY, master);
        xor_buf(items[i].user, MAX_ENTRY, master);
        xor_buf(items[i].pass, MAX_ENTRY, master);
    }
    fclose(f);
}

int load_vault(void) {
    FILE *f = fopen(VAULT_FILE, "rb");
    if (!f) return 0;                        /* no vault yet — fine */
    fread(&item_count, sizeof item_count, 1, f);
    for (int i = 0; i < item_count; i++) {
        fread(&items[i], sizeof items[i], 1, f);
        xor_buf(items[i].site, MAX_ENTRY, master);
        xor_buf(items[i].user, MAX_ENTRY, master);
        xor_buf(items[i].pass, MAX_ENTRY, master);
    }
    fclose(f);
    return 1;
}

/* ---------- input helper ---------- */

void read_line(char *buf, int size) {
    if (fgets(buf, size, stdin)) {
        buf[strcspn(buf, "\r\n")] = '\0';
    }
}

/* ---------- menu actions ---------- */

void add_entry(void) {
    if (item_count >= MAX_ITEMS) {
        printf("Vault is full!\n");
        return;
    }
    struct entry *e = &items[item_count];
    printf("Site name   : ");
    read_line(e->site, MAX_ENTRY);
    printf("Username    : ");
    read_line(e->user, MAX_ENTRY);
    printf("Password    : ");
    read_line(e->pass, MAX_ENTRY);
    if (e->site[0] == '\0') {
        printf("Cancelled (no site name).\n");
        return;
    }
    item_count++;
    save_vault();
    printf("Saved and encrypted!\n");
}

void list_entries(void) {
    if (item_count == 0) {
        printf("Vault is empty.\n");
        return;
    }
    printf("\n%-25s %-25s\n", "SITE", "USERNAME");
    printf("-----------------------------------------------\n");
    for (int i = 0; i < item_count; i++)
        printf("%d. %-23s %-25s\n", i + 1, items[i].site, items[i].user);
    printf("\n");
}

void show_password(void) {
    if (item_count == 0) { printf("Vault is empty.\n"); return; }
    list_entries();
    int n;
    printf("Show password for entry number (0 to cancel): ");
    scanf("%d", &n);
    while (getchar() != '\n') {}             /* clear leftover input */
    if (n < 1 || n > item_count) return;
    printf("Password for %s: %s\n", items[n - 1].site, items[n - 1].pass);
}

void delete_entry(void) {
    if (item_count == 0) { printf("Vault is empty.\n"); return; }
    list_entries();
    int n;
    printf("Delete entry number (0 to cancel): ");
    scanf("%d", &n);
    while (getchar() != '\n') {}
    if (n < 1 || n > item_count) return;
    for (int i = n - 1; i < item_count - 1; i++)   /* shift left */
        items[i] = items[i + 1];
    item_count--;
    save_vault();
    printf("Deleted.\n");
}

/* ---------- main ---------- */

int main(void) {
    printf("=== MyVault — simple password vault ===\n");
    printf("Master password: ");
    read_line(master, sizeof master);
    if (master[0] == '\0') {
        printf("No password given. Bye!\n");
        return 0;
    }

    load_vault();
    printf("Vault opened. %d entries loaded.\n", item_count);

    while (1) {
        printf("\n--- MENU ---\n");
        printf("1. Add entry\n");
        printf("2. List entries\n");
        printf("3. Show a password\n");
        printf("4. Delete an entry\n");
        printf("5. Quit\n");
        printf("Choice: ");
        int choice;
        if (scanf("%d", &choice) != 1) break;
        while (getchar() != '\n') {}         /* clear leftover input */

        if      (choice == 1) add_entry();
        else if (choice == 2) list_entries();
        else if (choice == 3) show_password();
        else if (choice == 4) delete_entry();
        else if (choice == 5) { printf("Locking vault. Bye!\n"); break; }
        else printf("Invalid choice.\n");
    }
    return 0;
}
