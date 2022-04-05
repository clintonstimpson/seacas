/*
 * Copyright(C) 2022 National Technology & Engineering Solutions
 * of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with
 * NTESS, the U.S. Government retains certain rights in this software.
 *
 * See packages/seacas/LICENSE for details
 */

#include "exodusII.h"     // for ex_err, etc
#include "exodusII_int.h" // for EX_FATAL, etc

int ex_put_field_metadata(int exoid, const ex_field field)
{
  /*
   * The attribute name is `Field@{name}@type`
   *
   * Field `nesting` is calculated as size of `type` field
   * The `type` field is a sequence of integers which are the values of the `ex_field_type` enum.
   * NOTE: For backward compatibility, we can only add new entries to this enum at the end.
   *
   * If size of `component_separator` == 0 then the default '_' separator used by all component
   * levels If size of `component_separator` == 1 then that separator used by all component levels
   * Else the size must equal nesting and it specifies a potentially different separator for each
   * level.
   */
  int  status;
  char errmsg[MAX_ERR_LENGTH];

  char       attribute_name[NC_MAX_NAME + 1];
  static int count = 0;
  count++;
  fprintf(stderr, "Field '%s' of type '%s' with separator '%s' on block %lld\n", field.name,
          ex_field_type_enum_to_string(field.type[0]), field.component_separator, field.entity_id);

  for (int i = 0; i < field.nesting; i++) {
    if (field.type[i] == EX_FIELD_TYPE_USER_DEFINED) {
      if (field.nesting > 1) {
        snprintf(errmsg, MAX_ERR_LENGTH,
                 "ERROR: Field '%s' nesting is %d, it must be 1 for a user-defined field type.",
                 field.name, field.nesting);
        ex_err_fn(exoid, __func__, errmsg, EX_BADPARAM);
        return EX_FATAL;
      }
    }
  }

  static char *field_template = "Field@%s@%s";
  sprintf(attribute_name, field_template, field.name, "type");
  if ((status = ex_put_integer_attribute(exoid, field.entity_type, field.entity_id, attribute_name,
                                         field.nesting, field.type)) != EX_NOERR) {
    snprintf(errmsg, MAX_ERR_LENGTH,
             "ERROR: failed to store field metadata type for field '%s' on %s with id %" PRId64
             " in file id %d",
             field.name, ex_name_of_object(field.entity_type), field.entity_id, exoid);
    ex_err_fn(exoid, __func__, errmsg, status);
    return EX_FATAL;
  }

  sprintf(attribute_name, field_template, field.name, "separator");
  if ((status = ex_put_text_attribute(exoid, field.entity_type, field.entity_id, attribute_name,
                                      field.component_separator)) != EX_NOERR) {
    snprintf(errmsg, MAX_ERR_LENGTH,
             "ERROR: failed to store field metadata separator for field '%s' on %s with id %" PRId64
             " in file id %d",
             field.name, ex_name_of_object(field.entity_type), field.entity_id, exoid);
    ex_err_fn(exoid, __func__, errmsg, status);
    return EX_FATAL;
  }

  bool needs_cardinality = false;
  for (int i = 0; i < field.nesting; i++) {
    if (field.type[i] == EX_FIELD_TYPE_USER_DEFINED || field.type[i] == EX_FIELD_TYPE_SEQUENCE) {
      needs_cardinality = true;
      break;
    }
  }
  if (needs_cardinality) {
    sprintf(attribute_name, field_template, field.name, "cardinality");
    if ((status = ex_put_integer_attribute(exoid, field.entity_type, field.entity_id,
                                           attribute_name, field.nesting, field.cardinality)) !=
        EX_NOERR) {
      snprintf(
          errmsg, MAX_ERR_LENGTH,
          "ERROR: failed to store field metadata cardinality for field '%s' on %s with id %" PRId64
          " in file id %d",
          field.name, ex_name_of_object(field.entity_type), field.entity_id, exoid);
      ex_err_fn(exoid, __func__, errmsg, status);
      return EX_FATAL;
    }
  }

  return EX_NOERR;
}

int ex_put_basis_metadata(int exoid, const ex_field field) { return EX_FATAL; }

int ex_put_quadrature_metadata(int exoid, const ex_field field) { return EX_FATAL; }

int ex_put_field_suffices(int exoid, const ex_field field, const char *suffices)
{
  /*
   * For a user-defined field metadata type, output the `cardinality`-count suffices.
   * The suffices are in a single comma-separated string.
   * This call is only valid if the field metadata type is user-defined.
   *
   * Example:  cardinality = 4, type is EX_USER_DEFINED, name is "Species"
   * Suffices = "h2o,gas,ch4,methane"
   * The fields would be "Species_h2o", "Species_gas", "Species_ch4", "Species_methane"
   *
   * Accesses field.entity_type, field.entity_id, field.name, field.type, field.cardinality
   * The attribute name is `Field@{name}@suffices`
   */
  int  status;
  char errmsg[MAX_ERR_LENGTH];

  char         attribute_name[NC_MAX_NAME + 1];
  static char *field_template = "Field@%s@%s";

  if (field.type[0] != EX_FIELD_TYPE_USER_DEFINED) {
    snprintf(
        errmsg, MAX_ERR_LENGTH,
        "ERROR: Field '%s' is not of type EX_FIELD_TYPE_USER_DEFINED; cannot specify suffices.",
        field.name);
    ex_err_fn(exoid, __func__, errmsg, EX_BADPARAM);
    return EX_FATAL;
  }

  /* Count the commas in the comma-separated list of suffices.  Must be one less than the field
   * cardinality... */
  int comma_count = 0;
  for (size_t i = 0; i < strlen(suffices); i++) {
    if (suffices[i] == ',') {
      comma_count++;
    }
  }
  if (comma_count + 1 != field.cardinality[0]) {
    snprintf(errmsg, MAX_ERR_LENGTH,
             "ERROR: Field '%s' cardinality is %d but there were %d suffices defined.  These must "
             "be equal.",
             field.name, field.cardinality[0], comma_count + 1);
    ex_err_fn(exoid, __func__, errmsg, EX_BADPARAM);
    return EX_FATAL;
  }

  sprintf(attribute_name, field_template, field.name, "suffices");
  if ((status = ex_put_text_attribute(exoid, field.entity_type, field.entity_id, attribute_name,
                                      suffices)) != EX_NOERR) {
    snprintf(errmsg, MAX_ERR_LENGTH,
             "ERROR: failed to store field suffices for field '%s' on %s with id %" PRId64
             " in file id %d",
             field.name, ex_name_of_object(field.entity_type), field.entity_id, exoid);
    ex_err_fn(exoid, __func__, errmsg, status);
    return EX_FATAL;
  }
  return EX_NOERR;
}
