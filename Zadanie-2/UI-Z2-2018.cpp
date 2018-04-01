#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

using std::vector;

static const unsigned char xSize = 6;
static const unsigned char ySize = 6;
static const unsigned char vehicleCount = (xSize * ySize) / 2;
static const unsigned char vehicleNameLength = 30;
static const bool debug = false;

struct pozicia {
	unsigned char farba;
	unsigned char dlzka;
	unsigned char x;
	unsigned char y;
	char smer;
}POZICIA;

struct tah {
	unsigned char farba;
	unsigned char dlzka;
	unsigned char oldx;
	unsigned char oldy;
	unsigned char newx;
	unsigned char newy;
	char smer;
}TAH;

struct krizovatka {
	unsigned char mapa[xSize][ySize];
};

typedef struct uzol {
	krizovatka krizovatka;
	pozicia **pozicie;
	tah *tah;
	unsigned char hlbka;
	struct uzol *pred;
}UZOL;

// Globalna premenna pre navstivene stavy
vector<UZOL> navstivene;

// Inicializacia krizovatky na nuly
void init(krizovatka *k) {
	unsigned char i, j;

	for (i = 0; i < xSize; i++)
		for (j = 0; j < ySize; j++)
			k->mapa[i][j] = 0;
}

// Vypis sucasneho stavu krizovatky
void vypis(krizovatka k) {
	unsigned char i, j;

	for (j = 0; j < ySize; j++) {
		for (i = 0; i < xSize; i++)
			printf("%d ", (unsigned int) k.mapa[i][j]);
		printf("\n");
	}
}

// Zistenie zhodnosti krizovatiek
bool areEqual(krizovatka k1, krizovatka k2) {
	unsigned char i, j;

	for (i = 0; i < xSize; i++)
		for (j = 0; j < ySize; j++)
			if (k1.mapa[i][j] != k2.mapa[i][j])
				return false;

	return true;
}

// Zisti ci je krizovatka v koncovom stave
bool isResult(krizovatka k, unsigned char cervene) {
	unsigned char i, j, l;

	for (i = 0; i < xSize; i++) {
		for (j = 0; j < ySize; j++) {
			if (k.mapa[i][j] == cervene) {
				for (l = i; l < xSize; l++) {
					if (k.mapa[l][j] != cervene)
						return false;
					if ((l == (xSize - 1)) && (k.mapa[l][j] == cervene))
						return true;
				}
			}
		}
	}

	return false;
}

// Zisti ci krizovatka nebola navstivena
bool isNotNavstivene(krizovatka k, unsigned char h) {
	unsigned int i, pocet = navstivene.size();

	for (i = 0; i < pocet; i++) {
		if (areEqual(k, navstivene[i].krizovatka) && (navstivene[i].hlbka <= h)) {
			return false;
		}
	}

	return true;
}

// Kontrola platnosti tahu
bool overTah(krizovatka input, tah *tah) {
	unsigned char i, j;

	// Horizontalny tah
	if (tah->smer == 'h') {
		// Kladny posun
		if (tah->newx > tah->oldx) {
			for (i = tah->oldx; i < (tah->newx + tah->dlzka); i++) {
				if ((input.mapa[i][tah->newy] != 0) && (input.mapa[i][tah->newy] != tah->farba)) {
					return false;
				}
			}
		}
		// Zaporny posun
		else {
			for (i = tah->newx; i < tah->oldx; i++) {
				if (input.mapa[i][tah->newy] != 0) {
					return false;
				}
			}
		}
	}
	// Vertikalny tah
	else {
		// Kladny posun
		if (tah->newy > tah->oldy) {
			for (j = tah->oldy; j < (tah->newy + tah->dlzka); j++) {
				if ((input.mapa[tah->newx][j] != 0) && (input.mapa[tah->newx][j] != tah->farba)) {
					return false;
				}
			}
		}
		// Zaporny posun
		else {
			for (j = tah->newy; j < tah->oldy; j++) {
				if (input.mapa[tah->newx][j] != 0) {
					return false;
				}
			}
		}
	}

	return true;
}

// Vykona tah nad krizovatkou input
krizovatka vykonajTah(krizovatka input, tah *tah) {
	unsigned char i, j;
	krizovatka output;

	// Okopirovanie hodnot z input-u do output-u (okrem farby daneho tahu)
	for (i = 0; i < xSize; i++) {
		for (j = 0; j < ySize; j++) {
			if (input.mapa[i][j] != tah->farba) {
				output.mapa[i][j] = input.mapa[i][j];
			}
			else output.mapa[i][j] = 0;
		}
	}

	// Horizontalny tah
	if (tah->smer == 'h') {
		for (i = 0; i < tah->dlzka; i++)
			output.mapa[tah->newx + i][tah->newy] = tah->farba;
	}
	// Vertikalny tah
	else {
		for (j = 0; j < tah->dlzka; j++)
			output.mapa[tah->newx][tah->newy + j] = tah->farba;
	}

	return output;
}

