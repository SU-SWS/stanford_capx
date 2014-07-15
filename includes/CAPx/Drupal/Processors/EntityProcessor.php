<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Processors;
use CAPx\Drupal\Mapper\EntityMapper;
use CAPx\Drupal\Util\CAPx;

class EntityProcessor extends ProcessorAbstract {

  /**
   * [execute description]
   * @return [type] [description]
   */
  public function execute() {
    $data = $this->getData();
    $mapper = $this->getMapper();

    $entityType = $mapper->getEntityType();
    $bundleType = $mapper->getBundleType();

    $entity = CAPx::getEntityByProfileId($entityType, $bundleType, $data['profileId']);

    // If we have an entity we need to update it.
    if ($entity) {
      $entity = entity_metadata_wrapper($entityType, $entity);
      $entity = $this->updateEntity($entity, $data, $mapper);
    }
    else {
      $entity = $this->newEntity($entityType, $bundleType, $data, $mapper);
    }

    return $entity;

  }

  /**
   * [updateEntity description]
   * @param  [type] $entity [description]
   * @param  [type] $data   [description]
   * @param  [type] $mapper [description]
   * @return [type]         [description]
   */
  public function updateEntity($entity, $data, $mapper) {
    $entity = $mapper->execute($entity, $data);
    $entity->save();
    return $entity;
  }

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
    );

    // Create an empty entity
    $entity = entity_create($entityType, $properties);

    // Wrap it up baby!
    $entity = entity_metadata_wrapper($entityType, $entity);
    $entity = $mapper->execute($entity, $data);
    $entity->save();

    // Write a new record
    CAPx::insertNewProfileRecord($entity, $data['profileId'], $data['meta']['etag']);

    return $entity;
  }




}
