// HexMineSweeeper.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <random>
#include <windows.h>
//#include <string> is this unnecessary?
using namespace std;

std::random_device rd;
std::mt19937 rng(rd());


uint32_t mine_num;
uint32_t spots_unrevealed;
uint32_t c_width = 80;
uint32_t c_height = 20;
uint32_t width = 13;
uint32_t height = 10;


struct hexLink
{
    uint8_t value = 0; // 0-6 normal values of mines, 7+ are mines.
    bool revealed = true;
    hexLink* upL = nullptr;
    hexLink* up = nullptr;
    hexLink* upR = nullptr;
    hexLink* downR = nullptr;
    hexLink* down = nullptr;
    hexLink* downL = nullptr;
    // Proably would be more properly made if it had a creator for a adjacent hex that transfers the adjacents from the original hex
};


// am confused on pointers again
hexLink* getHexLink(hexLink* start, uint32_t line_num, uint32_t col_num)
{
    // is line num and col num from 0 or from 1? I think it is 1, maybe change that, TODO
    auto hex_node = start;
    for (uint32_t i = 1; i < col_num; i++)
    {
        if (i % 2)
            if (hex_node->upR != nullptr)
                hex_node = hex_node->upR;
            else
                throw "col search error";
        else if (hex_node->downR != nullptr)
            hex_node = hex_node->downR;
        else
            throw "col search error";
    }
    for (uint32_t i = 1; i < line_num; i++)
    {
        if (hex_node->down != nullptr)
            hex_node = hex_node->down;
        else
            throw "row search error";

    }
    return hex_node;
}

hexLink* getHexLinkByCoord(COORD coord, hexLink* start)
{
    if (coord.Y == 0 or coord.Y == c_height - 1)
        return nullptr;
    if (coord.X % 4 == 1 and coord.Y % 2 == 0 or coord.X % 4 == 3 and coord.Y % 2 == 1)
        return getHexLink(start, (coord.Y - 1) / 2, coord.X / 2);
    return nullptr;
}


void placeMine(hexLink* hex, uint32_t& mines_unplaced, uint32_t& spots_left)
{
    if (mines_unplaced == 0)
        return;
    if (spots_left == 0)
        //throw "no more spots left to place mines";
        return;
    if (hex->value >= 7)
        throw "hex already has a mine somehow";
    std::uniform_int_distribution<int> dist(1, spots_left);
    spots_left--;
    if (dist(rng) <= mines_unplaced)
    {
        hex->value = 7;
        --mines_unplaced;
    }
    else
        return;
    // increment the values of adjacent hexes
    if (hex->upL != nullptr && hex->upL->value < 7)
        hex->upL->value++;
    if (hex->up != nullptr && hex->up->value < 7)
        hex->up->value++;
    if (hex->upR != nullptr && hex->upR->value < 7)
        hex->upR->value++;
    if (hex->downR != nullptr && hex->downR->value < 7)
        hex->downR->value++;
    if (hex->down != nullptr && hex->down->value < 7)
        hex->down->value++;
    if (hex->downL != nullptr && hex->downL->value < 7)
        hex->downL->value++;
    return;
}

/*enum linkDirection
void linkHexes
Perhaps a function that takes two hexes and a direction, and links them together in that direction would be better
*/

hexLink* gridInit()
{
    uint32_t mines_unplaced = mine_num;
    uint32_t spots_left = width * height;

    hexLink* start = new hexLink(); // uppermost leftmost hexagon
    hexLink* current = start;
    hexLink* first = start;
    hexLink* upper = start;
    for (uint32_t i = 0; i < height; i++) {
        if (i > 0) {
            current = new hexLink();
			current->up = upper;
			upper->down = current;
        }
        first = current;
        for (uint32_t j = 1; j < width; j++) {
            hexLink* new_hex = new hexLink();
            if (j % 2 == 0) {
                current->downR = new_hex;
                current->downR->upL = current; 
                current = current->downR;
                if (i > 0)
                    upper = upper->downR;
            }
            else {
                current->upR = new_hex;
                current->upR->downL = current;
                current = current->upR;
                if (i > 0)
                    upper = upper->upR;
            }
            if (i > 0) {
                //TODO upper not working properly
                current->up = upper;
                upper->down = current; //getting exception here because upper is nullptr but logic-wise this shouldn't be the case
                if (upper->downR != nullptr) {
                    current->upL = upper->downR;
                    upper->downR->downL = current;
                }
                if (upper->downL != nullptr) {
                    current->upR = upper->downL;
                    upper->downL->downR = current;
                }
            }
        }
        upper = first;
    }
    //Traverse the grid and place mines in the hexes that are not decorative, and increment the values of the adjacent hexes
    current = start;
    for (uint32_t i = 0; i < height; i++)
    {
        first = current;
        for (uint32_t j = 0; j < width; j++)
        {
            placeMine(current, mines_unplaced, spots_left);
            if (j < width - 1) {
                if (j % 2) {
                    current = current->downR;
                }
                else {
                    current = current->upR;
                }
            }
        }
        current = first->down;
    }
    return start;
}

