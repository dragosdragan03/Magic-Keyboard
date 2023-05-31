//Copyright <Dragan Dragos Ovidiu 313CA>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ALPHABET_SIZE 26

typedef struct trie_node_t trie_node_t;
struct trie_node_t {
	/* in value stochez cuvantul inserat */
	void *value;

	/* stochez numarul de aparitii al unui cuvant */
	int end_of_word;

	trie_node_t **children;
	int n_children;
};

typedef struct trie_t trie_t;
struct trie_t {
	trie_node_t *root;
};

trie_node_t *trie_create_node(trie_t *trie)
{
	trie_node_t *nod;
	nod = malloc(sizeof(trie_node_t));
	nod->n_children = 0;
	nod->children = calloc(ALPHABET_SIZE, sizeof(trie_node_t));
	nod->end_of_word = 0;
	nod->value = malloc(1);
	return nod;
}

trie_t *trie_create(void)
{
	trie_t *trie;
	trie = malloc(sizeof(trie_t));
	// vreau sa stochez in tree ul meu doar nodul root
	trie->root = trie_create_node(trie);
	trie->root->end_of_word = 0;
	return trie;
}

void parcurgere_autocorrect(char *cuvant, int n, trie_t *trie, trie_node_t *nod
	, int k, int *printari)
{
	if (cuvant[0] != k + 'a') // daca este difeit de litera mea
		n--;

	// inseamna ca difera cuvantul cu mai mult de k literec
	if (n < 0)
		return;

	// daca este final de cuvant de cuvant si are acelasi numar de litere
	if (nod->end_of_word && strlen(cuvant) == 1) {
		printf("%s\n", (char *)nod->value);
		(*printari)++;
	} else if (strlen(cuvant) == 0) {
		return;
	}

	for (int i = 0; i < 26; i++)
		if (nod->children[i])
			parcurgere_autocorrect(cuvant + 1, n, trie, nod->children[i], i
				, printari);
}

void autocorrect(char *cuvant, int n, trie_t *trie)
{
	trie_node_t *parent = trie->root;
	int k = 0; // pentru a parcurge vectorul meu noduri
	int printari = 0;
	while (k < 26) { // maximul vectorului meu de noduri
		if (parent->children[k])
			parcurgere_autocorrect(cuvant, n, trie, parent->children[k]
				, k, &printari);
		k++;
	}
	if (!printari)
		printf("No words found\n");
}

trie_node_t *cautare(char *key, trie_node_t *nod)
{
	if (!strlen(key)) { // daca s a terminat cuvantul meu
		return nod;
	}
	// stochez urmatoarea litera
	trie_node_t *next_nod = nod->children[key[0] - 'a'];

	if (!next_nod)  // nu exista nodul
		return NULL;

	cautare(key + 1, next_nod);
}

void criteriu1(trie_t *trie, trie_node_t *nod)
{
	// daca este final de cuvant inseamna ca pot afisa cuvantul
	if (nod->end_of_word) {
		printf("%s\n", (char *)nod->value);
		return;
	}

	for (int i = 0; i < 26; i++) {
		if (nod->children[i]) {
			criteriu1(trie, nod->children[i]);
			break;
		}
	}
}

void criteriu2(trie_t *trie, trie_node_t *nod, int *min, char **string_final)
{
	// daca este final de cuvant de cuvant si are acelasi numar de litere
	if (nod->end_of_word && *min > strlen((char *)nod->value)) {
		*string_final = (char *)nod->value;
		*min = strlen((char *)nod->value);
		return;
	}

	for (int i = 0; i < 26; i++) {
		if (nod->children[i])
			criteriu2(trie, nod->children[i], min, string_final);
	}
}

void criteriu3(trie_t *trie, trie_node_t *nod, int *max, char **string_final)
{
	// daca este final de cuvant de cuvant si are acelasi numar de litere
	if (nod->end_of_word) {
		if (*max < nod->end_of_word) {
			*max = nod->end_of_word;
			*string_final = (char *)nod->value;
		}
		if (*max == nod->end_of_word)
			if (strcmp((char *)nod->value, *string_final) < 0)
				*string_final = (char *)nod->value;
	}

	for (int i = 0; i < 26; i++)
		if (nod->children[i])
			criteriu3(trie, nod->children[i], max, string_final);
}

