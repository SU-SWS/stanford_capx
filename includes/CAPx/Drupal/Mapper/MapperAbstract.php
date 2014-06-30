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
   * [getRemoteDataByPath description]
   * @param  [type] $data [description]
   * @param  [type] $path [description]
   * @return [type]       [description]
   */
  // public function getRemoteDataByPath($data, $path) {
  //   $parts = explode("/", $path);
  //   $return_data = array();

  //   foreach($parts as $part) {

  //     array_shift($parts);

  //     if ($part == "*") {
  //       foreach ($data as $sub_data) {
  //         $new_path = implode("/", $parts);
  //         $tmp = $this->getRemoteDataByPath($sub_data, $new_path);
  //         $return_data = array_merge($return_data, $tmp);
  //       }
  //       return $return_data;
  //     }
  //     else if (isset($data[$part])) {
  //       $data = $data[$part];
  //     }
  //     else {
  //       throw new \Exception("Could not find data for path", 1);
  //     }

  //   }

  //   if (!is_array($data)) {
  //     $return_data[] = $data;
  //   }

  //   return $return_data;
  // }

  /**
   * Uses a JSONPath library and notation to find data in a parsed json array.
   * Documentation on JSON path: http://goessner.net/articles/JsonPath/
   *
   * @param  [type] $data [description]
   * @param  [type] $path [description]
   * @return [type]       [description]
   */
  public function getRemoteDataByJsonPath($data, $path) {

    $jsonParser = new JsonParser();
    $parsed = $jsonParser->get($data, $path);
    return $parsed;

  }

  /**
   * [getEntityType description]
   * @return [type] [description]
   */
  public function getEntityType() {
    return $this->getConfigSetting('entityType');
  }

  /**
   * [getEntityType description]
   * @return [type] [description]
   */
  public function getBundleType() {
    return $this->getConfigSetting('bundleType');
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
      throw new Exception("No config setting by that name");
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
