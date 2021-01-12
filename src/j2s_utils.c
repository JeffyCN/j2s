/*
 *  Copyright (c) 2020, Rockchip Electronics Co., Ltd
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 */

#include "j2s.h"
#include "j2s_generated.h"

#include <sys/stat.h>

typedef struct {
	void *ptr;
	bool freeable;
} j2s_ptr;

typedef struct {
	int num_data;
	j2s_ptr *data;
} j2s_priv_data;

void *j2s_alloc_data(j2s_ctx *ctx, size_t size) {
	void *ptr = calloc(size, 1);
	if (!ptr)
		return NULL;

	if (j2s_add_data(ctx, ptr, true) < 0) {
		free(ptr);
		return NULL;
	}

	return ptr;
}

int j2s_add_data(j2s_ctx *ctx, void *ptr, bool freeable) {
	j2s_priv_data *priv;

	if (!ctx->priv)
		ctx->priv = calloc(1, sizeof(j2s_priv_data));

	priv = ctx->priv;
	for (int i = 0; i < priv->num_data; i++) {
		j2s_ptr *data = &priv->data[i];
		if (data->ptr)
			continue;

		data->ptr = ptr;
		data->freeable = freeable;
		return 0;
	}

	priv->num_data ++;
	priv->data = realloc(priv->data, priv->num_data * sizeof(j2s_ptr));
	if (!priv->data) {
		ERR("failed to realloc\n");
		priv->num_data = 0;
		return -1;
	}

	priv->data[priv->num_data - 1].ptr = ptr;
	priv->data[priv->num_data - 1].freeable = freeable;
	return 0;
}

void j2s_release_data(j2s_ctx *ctx, void *ptr) {
	j2s_priv_data *priv = ctx->priv;

	for (int i = 0; priv && i < priv->num_data; i++) {
		j2s_ptr *data = &priv->data[i];
		if (ptr != data->ptr)
			continue;

		if (data->ptr && data->freeable)
			free(data->ptr);

		data->ptr = NULL;
		return;
	}

	if (ptr)
		free(ptr);
}

void* j2s_read_file(const char *file, size_t *size) {
	struct stat st;
	void *buf;
	int fd;

	DASSERT_MSG(file && !stat(file, &st), return NULL,
		    "no such file: '%s'\n", file ? file : "<null>");

	fd = open(file, O_RDONLY);
	DASSERT_MSG(fd >= 0, return NULL, "failed to open: '%s'\n", file);

	buf = malloc(st.st_size);
	DASSERT(buf, return NULL);

	DBG("Read file: '%s'\n", file);

	if (read(fd, buf, st.st_size) != st.st_size) {
		ERR("failed to read: '%s'\n", file);
		free(buf);
		close(fd);
		return NULL;
	}

	*size = st.st_size;

	close(fd);
	return buf;
}

int j2s_load_cache(const char *file, j2s_ctx *ctx) {
	struct stat st;
	void *buf, *ptr;
	size_t size;
	int ret = -1;

	if (!file || stat(file, &st) < 0) {
		DBG("No cache file: '%s'\n", file);
		return -1;
	}

	DASSERT_MSG(getuid() == st.st_uid, goto err,
		    "invalid cache: '%s'\n", file);

	buf = j2s_read_file(file, &size);
	DASSERT_MSG(buf && size > sizeof(*ctx), goto err,
		    "invalid cache: '%s'\n", file);

	DBG("Load cache: '%s'\n", file);

	ptr = buf;

	*ctx = *(j2s_ctx *)ptr;
	ctx->priv = NULL;
	ptr += sizeof(*ctx);

	DASSERT_MSG(ctx->magic == J2S_MAGIC &&
		    ctx->num_obj == J2S_NUM_OBJ &&
		    ctx->num_struct == J2S_NUM_STRUCT &&
		    ctx->num_enum == J2S_NUM_ENUM &&
		    ctx->num_enum_value == J2S_NUM_ENUM_VALUE, goto err,
		    "invalid cache: '%s'\n", file);

	ctx->objs = ptr;
	ptr += ctx->num_obj * sizeof(*ctx->objs);

	ctx->structs = ptr;
	ptr += ctx->num_struct * sizeof(*ctx->structs);

	ctx->enums = ptr;
	ptr += ctx->num_enum * sizeof(*ctx->enums);

	ctx->enum_values = ptr;
	ptr += ctx->num_enum_value * sizeof(*ctx->enum_values);

	DASSERT_MSG(ptr == buf + size, goto err,
		    "invalid cache: '%s'\n", file);

	DASSERT(!j2s_add_data(ctx, buf, true), goto err);
	return 0;
err:
	j2s_deinit(ctx);
	free(buf);
	return -1;
}

int j2s_save_cache(j2s_ctx *ctx, const char *file) {
	int fd;

	if (!file)
		return -1;

	unlink(file);
	fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	DASSERT_MSG(fd >= 0, return -1, "failed to open: '%s'\n", file);

	DBG("Save cache: '%s'\n", file);

	ctx->num_desc = 0;

	write(fd, ctx, sizeof(*ctx));
	write(fd, ctx->objs, ctx->num_obj * sizeof(*ctx->objs));
	write(fd, ctx->structs, ctx->num_struct * sizeof(*ctx->structs));
	write(fd, ctx->enums, ctx->num_enum * sizeof(*ctx->enums));
	write(fd, ctx->enum_values,
	      ctx->num_enum_value * sizeof(*ctx->enum_values));

	close(fd);
	return 0;
}

