<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Processors\PropertyProcessors;

abstract class PropertyProcessorAbstract implements PropertyProcessorInterface {

  // Entity
  protected $entity;

  // Property Name
  protected $propertyName;

  /**
   * [__construct description]
   * @param [type] $entity    [description]
   * @param [type] $propertyName [description]
   */
  public function __construct($entity, $propertyName) {
    $this->setEntity($entity);
    $this->setPropertyName($propertyName);
  }

  /**
   * Default implementation of put
   * @param  [type] $data [description]
   * @return [type]       [description]
   */
  public function put($data) {
    $entity = $this->getEntity();
    $propertyName = $this->getPropertyName();

    $data = is_array($data) ? array_shift($data) : $data;
    $entity->{$propertyName}->set($data);
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
   * [getPropertyName description]
   * @return [type] [description]
   */
  public function getPropertyName() {
    return $this->PropertyName;
  }

  /**
   * [setPropertyName description]
   * @param [type] $name [description]
   */
  public function setPropertyName($name) {
    $this->PropertyName = $name;
  }


}
