#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct IndexToCorner {
	bool BelongToTwoCorners;
	int firstCorner;
	int secondCorner;
};

struct SlotCoordinates {
	int rowCorrection;
	int colCorrection;
};

/*
corner0 | 0 1 2 | corner1
        | 3 4 5 | 
corner2 | 6 7 8 | corner3
where 0...3 and 5...8 are indexes of nearby slots and 4 is an index of a current slot
all other numbers are indexes of nearby slots
we have 4 corners with 3 slots in each of them
corner indexes are 0, 1, 2, 3 starting from the top left corner
*/
const IndexToCorner SLOT_INDEXES_TO_CORNER_INDEXES[9] = {
	{ false, 0, -1 }, // index == 0
	{ true, 0, 1 }, // index == 1
	{ false, 1, -1 }, // index == 2
	{ true, 0, 2 }, // index == 3
	{ false, -1, -1}, // index == 4 (it does not belong to any corner)
	{ true, 1, 3 }, // index == 5
	{ false, 2, -1 }, // index == 6
	{ true, 2, 3 }, // index == 7
	{ false, 3, -1 }, // index == 8
};

// first dimension is a corner index (0, 1, 2, 3)
// second dimension is a slot (each corner has 3 of them)
const SlotCoordinates CORNER_INDEXES_TO_SLOT_COORDS[4][3] = {
	{ {0, -1}, {-1, -1}, {-1, 0} },
	{ {-1, 0}, {-1, 1}, {0, 1} },
	{ {0, -1}, {1, -1}, {1, 0} },
	{ {1, 0}, {1, 1}, {0, 1} }
};

void insertMenuActionChoice(int &menuActionChoice);
void insertFieldSettings(int &size, int &mines);
void insertSlotCoordinates(int &row, int&col, char **front, int frontSize);
void insertActionType(int &actionType, int row, int col);

void printGreetings();
void printProposedFieldSettings();
void printNumberWidely(int n);
void printNumberNarrowly(int n);
void printCharWidely(char &c);
void printCharNarrowly(char &c);
void printSpacebarForArrowWidely();
void printSpacebarForArrowNarrowly();
void printField(char** field, int size);
void printFrontFieldWithChosenCoordinates(char** front, int frontSize, int row, int col);
void pauseConsole(char* str);
void printGameEnd();
void printReactionToOpeningMarkedSlot();
void printYouWon(int frontSize, int mines);
void printGameLegend();

void allocateMemoryForField(char** &field, int size);
void freeMemoryAllocatedForField(char** field, int size);
void fillFieldWithSymbol(char** field, int size, char symbol);
int generateRandomRowOrCol(int backSize);
void fillBackFieldWithMinesAndNumbers(char** back, int backSize, int mines); 
void openWholeFrontField(char** back, int backSize, char** front);
void replaceAllMinesWithFlags(char** front, int frontSize);
void openSlot(char** &back, char** &front, int &frontSize, int row, int col);
void openSlotFirstTouch(char** &back, char** &front, int &frontSize, int row, int col);
void markOrUnmarkSlot(char** front, int row, int col); 
bool checkWonOrNot(char** back, int backSize, char** front);

int main() {
	// 'back' field is the array that stores all information about the current game field and mines
	char **back;
	// 'front' field is the array that is shown to a player and that takes the information from 'back'
	char **front;
	int frontSize, backSize, mines;
	int row, col;
	/*
	'menuActionChoice' possible values:
	0 - exit the game
	1 - start the game
	2 - see the game legend
	*/
	int menuActionChoice = 1;
	/*
	'actionType' possible values:
	0 - cancel (re-choose the slot)
	1 - open the slot
	2 - mark/unmark the slot as the one with a mine
	*/
	int actionType;
	bool gameIsGoing;

	printGreetings();
	while (menuActionChoice) {
		insertMenuActionChoice(menuActionChoice);
		system("cls");
		if (menuActionChoice == 1) {
			gameIsGoing = true;
			printProposedFieldSettings();
			insertFieldSettings(frontSize, mines);
			backSize = frontSize + 2;

			allocateMemoryForField(back, backSize);
			allocateMemoryForField(front, frontSize);

			fillFieldWithSymbol(back, backSize, '0');
			fillBackFieldWithMinesAndNumbers(back, backSize, mines);
			fillFieldWithSymbol(front, frontSize, '#');

			while (gameIsGoing) {
				do {
					system("cls");
					printField(front, frontSize);
					insertSlotCoordinates(row, col, front, frontSize);
					system("cls");
					printFrontFieldWithChosenCoordinates(front, frontSize, row, col);
					insertActionType(actionType, row, col);
				} while (actionType == 0);

				if (actionType == 1 && front[row - 1][col - 1] == 'P') {
					printReactionToOpeningMarkedSlot();
				}
				else if (actionType == 1 && back[row][col] == '*') {
					back[row][col] = '%';
					openWholeFrontField(back, backSize, front);
					system("cls");
					printField(front, frontSize);
					printGameEnd();
					gameIsGoing = false;
				}
				else if (actionType == 1) {
					openSlotFirstTouch(back, front, frontSize, row, col);
					gameIsGoing = !checkWonOrNot(back, backSize, front);
					if (gameIsGoing == false) {
						openWholeFrontField(back, backSize, front);
						replaceAllMinesWithFlags(front, frontSize);
						system("cls");
						printField(front, frontSize);
						printYouWon(frontSize, mines);
					}
				}
				else { // if (actionType == 2)
					markOrUnmarkSlot(front, row, col);
				}
			}
			freeMemoryAllocatedForField(back, backSize);
			freeMemoryAllocatedForField(front, frontSize);
		}
		else if (menuActionChoice == 2) {
			printGameLegend();
		}
		system("cls");
	}
}

