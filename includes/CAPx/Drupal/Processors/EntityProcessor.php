<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Processors;
use CAPx\Drupal\Mapper\EntityMapper;
use CAPx\Drupal\Util\CAPx;

class EntityProcessor extends ProcessorAbstract {

  protected $entity;

  /**
   * The starting point for processing any entity. This function executes and
   * handles the saving and/or updating of an entity with the data that is
   * set to it.
   * @return Entity The new or updated entity.
   */
  public function execute() {
    $data = $this->getData();
    $mapper = $this->getMapper();
    $entityImporter = $this->getEntityImporter();
    $importerMachineName = $entityImporter->getMachineName();

    $entityType = $mapper->getEntityType();
    $bundleType = $mapper->getBundleType();

    // $entity = CAPx::getEntityByProfileId($entityType, $bundleType, $data['profileId']);
    $entity = null;
    $entities = CAPx::getProfiles($entityType, array('profile_id' => $data['profileId'], 'importer' => $importerMachineName));
    if (is_array($entities)) {
      $entity = array_pop($entities);
    }

    // If we have an entity we need to update it.
    if (!empty($entity)) {
      $this->setEntity($entity);

      // Check to see if the etag has changed. We can avoid processing a profile
      // if the etag is unchanged.

      if ($this->isETagDifferent()) {
        $entity = entity_metadata_wrapper($entityType, $entity);
        $entity = $this->updateEntity($entity, $data, $mapper);
        $this->setStatus(3, 'Etag expired. Profile was updated.');
      }
      else {
        $this->setStatus(2, 'Etag matched. No processing happened.');
      }

    }
    else {
      $entity = $this->newEntity($entityType, $bundleType, $data, $mapper);
      $this->setStatus(1, 'Created new entity.');
    }

    return $entity;

  }

  /**
   * Update the entity.
   * Slightly different from the new entity. If we have an entity we will
   * execute the mapper on it and re-save it.
   * @param  Entity $entity the entity to be updated
   * @param  array $data   The data to map into it.
   * @param  EntityMapper $mapper the entity mapper instance
   * @return Entity         the updated entity.
   */
  public function updateEntity($entity, $data, $mapper) {

    drupal_alter('capx_pre_update_entity', $entity, $data, $mapper);

    $entity = $mapper->execute($entity, $data);
    $entity->save();

    drupal_alter('capx_post_update_entity', $entity);

    return $entity;
  }

  /**
   * New entity.
   * An existing entity was not found and a new one should be created. Provide
   * some default values, create the entity, map the fields to it, and store
   * some additional data about where it came from.
   * @param  String $entityType the type of entity being created
   * @param  String $bundleType the bundle type of the entity being created
   * @param  array $data       the data to be mapped to the new entity
   * @param  EntityMapper $mapper     the EntityMapper instance
   * @return Entity             the new entity after it has been saved.
   */
  public function newEntity($entityType, $bundleType, $data, $mapper) {

    $properties = array(
      'type' => $bundleType,
      'uid' => 1, // @TODO - set this to something else
      'status' => 1, // @TODO - allow this to change
      'comment' => 0, // Any reason to set otherwise?
      'promote' => 0, // Fogetaboutit.
    );

    drupal_alter('capx_pre_entity_create', $properties, $entityType, $bundleType, $mapper);

    // Create an empty entity.
    $entity = entity_create($entityType, $properties);

    // Wrap it up baby!
    $entity = entity_metadata_wrapper($entityType, $entity);
    $entity = $mapper->execute($entity, $data);
    $entity->save();

    drupal_alter('capx_post_entity_create', $entity);

    // Write a new record.
    $entityImporter = $this->getEntityImporter();
    $importerMachineName = $entityImporter->getMachineName();
    CAPx::insertNewProfileRecord($entity, $data['profileId'], $data['meta']['etag'], $importerMachineName);

    return $entity;
  }

  /**
   * Check to see if the etag changed since last update.
   *
   * Validates the etag difference in the saved version to the api version. If
   * they are the same then the profile has not changed and we can carry on. If
   * the etag is different we need to run the update.
   * @return boolean [description]
   */
  protected function isETagDifferent() {
    $importer = $this->getEntityImporter()->getMachineName();
    $data = $this->getData();
    $etag = CAPx::getEntityETag($importer, $data['profileId']);

    return !($etag == $data['meta']['etag']);
  }

  /**
   * [setEntity description]
   * @param [type] $entity [description]
   */
  protected function setEntity($entity) {
    $this->entity = $entity;
  }

  /**
   * [getEntity description]
   * @return [type] [description]
   */
  protected function getEntity() {
    return $this->entity;
  }


}
