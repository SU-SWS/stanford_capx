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
  protected $fieldCollectionEntity = null;
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

    $entity = $this->newEntity($entityType, $bundleType, $data, $mapper);
    return $entity;
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
    $entity = $mapper->execute($entity, $data);
    $entity->save();

    drupal_alter('capx_new_fc', $entity);

    // Storage for something that may need it.
    $this->setFieldCollectionEntity($entity);

    return $entity;
  }


  /**
   * Setter function
   * @param FieldCollectionItem $entity the field collection item to be acted on
   */
  public function setFieldCollectionEntity($entity) {
    $this->fieldCollectionEntity = $entity;
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