void printGridLine(hexLink* start, uint32_t line_num)
{
    //TODO: add color and stuff
    if (line_num == 0)
    {
        for (uint32_t i = 0; i < c_width; i++)
        {
            if (i % 4 != 3)
                cout << " ";
            else
                cout << "_";
        }
        return;
	}
   
	// TODO: change start to actually be start lol. and not 2, 1 and make it global.

    hexLink* hex_node = nullptr;
    for (uint32_t i = 0; i < c_width; i++)
    {
        if (line_num % 2)
            switch (i % 4)
            {
            case 0:
                cout << "\\";
				break;
            case 1:
				cout << "_";
                break;
            case 2:
                cout << "/";
				break;
            case 3:
                hex_node = getHexLinkByCoord({ (short)i, (short)line_num }, start);
                if (hex_node == nullptr)
                {
                    cout << " ";
                    break;
                }
				else if (hex_node->revealed)
                {
                    if (hex_node->value == 0)
						cout << " ";
                    else if (hex_node->value < 7)
                        cout << (int)hex_node->value;
                    else
                        cout << "*";//useless?
                }
                else
                    cout << "?";
                break;
            }
        else
            switch (i % 4)
            {
            case 0:
                cout << "/";
                break;
            case 1:
                hex_node = getHexLinkByCoord({ (short)i, (short)line_num }, start);
                if (hex_node == nullptr)
                {
                    cout << " ";
                    break;
                }
                else if (hex_node->revealed)
                {
                    if (hex_node->value == 0)
                        cout << " ";
                    else if (hex_node->value < 7)
                        cout << (int)hex_node->value;
                    else
                        cout << "*";
                }
                else
                    cout << "?";
                break;
            case 2:
                cout << "\\";
                break;
            case 3:
                cout << "_";
                break;
            }
    }
    return;
}

void printGrid(hexLink* start)
{
    for (uint32_t i = 0; i < c_height; i++)
    {
        printGridLine(start, i);
		if (i < c_height - 1)
			cout << endl;
    }
}

void revealDeepSearch(hexLink* hex)
{
	if (hex->revealed)
		return;
	hex->revealed = true;
    spots_unrevealed--;
	if (hex->value == 0)
	{
		if (hex->up != nullptr)
			revealDeepSearch(hex->up);
		if (hex->upL != nullptr)
			revealDeepSearch(hex->upL);
		if (hex->upR != nullptr)
			revealDeepSearch(hex->upR);
		if (hex->down != nullptr)
			revealDeepSearch(hex->down);
		if (hex->downL != nullptr)
			revealDeepSearch(hex->downL);
		if (hex->downR != nullptr)
			revealDeepSearch(hex->downR);
	}
    return; //is this unnecessary?
}

int main()
{
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    DWORD mode;

    GetConsoleMode(hIn, &mode);
    mode |= ENABLE_MOUSE_INPUT;
    mode &= ~ENABLE_QUICK_EDIT_MODE;
    SetConsoleMode(hIn, mode);

    CONSOLE_CURSOR_INFO info;

    GetConsoleCursorInfo(
        hOut,
        &info
    );

    info.bVisible = FALSE; //important
    SetConsoleCursorInfo(
        hOut,
        &info
    );

    CONSOLE_SCREEN_BUFFER_INFO c_info;

    GetConsoleScreenBufferInfo(
        hOut,
        &c_info
    );

    c_width =
        c_info.srWindow.Right -
        c_info.srWindow.Left +
        1;

    c_height =
        c_info.srWindow.Bottom -
        c_info.srWindow.Top +
        1;

	width = (c_width - 1) / 2;
	height = (c_height - 2) / 2;
    //Color and console resize stuff can be left for later. TODO;

    mine_num = height * width / 6;
	spots_unrevealed = height * width - mine_num;

    //TODO, create the hexgrid by this point then print it.
    //TODO ask ai about pointers for structs because I am so confused
	hexLink* start = gridInit();
	printGrid(start);

    INPUT_RECORD record;

    DWORD eventsRead;

    bool game_ongoing = true;
    
    while (game_ongoing) {
        ReadConsoleInput(
            hIn,
            &record,
            1,
            &eventsRead
        );
        if (record.EventType == KEY_EVENT)
        {
            KEY_EVENT_RECORD key = record.Event.KeyEvent;

            if (key.bKeyDown &&
                key.wVirtualKeyCode == VK_ESCAPE)
            {
                game_ongoing = false;
            }
        }

        else if (record.EventType == MOUSE_EVENT)
        {
            MOUSE_EVENT_RECORD mouse =
                record.Event.MouseEvent;

            if (mouse.dwEventFlags == 0 &&
                mouse.dwButtonState &
                FROM_LEFT_1ST_BUTTON_PRESSED)
            {
                COORD hex_coord;
                // im so lost in pointers
				hexLink* target = getHexLinkByCoord(mouse.dwMousePosition, start);
                if (target != nullptr) {
                    if (target->value == 7)
                    {
                        //TODO: improve this
                        cout << "You hit a mine! Game over!" << endl;
                        game_ongoing = false;
                    }
                    else if (target->value == 0)
                        revealDeepSearch(target);
                    else
                    {
                        target->revealed = true;
                        spots_unrevealed--;
                    }
                }// maybe move this one if statement down
                if (spots_unrevealed == 0)
                {
                    //TODO: improve this
                    cout << "You win!" << endl;
                    game_ongoing = false;
                }
				if (game_ongoing)
				{
                    // cursor reset
                    COORD pos;
                    pos.X = 0;
                    pos.Y = 0;
                    SetConsoleCursorPosition(
                        hOut,
                        pos
                    );
					printGrid(start);
				}
            }
        }
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
