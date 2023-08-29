
#include "graph.h"
#include "system_settings.h"

using namespace adf;

MyGraph mygraph;

int main(int argc, char ** argv) { 

	mygraph.init();
	mygraph.run(1);
	mygraph.end();

	return 0;

}

