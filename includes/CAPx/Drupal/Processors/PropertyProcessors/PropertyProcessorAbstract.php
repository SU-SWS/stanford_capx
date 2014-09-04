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
   * Constructor Method
   * @param Entity $entity    The entity whose properties are being modified/set
   * @param string $propertyName the name of the property to modify/set
   */
  public function __construct($entity, $propertyName) {
    $this->setEntity($entity);
    $this->setPropertyName($propertyName);
  }

  /**
   * Default implementation of put
   * @param  array $data An array of CAP API data.
   */
  public function put($data) {
    $entity = $this->getEntity();
    $propertyName = $this->getPropertyName();

    $data = is_array($data) ? array_shift($data) : $data;

    drupal_alter('capx_pre_property_set', $entity, $data, $propertyName);

    // @todo: validate this data. try / catch.
    $entity->{$propertyName}->set($data);
  }


  // Getters and Setters
  // ---------------------------------------------------------------------------
  //

  /**
   * Getter function
   * @return Entity the entity being worked on.
   */
  public function getEntity() {
    return $this->entity;
  }

  /**
   * Setter function
   * @param Entity $entity the entity to be worked on.
   */
  public function setEntity($entity) {
    $this->entity = $entity;
  }

  /**
   * Getter function
   * @return string the name of the property being worked on.
   */
  public function getPropertyName() {
    return $this->PropertyName;
  }

  /**
   * Setter function
   * @param string $name the name of the property to be worked on.
   */
  public function setPropertyName($name) {
    $this->PropertyName = $name;
  }


}
