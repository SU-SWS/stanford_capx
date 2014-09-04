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
   * Construction method
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
   * Default implementation of put. Puts the information from the CAP API
   * In to the field via entity_metadata_wrapper. Tries to handle information
   * being provided to it for most field types. Specific FieldProcessors may
   * override this function to provide their own custom procsessing.
   * @param  array $data an array of data from the CAP API.
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

    // Allow others to alter the data before it is set to the field.
    drupal_alter('capx_field_processor_pre_set', $entity, $data, $fieldName);

    // Only want the first value for one cardinality field
    if ($fieldInfo['cardinality'] == "1") {
      $field->set($data[0][$key]);
    }
    else {
      // For everything else give it all.
      $field->set($data);
    }

  }

  /**
   * Takes the data from the CAP API and turns it into an array that can be
   * used by entity_metadata_wrapper's set function.
   * @param  array $data CAP API data
   * @return array       an array of data suitable for saving to a field.
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
   * @return string the field name being processed.
   */
  public function getFieldName() {
    return $this->fieldName;
  }

  /**
   * Setter function
   * @param string $name the field name to be processed.
   */
  public function setFieldName($name) {
    $this->fieldName = $name;
  }


  /**
   * Setter function
   * @param string $type The field type.
   */
  public function setType($type) {
    $this->type = $type;
  }

  /**
   * Getter function
   * @return string the field type.
   */
  public function getType() {
    return $this->type;
  }


}
