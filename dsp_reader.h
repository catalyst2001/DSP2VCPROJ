#pragma once
#include "main.h"

//# Microsoft Developer Studio Project File - Name = "shaderlib" - Package Owner = <4>
//# Microsoft Developer Studio Generated Build File, Format Version 6.00

#define DSP_STATIC "Win32 (x86) Static Library"

// 
// Project types
// 
#define DSP_INVALID_PROJECT 0
#define DSP_PROJ_STATIC 1
#define DSP_PROJ_DLL 2
#define DSP_PROJ_CONSOLE 3
#define DSP_PROJ_WIN32 4

// general list struct ( for free )
typedef struct list_s {
	struct list_s *p_lastlink;
	struct list_s *p_nextlink;
} list_t;

typedef struct source_s {
	struct source_s *p_lastlink;
	struct source_s *p_nextlink;
	char sourcename[512];
	char sourcepath[512];
} source_t;
static int c1 = sizeof(source_t);

typedef struct group_s {
	struct group_s *p_lastlink;
	struct group_s *p_nextlink;
	char group_name[512];
	char default_filter[512];
	source_t *p_head_element;
} group_t;
static int c2 = sizeof(group_t);

typedef struct build_s {
	struct build_s *p_lastlink;
	struct build_s *p_nextlink;
	char TargetDir[512];
	char TargetPath[512];
	char InputPath[512];
} build_t;

typedef struct target_name_s {
	struct target_names_s *p_lastlink;
	struct target_names_s *p_nextlink;
	char name[128];
} target_name_t;

typedef struct target_s {
	group_t *p_head_group;
	target_name_t *p_head_name;
} target_t;

typedef struct project_s {
	float version;
	int package_owner;
	int project_type;
	char project_name[512];
	char Scc_ProjName[128];
	char Scc_LocalPath[64];

	build_t *p_head_builds;
	target_t project_target;

	bool Use_MFC;
	bool Use_Debug_Libraries;

	int number_of_active_branches; // check to unexpected end of file
} project_t;
static int c3 = sizeof(project_t);

#define BIND(last, next) \
	(next)->p_lastlink = last;\
	(last)->p_nextlink = next;
	
bool load_developer_studio_project(project_t *p_project, const char *p_filename);
void free_developer_studio_project(project_t *p_project);

void dsp_free_recursive(list_t *p_head);
void dsp_free_groups(group_t *p_head);

// 
// debug utils
// 
void dsp_print_sources_recursive(source_t *p_head);
void dsp_print_bulds_recursive(build_t *p_head);