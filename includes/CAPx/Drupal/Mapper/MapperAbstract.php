<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Mapper;

use \Peekmo\JsonPath\JsonStore as JsonParser;

abstract class MapperAbstract implements MapperInterface {


  // Default and stored configuration
  protected $config = array();

  // The entity.
  protected $entity = null;


  /**
   * Merges default configuration options with the passed in set.
   * @param [type] $config [description]
   */
  public function __construct($config) {
    $myConfig = $this->getConfig();
    $myConfig = array_merge($myConfig, $config);
    $this->setConfig($myConfig);
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
  public function getConfig() {
    return $this->config;
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
   * @param array $config An array of configuration options.
   */
  public function setConfig($config) {
    $this->config = $config;
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


}
