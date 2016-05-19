<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Processors;

use CAPx\Drupal\Mapper\EntityMapper;
use CAPx\Drupal\Util\CAPx;

class EntityProcessor extends ProcessorAbstract {

  // Wrapped Drupal entity to be processed.
  protected $entity;

  // Skip etag check.
  protected $force = FALSE;

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

    try {
      $multi = $this->getMapper()->getConfigSetting('multiple');
    }
    catch (\Exception $e) {
      $multi = FALSE;
    }

    // Sometimes we need to create one, other times we need moar.
    if (!empty($multi) && $multi == 1) {
      $entity = $this->executeMultiple($force);
    }
    else {
      $entity = $this->executeSingle($force);
    }

    return $entity;
  }

  /**
   * Process the execution and creation of multiple entities per profile.
   * @param  [type] $force [description]
   * @return [type]        [description]
   */
  protected function executeMultiple($force) {

    $mapper = $this->getMapper();
    $data = $this->getData();
    $numEntities = $mapper->getMultipleEntityCountBySubquery($data);

    // @todo: Remove old ones if they exist.
    if ($numEntities <= 0) {
      return;
    }

    $entityImporter = $this->getEntityImporter();
    $importerMachineName = $entityImporter->getMachineName();
    $entityType = $mapper->getEntityType();
    $bundleType = $mapper->getBundleType();

    $i = 0;
    while ($i < $numEntities) {
      $mapper->setIndex($i);
      $entity = $this->newEntity($entityType, $bundleType, $data, $mapper);
      $this->setStatus(1, 'Created new entity.');
      $i++;
    }

  }


  /**
   * Process the execution and creation of a single entity per profile response.
   * @param  [type] $force [description]
   * @return [type]        [description]
   */
  protected function executeSingle($force) {
    $mapper = $this->getMapper();
    $data = $this->getData();
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

      if ($this->isETagDifferent() || $this->skipEtagCheck()) {
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

    // There is a possiblility that a field or property had an error while being
    // processed. These errors are stored for us to get later so that no one
    // field stops the processing of an entity. If there was an error somewhere
    // the eTag should be invalidated so that this entity gets updates on the
    // next import run.
    $errors = $mapper->getErrors();

    if (!empty($errors)) {
      // If there was an error on the field mapping set the etag to errors.
      $data['meta']['etag'] = "errors";
    }

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

    // There is a possiblility that a field or property had an error while being
    // processed. These errors are stored for us to get later so that no one
    // field stops the processing of an entity. If there was an error somewhere
    // the eTag should be invalidated so that this entity gets updates on the
    // next import run.
    $errors = $mapper->getErrors();

    if (!empty($errors)) {
      // If there was an error on the field mapping set the etag to errors.
      $data['meta']['etag'] = "errors";
    }

    // Allow altering.
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

  /**
   * [skipEtagCheck description]
   * @param  boolean $bool [description]
   * @return [type]        [description]
   */
  public function skipEtagCheck($bool = NULL) {
    if (is_bool($bool)) {
      $this->force = $bool;
    }
    return $this->force;
  }



}
