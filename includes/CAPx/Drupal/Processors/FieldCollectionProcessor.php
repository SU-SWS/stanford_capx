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
   * [newEntity description]
   * @param  [type] $entityType [description]
   * @param  [type] $bundleType [description]
   * @param  [type] $data       [description]
   * @param  [type] $mapper     [description]
   * @return [type]             [description]
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
   * [setFieldCollectionEntity description]
   * @param [type] $entity [description]
   */
  public function setFieldCollectionEntity($entity) {
    $this->fieldCollectionEntity = $entity;
  }

  /**
   * [getFieldCollectionEntity description]
   * @return [type] [description]
   */
  public function getFieldCollectionEntity() {
    return $this->fieldCollectionEntity;
  }

  /**
   * [setParentEntity description]
   * @param [type] $entity [description]
   */
  public function setParentEntity($entity) {

    $this->parentEntity = $entity;
  }

  /**
   * [getParentEntity description]
   * @return [type] [description]
   */
  public function getParentEntity() {
    return $this->parentEntity;
  }

}