void insertMenuActionChoice(int &menuActionChoice) {
	printf("You have the next options:\n");
	printf("1 - start the game\n");
	printf("2 - see the game legend\n");
	printf("0 - exit the game\n\n");
	printf("Choose what you want to do (type in the number): ");
	scanf("%d", &menuActionChoice);
	while (menuActionChoice < 0 || menuActionChoice > 2) {
		printf("Please, type in the correct number: ");
		scanf("%d", &menuActionChoice);
	}
}

void insertFieldSettings(int &size, int &mines) {
	printf("Type in the field size (one number): ");
	scanf("%d", &size);
	while (size < 1) {
		printf("Please, type in the correct field size: ");
		scanf("%d", &size);
	}

	printf("Type in the amount of mines: ");
	scanf("%d", &mines);
	while (mines < 0 || mines > size * size) {
		printf("Please, type in the correct amount of mines: ");
		scanf("%d", &mines);
	}
}

void insertSlotCoordinates(int &row, int&col, char **front, int frontSize) {
	bool chooseAgain = true;
	do {
		printf("Choose the slot - type in the row: ");
		scanf("%d", &row);
		while (row < 1 || row > frontSize) {
			printf("Please, type in the correct row: ");
			scanf("%d", &row);
		}

		printf("Now type in the column: ");
		scanf("%d", &col);
		while (col < 1 || col > frontSize) {
			printf("Please, type in the correct column: ");
			scanf("%d", &col);
		}
		printf("\n");

		if (front[row - 1][col - 1] == '#' || front[row - 1][col - 1] == 'P') {
			chooseAgain = false;
		}
		else {
			printf("This slot is already opened! Choose another one.\n\n");
		}
	} while (chooseAgain);
}

void insertActionType(int &actionType, int row, int col) {
	printf("You have the next options:\n");
	printf("0 - cancel (re-choose the slot)\n");
	printf("1 - open the slot\n");
	printf("2 - mark/unmark the slot as the one with a mine\n");
	printf("Choose what you want to do (type in the number): ");
	scanf("%d", &actionType);
	while (actionType < 0 || actionType > 2) {
		printf("Please, type in the correct number: ");
		scanf("%d", &actionType);
	}
	printf("\n");
}

void printGreetings() {
	printf("Welcome to Minesweeper game!\n\n");
}

void printProposedFieldSettings() {
	printf("There are some standard difficulty modes:\n");
	printf("Beginner - 9x9 field size, 10 mines\n");
	printf("Intermediate - 16x16 field size, 40 mines\n");
	printf("Expert - 22x22 field size, 100 mines\n\n");
	printf("However, you can choose any settings you want.\n\n");
}

void printNumberWidely(int n) {
	printf("%2d", n);
}

void printNumberNarrowly(int n) {
	printf("%d", n);
}

void printCharWidely(char &c) {
	printf("%c ", c);
}
void printCharNarrowly(char &c) {
	printf("%c", c);
}

void printSpacebarForArrowWidely(){
	printf("   ");
}

void printSpacebarForArrowNarrowly() {
	printf("  ");
}