UZOL *IDS(UZOL *uzol, unsigned char maxHlbka, unsigned char pocetVozidiel, unsigned char cervene) {

	// Ak uz je v maximalnej hlbke
	if (uzol->hlbka == maxHlbka) {
		if (isResult(uzol->krizovatka, cervene)) {
			return uzol;
		}

		return NULL;
	}

	int i = 0, j = 0, k = 0;
	struct tah *tah = new struct tah;
	UZOL *result, *novy = new UZOL;
	pozicia *pozicie[vehicleCount];

	for (i = 0; i < vehicleCount; i++) {
		pozicie[i] = new pozicia;
		pozicie[i] = uzol->pozicie[i];
	}

	novy->hlbka = uzol->hlbka + 1;
	novy->pred = uzol;

	for (i = 0; i < pocetVozidiel; i++) {
		tah->farba = uzol->pozicie[i]->farba;
		tah->dlzka = uzol->pozicie[i]->dlzka;
		tah->smer = uzol->pozicie[i]->smer;
		tah->oldx = uzol->pozicie[i]->x;
		tah->oldy = uzol->pozicie[i]->y;

		// Vozidlo je horizontalne
		if (uzol->pozicie[i]->smer == 'h') {
			tah->newy = uzol->pozicie[i]->y;

			// Najde vsetky mozne tahy
			for (j = 0; j < (xSize - tah->dlzka + 1); j++) {
				tah->newx = j;

				// Ak je tah platny
				if (overTah(uzol->krizovatka, tah)) {
					novy->krizovatka = vykonajTah(uzol->krizovatka, tah);

					// Ak krizovatka este nebola navstivena
					if (isNotNavstivene(novy->krizovatka, novy->hlbka)) {
						novy->tah = tah;

						for (k = 0; k < pocetVozidiel; k++) {
							if (k != i) {
								pozicie[k] = uzol->pozicie[k];
							}
							else {
								pozicie[k]->dlzka = uzol->pozicie[k]->dlzka;
								pozicie[k]->farba = uzol->pozicie[k]->farba;
								pozicie[k]->smer = uzol->pozicie[k]->smer;
								pozicie[k]->y = uzol->pozicie[k]->y;
								pozicie[k]->x = j;
							}
						}

						novy->pozicie = pozicie;

						result = IDS(novy, maxHlbka, pocetVozidiel, cervene);

						if (result != NULL) {
							return result;
						}
					}
				}
			}
		}
		// Vozidlo je vertikalne
		else {
			tah->newx = uzol->pozicie[i]->x;

			// Najde vsetky mozne tahy
			for (j = 0; j < (ySize - tah->dlzka + 1); j++) {
				tah->newy = j;

				// Ak je tah platny
				if (overTah(uzol->krizovatka, tah)) {
					novy->krizovatka = vykonajTah(uzol->krizovatka, tah);

					// Ak krizovatka este nebola navstivena
					if (isNotNavstivene(novy->krizovatka, novy->hlbka)) {
						novy->tah = tah;

						for (k = 0; k < pocetVozidiel; k++) {
							if (k != i) {
								pozicie[k] = uzol->pozicie[k];
							}
							else {
								pozicie[k]->dlzka = uzol->pozicie[k]->dlzka;
								pozicie[k]->farba = uzol->pozicie[k]->farba;
								pozicie[k]->smer = uzol->pozicie[k]->smer;
								pozicie[k]->x = uzol->pozicie[k]->x;
								pozicie[k]->y = j;
							}
						}

						novy->pozicie = pozicie;

						result = IDS(novy, maxHlbka, pocetVozidiel, cervene);

						if (result != NULL) {
							return result;
						}
					}
				}
			}
		}
	}

	return NULL;
}

