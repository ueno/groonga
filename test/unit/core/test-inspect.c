/* -*- c-basic-offset: 2; coding: utf-8 -*- */
/*
  Copyright (C) 2010  Kouhei Sutou <kou@clear-code.com>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License version 2.1 as published by the Free Software Foundation.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <gcutter.h>
#include <glib/gstdio.h>

#include "../lib/grn-assertions.h"

#include <str.h>
#include <util.h>

#define get(name) grn_ctx_get(context, name, strlen(name))

void test_null(void);
void test_void(void);
void test_int8(void);
void test_int16(void);
void test_int32(void);
void test_int64(void);
void test_uint8(void);
void test_uint16(void);
void test_uint32(void);
void test_uint64(void);
void test_float(void);
void test_time(void);
void test_bool_true(void);
void test_bool_false(void);
void test_text(void);
void test_geo_point_tokyo(void);
void test_geo_point_wgs84(void);
void test_array_empty(void);
void test_array_with_records(void);
void test_hash_empty(void);
void test_hash_with_records(void);
void test_patricia_trie_empty(void);
void test_patricia_trie_with_records(void);
void test_uvector_empty(void);
void test_uvector_with_records(void);
void test_uvector_bool(void);
void test_vector_empty(void);
void test_pvector_empty(void);
void test_pvector_with_records(void);
void data_accessor_column_name(void);
void test_accessor_column_name(gconstpointer data);
void data_accessor_dynamic_pseudo_column_name(void);
void test_accessor_dynamic_pseudo_column_name(gconstpointer data);

static gchar *tmp_directory;

static grn_ctx *context;
static grn_obj *database;

static grn_obj *inspected;

static grn_obj *void_value;
static grn_obj *int8, *int16, *int32, *int64;
static grn_obj *uint8, *uint16, *uint32, *uint64;
static grn_obj *float_value;
static grn_obj *time_value;
static grn_obj *bool_value;
static grn_obj *text;
static grn_obj *geo_point_tokyo, *geo_point_wgs84;
static grn_obj *uvector;
static grn_obj *pvector;
static grn_obj *vector;

void
cut_startup(void)
{
  tmp_directory = g_build_filename(grn_test_get_base_dir(),
                                   "tmp",
                                   "inspect",
                                   NULL);
}

void
cut_shutdown(void)
{
  g_free(tmp_directory);
}

static void
remove_tmp_directory(void)
{
  cut_remove_path(tmp_directory, NULL);
}

static void
setup_values(void)
{
  void_value = NULL;
  int8 = int16 = int32 = int64 = NULL;
  uint8 = uint16 = uint32 = uint64 = NULL;
  float_value = NULL;
  time_value = NULL;
  bool_value = NULL;
  text = NULL;
  geo_point_tokyo = geo_point_wgs84 = NULL;
  uvector = NULL;
  pvector = NULL;
  vector = NULL;
}

void
cut_setup(void)
{
  const gchar *database_path;

  inspected = NULL;
  setup_values();

  remove_tmp_directory();
  g_mkdir_with_parents(tmp_directory, 0700);

  context = g_new0(grn_ctx, 1);
  grn_ctx_init(context, 0);

  database_path = cut_build_path(tmp_directory, "database.groonga", NULL);
  database = grn_db_create(context, database_path, NULL);
}

static void
teardown_values(void)
{
  grn_obj_unlink(context, void_value);
  grn_obj_unlink(context, int8);
  grn_obj_unlink(context, int16);
  grn_obj_unlink(context, int32);
  grn_obj_unlink(context, int64);
  grn_obj_unlink(context, uint8);
  grn_obj_unlink(context, uint16);
  grn_obj_unlink(context, uint32);
  grn_obj_unlink(context, uint64);
  grn_obj_unlink(context, float_value);
  grn_obj_unlink(context, time_value);
  grn_obj_unlink(context, bool_value);
  grn_obj_unlink(context, text);
  grn_obj_unlink(context, geo_point_tokyo);
  grn_obj_unlink(context, geo_point_wgs84);
  grn_obj_unlink(context, uvector);
  grn_obj_unlink(context, pvector);
  grn_obj_unlink(context, vector);
}

void
cut_teardown(void)
{
  teardown_values();

  grn_obj_unlink(context, inspected);

  if (context) {
    grn_ctx_fin(context);
    g_free(context);
  }

  remove_tmp_directory();
}

static const gchar *
inspected_string (void)
{
  return cut_take_printf("%.*s",
                         (int)GRN_TEXT_LEN(inspected),
                         GRN_TEXT_VALUE(inspected));
}

void
test_null(void)
{
  inspected = grn_inspect(context, NULL, NULL);
  cut_assert_equal_string("(NULL)", inspected_string());
}

void
test_void_value(void)
{
  void_value = grn_obj_open(context, GRN_BULK, 0, GRN_DB_VOID);
  GRN_TEXT_PUTS(context, void_value, "void");
  inspected = grn_inspect(context, NULL, void_value);
  cut_assert_equal_string("\"void\"", inspected_string());
}

void
test_int8(void)
{
  int8 = grn_obj_open(context, GRN_BULK, 0, GRN_DB_INT8);
  GRN_INT8_SET(context, int8, G_MAXINT8);
  inspected = grn_inspect(context, NULL, int8);
  cut_assert_equal_string(cut_take_printf("%d", G_MAXINT8),
                          inspected_string());
}

void
test_int16(void)
{
  int16 = grn_obj_open(context, GRN_BULK, 0, GRN_DB_INT16);
  GRN_INT16_SET(context, int16, G_MAXINT16);
  inspected = grn_inspect(context, NULL, int16);
  cut_assert_equal_string(cut_take_printf("%" G_GINT16_FORMAT, G_MAXINT16),
                          inspected_string());
}

void
test_int32(void)
{
  int32 = grn_obj_open(context, GRN_BULK, 0, GRN_DB_INT32);
  GRN_INT32_SET(context, int32, G_MAXINT32);
  inspected = grn_inspect(context, NULL, int32);
  cut_assert_equal_string(cut_take_printf("%" G_GINT32_FORMAT, G_MAXINT32),
                          inspected_string());
}

void
test_int64(void)
{
  int64 = grn_obj_open(context, GRN_BULK, 0, GRN_DB_INT64);
  GRN_INT64_SET(context, int64, G_MAXINT64);
  inspected = grn_inspect(context, NULL, int64);
  cut_assert_equal_string(cut_take_printf("%" G_GINT64_FORMAT, G_MAXINT64),
                          inspected_string());
}

void
test_uint8(void)
{
  uint8 = grn_obj_open(context, GRN_BULK, 0, GRN_DB_UINT8);
  GRN_UINT8_SET(context, uint8, G_MAXUINT8);
  inspected = grn_inspect(context, NULL, uint8);
  cut_assert_equal_string(cut_take_printf("%u", G_MAXUINT8),
                          inspected_string());
}

void
test_uint16(void)
{
  uint16 = grn_obj_open(context, GRN_BULK, 0, GRN_DB_UINT16);
  GRN_UINT16_SET(context, uint16, G_MAXUINT16);
  inspected = grn_inspect(context, NULL, uint16);
  cut_assert_equal_string(cut_take_printf("%" G_GUINT16_FORMAT, G_MAXUINT16),
                          inspected_string());
}

void
test_uint32(void)
{
  uint32 = grn_obj_open(context, GRN_BULK, 0, GRN_DB_UINT32);
  GRN_UINT32_SET(context, uint32, G_MAXUINT32);
  inspected = grn_inspect(context, NULL, uint32);
  cut_assert_equal_string(cut_take_printf("%" G_GUINT32_FORMAT, G_MAXUINT32),
                          inspected_string());
}

void
test_uint64(void)
{
  uint64 = grn_obj_open(context, GRN_BULK, 0, GRN_DB_UINT64);
  GRN_UINT64_SET(context, uint64, G_MAXUINT64);
  inspected = grn_inspect(context, NULL, uint64);
  cut_assert_equal_string(cut_take_printf("%" G_GUINT64_FORMAT, G_MAXUINT64),
                          inspected_string());
}

void
test_float(void)
{
  float_value = grn_obj_open(context, GRN_BULK, 0, GRN_DB_FLOAT);
  GRN_FLOAT_SET(context, float_value, 0.29);
  inspected = grn_inspect(context, NULL, float_value);
  cut_assert_equal_string("0.29", inspected_string());
}

void
test_time(void)
{
  GTimeVal g_time_value;

  g_time_val_from_iso8601("2010-05-31T11:50:29.29+0900", &g_time_value);
  time_value = grn_obj_open(context, GRN_BULK, 0, GRN_DB_TIME);
  GRN_TIME_SET(context, time_value,
               g_time_value.tv_sec * G_USEC_PER_SEC + g_time_value.tv_usec);
  inspected = grn_inspect(context, NULL, time_value);
  cut_assert_equal_string(cut_take_printf("%ld.29", g_time_value.tv_sec),
                          inspected_string());
}

void
test_bool_true(void)
{
  bool_value = grn_obj_open(context, GRN_BULK, 0, GRN_DB_BOOL);
  GRN_BOOL_SET(context, bool_value, GRN_TRUE);
  inspected = grn_inspect(context, NULL, bool_value);
  cut_assert_equal_string("true", inspected_string());
}

void
test_bool_false(void)
{
  bool_value = grn_obj_open(context, GRN_BULK, 0, GRN_DB_BOOL);
  GRN_BOOL_SET(context, bool_value, GRN_FALSE);
  inspected = grn_inspect(context, NULL, bool_value);
  cut_assert_equal_string("false", inspected_string());
}

void
test_text(void)
{
  text = grn_obj_open(context, GRN_BULK, 0, GRN_DB_TEXT);
  GRN_TEXT_PUTS(context, text, "niku");
  inspected = grn_inspect(context, NULL, text);
  cut_assert_equal_string("\"niku\"", inspected_string());
}

void
test_geo_point_tokyo(void)
{
  gint takane_latitude, takane_longitude;

  takane_latitude = grn_test_coordinate_in_milliseconds(35.6954581363924);
  takane_longitude = grn_test_coordinate_in_milliseconds(139.564207350021);

  geo_point_tokyo = grn_obj_open(context, GRN_BULK, 0, GRN_DB_TOKYO_GEO_POINT);
  GRN_GEO_POINT_SET(context, geo_point_tokyo, takane_latitude, takane_longitude);
  inspected = grn_inspect(context, NULL, geo_point_tokyo);
  cut_assert_equal_string("\"130194581x503802073\"", inspected_string());
}

void
test_geo_point_wgs84(void)
{
  gint takane_latitude, takane_longitude;

  takane_latitude = grn_test_coordinate_in_milliseconds(35.69869);
  takane_longitude = grn_test_coordinate_in_milliseconds(139.56099);

  geo_point_wgs84 = grn_obj_open(context, GRN_BULK, 0, GRN_DB_WGS84_GEO_POINT);
  GRN_GEO_POINT_SET(context, geo_point_wgs84, takane_latitude, takane_longitude);
  inspected = grn_inspect(context, NULL, geo_point_wgs84);
  cut_assert_equal_string("\"130226900x503769900\"", inspected_string());
}

void
test_array_empty(void)
{
  assert_send_command("table_create Sites TABLE_NO_KEY");
  inspected = grn_inspect(context, NULL, get("Sites"));
  cut_assert_equal_string("[]", inspected_string());
}

void
test_array_with_records(void)
{
  cut_omit("array with record isn't supported yet.");
  assert_send_command("table_create Sites TABLE_NO_KEY");
  assert_send_command("column_create Sites name COLUMN_SCALAR Text");
  assert_send_command("load "
                      "'[[\"name\"],[\"groonga.org\"],[\"razil.jp\"]]' "
                      "Sites");
  inspected = grn_inspect(context, NULL, get("Sites"));
  cut_assert_equal_string("[1, 2]", inspected_string());
}

void
test_hash_empty(void)
{
  assert_send_command("table_create Sites TABLE_HASH_KEY ShortText");
  inspected = grn_inspect(context, NULL, get("Sites"));
  cut_assert_equal_string("[]", inspected_string());
}

void
test_hash_with_records(void)
{
  assert_send_command("table_create Sites TABLE_HASH_KEY ShortText");
  assert_send_command("column_create Sites name COLUMN_SCALAR Text");
  assert_send_command("load "
                      "'["
                      "[\"_key\",\"name\"],"
                      "[\"groonga.org\",\"groonga\"],"
                      "[\"razil.jp\",\"Brazil\"]"
                      "]' "
                      "Sites");
  inspected = grn_inspect(context, NULL, get("Sites"));
  cut_assert_equal_string("[\"groonga.org\",\"razil.jp\"]", inspected_string());
}

void
test_patricia_trie_empty(void)
{
  assert_send_command("table_create Sites TABLE_PAT_KEY ShortText");
  inspected = grn_inspect(context, NULL, get("Sites"));
  cut_assert_equal_string("[]", inspected_string());
}

void
test_patricia_trie_with_records(void)
{
  assert_send_command("table_create Sites TABLE_PAT_KEY ShortText");
  assert_send_command("column_create Sites name COLUMN_SCALAR Text");
  assert_send_command("load "
                      "'["
                      "[\"_key\",\"name\"],"
                      "[\"groonga.org\",\"groonga\"],"
                      "[\"razil.jp\",\"Brazil\"]"
                      "]' "
                      "Sites");
  inspected = grn_inspect(context, NULL, get("Sites"));
  cut_assert_equal_string("[\"groonga.org\",\"razil.jp\"]", inspected_string());
}

void
test_uvector_empty(void)
{
  assert_send_command("table_create Sites TABLE_PAT_KEY ShortText");
  uvector = grn_obj_open(context, GRN_UVECTOR, 0,
                         grn_obj_id(context, get("Sites")));
  inspected = grn_inspect(context, NULL, uvector);
  cut_assert_equal_string("[]", inspected_string());
}

void
test_uvector_with_records(void)
{
  assert_send_command("table_create Sites TABLE_PAT_KEY ShortText");
  assert_send_command("load "
                      "'[[\"_key\"],[\"groonga.org\"],[\"razil.jp\"]]' "
                      "Sites");
  uvector = grn_obj_open(context, GRN_UVECTOR, 0,
                         grn_obj_id(context, get("Sites")));
  GRN_RECORD_PUT(context, uvector, 1);
  GRN_RECORD_PUT(context, uvector, 2);
  inspected = grn_inspect(context, NULL, uvector);
  cut_assert_equal_string("[\"groonga.org\",\"razil.jp\"]", inspected_string());
}

void
test_uvector_bool(void)
{
  uvector = grn_obj_open(context, GRN_UVECTOR, 0, GRN_DB_BOOL);
  GRN_BOOL_PUT(context, uvector, TRUE);
  GRN_BOOL_PUT(context, uvector, FALSE);
  inspected = grn_inspect(context, NULL, uvector);
  cut_assert_equal_string("[true,false]", inspected_string());
}

void
test_pvector_empty(void)
{
  pvector = grn_obj_open(context, GRN_PVECTOR, 0, GRN_ID_NIL);
  inspected = grn_inspect(context, NULL, pvector);
  cut_assert_equal_string("[]", inspected_string());
}

void
test_pvector_with_records(void)
{
  grn_obj *groonga, *razil;

  pvector = grn_obj_open(context, GRN_PVECTOR, 0, GRN_ID_NIL);
  groonga = grn_obj_open(context, GRN_BULK, 0, GRN_DB_SHORT_TEXT);
  razil = grn_obj_open(context, GRN_BULK, 0, GRN_DB_SHORT_TEXT);
  GRN_TEXT_PUTS(context, groonga, "groonga");
  GRN_TEXT_PUTS(context, razil, "razil");
  GRN_PTR_PUT(context, pvector, groonga);
  GRN_PTR_PUT(context, pvector, razil);
  inspected = grn_inspect(context, NULL, pvector);
  cut_assert_equal_string("[\"groonga\",\"razil\"]", inspected_string());
}

void
test_vector_empty(void)
{
  vector = grn_obj_open(context, GRN_VECTOR, 0, GRN_DB_TEXT);
  inspected = grn_inspect(context, NULL, vector);
  cut_assert_equal_string("[]", inspected_string());
}

void
data_accessor_column_name(void)
{
#define ADD_DATUM(table, accessor)                      \
  gcut_add_datum(table "." accessor,                    \
                 "table", G_TYPE_STRING, table,         \
                 "accessor", G_TYPE_STRING, accessor,   \
                 NULL)

  ADD_DATUM("Sites", "_id");
  ADD_DATUM("Sites", "_key");
  ADD_DATUM("Sites", "_value");
  ADD_DATUM("Sites", "name._id");
  ADD_DATUM("Sites", "name._key");
  ADD_DATUM("Sites", "name._value");
  ADD_DATUM("Sites", "name.site");
  ADD_DATUM("Sites", "name.site.name");
  ADD_DATUM("Names", "site.name.site");

#undef ADD_DATUM
}

void
test_accessor_column_name(gconstpointer data)
{
  const char *table_name = gcut_data_get_string(data, "table");
  const char *accessor_name = gcut_data_get_string(data, "accessor");
  grn_obj *object, *accessor;

  assert_send_command("table_create Sites TABLE_PAT_KEY ShortText Int32");
  assert_send_command("table_create Names TABLE_PAT_KEY ShortText UInt32");
  assert_send_command("column_create Sites name COLUMN_SCALAR Names");
  assert_send_command("column_create Names site COLUMN_SCALAR Sites");

  object = get_object(table_name);
  accessor = grn_obj_column(context, object,
                            accessor_name, strlen(accessor_name));
  cut_assert_not_null(accessor);
  inspected = grn_inspect(context, NULL, accessor);
  cut_assert_equal_string(accessor_name, inspected_string());
}

void
data_accessor_dynamic_pseudo_column_name(void)
{
#define ADD_DATUM(accessor)                             \
  gcut_add_datum(accessor,                              \
                 "accessor", G_TYPE_STRING, accessor,   \
                 NULL)

  ADD_DATUM("_score");
  ADD_DATUM("_nsubrecs");

#undef ADD_DATUM
}

void
test_accessor_dynamic_pseudo_column_name(gconstpointer data)
{
  const char *query, *accessor_name;
  grn_obj *table, *result, *expression, *variable, *accessor;

  assert_send_command("table_create Sites TABLE_PAT_KEY ShortText");
  assert_send_command("column_create Sites uri COLUMN_SCALAR ShortText");
  assert_send_command("load "
                      "'["
                      "[\"_key\",\"uri\"],"
                      "[\"groonga\",\"http://groonga.org/\"]"
                      "]' "
                      "Sites");

  accessor_name = gcut_data_get_string(data, "accessor");

  table = get_object("Sites");
  GRN_EXPR_CREATE_FOR_QUERY(context, table, expression, variable);
  query = "_key:groonga";
  grn_expr_parse(context, expression,
                 query, strlen(query),
                 NULL, GRN_OP_MATCH, GRN_OP_AND,
                 GRN_EXPR_SYNTAX_QUERY | GRN_EXPR_ALLOW_COLUMN);
  result = grn_table_select(context, table, expression, NULL, GRN_OP_AND);
  accessor = grn_obj_column(context, result,
                            accessor_name, strlen(accessor_name));
  cut_assert_not_null(accessor);
  inspected = grn_inspect(context, NULL, accessor);
  cut_assert_equal_string(accessor_name, inspected_string());
}