void autocomplete(trie_t *trie, char *prefix, int nr_criteriu)
{
	// retin in parent prefixul, mai exact ultimul nod
	trie_node_t *parent = cautare(prefix, trie->root);
	if (!parent) { // daca nu exista prefixul
		printf("No words found\n");
		if (!nr_criteriu) {
			printf("No words found\n");
			printf("No words found\n");
		}
		return;
	}

	if (!nr_criteriu) { // aplic toate cele 3 criterii
		criteriu1(trie, parent);
		char *string_final = NULL;
		int min = 1000;
		criteriu2(trie, parent, &min, &string_final);
		if (!string_final)
			printf("No words found\n");
		else
			printf("%s\n", string_final);
		string_final = NULL;
		int max = 0;
		criteriu3(trie, parent, &max, &string_final);
		if (!string_final)
			printf("No words found\n");
		else
			printf("%s\n", string_final);
	} else if (nr_criteriu == 1) {
		// Cel mai mic lexicografic cuvant cu prefixul dat
		criteriu1(trie, parent);
	} else if (nr_criteriu == 2) {
		// Cel mai scurt cuvant cu prefixul dat
		char *string_final = NULL;
		int min = 1000;
		criteriu2(trie, parent, &min, &string_final);
		if (!string_final)
			printf("No words found\n");
		else
			printf("%s\n", string_final);
	} else if (nr_criteriu == 3) {
		// Cel mai frecvent folosit cuvant cu prefixul dat
		char *string_final = NULL;
		int max = 0;
		criteriu3(trie, parent, &max, &string_final);
		if (!string_final)
			printf("No words found\n");
		else
			printf("%s\n", string_final);
	}
}

void trie_insert(trie_t *trie, trie_node_t *nod, char *key, char *value)
{
	if (strlen(key) == 0) { // daca s a terminat cuvantul meu
		// inseamna ca sunt mai multe aparitii ale cuvantului
		nod->end_of_word++;
		int l = strlen(value);
		nod->value = realloc(nod->value, l + 1);
		memcpy(nod->value, value, l + 1);
		return;
	}

	// stochez urmatoarea litera
	trie_node_t *next_nod = nod->children[key[0] - 'a'];

	if (!next_nod) { // nu exista nodul
		next_nod = trie_create_node(trie); // creez nodul
		// retin in nodul precedent litera coresp
		nod->children[key[0] - 'a'] = next_nod;
		nod->n_children++;
	}
	trie_insert(trie, next_nod, key + 1, value);
}

int trie_remove(trie_t *trie, trie_node_t *nod, char *key)
{
	if (!strlen(key)) { // daca s a terminat cuvantul meu
		if (nod->end_of_word) {
			nod->end_of_word = 0;
			return (nod->n_children == 0);
		}
		return 0;
	}

	trie_node_t *next_nod = nod->children[key[0] - 'a'];

	if (!next_nod)
		return 0;

	if (next_nod && trie_remove(trie, next_nod, key + 1)) {
		free(next_nod->value);
		free(next_nod->children);
		free(next_nod);
		nod->children[key[0] - 'a'] = NULL;
		nod->n_children--;
		if (!nod->n_children && !nod->end_of_word)
			return 1;
	}
	return 0;
}

void parcurgere_free(trie_t **trie, trie_node_t *nod)
{
	for (int i = 0; i < 26; i++)
		if (nod->children[i])
			parcurgere_free(trie, nod->children[i]);
	free(nod->value);
	free(nod->children);
	free(nod);
}

void trie_free(trie_t **ptrie)
{
	trie_node_t *parent = (*ptrie)->root;
	int k = 0; // pentru a parcurge vectorul meu noduri

	for (int i = 0; i < 26; i++) { // maximul vectorului meu de noduri
		if (parent->children[i])
			parcurgere_free(ptrie, parent->children[i]);
	}
	free((*ptrie)->root->value);
	free((*ptrie)->root->children);
	free((*ptrie)->root);
	free(*ptrie);
	*ptrie = NULL;
}

void load(char *filename, trie_t *trie)
{
	char cuvant[10000];
	FILE *in = fopen(filename, "r");
	while (!feof(in)) { // citesc fiecare cuvant din fisier
		fscanf(in, "%s", cuvant);
		trie_insert(trie, trie->root, cuvant, cuvant);
	}
}

int main(void)
{
	char sir[100];
	trie_t *trie = trie_create();

	while (fgets(sir, 100, stdin)) {
		char *token = strtok(sir, " ");
		if (!strcmp(token, "LOAD")) {
			token = strtok(NULL, " "); // stochez in token numele fisierului
			token[strlen(token) - 1] = '\0';
			load(token, trie);
			continue;
		}
		if (!strncmp(token, "INSERT", 6)) {
			token = strtok(NULL, " "); // retin cuvantul
			token[strlen(token) - 1] = '\0';
			trie_insert(trie, trie->root, token, token);
			continue;
		}
		if (!strncmp(token, "REMOVE", 6)) {
			token = strtok(NULL, " "); // retin cuvantul
			token[strlen(token) - 1] = '\0';
			trie_remove(trie, trie->root, token);
			continue;
		}
		if (!strncmp(token, "AUTOCOMPLETE", 12)) {
			token = strtok(NULL, " "); // retin cuvantul
			char *token2 = strtok(NULL, "\n");
			int nr_criteriu = atoi(token2);
			autocomplete(trie, token, nr_criteriu);
			continue;
		}
		if (!strncmp(token, "AUTOCORRECT", 11)) {
			token = strtok(NULL, " "); // retin cuvantul
			char *token2 = strtok(NULL, "\n");
			int nr_criteriu = atoi(token2);
			autocorrect(token, nr_criteriu, trie);
			continue;
		}
		if (!strncmp(token, "EXIT", 4)) {
			trie_free(&trie);
			break;
		}
		printf("Invalid command\n");
	}
	return 0;
}