void j2s_init(j2s_ctx *ctx) {
	DBG("J2S version: %s\n", J2S_VERSION);

#ifdef J2S_ENABLE_DESC
        _j2s_init(ctx);
#else
	const char *file = getenv("J2S_CACHE");

	file = file ?: "/var/cache/j2s-cache";

        if (j2s_load_cache(file, ctx) < 0) {
		_j2s_init(ctx);
		j2s_save_cache(ctx, file);
	}
#endif

	ctx->manage_data = true;
}

void j2s_deinit(j2s_ctx *ctx) {
	j2s_priv_data *priv = ctx->priv;

	for (int i = 0; priv && i < priv->num_data; i++) {
		j2s_ptr *data = &priv->data[i];
		if (!data->ptr || !data->freeable)
			continue;

		/* Always free the cache file buf */
		if (ctx->manage_data ||
		    data->ptr + sizeof(*ctx) == ctx->objs)
			free(data->ptr);
	}

	if (priv) {
		if (priv->data)
			free(priv->data);
		free(priv);
	}
}

int j2s_json_file_to_struct(j2s_ctx *ctx, const char *file, void *ptr) {
	char *buf;
	size_t size;
	int ret;

	buf = j2s_read_file(file, &size);
	DASSERT(buf, return -1);

	DBG("Parse file: '%s', content:\n%s\n", file, buf);

	ret = j2s_modify_struct(ctx, buf, ptr);

	free(buf);
	return ret;
}

char *j2s_dump_struct(j2s_ctx *ctx, const char *name, void *ptr) {
	cJSON *json, *item;
	char *buf;

	DBG("Dump: %s\n", name ? name : "root struct");

	if (!name) {
		json = j2s_root_struct_to_json(ctx, ptr);
	} else {
		json = j2s_struct_to_json(ctx, name, ptr);
	}

	DASSERT(json, return NULL);

	if (ctx->dump_enums) {
		item = j2s_enums_to_json(ctx);
		if (item)
			cJSON_AddItemToObject(json, "@enum", item);
	}

	if (ctx->format_json) {
		buf = cJSON_Print(json);
	} else {
		buf = cJSON_PrintUnformatted(json);
	}

	cJSON_Delete(json);
	return buf;
}

int j2s_modify_struct(j2s_ctx *ctx, const char *str, void *ptr) {
	cJSON *json;
	int ret = -1;

	json = cJSON_Parse(str);
	DASSERT_MSG(json, return -1, "failed to parse: '%s'\n", str);

	DBG("Modify:\n%s\n", str);

	ret = j2s_json_to_root_struct(ctx, json, ptr);

	cJSON_Delete(json);
	return ret;
}

char *j2s_query_struct(j2s_ctx *ctx, const char *str, void *ptr) {
	cJSON *json;
	char *buf;

	json = cJSON_Parse(str);
	DASSERT_MSG(json, return NULL, "failed to parse: '%s'\n", str);

	DBG("Query:\n%s\n", str);

	if (j2s_json_from_root_struct(ctx, json, ptr) < 0) {
		cJSON_Delete(json);
		return NULL;
	}

	if (ctx->format_json) {
		buf = cJSON_Print(json);
	} else {
		buf = cJSON_PrintUnformatted(json);
	}

	cJSON_Delete(json);
	return buf;
}

char *j2s_dump_template_struct(j2s_ctx *ctx, const char *name) {
	cJSON *json, *item;
	char *buf;

	DBG("Dump template: %s\n", name ? name : "root struct");

	if (!name) {
		json = j2s_root_struct_to_template_json(ctx);
	} else {
		json = j2s_struct_to_template_json(ctx, name);
	}

	DASSERT(json, return NULL);

	if (ctx->dump_enums) {
		item = j2s_enums_to_json(ctx);
		if (item)
			cJSON_AddItemToObject(json, "@enum", item);
	}

	if (ctx->format_json) {
		buf = cJSON_Print(json);
	} else {
		buf = cJSON_PrintUnformatted(json);
	}

	cJSON_Delete(json);
	return buf;
}

char *j2s_dump_structs(j2s_ctx *ctx, j2s_struct_info *info) {
	cJSON *json, *item;
	char *buf;

	if (!info || !info->name)
		return NULL;

	DBG("Dump structs\n");

	json = cJSON_CreateObject();
	DASSERT(json, return NULL);

	for (; info->name; info++) {
		item = j2s_struct_to_json(ctx, info->name, info->ptr);
		if (!item)
			continue;

		cJSON_AddItemToObject(json, info->name, item);
	}

	if (ctx->dump_enums) {
		item = j2s_enums_to_json(ctx);
		if (item)
			cJSON_AddItemToObject(json, "@enum", item);
	}

	if (ctx->format_json) {
		buf = cJSON_Print(json);
	} else {
		buf = cJSON_PrintUnformatted(json);
	}

	cJSON_Delete(json);
	return buf;
}
