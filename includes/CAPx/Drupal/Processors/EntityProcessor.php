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
   * Wrapped Drupal entity to be processed.
   */
  protected $entity;

  /**
   * Process entity.
   *
   * The starting point for processing any entity. This function executes and
   * handles the saving and/or updating of an entity with the data that is
   * set to it.
   *
   * @param bool $force
   *   Synchronize even if synchronization is disable('sync' = 0).
   *
   * @return object
   *   The new or updated wrapped entity.
   */
  public function execute($force = FALSE) {
    $data = $this->getData();
    $mapper = $this->getMapper();
    $entityImporter = $this->getEntityImporter();
    $importerMachineName = $entityImporter->getMachineName();

    $entityType = $mapper->getEntityType();
    $bundleType = $mapper->getBundleType();

    $entity = NULL;
    $entities = CAPx::getProfiles($entityType, array('profile_id' => $data['profileId'], 'importer' => $importerMachineName));
    if (is_array($entities)) {
      $entity = array_pop($entities);
    }

    // If we have an entity we need to update it.
    if (!empty($entity)) {

      // Profile synchronization has been disabled.
      if (empty($entity->capx['sync']) && !$force) {
        return NULL;
      }

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
        // Call this function so that the timestamp of sync is updated.
        $entity = entity_metadata_wrapper($entityType, $entity);
        CAPx::updateProfileRecord($entity, $data['profileId'], $data['meta']['etag'], $importerMachineName);
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
   *
   * Slightly different from the new entity. If we have an entity we will
   * execute the mapper on it and re-save it.
   *
   * @param object $entity
   *   The entity to be updated
   * @param array $data
   *   The data to map into it.
   * @param object $mapper
   *   The entity mapper instance
   *
   * @return object
   *   The updated entity.
   */
  public function updateEntity($entity, $data, $mapper) {
    $entity_info = entity_get_info($entity->type());
    drupal_alter('capx_pre_update_entity', $entity, $data, $mapper);

    $entity = $mapper->execute($entity, $data);

    // Nodes have special sauces.
    if ($entity->type() == "node") {
      // Set up default values, if required.
      $node_options = variable_get('node_options_' . $entity->getBundle(), array('status', 'promote'));
      // Always use the default revision setting.
      $entity->revision->set(in_array('revision', $node_options));
    }

    // Save the entity.
    $entity->save();

    $entityImporter = $this->getEntityImporter();
    $importerMachineName = $entityImporter->getMachineName();
    CAPx::updateProfileRecord($entity, $data['profileId'], $data['meta']['etag'], $importerMachineName);

    drupal_alter('capx_post_update_entity', $entity);

    return $entity;
  }

  /**
   * New entity.
   *
   * An existing entity was not found and a new one should be created. Provide
   * some default values, create the entity, map the fields to it, and store
   * some additional data about where it came from.
   *
   * @param string $entityType
   *   The type of entity being created
   * @param string $bundleType
   *   The bundle type of the entity being created
   * @param array $data
   *   The data to be mapped to the new entity
   * @param object $mapper
   *   The EntityMapper instance
   *
   * @return object
   *   The new entity after it has been saved.
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
    // @todo Need to catch exceptions here as well.
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
