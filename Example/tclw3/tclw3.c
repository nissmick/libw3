/*
 * $Id$
 *
 * Tcl/W3
 */

#include <W3Tcl.h>

#include <tcl.h>

#include <stdio.h>

int main(int argc, char** argv) {
	if(argc < 2) {
		fprintf(stderr, "Usage: %s filename\n", argv[0]);
		return 1;
	}
	Tcl_Interp* interp = Tcl_CreateInterp();
	Tcl_FindExecutable(argv[0]);
	if(Tcl_Init(interp) == TCL_ERROR) {
		const char* errmsg = Tcl_GetStringResult(interp);
		fprintf(stderr, "Tcl error: %s\n", errmsg);
		return -1;
	}
	if(W3_Init(interp) == TCL_ERROR) {
		const char* errmsg = Tcl_GetStringResult(interp);
		fprintf(stderr, "LibW3 error: %s\n", errmsg);
		return -1;
	}
	if(Tcl_EvalFile(interp, argv[1]) == TCL_ERROR) {
		const char* errmsg = Tcl_GetStringResult(interp);
		fprintf(stderr, "Tcl error: %s\n", errmsg);
		return -1;
	}
	Tcl_DeleteInterp(interp);
}
