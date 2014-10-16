<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Mapper;
use CAPx\Drupal\Processors\FieldProcessors\FieldProcessor;
use CAPx\Drupal\Processors\PropertyProcessors\PropertyProcessor;
use CAPx\Drupal\Processors\FieldCollectionProcessor;

class EntityMapper extends MapperAbstract {

  /**
   * Execute starts the mapping process.
   *
   * @param Entity $entity
   *   Expects the entity to be wrapped in entity_metadata_wrapper
   * @param array $data
   *   An array of json data. The response from the API.
   *
   * @return Entity
   *   A fully saved or updated entity.
   */
  public function execute($entity, $data) {

    // Store this for later.
    $this->setEntity($entity);

    // Mappy map.
    $this->mapFields($data);
    $this->mapProperties($data);
    // Field Collections are special. Special means more code. They get their
    // own mapProcess even though they are sort of a field.
    $this->mapFieldCollections($data);

    return $entity;
  }

  /**
   * Map all of the fields.
   *
   * Map all of the fields that have settings in the mapper to their field on
   * the entity. Loop through each setting and get the data out of the response
   * array.
   *
   * @param array $data
   *   An array of data from the API.
   */
  public function mapFields($data) {

    $config = $this->getConfig();
    $entity = $this->getEntity();

    // Loop through each field and run a field processor on it.
    foreach ($config['fields'] as $fieldName => $remoteDataPaths) {
      $info = array();

      drupal_alter('capx_pre_map_field', $entity, $fieldName, $remoteDataPaths);

      // Allow just one path as a string.
      if (!is_array($remoteDataPaths)) {
        $remoteDataPaths = array($remoteDataPaths);
      }

      // Loop through each of the data paths.
      foreach ($remoteDataPaths as $key => $dataPath) {

        // No setting was provided.
        if (empty($dataPath)) {
          continue;
        }

        // Attempt to get the data based on the path that was provided.
        // No guarentee that the user wont enter valid jsonpath notation that
        // does not have a valid result.
        try {
          $info[$key] = $this->getRemoteDataByJsonPath($data, $dataPath);
        } catch(\Exception $e) {
          // ... silently continue. Please dont shoot me.
          // @todo: Add debugging logs/help here.
          continue;
        }
      }

      // We got nothing!
      if (empty($info)) {
        // @todo: log this
        continue;
      }

      // Get some information about the field we are going to process.
      $fieldInfoInstance = field_info_instance($entity->type(), $fieldName, $entity->getBundle());
      $fieldInfoField = field_info_field($fieldName);

      // Widgets can change the way the data needs to be parsed. Provide that
      // to the FieldProcessor.
      $widget = $fieldInfoInstance['widget']['type'];
      $field = $fieldInfoField['type'];

      // Create a new field processor and let it do its magic.
      $fieldProcessor = new FieldProcessor($entity, $fieldName);
      $fieldProcessor->field($field)->widget($widget)->put($info);

      // Allow altering of an entity after this process.
      drupal_alter('capx_post_map_field', $entity, $fieldName);

    }

    // Set the entity again for changes.
    $this->setEntity($entity);

  }

  /**
   * Map properties to the entity.
   *
   * Take the data out of the JSON array and put it into a property on the
   * entity. Properties are much simplier than fields as they do not have a
   * number of columns and/or other special properties to worry about.
   *
   * @param array $data
   *   The response data from the API
   */
  public function mapProperties($data) {

    $config = $this->getConfig();
    $entity = $this->getEntity();

    // Loop through each property and run a property processor on it.
    foreach ($config['properties'] as $propertyName => $remoteDataPath) {
      try {
        $info = $this->getRemoteDataByJsonPath($data, $remoteDataPath);
      }
      catch(\Exception $e) {
        // ... silently continue. Please dont shoot me.
        // @todo: log this for debugging.
        continue;
      }

      // Let the property processor do its magic.
      $propertyProcessor = new PropertyProcessor($entity, $propertyName);
      $propertyProcessor->put($info);
    }

    // Set the entity again for changes.
    $this->setEntity($entity);
  }

  /**
   * Process field collection fields uniquely.
   *
   * Field Collection fields are a special field and need to be handled
   * differently. The field collection data needs to be saved as its own enitty
   * and then attached to the parent entity. Allow for this.
   *
   * @param array $data
   *   An array of field collection data information
   */
  public function mapFieldCollections($data) {

    try {
      $collections = $this->getConfigSetting('fieldCollections');
    }
    catch(\Exception $e) {
      // No collections. Just return.
      return;
    }

    // The parent entity.
    $entity = $this->getEntity();

    // Each field collection field.
    foreach ($collections as $fieldName => $collectionMapper) {

      // Validate that the field exists.
      if (!isset($entity->{$fieldName})) {
        watchdog('stanford_capx', 'No field collection field on this entity with name: ' . $fieldName, WATCHDOG_NOTICE);
        continue;
      }

      // If field exists we will need to clear out the old values. Create a new
      // field collection each time.
      // @todo: rethink the new field collection each time as there may be
      // additional data on the FC.
      $entity->{$fieldName}->set(null);

      // Allow the field collection processor to do its magic.
      $collectionProcessor = new FieldCollectionProcessor($collectionMapper, $data);
      $collectionProcessor->setParentEntity($entity);
      $collectionProcessor->execute();

    }

    // Set the entity again for changes.
    $this->setEntity($entity);
  }


}
