<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Processors\FieldProcessors;

abstract class FieldProcessorAbstract implements FieldProcessorInterface {

  // Entity
  protected $entity;

  // Field Name
  protected $fieldName;

  // Type of field or widget
  protected $type;

  /**
   * [__construct description]
   * @param [type] $entity    [description]
   * @param [type] $fieldName [description]
   */
  public function __construct($entity, $fieldName, $type = null) {
    $this->setEntity($entity);
    $this->setFieldName($fieldName);

    if(!is_null($type)) {
      $this->setType($type);
    }

  }

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

    $keys = array_keys($fieldInfo['columns']);
    $key = $keys[0];

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

    drupal_alter('capx_field_processor_pre_set', $entity, $data, $fieldName);

    // Only want the first value for one card field
    if ($fieldInfo['cardinality'] == "1") {
      $field->set($data[0][$key]);
    }
    else {
      // For everything else give it all.
      $field->set($data);
    }

  }

  /**
   * [repackageJsonDataForDrupal description]
   * @param  [type] $data [description]
   * @return [type]       [description]
   */
  public function repackageJsonDataForDrupal($data, $fieldInfo) {
    $return = array();
    $columns = array_keys($fieldInfo['columns']);

    // If the configuration passed doesn't specify a field column to insert the
    // data into assume the first key in the field info columns array
    $columnKey = $columns[0];

    // For when data is passed in with column keys.
    foreach ($columns as $key) {
      if (isset($data[$key])) {
        foreach ($data[$key] as $index => $value) {
          $return[$index][$key] = $value;
        }
      }
    }

    // If no key value was specified then assume the column key.
    if (isset($data[0][0])) {
      foreach($data[0] as $int => $value) {
        $return[$int][$columnKey] = $value;
      }
    }

    return $return;
  }


  //
  // ---------------------------------------------------------------------------
  //

  /**
   * [getEntity description]
   * @return [type] [description]
   */
  public function getEntity() {
    return $this->entity;
  }

  /**
   * [setEntity description]
   * @param [type] $entity [description]
   */
  public function setEntity($entity) {
    $this->entity = $entity;
  }

  /**
   * [getFieldName description]
   * @return [type] [description]
   */
  public function getFieldName() {
    return $this->fieldName;
  }

  /**
   * [setFieldName description]
   * @param [type] $name [description]
   */
  public function setFieldName($name) {
    $this->fieldName = $name;
  }


  /**
   * [setType description]
   * @param [type] $type [description]
   */
  public function setType($type) {
    $this->type = $type;
  }

  /**
   * [getType description]
   * @return [type] [description]
   */
  public function getType() {
    return $this->type;
  }


}
