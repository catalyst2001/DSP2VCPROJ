//#pragma comment(linker, "/STACK:36777216")
#include "main.h"
#include "dsp_reader.h"

int main(int argc, char **argv)
{
	project_t dspproj;
	if (!load_developer_studio_project(&dspproj, "shaderlib.dsp")) {
		printf("Failed to load DSP\n");
		return 1;
	}

	//dsp_print_sources_recursive(dspproj.project_target.p_head_group[0].p_head_element);
	dsp_print_bulds_recursive(dspproj.p_head_builds);

	free_developer_studio_project(&dspproj);
	return 0;
}