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
   * [execute description]
   * @param  [type] $entity Expects the entity to be wrapped in entity_metadata_wrapper
   * @param  [type] $data   [description]
   * @return [type]         [description]
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
   * [mapFields description]
   * @param  [type] $field_info [description]
   * @param  [type] $data   [description]
   * @return [type]         [description]
   */
  public function mapFields($data) {

    $config = $this->getConfig();
    $entity = $this->getEntity();

    // Loop through each field and run a field processor on it.
    foreach ($config['fields'] as $fieldName => $remoteDataPaths) {
      $info = array();

      drupal_alter('capx_pre_map_field', $entity, $fieldName, $remoteDataPaths);

      // Allow just one path as a string
      if (!is_array($remoteDataPaths)) {
        $remoteDataPaths = array($remoteDataPaths);
      }

      // Loop through each of the data paths.
      foreach ($remoteDataPaths as $key => $dataPath) {
        try {
          $info[$key] = $this->getRemoteDataByJsonPath($data, $dataPath);
        } catch(\Exception $e) {
          // ... silently continue. Please dont shoot me.
          // @todo: Add debugging logs/help here.
          continue;
        }
      }

      $fieldInfoInstance = field_info_instance($entity->type(), $fieldName, $entity->getBundle());
      $fieldInfoField = field_info_field($fieldName);

      $widget = $fieldInfoInstance['widget']['type'];
      $field = $fieldInfoField['type'];

      $fieldProcessor = new FieldProcessor($entity, $fieldName);
      $fieldProcessor->field($field)->widget($widget)->put($info);

      drupal_alter('capx_post_map_field', $entity, $fieldName);

    }

    $this->setEntity($entity);

  }

  /**
   * [mapProperties description]
   * @param  [type] $properties [description]
   * @param  [type] $data       [description]
   * @return [type]             [description]
   */
  public function mapProperties($data) {

    $config = $this->getConfig();
    $entity = $this->getEntity();

        // Loop through each property and run a property processor on it.
    foreach ($config['properties'] as $propertyName => $remoteDataPath) {
      try {
        $info = $this->getRemoteDataByJsonPath($data, $remoteDataPath);
      } catch(\Exception $e) {
        // ... silently continue. Please dont shoot me.
        continue;
      }
      $propertyProcessor = new PropertyProcessor($entity, $propertyName);
      $propertyProcessor->put($info);
    }

    $this->setEntity($entity);
  }

  /**
   * [mapFieldCollections description]
   * @param  [type] $data [description]
   * @return [type]       [description]
   */
  public function mapFieldCollections($data) {

    try {
      $collections = $this->getConfigSetting('fieldCollections');
    }
    catch(\Exception $e) {
      // No collections. Just return.
      return;
    }

    $entity = $this->getEntity();
    // $collectionEntities = array();

    foreach ($collections as $fieldName => $collectionMapper) {

      // Validate that the field exists.
      if (!isset($entity->{$fieldName})) {
        watchdog('stanford_capx', 'No field collection field on this entity with name: ' . $fieldName, WATCHDOG_NOTICE);
        continue;
      }

      // If field exists we will need to clear out the old values.
      $entity->{$fieldName}->set(null);

      // Create a new processor as we need to create the field collection item.
      $collectionProcessor = new FieldCollectionProcessor($collectionMapper, $data);
      $collectionProcessor->setParentEntity($entity);
      $collectionProcessor->execute();
      // $collectionEntities[] = $collectionProcessor->getFieldCollectionEntity();

    }

    $this->setEntity($entity);
  }


}
