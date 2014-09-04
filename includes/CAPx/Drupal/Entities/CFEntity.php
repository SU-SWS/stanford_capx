<?php
/**
 * @file
 * @author
 */

namespace CAPx\Drupal\Entities;

use CAPx\Drupal\Mapper\EntityMapper;
use CAPx\Drupal\Mapper\FieldCollectionMapper;
use CAPx\Drupal\Util\CAPxMapper;
use CAPx\Drupal\Util\CAPxConnection;
use CAPx\Drupal\Importer\EntityImporter;

class CFEntity extends \Entity {

  /**
   * This constructor takes the top level items in the settings property array
   * and exposes them as properties of the instance. This is just a syntax
   * reducing bit of fun.
   */
  public function __construct(array $values = array(), $entityType = NULL) {
    parent::__construct($values, $entityType);

    /**
     * Expose settings for easier get access.
     */

    if (isset($this->settings)) {

      $ar = $this->settings;

      if (!is_array($this->settings)) {
        $ar = unserialize($this->settings);
      }

      foreach ($ar as $key => $value) {
        if (!isset($this->{$key})) {
          $this->{$key} = $value;
        }
      }
    }

  }

  /**
   * Implements defaultLable()
   * @return [type] [description]
   */
  protected function defaultLabel() {
    return $this->title;
  }

  /**
   * Implements defaultUIR
   * @return [type] [description]
   */
  protected function defaultUri() {
    return array('path' => 'cfe/' . $this->identifier());
  }

  /**
   * Returns the entity Mapper for this CFE. The entity mapper is different from
   * the mapper configuration entity.
   * @return [type] [description]
   */
  public function getEntityMapper() {

    if ($this->type !== "mapper") {
      throw new \Exception("Cannot call getEntityMapper on non mapper type.");
    }

    $mapperConfig = $this->getEntityMapperConfig();
    $mapper = new EntityMapper($mapperConfig);
    return $mapper;
  }

  /**
   * This function takes the saved settings and retuns an array that
   * matches the API importer library settings.
   * @return [type] [description]
   */
  public function getEntityMapperConfig() {

    if ($this->type !== "mapper") {
      throw new \Exception("Cannot call getEntityMapperConfig on non mapper type.");
    }

    $settings = $this->settings;
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

    return $settings;
  }

  /**
   * This function takes the saved settings and retuns an array that
   * matches the API importer library settings.
   * @return [type] [description]
   */
  public function getEntityImporterConfig() {

    if ($this->type !== "importer") {
      throw new \Exception("Cannot call getEntityImporterConfig on non importer type.");
    }

    $settings = $this->settings;
    $settings['machine_name'] = $this->machine_name;

    if (!empty($settings['organization'])) {
      $settings['types'][] = 'orgCodes';
      $settings['values'][] = explode(",", $settings['organization']);
    }

    if (!empty($settings['workgroup'])) {
      $settings['types'][] = 'privGroups';
      $settings['values'][] = explode(",", $settings['workgroup']);
    }

    if (!empty($settings['sunet_id'])) {
      $settings['types'][] = 'uids';
      $settings['values'][] = explode(",", $settings['sunet_id']);
    }

    return $settings;
  }

  /**
   * Returns a ready to use entity importer
   * @return EntityImporter - Ready to use entity importer.
   */
  public function getEntityImporter() {

    if ($this->type !== "importer") {
      throw new \Exception("Cannot call getEntityImporter on non importer type.");
    }

    $config = self::getEntityImporterConfig();
    $mapper = CAPxMapper::loadEntityMapper($this->mapper);
    $client = CAPxConnection::getAuthenticatedHTTPClient();

    $importer = new EntityImporter($config, $mapper, $client);
    return $importer;
  }

}
