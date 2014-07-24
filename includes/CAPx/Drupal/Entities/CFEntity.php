<?php
/**
 * @file
 * @author
 */

namespace CAPx\Drupal\Entities;
use CAPx\Drupal\Mapper\EntityMapper;
use CAPx\Drupal\Mapper\FieldCollectionMapper;

class CFEntity extends \Entity {

  /**
   * [defaultLabel description]
   * @return [type] [description]
   */
  protected function defaultLabel() {
    return $this->title;
  }

  /**
   * [defaultUri description]
   * @return [type] [description]
   */
  protected function defaultUri() {
    return array('path' => 'cfe/' . $this->identifier());
  }

  /**
   * Returns the entity Mapper for this CFE.
   * @return [type] [description]
   */
  public function getEntityMapper() {
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
    $settings = $this->settings;
    $settings['entityType'] = $settings['config']['entity-type'];
    $settings['bundleType'] = $settings['config']['bundle'];
    $settings['fieldCollections'] = array();

    if (isset($settings['collections'])) {
      foreach ($settings['collections'] as $fieldName => $fields) {
        $collectionConfig = array();
        $collectionConfig['bundleType'] = $fieldName;
        $collectionConfig['fields'] = $fields;
        $collectionConfig['properties'] = array();
        $settings['fieldCollections'][$fieldName] = new FieldCollectionMapper($collectionConfig);
      }
    }

    unset($settings['collections']);

    return $settings;
  }

}
