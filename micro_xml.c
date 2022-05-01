#include "micro_xml.h"

typedef struct xmldocument_handle_data_s {
	FILE *fp;
	char *p_filename;
	char version[32];
} xmldocument_handle_data_t;

xmlhandle_t *xml_create_document(const char *p_filename, const char *p_version)
{
	FILE *fp = fopen(p_filename, "w");
	if (!fp) {
		DP("xml_create_document: Failed to open file!\n");
		return NULL;
	}

	int size = FULLHANDLESIZE(xmldocument_handle_data_t);
	xmlhandle_t *p_handle = (xmlhandle_t *)NEW(size);
	p_handle->type = XMLHANDLE_TYPE_ROOTDOCUMENT;
	p_handle->size = size;

	xmldocument_handle_data_t *p_handle_data = (xmldocument_handle_data_t *)HANDLE_DATA(p_handle);
	p_handle_data->fp = fp;
	p_handle_data->p_filename = _strdup(p_filename);
	strcpy_s(p_handle_data->version, sizeof(p_handle_data->version), p_version);
	return p_handle;
}