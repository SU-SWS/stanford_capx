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

  /**
   * [__construct description]
   * @param [type] $entity    [description]
   * @param [type] $fieldName [description]
   */
  public function __construct($entity, $fieldName) {
    $this->setEntity($entity);
    $this->setFieldName($fieldName);
  }

  /**
   * Default implementation of put
   * @param  [type] $data [description]
   * @return [type]       [description]
   */
  public function put($data) {

    if (is_null($data)) {
      return;
    }

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

    $field->set($data);
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


}
