#ifndef GPIO_CLASS_H
#define GPIO_CLASS_H

#include <string>
using namespace std;

class GPIOClass   //GPIO class for raspberry pi
{
public:
    GPIOClass();  
    GPIOClass(string x); // create a GPIO object that controls GPIOx, where x is passed to this constructor
    int export_gpio(); // exports GPIO
	int setval_gpio(string val); // Set GPIO Value (putput pins)
    int setdir_gpio(string dir); // Set GPIO Direction
    
private:
    string gpionum; // GPIO number
};

#endif
