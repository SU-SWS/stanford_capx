<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Mapper;

use CAPx\Drupal\Mapper\FieldCollectionMapper;
use \Peekmo\JsonPath\JsonStore as JsonParser;

abstract class MapperAbstract implements MapperInterface {

  // Configuration settings.
  protected $config = array();

  // The entity that is being mapped.
  protected $entity;

  // The mapper configuration entity (CFEntity).
  protected $mapper;


  /**
   * Merges default configuration options with the passed in set.
   *
   * @param CFEntity $mapper
   *   [description]
   */
  public function __construct($mapper) {
    $this->setMapper($mapper);
    $this->addConfig($mapper->settings);
  }

  /**
   * Uses a JSONPath library and notation to find data in a parsed json array.
   * Documentation on JSON path: http://goessner.net/articles/JsonPath/
   *
   * @param  array $data the JSON array of data from the API
   * @param  string $path The JSONPath
   * @return array       an array of data sliced from the $data by $path.
   */
  public function getRemoteDataByJsonPath($data, $path) {

    if (empty($path)) {
      throw new \Exception("Path cannot be empty", 1);
    }

    $jsonParser = new JsonParser($data);
    $parsed = $jsonParser->get($path);
    return $parsed;

  }

  /**
   * Getter function
   * @return string the entity type
   */
  public function getEntityType() {
    return $this->getConfigSetting('entity_type');
  }

  /**
   * Getter function
   * @return string the bundle type
   */
  public function getBundleType() {
    return $this->getConfigSetting('bundle_type');
  }

  /**
   * Getter function
   * @return array the configuration array
   */
  protected function setConfig($config) {
    $this->config = $config;
  }

  /**
   * Getter function
   * @return array the configuration array
   */
  public function getConfig() {
    return $this->config;
  }

  /**
   * [addConfig description]
   * @param [type] $settings [description]
   */
  public function addConfig($settings) {

    $config = $this->getConfig();
    $settings['fieldCollections'] = array();

    if (isset($settings['collections'])) {
      foreach ($settings['collections'] as $fieldName => $fields) {
        $collectionConfig = array();
        $collectionConfig['bundle_type'] = $fieldName;
        $collectionConfig['fields'] = $fields;
        $collectionConfig['properties'] = array();
        $settings['fieldCollections'][$fieldName] = new FieldCollectionMapper($collectionConfig);
      }
    }

    unset($settings['collections']);

    $config = array_merge($config, $settings);
    $this->setConfig($config);
  }

  /**
   * Getter function for a key in the config array
   * @param string $name the index key for an item in the config array.
   * @return mixed the value for a key in an associative array.
   */
  public function getConfigSetting($name) {
    if (isset($this->config[$name])) {
      return $this->config[$name];
    }
    else {
      throw new \Exception("No config setting by that name: " . $name);
    }
  }

  /**
   * Setter function
   * @param Entity $entity The entity to be worked on.
   */
  public function setEntity($entity) {
    $this->entity = $entity;
  }

  /**
   * Getter function
   * @return Entity the entity being worked on.
   */
  public function getEntity() {
    return $this->entity;
  }

  /**
   * [setMapper description]
   * @param [type] $mapper [description]
   */
  public function setMapper($mapper) {
    $this->mapper = $mapper;
  }

  /**
   * [getMapper description]
   * @return [type] [description]
   */
  public function getMapper() {
    return $this->mapper;
  }

}
