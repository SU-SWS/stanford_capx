<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Mapper;

class FieldCollectionMapper extends EntityMapper {

  protected $bundle = null;

  /**
   * [__construct description]
   * @param [type] $config [description]
   */
  public function __construct($config) {
    parent::__construct($config);
    $this->setBundle($config['bundleType']);
  }

  /**
   * Parent override...
   * @return [type] [description]
   */
  public function getEntityType() {
    return 'field_collection_item';
  }

  /**
   * Parent override.
   * @return [type] [description]
   */
  public function getBundleType() {
    return $this->bundle;
  }

  /**
   * [setBundle description]
   * @param [type] $name [description]
   */
  public function setBundle($name) {
    $this->bundle = $name;
  }

}
