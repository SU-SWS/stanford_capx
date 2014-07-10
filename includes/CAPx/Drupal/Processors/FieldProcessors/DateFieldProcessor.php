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
   * [getDBFormat description]
   * @param  [type] $type [description]
   * @return [type]       [description]
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
   * Default implementation of put
   * @param  [type] $data [description]
   * @return [type]       [description]
   */
  public function put($data) {
    $data = $this->prepareData($data);
    $entity = $this->getEntity();
    $fieldName = $this->getFieldName();
    $fieldInfo = field_info_field($fieldName);
    $field = $entity->{$fieldName};


    // No need for anything fancy when there is nothing to parse :)
    if (count($data) == 1 && empty($data[0])) {
      $field->set(null);
      return;
    }

    // Reformat the jsonpath return data so it works with Durp.
    $data = $this->repackageJsonDataForDrupal($data, $fieldInfo);

    // No valid colums were found. Truncate field.
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

      $field->set($value);
    }
    else {
      // For everything else give it all.
      $field->set($data);
    }

  }

  /**
   * [widget description]
   * @param  [type] $type [description]
   * @return [type]       [description]
   */
  public function widget($type) {
    return $this;
  }

  /**
   * [prepareData description]
   * @param  [type] $data [description]
   * @return [type]       [description]
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