int main() {
	unsigned char i = 0, j = 0, cervene = 0, hlbka = 0, pocetVozidiel = 0, pom[4];
	char farby[vehicleCount][vehicleNameLength] = { "\0" };
	char c, *token, *line = (char *) malloc (100);
	uzol *uzol, *result = NULL;
	pozicia *pozicie[vehicleCount];

	for (i = 0; i < vehicleCount; i++) {
		pozicie[i] = new pozicia;
	}
	
	i = 0;

	uzol = (UZOL *) malloc (sizeof(UZOL));
	uzol->hlbka = 0;
	uzol->tah = NULL;
	uzol->pred = NULL;
	init(&uzol->krizovatka);

	printf("Zadaj farby vozidiel (max %d), kazdu na samostatny riadok.\n", vehicleCount);
	printf("Nacitavanie ukoncis textom \"OK\" na prazdnom riadku.\n");

	while (fgets(line, 100, stdin) > 0) {
		if (!strcmp(line, "OK\n")) break;
		strncpy(farby[i], line, strstr(line, "\n") - line);
		printf("ID %d: %s\n", i + 1, farby[i]);

		if (!strcmp(farby[i], "Cervena") ||
			!strcmp(farby[i], "Cervene") ||
			!strcmp(farby[i], "cervene") ||
			!strcmp(farby[i], "cervena")) {
			cervene = i + 1;
		}

		i++;
	}

	if (cervene == 0) {
		printf("CHYBA! Nebolo zadane cervene vozidlo!\n");
		printf("Pre ukoncenie programu stlacte Enter");
		getchar();
		return -1;
	}

	memset(line, '\0', 100);
	printf("Nacitavanie farieb ukoncene!\n");
	printf("Zadaj pozicie vozidiel.\n");
	printf("Format vstupu (X a Y su od 1 po 6 vratane)\nID_farby dlzka_vozidla X Y h/v\n");
	printf("Nacitavanie ukoncis textom \"OK\" na prazdnom riadku.\n");

	while (fgets(line, 100, stdin) > 0) {
		if (!strcmp(line, "OK\n")) break;

		for (i = 0; i < 4; i++) pom[i] = 0;

		token = strtok(line, " ");

		i = 0;

		while (token != NULL) {
			if (i < 4) {
				pom[i] = atoi(token);
			}
			else {
				c = token[0];
				pozicie[j]->farba = pom[0];
				pozicie[j]->dlzka = pom[1];
				pozicie[j]->x = pom[2] - 1;
				pozicie[j]->y = pom[3] - 1;
				pozicie[j]->smer = c;
				uzol->pozicie = pozicie;

				j++;

				if (c == 'h') {
					printf("Vozidlo farby %s, dlzky %d je na pozicii %d X %d horizontalne.\n",
						farby[pom[0] - 1], pom[1], pom[2], pom[3]);

					for (i = (pom[2] - 1); i < (pom[2] + pom[1] - 1); i++) {
						uzol->krizovatka.mapa[i][pom[3] - 1] = pom[0];
					}
				}
				else if (c == 'v') {
					printf("Vozidlo farby %s, dlzky %d je na pozicii %d X %d vertikalne.\n",
						farby[pom[0] - 1], pom[1], pom[2], pom[3]);

					for (i = (pom[3] - 1); i < (pom[3] + pom[1] - 1); i++) {
						uzol->krizovatka.mapa[pom[2] - 1][i] = pom[0];
					}
				}
			}

			token = strtok(NULL, " ");
			i++;
		}
	}

	pocetVozidiel = j;

	printf("Nacitavanie pociatocneho stavu ukoncene!\n");

	vypis(uzol->krizovatka);

	if (isResult(uzol->krizovatka, cervene)) {
		printf("Pociatocny stav je zaroven aj koncovy.\n");
		printf("Pre ukoncenie programu stlacte Enter");
		getchar();
		return 0;
	}

	navstivene.insert(navstivene.begin(), *uzol);

	printf("Zadaj hlbku do ktorej hladat\n");
	scanf("%hhu", &hlbka);
	getchar();	// Preskocenie noveho riadku
	printf("Spustam hladanie koncoveho stavu do hlbky %d.\n", hlbka);

	for (i = 1; i <= hlbka; i++) {
		result = IDS(uzol, i, pocetVozidiel, cervene);

		if (result != NULL) {
			printf("V hlbke %d sa nasiel vysledok!\n\n", i);

			printf("Vysledna krizovatka:\n");
			vypis(result->krizovatka);

			UZOL* akt = result;

			while (akt != NULL) {
				printf("Predchadzajuci stav:\n");
				vypis(akt->krizovatka);
				akt = akt->pred;
			}

			printf("Pre ukoncenie programu stlacte Enter");
			getchar();
			return 0;
		}
		else {
			printf("V hlbke %d sa nenachadza vysledok\n", i);
		}
	}
	
	printf("Pre ukoncenie programu stlacte Enter");
	getchar();
	return 0;
}
