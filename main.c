#include <stdlib.h>
#include "game.h"

int main()
{
	system("color F0");
	system("mode con cols=100 lines=36");
	int mode;

	while (1) {

		MenuPrint();
		while (1) {

			mode = ModeSelect();

			if(mode==2)
				FriendsGame();
			else
				AIGame();

			break;
		}
		system("cls");
	}

	return 0;
}
