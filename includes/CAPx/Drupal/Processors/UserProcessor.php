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
