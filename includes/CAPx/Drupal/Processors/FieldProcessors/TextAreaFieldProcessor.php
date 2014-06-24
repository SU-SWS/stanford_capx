<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Processors\FieldProcessors;

class TextAreaFieldProcessor extends FieldProcessor {

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

    if ($fieldInfo['cardinality'] !== "1") {
      $data = is_array($data) ? $data : array($data);
    }
    else {
      $data = is_array($data) ? array_shift($data) : $data;
    }

    $field->value->set($data);
  }


}
