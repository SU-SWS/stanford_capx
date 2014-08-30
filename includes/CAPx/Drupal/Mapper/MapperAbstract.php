<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Mapper;

use \Peekmo\JsonPath\JsonStore as JsonParser;

abstract class MapperAbstract implements MapperInterface {

  /**
   * Default and stored configuration
   * @var array
   */
  protected $config = array();

  // The entity.
  protected $entity = null;


  /**
   * [__construct description]
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
   * @param  [type] $data [description]
   * @param  [type] $path [description]
   * @return [type]       [description]
   */
  public function getRemoteDataByJsonPath($data, $path) {

    if (empty($path)) {
      throw new \Exception("Path cannot be empty", 1);
    }

    $jsonParser = new JsonParser();
    $parsed = $jsonParser->get($data, $path);
    return $parsed;

  }

  /**
   * [getEntityType description]
   * @return [type] [description]
   */
  public function getEntityType() {
    return $this->getConfigSetting('entity_type');
  }

  /**
   * [getEntityType description]
   * @return [type] [description]
   */
  public function getBundleType() {
    return $this->getConfigSetting('bundle_type');
  }

  /**
   * [getConfig description]
   * @return [type] [description]
   */
  public function getConfig() {
    return $this->config;
  }

  /**
   * [getConfig description]
   * @return [type] [description]
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
   * [setConfig description]
   * @param [type] $config [description]
   */
  public function setConfig($config) {
    $this->config = $config;
  }

  /**
   * [setEntity description]
   * @param [type] $entity [description]
   */
  public function setEntity($entity) {
    $this->entity = $entity;
  }

  /**
   * [getEntity description]
   * @return [type] [description]
   */
  public function getEntity() {
    return $this->entity;
  }


}