void printField(char** field, int size) {
	void(*printNumber)(int);
	void(*printChar)(char&);
	if (size > 9) {
		printNumber = printNumberWidely;
		printChar = printCharWidely;
	}
	else {
		printNumber = printNumberNarrowly;
		printChar = printCharNarrowly;
	}

	printf("     ");
	for (int m = 1; m < size + 1; ++m) {
		printNumber(m);
		printf(" ");
	}
	printf("\n\n");

	for (int k = 0; k < size; ++k) {
		printf(" ");
		printNumber(k + 1);
		printf("   ");
		for (int i = 0; i < size; ++i) {
			printChar(field[k][i]);
			printf(" ");
		}
		printf("\n");
	}
	printf("\n");
}

void printFrontFieldWithChosenCoordinates(char** front, int frontSize, int row, int col) {
	void(*printNumber)(int);
	void(*printChar)(char&);
	void(*printSpacebarForArrow)();
	if (frontSize > 9) {
		printNumber = printNumberWidely;
		printChar = printCharWidely;
		printSpacebarForArrow = printSpacebarForArrowWidely;
	}
	else {
		printNumber = printNumberNarrowly;
		printChar = printCharNarrowly;
		printSpacebarForArrow = printSpacebarForArrowNarrowly;
	}

	printf("     ");
	for (int m = 1; m < frontSize + 1; ++m) {
		printNumber(m);
		printf(" ");
	}
	printf("\n\n");

	for (int k = 0; k < frontSize; ++k) {
		printf(" ");
		printNumber(k + 1);
		printf("   ");
		for (int i = 0; i < frontSize; ++i) {
			printChar(front[k][i]);
			printf(" ");
		}
		if (k == row - 1) { printf("< ----"); }
		printf("\n");
	}
	for (int m = 0; m < 3; ++m) {
		printf("   ");
		printSpacebarForArrow();
		for (int n = 0; n < col - 1; ++n) {
			printSpacebarForArrow();
		}
		if (m == 0) { printf("^"); }
		else { printf("|"); }
		printf("\n");
	}
	printf("You have chosen the slot in row %d, column %d.\n\n", row, col);
}

void pauseConsole(const char* str) {
	printf("Press Enter to ");
	printf(str);
	printf(".\n");
	getchar();
	getchar();
}

void printGameEnd() {
	printf("Game over! You have chosen the slot with a mine!\n");
	pauseConsole("continue");
}

void printReactionToOpeningMarkedSlot() {
	printf("You cannot open the marked slot! Unmark it first.\n");
	pauseConsole("continue");
}

void printYouWon(int frontSize, int mines) {
	printf("Congratulations! You won!\nField size - %dx%d, amount of mines - %d.\n", frontSize, frontSize, mines);
	pauseConsole("continue");
}

void printGameLegend() {
	printf("Here you can find explanation of symbols used in the game:\n"); // ...
	printf("# - an unopened slot\n");
	printf("P - a slot that was flagged as containing a mine\n");
	printf("* - a mine\n");
	printf("%% - an exploded mine\n");
	printf("Numbers 0...8 - an opened slot showing an amount of mines in nearby slots\n\n");
	pauseConsole("return to the menu");
}

void allocateMemoryForField(char** &field, int size) {
	field = (char **)malloc(sizeof(char *) * size);
	for (int i = 0; i < size; ++i) {
		field[i] = (char *)malloc(sizeof(char) * size);
	}
}

void freeMemoryAllocatedForField(char** field, int size) {
	for (int i = 0; i < size; ++i) {
		free(field[i]);
	}
	free(field);
}

void fillFieldWithSymbol(char** field, int size, char symbol) {
	for (int k = 0; k < size; ++k) {
		for (int i = 0; i < size; ++i) {
			field[k][i] = symbol;
		}
	}
}

int generateRandomRowOrCol(int backSize) {
	return rand() % (backSize - 2) + 1;
}

void fillBackFieldWithMinesAndNumbers(char** back, int backSize, int mines) {
	int row, col;
	char *mineSlot, *tmpSlot;
	srand(time(0));
	for (int i = 0; i < mines; ) {
		row = generateRandomRowOrCol(backSize);
		col = generateRandomRowOrCol(backSize);
		mineSlot = &back[row][col];

		if (*mineSlot == '*') { continue; }
		*mineSlot = '*';

		for (int k = -1; k < 2; ++k) {
			for (int l = -1; l < 2; ++l) {
				tmpSlot = &back[row + k][col + l];
				if (*tmpSlot == '*') { continue; }
				*tmpSlot += 1;
			}
		}

		++i;
	}
}

