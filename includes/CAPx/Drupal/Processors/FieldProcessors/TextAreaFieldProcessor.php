<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Processors\FieldProcessors;

class TextAreaFieldProcessor extends FieldTypeProcessor {

  /**
   * Default implementation of put
   * @param  [type] $data [description]
   * @return [type]       [description]
   */
  public function put($data) {
    $entity = $this->getEntity();
    $fieldName = $this->getFieldName();
    $fieldInfo = field_info_field($fieldName);
    $field = $entity->{$fieldName};
    $type = $this->getType();

    $keys = array_keys($fieldInfo['columns']);
    $key = $keys[0];

    // No need for anything fancy when there is nothing to parse :)
    if (count($data) == 1 && empty($data[0])) {
      $field->set(null);
      return;
    }

    // Also consider field as empty when value and summary passed as empty arrays.
    if (isset($data['value']) && isset($data['summary'])) {
      if (empty($data['value']) && empty($data['summary'])) {
        $field->set(null);
        return;
      }
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

      if ($type == "text" || $type == "text_long") {
        $field->set($data[0][$key]);
      }
      else {
        $field->set($data[0]);
      }

    }
    else {
      // For everything else give it all.
      $field->set($data);
    }
  }


}
