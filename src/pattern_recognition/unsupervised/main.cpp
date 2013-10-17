
#include "all.hpp"
#include "unsupervised.hpp"

int main(const int argc, const char * argv[])
{
	int ret = 0;

	try
	{
		unsigned ab;

		throw_if((argc != 3));
		throw_if(((ab = atoi(argv[2]) - 1) > 6));;

		if(! strcmp(argv[1], "gen"))
		{
			throw_if(gen(ab));
		}
		else if(! strcmp(argv[1], "eval"))
		{
			throw_if(eval(ab));
		}
		else
		{
			throw_if((1));
		}
	}
	catch(...)
	{
		ret = -1;

		printf("\nФормат вызова: ./program COMMAND NUM\n\nЗдесь:\n\n\tCOMMAND - одна из следующих команд:\n\n\t\tgen - генерировать контрольный пример;\n\t\teval - рассчитать количество букв (рун) в алфавите;\n\n\tNUM - номер алфавита (от 1 до 7).\n\n");
	}

	return ret;
}

