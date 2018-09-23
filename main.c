#include <stdlib.h>
#include "main_game.h"
#include "controller.h"
#include "eval_board.h"

int main()
{
	system("color F0");
	system("mode con cols=100 lines=42");
	int mode;
	initHASH();
	
	while (1)
	{

		MenuPrint();

		mode = ModeSelect();

		if ( mode==2 )
			FriendsGame(mode);
		else // if ( mode==0 || mode==1 )
			AIGame(mode);

		system("cls");
	}

	return 0;
}
