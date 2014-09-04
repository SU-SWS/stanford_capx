<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Processors\FieldProcessors;

/*
 define('DATE_ISO', 'date');
 define('DATE_UNIX', 'datestamp');
 define('DATE_DATETIME', 'datetime');
 define('DATE_ARRAY', 'array');
 define('DATE_OBJECT', 'object');
*/

class DateFieldProcessor extends FieldTypeProcessor {

  protected $dbFormatValue = "date";

  /**
   * Getter function.
   * A switch statement to get the type of date format the field is storing.
   * @param  string $type the name of the type of date the field is storing.
   * @return integer?       The date type format for reals.
   */
  public function getDBFormat($type) {
    switch ($type) {
      case 'datestamp':
        return date_type_format(DATE_UNIX);
      case 'datetime':
        return date_type_format(DATE_DATETIME);
      case 'date':
        return date_type_format(DATE_ISO);
      case 'array':
        return date_type_format(DATE_ARRAY);
      default:
        return date_type_format(DATE_OBJECT);
    }
  }

  /**
   * Put the data from the CAP API into the field that is being processed.
   * Mangle, fandangle and change the data so that it fits the field's
   * configuration.
   * @param  array $data An array of data from the CAP API.
   */
  public function put($data) {

    // Set values...
    $data = $this->prepareData($data);
    $entity = $this->getEntity();
    $fieldName = $this->getFieldName();
    $fieldInfo = field_info_field($fieldName);
    $field = $entity->{$fieldName};


    // No need for anything fancy when there is nothing to parse :)
    // Just empty out the field and be done.
    if (count($data) == 1 && empty($data[0])) {
      $field->set(null);
      return;
    }

    // Reformat the jsonpath return data so it works with Drupal.
    $data = $this->repackageJsonDataForDrupal($data, $fieldInfo);

    // No valid colums were found. Truncate field and be done.
    if (empty($data)) {
      drupal_set_message('No valid columns found for ' . $fieldName, 'error');
      $field->set(null);
      return;
    }

    // Only want the first value for one card field
    if ($fieldInfo['cardinality'] == "1") {
      $value = array_pop($data);

      // If there is only one value and the key is value then just give up the
      // text. An Array will blow things up. It also looks like entity metadata
      // wrappers expects the value to be an integer when just one. Fucker.
      if (count($value) == 1 && isset($value['value'])) {
        $value = $value['value'];
      }

      // entity_metadata_wrapper set function.
      // @todo: Stop it from puking.
      $field->set($value);
    }
    else {
      // For everything else give it all.
      // entity_metadata_wrapper set function.
      // @todo: Stop it from puking.
      $field->set($data);
    }

  }

  /**
   * Default widget implementation. Just  return itself as we do not have any
   * special per widget type processing.
   * @param  string $type the type of date widget.
   * @return DateFieldProcessor self.
   */
  public function widget($type) {
    return $this;
  }

  /**
   * Prepare data takes what was given to us from the API and turned into a
   * usable format for the entity_metadata_wrapper functions.
   * @param  array $data an array of data from the CAP API
   * @return array       a formatted array of data that can be saved to a field.
   */
  public function prepareData($data) {

    $entity = $this->getEntity();
    $entityType = $entity->type();
    $entityRaw = $entity->raw();
    list($id, $vid, $bundle) = entity_extract_ids($entityType, $entityRaw);
    $fieldName = $this->getFieldName();
    $fieldInfo = field_info_field($fieldName);
    $columns = array_keys($fieldInfo['columns']);
    $timezone = date_get_timezone_db($fieldInfo['settings']['tz_handling']);
    // $fieldInstance = field_info_instance($entityType, $fieldName, $bundle);
    $format = $this->getDBFormat($this->dbFormatValue);

    foreach($data as $index => $row) {

        $value = isset($row[0]) ? $row[0] : $row['value'];
        $value2 = isset($row['value2']) ? $row['value2'] : $value;

        $value = strtotime($value);
        $value2 = strtotime($value2);

        // If there is only one value and the key is value then just give up the
        // text. An Array will blow things up. It also looks like entity metadata
        // wrappers expects the value to be an integer when just one. Fucker.
        if (in_array('value2', $columns)) {
          $value = format_date($value, 'custom', $format, $timezone);
          $value2 = format_date($value2, 'custom', $format, $timezone);
        }

        $data['value'][] = $value;
        $data['value2'][] = $value;

        unset($data[$index][0]);
    }
    unset($data[0]);

    return $data;
  }


}
