<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Processors\FieldProcessors;

class TextFieldFieldProcessor extends FieldTypeProcessor {

  /**
   * Default implementation of put.
   *
   * @see FieldProcessorAbstract::put()
   */
  public function put($data) {
    $data = $this->prepareData($data);

    parent::put($data);
  }

  /**
   * Prepares CAP API data to feet to Drupal field.
   *
   * @param array $data
   *   CAP API field data.
   *
   * @return array
   *   Prepared data.
   */
  public function prepareData($data) {
    $fieldName = $this->getFieldName();
    $fieldInfo = field_info_field($fieldName);
    $maxLength = $fieldInfo['settings']['max_length'];

    foreach ($data as $column => $values) {
      foreach ($values as $delta => $value) {
        // @todo: Should we log this? Keep in mind that log will be polluted.
        $data[$column][$delta] = drupal_substr($value, 0, $maxLength);
      }
    }

    return $data;
  }

}
