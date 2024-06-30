#include <iostream>
#include <Python.h>

using namespace std;

int simulateTile(wchar_t* argv[])
{
	Py_Initialize();
	PyObject *obj = Py_BuildValue("s", "Py_Tile.py");
	FILE *file = _Py_fopen_obj(obj, "r+");
	PySys_SetArgvEx(6, argv, 0);
	if(file != NULL) 
	{
		PyRun_SimpleFile(file, "Py_Tile.py");
		fclose(file);
	}
}

int main()
{
	int argc = 6;
	wchar_t *argv[6];

	// NOR 0 1 2
	argv[0] = L"Py_Tile.py";		//Python file name
	argv[1] = L"SET";				//NOR or READ
	argv[2] = L"0";  				//Tile number
	argv[3] = L"2";  				//Input 1 column address
	argv[4] = L"2";  				//Input 2 column address
	argv[5] = L"2";  				//Output column address

	simulateTile(argv);
}
