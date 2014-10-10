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
   * Returns the metadata
   * @return [type] [description]
   */
  public function getMeta() {
    return $this->meta;
  }

  /**
   * Sets the metadata array
   * @param [type] $meta [description]
   */
  public function setMeta($meta = null) {

    /**
     * Populate some defaults if empty.
     */
    if (empty($meta)) {
      $meta = array(
        'lastUpdate' => 0,
        'lastUpdateHuman' => t('Never'),
        'count' => 0,
      );
    }

    // Set the stuff.
    $this->meta = $meta;
  }

  /**
   * Returns the machine name of this entity.
   *
   * @return string
   *   The machine name
   */
  public function getMachineName() {
    return $this->machine_name;
  }

}
