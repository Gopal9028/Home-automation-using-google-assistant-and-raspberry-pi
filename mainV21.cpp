/*
 * NGINX C++ main code for GPIO(Receives and processes web request from Google Assistant)
 * Author : Gopal Vishwakarma
 * */
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include "fcgio.h"  
#include <fstream>
#include "GPIO_class.h"			//GPIO class header file
using namespace std;

//Reveive buffer size
const unsigned long STDIN_MAX = 1000;


string get_request_content(const FCGX_Request & request)  		// function for retriving data from webrequest
	{
		char * content_length_str = FCGX_GetParam("CONTENT_LENGTH", request.envp);		// Get size of data for future validation
		unsigned long content_length = STDIN_MAX;
		if (content_length_str) 
			{
				content_length = strtol(content_length_str, &content_length_str, 10); 	// setup for 10 bytes of data limitation , decided for our application  
				if (*content_length_str)
				 {
					cerr << "Can't Parse 'CONTENT_LENGTH= '"<< FCGX_GetParam("CONTENT_LENGTH", request.envp) << "'. Consuming stdin up to " << STDIN_MAX << endl;
				 }
				if (content_length > STDIN_MAX)
				 {
					content_length = STDIN_MAX;
				 }
			} 
			else
			{
				// Do not read from stdin if CONTENT_LENGTH is missing
				content_length = 0;
			}	

    char * content_buffer = new char[content_length];      // Initialize buffer
    cin.read(content_buffer, content_length);
    content_length = cin.gcount();
    do cin.ignore(1024); 		// igonre funtion for junk data if any. Fast cgi does not handle junk data effiently
    while (cin.gcount() == 1024);    	// Checking for input data

    string content(content_buffer, content_length);
    delete [] content_buffer;  
    return content;              // return actual data 
}

int main(void) {
	
// Backup the stdio streambufs
    streambuf * cin_streambuf  = cin.rdbuf();
    streambuf * cout_streambuf = cout.rdbuf();
    streambuf * cerr_streambuf = cerr.rdbuf();
    string content = "";

//GPIO config at raspberry pi
	string inputstate;
    GPIOClass* gpio4 = new GPIOClass("4"); //create new GPIO object to be attached to  GPIO4  // attached light bulb on this pin
    GPIOClass* gpio17 = new GPIOClass("17"); //create new GPIO object to be attached to  GPIO17  // attached coffee machine on this pin

    gpio4->export_gpio(); //export GPIO4
    gpio17->export_gpio(); //export GPIO17

    cout << " GPIO pins exported" << endl;

    gpio4->setdir_gpio("out"); // GPIO4 set to out (direction)

    cout << " Set GPIO pin directions" << endl;
//GPIO config end
//File write for backup (start)
    ofstream myfile;
    myfile.open ("rpidata.txt");
    cout << "Created/Overwritten to file 'rpidata.txt' \n";
    myfile.close();
//file write end

    FCGX_Request request;   //object creation for FCGX_Request

    FCGX_Init();			// Initialization for FCGX
    FCGX_InitRequest(&request, 0, 0); 	// Initialization for FCGX request to zero (default)
    cout << "FCGX initialized\n";

    while (FCGX_Accept_r(&request) == 0) {
        fcgi_streambuf cin_fcgi_streambuf(request.in);
        fcgi_streambuf cout_fcgi_streambuf(request.out);
        fcgi_streambuf cerr_fcgi_streambuf(request.err);

        cin.rdbuf(&cin_fcgi_streambuf);
        cout.rdbuf(&cout_fcgi_streambuf);
        cerr.rdbuf(&cerr_fcgi_streambuf);

		content ="";   // clearing content buffer
		
        content = get_request_content(request);
// write updated value to file
	myfile.open ("rpidata.txt");
    myfile << content << "\n";
    myfile.close();
        if (content.length() == 0) {
            content = "No data found";
        }
		
		if (content == "TWO")
		{
			usleep(500000);
			gpio4->setval_gpio("0");  	// make GPIO low 
			cout << "GPIO set" << endl;
		}
		else if(content == "ONE")
		{
			 usleep(500000);
			gpio4->setval_gpio("1");	// make GPIO high
			cout << "GPIO set" << endl;
		} 
		content = "";
       

        // Note: the fcgi_streambuf destructor will auto flush
    }

    // restore stdio streambufs
    cin.rdbuf(cin_streambuf);
    cout.rdbuf(cout_streambuf);
    cerr.rdbuf(cerr_streambuf);

    return 0;
}
