// HexMineSweeeper.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <random>
//TODO: win console include for color and stuff
using namespace std;

std::random_device rd;
std::mt19937 rng(rd());

struct hexLink
{
    uint8_t value = 0; // 0-6 normal values of mines, 7+ are mines.
    bool revealed = false;
    hexLink* upL = nullptr;
    hexLink* up = nullptr;
    hexLink* upR = nullptr;
    hexLink* downR = nullptr;
    hexLink* down = nullptr;
    hexLink* downL = nullptr;
    // Proably would be more properly made if it had a creator for a adjacent hex that transfers the adjecents from the original hex
};

void placeMine(hexLink* hex, uint32_t& mines_unplaced, uint32_t& spots_left)
{
    if (mines_unplaced == 0)
        return;
    if (spots_left == 0)
        //throw "no more spots left to place mines";
        return;
    if (hex->value >= 7)
        throw "hex already has a mine";
    std::uniform_real_distribution<double> chance(0.0, 1.0);
	double probability = (double)mines_unplaced / (double)spots_left;
    if (chance(rng) < probability)
        hex->value = 7; // set the value to a mine
    else
        return;
    mines_unplaced--;
    spots_left--;
    // increment the values of adjacent hexes
    if (hex->upL != nullptr && hex->upL->value < 7 && !hex->upL->revealed)
        hex->upL->value++;
    if (hex->up != nullptr && hex->up->value < 7 && !hex->upL->revealed)
        hex->up->value++;
    if (hex->upR != nullptr && hex->upR->value < 7 && !hex->upL->revealed)
        hex->upR->value++;
    if (hex->downR != nullptr && hex->downR->value < 7 && !hex->upL->revealed)
        hex->downR->value++;
    if (hex->down != nullptr && hex->down->value < 7 && !hex->upL->revealed)
        hex->down->value++;
    if (hex->downL != nullptr && hex->downL->value < 7 && !hex->upL->revealed)
        hex->downL->value++;
}

/*enum linkDirection
void linkHexes
Perhaps a function that takes two hexes and a direction, and links them together in that direction would be better
*/

// I fucked up in math regarding decorative tiles? decorative tiles should be a thing only if console height in characters is odd
// Actually it is better with no decorative tiles, leaving an empty row if console height is odd.
hexLink initializeGrid(uint32_t width, uint32_t height)
{
    if (width < 2 || height < 2)
		throw "width and height must be greater than 2";

    uint32_t mines_unplaced = mine_num;
    uint32_t spots_left;
    if (height % 2)
        spots_left = width * height - width / 2 - width % 2; // parts of last row are purely decorative.
    else
        spots_left = width * height - width / 2;

    hexLink* start = new hexLink(); // uppermost left hexagon, it is second in the row
    //placeMine(start, mines_unplaced, spots_left);
    hexLink* current = start;
    current->downL = new hexLink();
    current->downL->upR = current;
	hexLink* first = current->downL; // first hexagon in the second 
    hexLink* upper = first;
    uint32_t line_counter = 0;
    uint32_t k = 2; // k is the starting column for each line, since the first line has 2 already initialized

    for (uint32_t i = 0; i < height; i++) {
        if (i > 0)
		    first = current;
        upper = first;
        for (uint32_t j = k; j < width; j++) {
			hexLink* new_hex = new hexLink();
            //placeMine(new_hex, mines_unplaced, spots_left);
            //TODO: value generation and mine placement;

			if (j % 2 == 0) {
                current->downR = new_hex;
                //current->downR->upL = current;
                current = current->downR;
            }
            else {
                current->upR = new_hex;
                current->upR->downL = current;
                current = current->upR;
            }
            if (i > 0) {
                current->up = upper;
                current->upL = upper->downL;
                upper->downL->downR = current;
                current->upR = upper->downR;
                upper->down = current;
                upper->downR->downL = current;
                if (j % 2 == 0)
                    upper = upper->downR;
                else
					upper = upper->downL;
            }
        }
        k = 1;
        if (i < height - 1) {
            first->down = new hexLink();
			first->down->up = first;
            first->down->upR = first->downR;
			first = first->down;
            current = first;
        }
    }
	//Traverse the grid and place mines in the hexes that are not decorative, and increment the values of the adjacent hexes
    current = start->downL;
    for (uint32_t i = 0; i < height; i++)
    {
        first = current;
        upper = first;
        for (uint32_t j = 0; j < width; j++)
        {
            // decorative hex skip
            if (i == (height - 1))
                if (height % 2)
                    if (j % 2)
                        continue;
                    else
                    {
						current->revealed = true;
						current->value = 0;
                    }
                else
                    if (j % 2)
			placeMine(current, mines_unplaced, spots_left);
            if (j % 2 == 0) {
                current = current->downR;
            }
            else {
                current = current->upR;
            }
        }
        for (uint32_t j = 0; j < width; j++)
        {
            if (i % 2 == 0 && j % 2 == 1)
            {
                // decorative hex, skip
            }
            else
            {
                placeMine(current, mines_unplaced, spots_left);
            }
            if (j < width - 1)
            {
                if (j % 2 == 0)
                    current = current->downR;
                else
                    current = current->upR;
            }
        }
	}
	//TODO: reset value of the decorative hexes to 0, and set their revealed to true, so they don't get printed or interacted with
    return *start;
}

void printGrid(hexLink* start, uint32_t width, uint32_t height)
{
    for (uint32_t i = 0; i < height; i++)
    {
        printGridLine(start, i, width);
        cout << endl;
	}
}

void printGridLine(hexLink* start, uint32_t line_num, uint32_t width)
{
    //TODO: add color and stuff
    if (line_num == 0)
    {
        for (uint32_t i = 0; i < width; i++)
        {
            if (i % 4 != 0)
                cout << " ";
            else
                cout << "_";
        }
        cout << endl;
        return;
	}
    auto hex_node = start;
	// TODO: add and use a function to get the hex node at a specific line and column
    for (uint32_t i = 0; i < line_num; i++)
    {
        if (hex_node->downL != nullptr)
            hex_node = hex_node->downL;
        else if (hex_node->down != nullptr)
            hex_node = hex_node->down;
        else if (hex_node->downR != nullptr)
            hex_node = hex_node->downR;
        else
            throw "no specified line exists";
    }

    for (uint32_t i = 0; i < width; i++)
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
                if (start->revealed)
                {
                    if (hex_node->value == 0)
						cout << " ";
                    else if (hex_node->value < 7)
                        cout << hex_node->value;
                    else
                        cout << "*";
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
                if (start->revealed)
                {
                    if (hex_node->value == 0)
                        cout << " ";
                    else if (hex_node->value < 7)
                        cout << hex_node->value;
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

uint32_t mine_num = 0;

int main()
{
    //ine_num = height * width / 6
	//TODO: hide cursor, get console size, figure out how to rewrite existing lines in console
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
