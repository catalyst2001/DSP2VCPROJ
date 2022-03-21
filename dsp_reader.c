#include "dsp_reader.h"

// TODO: support two project types!
int project_type_from_string(const char *p_format_string)
{
	if (strstr(p_format_string, "Static Library"))
		return DSP_PROJ_STATIC;

	// TODO: ALL NEXT IS NOT CHECKED !!!
	if (strstr(p_format_string, "Dynamic Library"))
		return DSP_PROJ_DLL;

	return DSP_INVALID_PROJECT;
}

bool load_developer_studio_project(project_t *p_project, const char *p_filename)
{
	FILE *fp = fopen(p_filename, "rt");
	if(!fp)
		return false;

	memset(p_project, NULL, sizeof(*p_project));

	float version;
	int values[10];
	char line[1024], tempstr[512];
	while (!feof(fp) && !ferror(fp)) {
		fgets(line, sizeof(line), fp);
		SKIP_JUNK_IN_LINE(line)
		if (sscanf(line, "# Microsoft Developer Studio Project File - Name=\"%s\" - Package Owner=<%d>", tempstr, &values[0]) == 2) {
			strcpy(p_project->project_name, tempstr);
			p_project->package_owner = values[0];
			DP("[ OK ] Readed 'Microsoft Developer Studio Project File' header. Name='%s' Package Owner=<%d>\n", tempstr, values[0]);
		}
		else if (sscanf(line, "# Microsoft Developer Studio Generated Build File, Format Version %f", &version) == 1) {
			p_project->version = version;
			DP("[ OK ] Readed Microsoft Developer Studio Build File.  Format Version=%f\n", version);
		}
		else if (sscanf(line, "# TARGTYPE %s 0x%x", tempstr, &values[0]) == 2) {
			int project_type = project_type_from_string(tempstr);
			if (project_type != DSP_INVALID_PROJECT) {
				p_project->project_type = project_type;
				DP("[ OK ] Readed Target type: '%s' (0x%x)   PROJTYPE: %d\n", tempstr, values[0], p_project->project_type);
				continue;
			}
			DP("CRITICAL ERROR: Unsupported target type!\n");
			getchar();
			return false;
		}

		// TODO: ?????????
		//else if (sscanf(line, "", &version) == 1) {
		//	DP("[ OK ] \n", version);
		//}

		//
		// parse project section
		//
		else if (strcmp(line, "# Begin Project")) {
			p_project->number_of_active_branches++;
			build_t *p_last_build = NULL;
			while (!feof(fp) && !ferror(fp) && strcmp(line, "# End Project")) {
				fgets(line, sizeof(line), fp);
				SKIP_JUNK_IN_LINE(line)

				// 
				// parse project build
				// 
				else if (strstr(line, "Begin Custom Build")) {
					p_project->number_of_active_branches++;
					DP("Reading project custom build...\n");
					build_t *p_current_build = NEW(build_t);		
					while (!feof(fp) && !ferror(fp) && !strstr(line, "End Custom Build") != 0) {
						fgets(line, sizeof(line), fp);
						SKIP_JUNK_IN_LINE(line)

						if (sscanf(line, "TargetDir=%s", tempstr) == 1) {
							strcpy(p_current_build->TargetDir, tempstr);
							DP("[ OK ] Readed custom build TargetDir = '%s'\n", tempstr);
						}
						else if (sscanf(line, "TargetPath=%s", tempstr) == 1) {
							strcpy(p_current_build->TargetPath, tempstr);
							DP("[ OK ] Readed custom build TargetPath = '%s'\n", tempstr);
						}
						else if (sscanf(line, "InputPath=%s", tempstr) == 1) {
							strcpy(p_current_build->InputPath, tempstr);
							DP("[ OK ] Readed custom build InputPath = '%s'\n", tempstr);
						}
					}
					p_project->number_of_active_branches--;

					// set current build to head, if head is not set
					if (!p_project->p_head_builds)
						p_project->p_head_builds = p_current_build;

					if (p_last_build) {
						BIND(p_last_build, p_current_build)
					}
					p_last_build = p_current_build;
					DP("[ OK ] End of project custom build.\n");
				}

				// 
				// parse project target
				// 
				else if (strstr(line, "Begin Target")) {
					p_project->number_of_active_branches++;
					DP("Reading project target...\n");
					target_t *p_current_target = &p_project->project_target;
					target_name_t *p_lastname = NULL;
					group_t *p_last_group = NULL;
					while (!feof(fp) && !ferror(fp) && !strstr(line, "End Target")) {
						fgets(line, sizeof(line), fp);
						SKIP_JUNK_IN_LINE(line)

						target_name_t *p_newname = NEW(target_name_t);
						if (sscanf(line, "# Name %s", tempstr) == 1) {
							strcpy(p_newname->name, tempstr);

							if (p_lastname) {
								BIND(p_lastname, p_newname)
							}

							p_lastname = p_newname;

							if (!p_current_target->p_head_name)
								p_current_target->p_head_name = p_newname;

							DP("Readed Target Name: '%s'\n", tempstr);
						}

						// 
						// reading target group
						// 
						else if (sscanf(line, "# Begin Group %s", tempstr) == 1) {
							p_project->number_of_active_branches++;
							DP("Reading project group '%s'...\n", tempstr);
							group_t *p_newgroup = NEW(group_t);
							strcpy(p_newgroup->group_name, tempstr);

							source_t *p_last_source = NULL;
							while (!feof(fp) && !ferror(fp) && !strstr(line, "End Group")) {
								fgets(line, sizeof(line), fp);
								SKIP_JUNK_IN_LINE(line)

								char *p_property = strstr(line, "# PROP ");
								if (p_property && sscanf(p_property, "Default_Filter %s", tempstr) == 1) {
									strcpy(p_newgroup->default_filter, tempstr);
									DP("Read Default_Filter='%s' from group\n", tempstr);
								}

								// 
								// reading target group source
								// 
								else if (strstr(line, "# Begin Source File")) {
									p_project->number_of_active_branches++;
									source_t *p_newsource = NEW(source_t);
									while (!feof(fp) && !ferror(fp) && !strstr(line, "End Source File")) {
										fgets(line, sizeof(line), fp);
										SKIP_JUNK_IN_LINE(line)

										if (sscanf(line, "SOURCE=%s", tempstr) == 1) {
											strcpy(p_newsource->sourcepath, tempstr);
											char *p_source_name = strrchr(p_newsource->sourcepath, '\\');
											if (p_source_name) {
												p_source_name++;
												strcpy(p_newsource->sourcename, p_source_name);
											}
											else {
												strcpy(p_newsource->sourcename, p_newsource->sourcepath);
											}
										}
									}

									if (p_last_source) {
										BIND(p_last_source, p_newsource)
									}

									if (!p_newgroup->p_head_element)
										p_newgroup->p_head_element = p_newsource;

									p_last_source = p_newsource;
									p_project->number_of_active_branches--;
								}
							}
							if (p_last_group) {
								BIND(p_last_group, p_newgroup)
							}
							
							if (!p_current_target->p_head_group)
								p_current_target->p_head_group = p_newgroup;

							p_last_group = p_newgroup;
							p_project->number_of_active_branches--;
							DP("[ OK ] End of project group\n");
						}
					}

					p_project->number_of_active_branches--;
					DP("[ OK ] End of project target.\n");
				}
			}
			p_project->number_of_active_branches--;
		}
	}

	// number of active branches must be equals 0!!
	// if active branches is not equals 0, occured critical reading file error or unexpected end of file
	return (!p_project->number_of_active_branches); 
}

