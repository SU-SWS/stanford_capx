<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Mapper;
use CAPx\Drupal\Processors\FieldProcessors\FieldProcessor;

class EntityMapper extends MapperAbstract {

  /**
   * [execute description]
   * @param  [type] $entity Expects the entity to be wrapped in entity_metadata_wrapper
   * @param  [type] $data   [description]
   * @return [type]         [description]
   */
  public function execute($entity, $data) {

    $this->setEntity($entity);

    $config = $this->getConfig();
    $type = $entity->type();
    $bundle = $entity->getBundle();

    $prop_info = entity_get_property_info($type);
    $fields = $prop_info['bundles'][$bundle]['properties'];
    $properties = $prop_info['properties'];

    // body is placed in both. It should be treated as a field.
    if (isset($properties['body'])) {
      unset($properties['body']);
    }

    // Mappy map.
    $this->mapFields($fields, $data);
    $this->mapProperties($properties, $data);

    return $entity;
  }

  /**
   * [mapFields description]
   * @param  [type] $field_info [description]
   * @param  [type] $data   [description]
   * @return [type]         [description]
   */
  public function mapFields($fields, $data) {

    $config = $this->getConfig();
    $entity = $this->getEntity();

    // Loop through each field and run a field processor on it.
    foreach ($config['fields'] as $field_name => $remote_data_paths) {
      $info = array();

      // Allow just one path as a string
      if (!is_array($remote_data_paths)) {
        $remote_data_paths = array($remote_data_paths);
      }

      // Loop through each of the data paths.
      foreach ($remote_data_paths as $key => $data_path) {
        try {
          $info[$key] = $this->getRemoteDataByJsonPath($data, $data_path);
        } catch(\Exception $e) {
          // ... silently continue. Please dont shoot me.
          continue;
        }
      }

      $field_info_instance = field_info_instance($entity->type(), $field_name, $entity->getBundle());
      $field_info_field = field_info_field($field_name);

      $widget = $field_info_instance['widget']['type'];
      $field = $field_info_field['type'];

      $field_processor = new FieldProcessor($entity, $field_name);
      $field_processor->field($field)->widget($widget)->put($info);

    }

    $this->setEntity($entity);

  }

  /**
   * [mapProperties description]
   * @param  [type] $properties [description]
   * @param  [type] $data       [description]
   * @return [type]             [description]
   */
  public function mapProperties($properties, $data) {

    $config = $this->getConfig();
    $entity = $this->getEntity();

        // Loop through each property and run a property processor on it.
    foreach ($config['properties'] as $property_name => $remote_data_path) {
      try {
        $info = $this->getRemoteDataByJsonPath($data, $remote_data_path);
      } catch(\Exception $e) {
        // ... silently continue. Please dont shoot me.
        continue;
      }
      $property_processor = new \CAPx\Drupal\Processors\PropertyProcessors\PropertyProcessor($entity, $property_name);
      $property_processor->put($info);
    }

    $this->setEntity($entity);
  }


}
