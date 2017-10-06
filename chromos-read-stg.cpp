// chromos-read-stg.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ChmFile.h"
#include "stdio.h"
#include <conio.h>

int _tmain(int argc, _TCHAR* argv[])
{
	char oembuff[32];

	if (argc != 2) {
		printf("\nUsage:  stgreader filename.stg\n");
		return 1;
	}

	CChmFile stg_file;

	stg_file.ReadChmFile(argv[1]);

	printf("N\tHeight\tArea\tConcentration\tName\n");

	for (unsigned int i = 0; i<stg_file.peeks.size(); i++) {

		CharToOem(stg_file.peeks[i].GetComment(), oembuff);

		printf("%2d\t%.0f\t%.0f\t%.2f\t\t%s\n", i + 1, stg_file.peeks[i].GetHeight(), stg_file.peeks[i].GetArea(), stg_file.peeks[i].GetConcent(), oembuff);

		TRACE("%2d\t%.0f\t%.0f\t%.2f\t%s\n", i + 1, stg_file.peeks[i].GetHeight(), stg_file.peeks[i].GetArea(), stg_file.peeks[i].GetConcent(), stg_file.peeks[i].GetComment());
	}

	_getch();
	return 0;
}

