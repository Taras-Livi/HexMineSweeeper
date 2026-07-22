// HexMineSweeeper.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
//TODO: win console include for color and stuff
using namespace std;

struct hexLink
{
    uint8_t value; // 0-6 normal values of mines, 7+ are mines.
    bool revealed = false;
    hexLink* upL = nullptr;
    hexLink* up = nullptr;
    hexLink* upR = nullptr;
    hexLink* downR = nullptr;
    hexLink* down = nullptr;
    hexLink* downL = nullptr;
    // Proably would be more properly made if it had a creator for a adjacent hex that transfers the adjecents from the original hex
};

hexLink initializeGrid(uint32_t width, uint32_t height)
{
    if (width < 2 || height < 2)
		throw "width and height must be greater than 2";

    hexLink* start = new hexLink(); // uppermost left hexagon, it is second in the row
    hexLink* current = start;
    current->downL = new hexLink();
    current->downL->upR = current;
	hexLink* first = current->downL; // first hexagon in the second 
    hexLink* upper = first;
    uint32_t line_counter = 0;
	int k = 2; // k is the starting column for each line, since the first line has 2 already initialized

    for (uint32_t i = 0; i < height; i++) {
        if (i > 0)
		    first = current;
        upper = first;
        for (uint32_t j = k; j < width; j++) {
			hexLink* new_hex = new hexLink();
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
