#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct xmlhandle_s {
	char type;
	int size;
} xmlhandle_t;

xmlhandle_t *xml_create_document(const char *p_filename, const char *p_version);
xmlhandle_t *xml_begin_node(xmlhandle_t *p_xmlparentnode, const char *p_nodename, int n_attribs, ...);
bool         xml_end_node(xmlhandle_t *p_xmlnode);
bool         xml_node_value(xmlhandle_t *p_xmlnode, const char *p_valformattype, void *p_value);
bool         xml_close_document(xmlhandle_t *p_xmldoc);