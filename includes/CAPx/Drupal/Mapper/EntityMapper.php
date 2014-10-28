<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Mapper;

use CAPx\Drupal\Processors\FieldProcessors\FieldProcessor;
use CAPx\Drupal\Processors\PropertyProcessors\PropertyProcessor;
use CAPx\Drupal\Processors\FieldCollectionProcessor;
use CAPx\Drupal\Util\CAPxMapper;

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
      // Get some information about the field we are going to process.
      $fieldInfoField = field_info_field($fieldName);
      if ($fieldInfoField) {
        $fieldInfoInstance = field_info_instance($entity->type(), $fieldName, $entity->getBundle());
        if ($fieldInfoInstance) {
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
            }
            catch (\Exception $e) {
              $message = 'There was an exception when trying to get data by @path. Exception message is: @message.';
              $message_vars = array(
                '@path' => $dataPath,
                '@message' => $e->getMessage(),
              );
              watchdog('stanford_capx_jsonpath', $message, $message_vars);
              continue;
            }
          }

          // We got nothing!
          if (empty($info)) {
            // @todo: log this
            continue;
          }

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
      }
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

  /**
   * Checks that fields used in this mapper still in place.
   *
   * If it happens that field used in mapper will be removed from bundle
   * or from the system entirely this will put a watchdog message and put a
   * persistent message for admin users on admin pages.
   */
  public function checkFields() {
    $config = $this->getConfig();
    $entity_type = $config['entity_type'];
    $bundle = $config['bundle_type'];
    $fields = $config['fields'];

    foreach (array_keys($fields) as $fieldName) {
      $fieldInfoField = field_info_field($fieldName);
      if ($fieldInfoField) {
        $fieldInfoInstance = field_info_instance($entity_type, $fieldName, $bundle);
        if (!$fieldInfoInstance) {
          // Field was removed from this bundle.
          $messages = variable_get('stanford_capx_field_issues', array());
          $mapper = $this->getMapper();
          $message_key = $mapper->getMachineName() . ':' . $entity_type . ':' . $bundle . ':' . $fieldName;

          if (empty($messages[$message_key])) {
            $message_vars = array(
              '%field' => $fieldName,
              '%entity_type' => $entity_type,
              '%bundle' => $bundle,
              '!mapper' => l(check_plain($mapper->label()), 'admin/config/capx/mapper/edit/' . $mapper->getMachineName()),
            );
            $message = t('Field %field was removed from the %entity_type bundle %bundle, but is still used in !mapper. You should check configuration of the specified mapper!');
            watchdog('stanford_capx_field_issues', $message, $message_vars, WATCHDOG_ERROR);
            $messages[$message_key] = array(
              'text' => $message,
              'message_vars' => $message_vars,
            );
            variable_set('stanford_capx_field_issues', $messages);
          }
        }
      }
      else {
        // Field was removed from system.
        $messages = variable_get('stanford_capx_field_issues', array());
        $message_key = $fieldName;

        if (empty($messages[$message_key])) {
          $mappers = CAPxMapper::loadAllMappers();
          // Filtering mappers that uses this field.
          $mapper_links = array();
          foreach ($mappers as $mapper) {
            if (array_key_exists($fieldName, $mapper->fields)) {
              $mapper_links[$mapper->getMachineName()] = l(check_plain($mapper->label()), 'admin/config/capx/mapper/edit/' . $mapper->getMachineName());
            }
          }
          $message_vars = array('%field' => $fieldName, '!mappers' => implode(', ', $mapper_links));
          $message = t('Field %field was removed from the system, but is still used in !mappers. You should check configuration of the specified mappers!');
          watchdog('stanford_capx_field_issues', $message, $message_vars, WATCHDOG_ERROR);
          $messages[$message_key] = array(
            'text' => $message,
            'message_vars' => $message_vars,
            'mappers' => $mapper_links,
          );
          variable_set('stanford_capx_field_issues', $messages);
        }
      }
    }
  }
}
