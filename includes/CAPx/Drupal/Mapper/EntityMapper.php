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
use CAPx\Drupal\Util\CAPxImporter;

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
          // @todo For data structures like files we shouldn't convert data path to array.
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
            // No guarantee that the user will enter valid jsonpath notation
            // that does have a valid result.
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

          // Widgets can change the way the data needs to be parsed. Provide
          // that to the FieldProcessor.
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
        $message = 'There was an exception when trying to get data by @path. Exception message is: @message.';
        $message_vars = array(
          '@path' => $remoteDataPath,
          '@message' => $e->getMessage(),
        );
        watchdog('stanford_capx_jsonpath', $message, $message_vars);
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
   *
   * @return bool
   *   Boolean indicating status of mapper fields.
   */
  public function checkFields() {
    $fields_status = TRUE;
    $config = $this->getConfig();
    $entity_type = $config['entity_type'];
    $bundle = $config['bundle_type'];
    $fields = $config['fields'];
    $collections = $config['fieldCollections'];
    $items = array();

    foreach ($fields as $fieldName => $values) {
      $items[] = array("name" => $fieldName, "bundle" => $bundle, "entity" => $entity_type);
    }

    foreach ($collections as $fcMapper) {
      $fcConfig = $fcMapper->getConfig();
      foreach ($fcConfig['fields'] as $fieldName => $values) {
        $items[] = array("name" => $fieldName, "bundle" => $fcMapper->bundle, "entity" => "field_collection_item");
      }
    }

    foreach ($items as $fieldInfo) {

      $fieldName = $fieldInfo['name'];
      $bundle = $fieldInfo['bundle'];
      $entity_type = $fieldInfo['entity'];

      $fieldInfoField = field_info_field($fieldName);
      if ($fieldInfoField) {
        $fieldInfoInstance = field_info_instance($entity_type, $fieldName, $bundle);
        if (!$fieldInfoInstance) {
          // Field was removed from this bundle.
          $fields_status = FALSE;
          $messages = variable_get('stanford_capx_admin_messages', array());
          $mapper = $this->getMapper();
          $message_key = $mapper->getMachineName() . ':' . $entity_type . ':' . $bundle . ':' . $fieldName;

          if (empty($messages[$message_key])) {
            $message_vars = array(
              '%field' => $fieldName,
              '%entity_type' => $entity_type,
              '%bundle' => $bundle,
              '!mapper' => l(check_plain($mapper->label()), 'admin/config/capx/mapper/edit/' . $mapper->getMachineName()),
            );
            $message = t('Field %field was removed from the %entity_type bundle %bundle, but is still used in !mapper. You should check configuration of the specified mapping!');
            watchdog('stanford_capx_admin_messages', $message, $message_vars, WATCHDOG_ERROR);
            $messages[$message_key] = array(
              'text' => $message,
              'message_vars' => $message_vars,
            );
            variable_set('stanford_capx_admin_messages', $messages);
          }
        }
      }
      else {
        // Field was removed from system.
        $fields_status = FALSE;
        $messages = variable_get('stanford_capx_admin_messages', array());
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
          watchdog('stanford_capx_admin_messages', $message, $message_vars, WATCHDOG_ERROR);
          $messages[$message_key] = array(
            'text' => $message,
            'message_vars' => $message_vars,
            'mappers' => $mapper_links,
          );
          variable_set('stanford_capx_admin_messages', $messages);
        }
      }
    }

    return $fields_status;
  }

  /**
   * Checks mapper status.
   *
   * @param string $importer
   *   Importer machine name.
   *
   * @return bool
   *  Mapper status.
   */
  public function valid($importer) {
    $this->setImporter($importer);

    $entity_status = $this->checkEntity();
    $fields_status = $this->checkFields();

    return ($entity_status && $fields_status);
  }

  /**
   * Checks if the entity type and bundle configured for mapper is still in place.
   *
   * @return bool
   *   Boolean indicating status of mapper entity.
   */
  public function checkEntity() {
    $entity_status = FALSE;
    $entity_type = $this->getConfigSetting('entity_type');
    $bundle = $this->getConfigSetting('bundle_type');

    $entity_info = entity_get_info($entity_type);
    if ($entity_info) {
      if (!isset($entity_info['bundles']) || empty($entity_info['bundles'][$bundle])) {
        $mapper = $this->getMapper();
        $message_key = $this->getImporter();
        $message_vars = array(
          '%mapper' => $mapper->label(),
          '%entity_type' => $entity_info['label'],
          '%bundle' => $bundle,
        );
        $message_text = t('Invalid bundle setting on %mapper. The bundle %bundle is no longer available and should either be restored, or the mapping %mapper should be deleted.');
        watchdog('stanford_capx_mapper_issue', $message_text, $message_vars, WATCHDOG_ERROR);
      }
      else {
        // Entity end bundle info is in place - status OK.
        $entity_status = TRUE;
      }
    }
    else {
      $mapper = $this->getMapper();
      $message_key = $this->getImporter();
      $message_vars = array(
        '%mapper' => $mapper->label(),
        '%entity_type' => $entity_type,
        '%bundle' => $bundle,
      );
      $message_text = t('Invalid entity setting on %mapper. The entity %entity_type is no longer available. Please restore the entity type or remove the mapping.');
      watchdog('stanford_capx_mapper_issue', $message_text, $message_vars, WATCHDOG_ERROR);
    }

    // Something is wrong - removing mapper config.
    if (!$entity_status) {

      $importers = $this->getAffectedImporters();
      $importer_links = array();
      foreach ($importers as $importer) {
        $importer_links[$importer->getMachineName()] = l(check_plain($importer->label()), 'admin/config/capx/importer/edit/' . $importer->getMachineName());
      }

      $message_vars['!importers'] = implode(', ', $importer_links);
      $message_text .= ' ';
      $message_text .= t('The following importers are using an invalid mapping. Please update or delete the mapping settings: !importers.');

      $messages = variable_get('stanford_capx_admin_messages', array());
      $messages[$message_key] = array(
        'text' => $message_text,
        'message_vars' => $message_vars,
        'importers' => $importer_links,
      );

      variable_set('stanford_capx_admin_messages', $messages);
    }

    return $entity_status;
  }

  /**
   * Returns array of importers that are using current mapper.
   *
   * @return array
   *   Array is keyed by importer machine name.
   */
  public function getAffectedImporters() {
    $mapper = $this->getMapper();
    $importers = CAPxImporter::loadAllImporters();
    $affected = array();
    foreach ($importers as $importer) {
      if ($importer->mapper == $mapper->getMachineName()) {
        $affected[$importer->getMachineName()] = $importer;
      }
    }

    return $affected;
  }

}
