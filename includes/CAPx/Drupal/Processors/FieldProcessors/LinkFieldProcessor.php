<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Processors\FieldProcessors;

class LinkFieldProcessor extends FieldTypeProcessor {

  /**
   * Link field override of the put function.
   * @see  parent::put();
   * @param  array $data an array of CAP API data
   * @return [type]       [description]
   */
  public function put($data) {
    $entity = $this->getEntity();
    $fieldName = $this->getFieldName();
    $fieldInfo = field_info_field($fieldName);
    $field = $entity->{$fieldName};

    $keys = array_keys($fieldInfo['columns']);

    // No need for anything fancy when there is nothing to parse :)
    if (empty($data) && empty($data[0])) {
      $field->set(null);
      return;
    }

    // Reformat the jsonpath return data so it works with Durp.
    $data = $this->repackageJsonDataForDrupal($data, $fieldInfo);

    if ($fieldInfo['cardinality'] !== "1") {
      $field->set($data);
    }
    else {
      foreach($keys as $columnName) {
        if (isset($data[0][$columnName])) {
          $field->{$columnName}->set($data[0][$columnName]);
        }
      }
    }

  }

}