void free_developer_studio_project(project_t *p_project)
{
	dsp_free_recursive((list_t *)p_project->p_head_builds); // free builds
	dsp_free_recursive((list_t *)p_project->project_target.p_head_name); // free target names
	dsp_free_groups(p_project->project_target.p_head_group); // free project groups
}

void dsp_free_recursive(list_t *p_head)
{
	if (!p_head)
		return;

	dsp_free_recursive(p_head->p_nextlink);
	free(p_head);
}

void dsp_free_groups(group_t *p_head)
{
	if (!p_head)
		return;

	dsp_free_recursive((list_t *)p_head->p_head_element);
	dsp_free_groups(p_head->p_nextlink);
	free(p_head);
}

void dsp_print_sources_recursive(source_t *p_head)
{
	if (!p_head)
		return;

	printf("Source:   Name: '%s'  Path: '%s'\n", p_head->sourcename, p_head->sourcepath);
	dsp_print_sources_recursive(p_head->p_nextlink);
}

void dsp_print_bulds_recursive(build_t *p_head)
{
	if (!p_head)
		return;

	printf("Build:   InputPath: '%s'  TargetDir: '%s'  TargetPath: '%s'\n", p_head->InputPath, p_head->TargetDir, p_head->TargetPath);
	dsp_print_bulds_recursive(p_head->p_nextlink);
}