void openWholeFrontField(char** back, int backSize, char** front) {
	for (int k = 1; k < backSize - 1; ++k) {
		for (int i = 1; i < backSize - 1; ++i) {
			front[k - 1][i - 1] = back[k][i];
		}
	}
}

void replaceAllMinesWithFlags(char** front, int frontSize) {
	for (int k = 0; k < frontSize; ++k) {
		for (int i = 0; i < frontSize; ++i) {
			if (front[k][i] == '*') {
				front[k][i] = 'P';
			}
		}
	}
}

void openSlot(char** &back, char** &front, int &frontSize, int row, int col) {
	if (front[row - 1][col - 1] != '#') {
		return void();
	}

	front[row - 1][col - 1] = back[row][col]; // opening the slot

	if (back[row][col] == '0') { // opening nearby slots
		for (int k = -1; k < 2; ++k) {
			for (int l = -1; l < 2; ++l) {
				if (row + k <= frontSize && row + k >= 1 
					&& col + l <= frontSize && col + l >= 1)
					openSlot(back, front, frontSize, row + k, col + l);
			}
		}
	}
}

void openSlotFirstTouch(char** &back, char** &front, int &frontSize, int row, int col) {
	// if one of the nearby slots contains '0' or the current slot contains '0'
	// then we call usual 'openSlot(...)'
	for (int k = -1; k < 2; ++k) {
		for (int l = -1; l < 2; ++l) {
			if (row + k <= frontSize && row + k >= 1
				&& col + l <= frontSize && col + l >= 1
				&& back[row + k][col + l] == '0') {
				openSlot(back, front, frontSize, row + k, col + l);
				return void();
			}
		}
	}

	// if none of the nearby slots contains '0' then we open the current slot
	// and check whether nearby slots in corners do not contain mines and open them if not

	front[row - 1][col - 1] = back[row][col]; // opening the current slot

	int cornerSlotsWithoutMine[4] = {};
	int slotIndex = 0, cornerIndex;
	IndexToCorner indexToCorner;

	// counting how many corners do not contain mines
	for (int k = -1; k < 2; ++k) {
		for (int l = -1; l < 2; ++l) {
			if (row + k <= frontSize && row + k >= 1
				&& col + l <= frontSize && col + l >= 1
				&& back[row + k][col + l] != '*'
				&& slotIndex != 4) {
				indexToCorner = SLOT_INDEXES_TO_CORNER_INDEXES[slotIndex];
				if (indexToCorner.BelongToTwoCorners) {
					cornerIndex = indexToCorner.firstCorner;
					cornerSlotsWithoutMine[cornerIndex] += 1;
					cornerIndex = indexToCorner.secondCorner;
					cornerSlotsWithoutMine[cornerIndex] += 1;
				}
				else {
					cornerIndex = indexToCorner.firstCorner;
					cornerSlotsWithoutMine[cornerIndex] += 1;
				}
			}
			slotIndex += 1;
		}
	}

	SlotCoordinates slotCoords;
	int rowCorrected, colCorrected;
	cornerIndex = 0;
	// opening corners that do not contain mines
	for (; cornerIndex < 4; ++cornerIndex) {
		if (cornerSlotsWithoutMine[cornerIndex] == 3) {
			for (int i = 0; i < 3; ++i) {
				slotCoords = CORNER_INDEXES_TO_SLOT_COORDS[cornerIndex][i];
				rowCorrected = row + slotCoords.rowCorrection;
				colCorrected = col + slotCoords.colCorrection;
				if (rowCorrected <= frontSize && rowCorrected >= 1
					&& colCorrected <= frontSize && colCorrected >= 1
					&& front[rowCorrected - 1][colCorrected - 1] == '#') {
					front[rowCorrected - 1][colCorrected - 1] 
						= back[rowCorrected][colCorrected];
				}
			}
		}
	}
}

void markOrUnmarkSlot(char** front, int row, int col) {
	char &slot = front[row - 1][col - 1];
	if (slot == '#') {
		slot = 'P';
	}
	else {
		slot = '#';
	}
}

bool checkWonOrNot(char** back, int backSize, char** front) {
	for (int k = 1; k < backSize - 1; ++k) {
		for (int i = 1; i < backSize - 1; ++i) {
			if ( (front[k - 1][i - 1] == '#' || front[k - 1][i - 1] == 'P') && back[k][i] != '*') {
				return false;
			}
		}
	}
	return true;
}