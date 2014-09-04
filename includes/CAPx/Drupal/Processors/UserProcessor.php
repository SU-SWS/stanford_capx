<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Processors;
use CAPx\Drupal\Mapper\EntityMapper;
use CAPx\Drupal\Util\CAPx;

class UserProcessor extends EntityProcessor {

  /**
   * newEntity method override.
   * @see  parent:newEntity();
   * User entities have some special needs around their default values.
   * return Entity the saved user entity.
   */
  public function newEntity($entityType, $bundleType, $data, $mapper) {

    $properties = array(
      'type' => $bundleType,
      'status' => 1, // @TODO - allow this to change
    );

    drupal_alter('capx_pre_entity_create', $properties, $entityType, $bundleType, $mapper);

    // Create an empty entity
    $entity = entity_create($entityType, $properties);

    // Wrap it up baby!
    $entity = entity_metadata_wrapper($entityType, $entity);
    $entity = $mapper->execute($entity, $data);
    $entity->save();

    drupal_alter('capx_post_entity_create', $entity);

    // Write a new record
    $entityImporter = $this->getEntityImporter();
    $importerMachineName = $entityImporter->getMachineName();
    CAPx::insertNewProfileRecord($entity, $data['profileId'], $data['meta']['etag'], $importerMachineName);

    return $entity;
  }


}
