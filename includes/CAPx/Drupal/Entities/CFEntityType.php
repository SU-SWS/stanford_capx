<?php
/**
 * @file
 * @author
 */

namespace CAPx\Drupal\Entities;

class CFEntityType extends \Entity {
  public $type;
  public $label;
  public $weight = 0;

  /**
   * [__construct description]
   * @param array $values [description]
   */
  public function __construct($values = array()) {
    parent::__construct($values, 'capx_cfe_type');
  }

  /**
   * [isLocked description]
   * @return boolean [description]
   */
  function isLocked() {
    return isset($this->status) && empty($this->is_new) && (($this->status & ENTITY_IN_CODE) || ($this->status & ENTITY_FIXED));
  }

}
