<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Processors;
use CAPx\Drupal\Processors\EntityProcessor;
use CAPx\Drupal\Mapper\EntityMapper;
use CAPx\Drupal\Util\CAPx;

class FieldCollectionProcessor extends EntityProcessor {

  // The field collection entity
  protected $fieldCollectionEntity = array();
  // The parent entity
  protected $parentEntity = null;

  /**
   * Override method execute.
   *
   * The field collection processor will assume all updates will create a new
   * field collection and toss the old one. No need to check for existing item.
   * @see  parent::execute();
   * @return FieldCollection the saved field collection.
   */
  public function execute() {
    $data = $this->getData();
    $mapper = $this->getMapper();
    $entityType = $mapper->getEntityType();
    $bundleType = $mapper->getBundleType();

    // Use this funciton to find out how many items we really need to create.
    $dataPile = $this->fetchDataPile($data, $mapper);

    // Loop through an create new field collections based on the number of
    // results for each field.
    foreach ($dataPile as $fcData) {
      $entity = $this->newEntity($entityType, $bundleType, $fcdData, $mapper);
      drupal_alter('capx_new_fc', $entity);
      $entity = $mapper->execute($entity, $data);
      $entity->save();
      // Storage for something that may need it.
      $this->addFieldCollectionEntity($entity);
    }

    // Return all the things we just created.
    return $this->getEntity();
  }


  /**
   * New entity override as FieldCollections have some different defualts.
   * @see  parent:newEntity();
   */
  public function newEntity($entityType, $bundleType, $data, $mapper) {

    $properties = array(
      'type' => $bundleType,
      'uid' => 1, // @TODO - set this to something else
      'status' => 1, // @TODO - allow this to change
      'comment' => 0, // Any reason to set otherwise?
      'promote' => 0, // Fogetaboutit.
      'field_name' => $bundleType,
    );

    // Create an empty entity
    $entity = entity_create($entityType, $properties);

    $hostEntity = $this->getParentEntity();
    $hostType = $hostEntity->type();
    $entity->setHostEntity($hostType, $hostEntity->raw());

    // Wrap it up baby!
    $entity = entity_metadata_wrapper($entityType, $entity);
    return $entity;
  }

  /**
   * Return a multidimensional array of result data
   *
   * @param $data
   * @param $mapper
   * @return array
   */
  protected function fetchDataPile($data, $mapper) {
    $pile = array();
    $pile = array_pad($pile, 10, $data);
    return $pile;
  }


  /**
   * Setter function
   * @param Array $entities an array of field collection items
   */
  protected function addFieldCollectionEntity($entity) {
    $this->fieldCollectionEntity[] = $entity;
  }


  /**
   * Setter function
   * @param Array $entities an array of field collection items
   */
  public function setFieldCollectionEntity($entities) {
    $this->fieldCollectionEntity = $entities;
  }

  /**
   * Getter function
   * @return FieldCollectionItem the field collection item.
   */
  public function getFieldCollectionEntity() {
    return $this->fieldCollectionEntity;
  }

  /**
   * Setter function
   * @param Entity $entity The parent entity that the field collection
   * belongs to
   */
  public function setParentEntity($entity) {
    $this->parentEntity = $entity;
  }

  /**
   * Getter function
   * @return Entity The parent entity that the field collection belongs to.
   */
  public function getParentEntity() {
    return $this->parentEntity;
  }

}
