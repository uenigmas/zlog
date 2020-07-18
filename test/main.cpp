#include <iostream>
#include "../zlog.h"
using namespace std;
using namespace zlog;

int main(int argc, char *argv[])
{
	log_config.level = LOG_INFO;
	log_config.cur_prog_name = argv[0];
	log_config.use_file_output = false;

	string a = string("ddemod");
	trace_print("trace");
	debug_print("debug");
	error_print("error");
	debug_log("%s 6666 %s", a.c_str(), "8888");
	close_log_stream();

	cout << "Hello World!" << endl;
	return 0;
}
