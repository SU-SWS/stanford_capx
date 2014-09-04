<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Mapper;

class FieldCollectionMapper extends EntityMapper {

  protected $bundle = null;

  /**
   * @see parent::__construct()
   * Additionaly to the parent set the bundle "field_name"
   * @param array $config additional configuration.
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
   * Set the bundle type of this FieldCollectionMapper.
   * @param [type] $name [description]
   */
  public function setBundle($name) {
    $this->bundle = $name;
  }

}
